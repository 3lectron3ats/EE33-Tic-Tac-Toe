/**
  * @file      EE33_LED.cpp
  * @version   v1.0
  * @author    3lectron3ats <3lectron3ats@gmail.com>
  * @date      2022.06.09
  * @brief     Implement of LED to display Tic-Tac-Toe.
  * @copyright GPL v3.0
  * *************************************************************************************
  * @attention
  * <h2><center>&copy; COPYRIGHT 2023 3lectron3ats </center></h2>
  * *************************************************************************************
  */

#include <EE33_Tic_Tac_Toe.h>
#include <EE33_LED.h>
#include <EE33_SimpleQueue.h>

const T_LED_LINE line_list[LINE_SEQ_TOTAL] =
{
	[LINE_1] = {LED_1, LED_2, LED_3},
	[LINE_2] = {LED_4, LED_5, LED_6},
	[LINE_3] = {LED_7, LED_8, LED_9},
	[LINE_4] = {LED_1, LED_4, LED_7},
	[LINE_5] = {LED_2, LED_5, LED_8},
	[LINE_6] = {LED_3, LED_6, LED_9},
	[LINE_7] = {LED_1, LED_5, LED_9},
	[LINE_8] = {LED_3, LED_5, LED_7},
};

T_MATRIX_3X3 StraightLine_3X3[LINE_SEQ_TOTAL] =
{
	[0] = {.d16 = 0x07},  //.m11 = 1, .m12 = 1, .m13 = 1
	[1] = {.d16 = 0x38},  //.m21 = 1, .m22 = 1, .m23 = 1
	[2] = {.d16 = 0x1C0}, //.m31 = 1, .m32 = 1, .m33 = 1
	[3] = {.d16 = 0x49},  //.m11 = 1, .m21 = 1, .m31 = 1
	[4] = {.d16 = 0x92},  //.m12 = 1, .m22 = 1, .m32 = 1
	[5] = {.d16 = 0x124}, //.m13 = 1, .m23 = 1, .m33 = 1
	[6] = {.d16 = 0x111}, //.m11 = 1, .m22 = 1, .m33 = 1
	[7] = {.d16 = 0x54},  //.m13 = 1, .m22 = 1, .m31 = 1
};


LED_SQUARE::LED_SQUARE(byte *color_A_pin, byte *color_B_pin)
{
	LED_blink_status = LOW;
	LED_blink_millis_accumulation = 0;

	SINGLE_LED_T *p_led = &LED_data[LED_1][LED_COLOR_A];
	
	for (uint8_t i = 0; i < LED_SEQ_TOTAL; i++)
	{
		p_led->pin = color_A_pin[i];
		p_led += LED_COLOR_TOTAL;
	}

	p_led = &LED_data[LED_1][LED_COLOR_B];
	
	for (uint8_t i = 0; i < LED_SEQ_TOTAL; i++)
	{
		p_led->pin = color_B_pin[i];
		p_led += LED_COLOR_TOTAL;
	}
	
}

LED_SQUARE::~LED_SQUARE()
{
	end();
}

void LED_SQUARE::begin(void)
{
	_allPin_OutputMode();
	set_allOff();
}

void LED_SQUARE::end(void)
{
	set_allOff();

	LED_blink_status = LOW;
	LED_blink_millis_accumulation = 0;
}

void LED_SQUARE::set_LEDStatus(LED_SEQUENCE LED_seq, LED_COLOR LED_color, LED_STATUS status)
{
    SINGLE_LED_T *p_led = (SINGLE_LED_T *)&LED_data[LED_seq][LED_color];
	p_led->status = status;

	T_MATRIX_3X3 *bitmap = (T_MATRIX_3X3 *)&MatrixBitmap[LED_color];
	bitmap->d16 = (status == LED_STATUS_ON) ? bitmap->d16 | bit(LED_seq) : (status == LED_STATUS_OFF) ? bitmap->d16 & ~bit((uint8_t)LED_seq) : bitmap->d16;

	if (status == LED_STATUS_OFF || 
		status == LED_STATUS_ON)
	{
		_controlLED(p_led->pin, (status == LED_STATUS_ON) ? HIGH : LOW);
		
	}	
}

void LED_SQUARE::set_allOff(void)
{
	for (uint8_t color = 0; color < LED_COLOR_TOTAL; color++)
	{
		for (uint8_t sequence = 0; sequence < LED_SEQ_TOTAL; sequence++)
		{
			set_LEDStatus(sequence, color , LED_STATUS_OFF);	
		}
	}
}

void LED_SQUARE::set_allOn(void)
{
	for (uint8_t color = 0; color < LED_COLOR_TOTAL; color++)
	{
		for (uint8_t sequence = 0; sequence < LED_SEQ_TOTAL; sequence++)
		{
			set_LEDStatus(sequence, color , LED_STATUS_ON);	
		}
	}
}

void LED_SQUARE::set_LineStatus(LINE_SEQUENCE Line_seq, LED_COLOR LED_color, LED_STATUS status)
{
	T_LED_LINE target_line;

	target_line = line_list[Line_seq];

	for (uint8_t i = 0; i < MATRIX_3X3_LENGTH; i++)
	{
		set_LEDStatus(target_line.seq[i], LED_color, status);
	}
}

LED_STATUS LED_SQUARE::get_LEDStatus(LED_SEQUENCE LED_seq, LED_COLOR LED_color)
{
	SINGLE_LED_T *p_led = (SINGLE_LED_T *)&LED_data[LED_seq][LED_color];

	return p_led->status;
}

LED_SEQUENCE LED_SQUARE::find_IdleLed_Forward(LED_SEQUENCE LED_now)
{
	return _find_IdleLed(LED_now, true);
}

LED_SEQUENCE LED_SQUARE::find_IdleLed_Backword(LED_SEQUENCE LED_now)
{
	return _find_IdleLed(LED_now, false);
}

bool LED_SQUARE::Is_LineBingo(LINE_SEQUENCE LineSeq, LED_COLOR LED_color, LED_STATUS status)
{
	T_LED_LINE target_line;

	target_line = line_list[LineSeq];

	for (uint8_t i = 0; i < MATRIX_3X3_LENGTH; i++)
	{
		if (get_LEDStatus(target_line.seq[i], LED_color) != status)
		{
		    return false;  //not all the LED in the line have the same status
		}
	}

	return true;
}

uint8_t LED_SQUARE::get_LineBingo_number(LED_COLOR LED_color, LED_STATUS status)
{
    uint8_t result = 0;

	for (uint8_t i = 0; i < LINE_SEQ_TOTAL; i++)
	{
		T_LED_LINE line;

		if (Is_LineBingo(i, LED_color, status))
		{
		    result++;
		}
	}	

	return result;
}

void LED_SQUARE::blinkRoutine(void)
{
	if (millis() - LED_blink_millis_accumulation > LED_BLINK_INTERVAL_MS)
	{
	    LED_blink_millis_accumulation = millis();
		
		LED_blink_status = (LED_blink_status == HIGH) ? LOW : HIGH;

		for (uint8_t color = 0; color < LED_COLOR_TOTAL; color++)
		{
			for (uint8_t sequence = 0; sequence < LED_SEQ_TOTAL; sequence++)
			{
				if (get_LEDStatus(sequence, color) == LED_STATUS_BLINK)
				{
					SINGLE_LED_T *p_led = (SINGLE_LED_T *)&LED_data[sequence][color];
					_controlLED(p_led->pin, LED_blink_status);
				}
			}
		}
	}
}

void LED_SQUARE::_allPin_OutputMode(void)
{
	for (uint8_t color = 0; color < LED_COLOR_TOTAL; color++)
	{
		for (uint8_t sequence = 0; sequence < LED_SEQ_TOTAL; sequence++)
		{
			pinMode(LED_data[sequence][color].pin, OUTPUT);
		}
	}
}

void LED_SQUARE::_controlLED(byte pin_number, uint8_t status)
{
    if (pin_number >= 0 && pin_number <= 13)
    {
		digitalWrite(pin_number, status);
    }
    else if (pin_number >= A0 && pin_number <= A5)
    {
		if (status == HIGH)
		{
		  analogWrite(pin_number, 1023);
		}
		else if (status == LOW)
		{
		  analogWrite(pin_number, 0);
		}
    }
}

LED_SEQUENCE LED_SQUARE::_find_IdleLed(LED_SEQUENCE LED_now, bool is_next)
{
	uint8_t color, sequence;
	LED_SEQUENCE led_position;

	led_position = (is_next) ? SEQUENCE_INCREASE_CYCLE(LED_now) : SEQUENCE_DECREASE_CYCLE(LED_now);

	for (sequence = 0; sequence < LED_SEQ_TOTAL; sequence++)
	{
		for (color = 0; color < LED_COLOR_TOTAL; color++)
		{
			if (get_LEDStatus(led_position, color) != LED_STATUS_OFF)
			{
				break;
			}
		}

		if (color == LED_COLOR_TOTAL)
		{
		    return led_position;			
		}

		led_position = (is_next) ? SEQUENCE_INCREASE_CYCLE(led_position) : SEQUENCE_DECREASE_CYCLE(led_position);
	}

	return LED_SEQ_TOTAL;
}

uint8_t LED_SQUARE::_minmax_popcount(uint32_t n)
{
    uint8_t count = 0;

    while (n)
    {
        n &= (n - 1);
        count++;
    }

    return count;
}

int16_t LED_SQUARE::_minmax_cal_score(T_MATRIX_3X3 MatrixMap_Friend, T_MATRIX_3X3 MatrixMap_Enmy)
{
	int16_t score = 0;
	uint8_t index;

	for (index = 0; index < LINE_SEQ_TOTAL; index++)
	{
		score = (_minmax_popcount(MatrixMap_Friend.d16 & StraightLine_3X3[index].d16) == MATRIX_3X3_LENGTH) ? score + 100: score;
		score = (_minmax_popcount(MatrixMap_Enmy.d16 & StraightLine_3X3[index].d16) == MATRIX_3X3_LENGTH) ? score - 100: score;
	}

	return score;
}


LED_SEQUENCE LED_SQUARE::BestNextMove(LED_COLOR LED_color)
{
	T_MATRIX_3X3 map_friend = MatrixBitmap[LED_color];
	T_MATRIX_3X3 map_enmy = MatrixBitmap[COLOR_CYCLE(LED_color)];

	return (LED_SEQUENCE)_BestNextMove_Wrapper(map_friend, map_enmy, PLAYER_TYPE_FRIEND);
}

MATRIX_3X3_ELEMENT LED_SQUARE::_BestNextMove_Wrapper(T_MATRIX_3X3 MatrixMap_Friend, T_MATRIX_3X3 MatrixMap_Enmy, PLAYER_TYPE WhoseTurn)
{
	if (_minmax_popcount(MatrixMap_Friend.d16 | MatrixMap_Enmy.d16) == LED_SEQ_TOTAL)  //matrix full
	{
		return MATRIX_3X3_ELEMENT_TOTAL;
	}

	if (_minmax_popcount(MatrixMap_Friend.d16 | MatrixMap_Enmy.d16) == 0)
	{
		return random(LED_SEQ_TOTAL);
	}

	T_MATRIX_3X3 MatrixMap;
	MATRIX_3X3_ELEMENT index, NextMove = MATRIX_3X3_ELEMENT_TOTAL;
	int16_t ScoreMinMax;
    
	int16_t ScoreAlpha = INT16_MIN;
	int16_t ScoreBeta = INT16_MAX;
	
	if (WhoseTurn == PLAYER_TYPE_FRIEND)
	{
		ScoreMinMax = INT16_MIN;

        SimpleQueue queue(sizeof(MATRIX_3X3_ELEMENT), MATRIX_3X3_ELEMENT_TOTAL);

		for (index = MATRIX_3X3_M11; index < LED_SEQ_TOTAL; index = index + 1)
		{
		    MatrixMap = (WhoseTurn == PLAYER_TYPE_FRIEND) ? MatrixMap_Friend : MatrixMap_Enmy;
			if (!(MatrixMap.d16 & bit(index)) && !(MatrixMap_Enmy.d16 & bit(index)))
			{
				int score;
				
				MatrixMap.d16 |= bit(index);
				score = _minmax(MatrixMap, MatrixMap_Enmy, ScoreAlpha, ScoreBeta, PLAYER_TYPE_ENMY);

				if (score > ScoreMinMax)
				{
					ScoreMinMax = score;

					queue.flush();
					queue.push(&index);				
				}
				else if (score == ScoreMinMax)
				{
					queue.push(&index);
				}
			}
		}

		int32_t NextMoveTotalNum;
		int32_t NextMoveChoose;
		
		NextMoveTotalNum = queue.available();

		NextMoveChoose = (int32_t)random(NextMoveTotalNum);

		queue.peekIndex(&NextMove, NextMoveChoose);
	}
	else if (WhoseTurn == PLAYER_TYPE_ENMY)
	{
		ScoreMinMax = INT16_MAX;
		for (index = MATRIX_3X3_M11; index < LED_SEQ_TOTAL; index = index + 1)
		{
		    MatrixMap = (WhoseTurn == PLAYER_TYPE_FRIEND) ? MatrixMap_Friend : MatrixMap_Enmy;
			if (!(MatrixMap.d16 & bit(index)) && !(MatrixMap_Friend.d16 & bit(index)))
			{
				int score;
				
				MatrixMap.d16 |= bit(index);
				score = _minmax(MatrixMap_Friend, MatrixMap, ScoreAlpha, ScoreBeta, PLAYER_TYPE_FRIEND);
				if (score < ScoreMinMax)
				{
					ScoreMinMax = score;
					NextMove = index;
				}				
			}
		}	
	}

	return NextMove;
}

int16_t LED_SQUARE::_minmax(T_MATRIX_3X3 MatrixMap_Friend, T_MATRIX_3X3 MatrixMap_Enmy, int16_t alpha_score, int16_t beta_score, PLAYER_TYPE WhoseTurn)
{
	T_MATRIX_3X3 MatrixMap_tmp;

	int16_t ScoreMinMax;

	ScoreMinMax = _minmax_cal_score(MatrixMap_Friend, MatrixMap_Enmy);

	if (ScoreMinMax || _minmax_popcount(MatrixMap_Friend.d16 | MatrixMap_Enmy.d16) == LED_SEQ_TOTAL)
	{
		//someone win or tie
		return ScoreMinMax;
	}
	
	T_MATRIX_3X3 MatrixMap;
	
	MATRIX_3X3_ELEMENT index;

	if (WhoseTurn == PLAYER_TYPE_FRIEND)
	{
		ScoreMinMax = INT16_MIN;
		
		for (index = MATRIX_3X3_M11; index < LED_SEQ_TOTAL; index = index + 1)
		{
			MatrixMap = (WhoseTurn == PLAYER_TYPE_FRIEND) ? MatrixMap_Friend : MatrixMap_Enmy;
			if (!(MatrixMap.d16 & bit(index)) && !(MatrixMap_Enmy.d16 & bit(index)))
			{
				int score;
				
				MatrixMap.d16 |= bit(index);
				score = _minmax(MatrixMap, MatrixMap_Enmy, alpha_score, beta_score, PLAYER_TYPE_ENMY);
				if (score > ScoreMinMax)
				{
					ScoreMinMax = score;
				}

				if (score > alpha_score)
				{
					alpha_score = score;
				}

				if (beta_score <= alpha_score)
				{
					break;
				}
			}
		}
	}
	else if (WhoseTurn == PLAYER_TYPE_ENMY)
	{
		ScoreMinMax = INT16_MAX;
		
		for (index = MATRIX_3X3_M11; index < LED_SEQ_TOTAL; index = index + 1)
		{
			MatrixMap = (WhoseTurn == PLAYER_TYPE_FRIEND) ? MatrixMap_Friend : MatrixMap_Enmy;
			if (!(MatrixMap.d16 & bit(index)) && !(MatrixMap_Friend.d16 & bit((index))))
			{
				int score;
				
				MatrixMap.d16 |= bit((index));
				score = _minmax(MatrixMap_Friend, MatrixMap, alpha_score, beta_score, PLAYER_TYPE_FRIEND);
				if (score < ScoreMinMax)
				{
					ScoreMinMax = score;
				}

				if (score < beta_score)
				{
					beta_score = score;
				}

				if (beta_score <= alpha_score)
				{
					break;
				}				
			}
		}	
	}

	return ScoreMinMax;
}

void LED_SQUARE::_print_matrix_row(uint8_t friend_enable, uint8_t enmy_enable)
{
  if (friend_enable && enmy_enable)
  {
  	Tic_Tac_Toe_print("O X");
  }
  else if (friend_enable)
  {
    Tic_Tac_Toe_print(" O ");
  }
  else if (enmy_enable)
  {
    Tic_Tac_Toe_print(" X ");
  }  
  else
  {
    Tic_Tac_Toe_print("   ");
  }
}


void LED_SQUARE::_print_matrix(T_MATRIX_3X3 MatrixMap_Friend, T_MATRIX_3X3 MatrixMap_Enmy)
{
  Tic_Tac_Toe_println("-------------");
  Tic_Tac_Toe_print("|");
  _print_matrix_row(MatrixMap_Friend.m11, MatrixMap_Enmy.m11);
  Tic_Tac_Toe_print("|");
  _print_matrix_row(MatrixMap_Friend.m12, MatrixMap_Enmy.m12);
  Tic_Tac_Toe_print("|");
  _print_matrix_row(MatrixMap_Friend.m13, MatrixMap_Enmy.m13);
  Tic_Tac_Toe_println("|");
  
  Tic_Tac_Toe_println("-------------");
  Tic_Tac_Toe_print("|");

  _print_matrix_row(MatrixMap_Friend.m21, MatrixMap_Enmy.m21);
    Tic_Tac_Toe_print("|");
  _print_matrix_row(MatrixMap_Friend.m22, MatrixMap_Enmy.m22);
    Tic_Tac_Toe_print("|");
  _print_matrix_row(MatrixMap_Friend.m23, MatrixMap_Enmy.m23);

  Tic_Tac_Toe_println("|");

  Tic_Tac_Toe_println("-------------");
  Tic_Tac_Toe_print("|");

  _print_matrix_row(MatrixMap_Friend.m31, MatrixMap_Enmy.m31);
    Tic_Tac_Toe_print("|");
  _print_matrix_row(MatrixMap_Friend.m32, MatrixMap_Enmy.m32);
    Tic_Tac_Toe_print("|");
  _print_matrix_row(MatrixMap_Friend.m33, MatrixMap_Enmy.m33);

  Tic_Tac_Toe_println("|");
  Tic_Tac_Toe_println("-------------");  
}

void LED_SQUARE::print_matrix(void)
{
  T_MATRIX_3X3 MatrixMap_A = MatrixBitmap[LED_COLOR_A];
  T_MATRIX_3X3 MatrixMap_B = MatrixBitmap[LED_COLOR_B];

  _print_matrix(MatrixMap_A, MatrixMap_B);
}


/*
 * @release note
 * | v1.0 2023-06-09 - 3lectron3ats : Initial Release
 * #
*/
