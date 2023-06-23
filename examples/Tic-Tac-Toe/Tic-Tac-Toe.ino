/**
  * @file      Tic-Tac-Toe.h
  * @version   v1.0.0
  * @author    3lectron3ats <3lectron3ats@gmail.com>
  * @date      2022.06.09
  * @brief     Tic-Tac-Toe Game Project
  * @copyright GPL v3.0
  * *************************************************************************************
  * @attention
  * <h2><center>&copy; COPYRIGHT 2023 3lectron3ats </center></h2>
  * *************************************************************************************
  */


#include <EE33_Tic_Tac_Toe.h>
#include <EE33_EncoderButton.h>
#include <EE33_LED.h>

const byte player_A_pin[LED_SEQ_TOTAL] = {0,4,6,8,10,12,A0,A2,A4};
const byte player_B_pin[LED_SEQ_TOTAL] = {1,5,7,9,11,13,A1,A3,A5};

#define LED_SHOW_TIME  500

GAME_MODE GameMode;
bool PC_first_step = true;
	
LED_SEQUENCE ActiveBlinkLED_Seq = LED_SEQ_TOTAL;
LED_COLOR    ActiveBlinkLED_Color = LED_COLOR_TOTAL;

const LED_SQUARE led_square(player_A_pin, player_B_pin);

Button button(EXTERNAL_INTERRUPT_D3, 400);
Encoder encoder(EXTERNAL_INTERRUPT_D2, A6);


void game_start(void)
{
	led_square.set_allOff();

	if (GAME_MODE_PLAYER_VS_PLAYER == GameMode)
	{
		led_square.set_LEDStatus(LED_1, LED_COLOR_A, LED_STATUS_BLINK);

		ActiveBlinkLED_Seq = LED_1;
		ActiveBlinkLED_Color = LED_COLOR_A;
	}

	if (GAME_MODE_PLAYER_VS_COMPUTER == GameMode)
	{
		randomSeed(analogRead(A7));
		
		if (PC_first_step)
		{
			//PC first

			LED_SEQUENCE next_step;
			LED_SEQUENCE idle_led_seq;

			next_step = led_square.BestNextMove(LED_COLOR_A);
			
			led_square.set_LEDStatus(next_step, LED_COLOR_B, LED_STATUS_ON);

			idle_led_seq = led_square.find_IdleLed_Forward((LED_SEQUENCE)next_step);
		
			led_square.set_LEDStatus((LED_SEQUENCE)idle_led_seq, LED_COLOR_A, LED_STATUS_BLINK);

			ActiveBlinkLED_Seq = (LED_SEQUENCE)idle_led_seq;
			ActiveBlinkLED_Color = LED_COLOR_A;
		}
		else
		{
			//player first

			led_square.set_LEDStatus(LED_1, LED_COLOR_A, LED_STATUS_BLINK);

			ActiveBlinkLED_Seq = LED_1;
			ActiveBlinkLED_Color = LED_COLOR_A;			
		}

		PC_first_step = !PC_first_step;
	}
}

void game_win(LED_COLOR winner_color)
{
	SimpleQueue line_blink_queue(sizeof(LINE_SEQUENCE), LINE_SEQ_TOTAL);

    //save all bingo line
    for (LINE_SEQUENCE i = LINE_1; i < LINE_SEQ_TOTAL; i = i + 1)
	{
    	if (led_square.Is_LineBingo(i, winner_color, LED_STATUS_ON))
		{
			line_blink_queue.push(&i);
		}
	}

	led_square.set_allOff();

	for (int32_t i = 0; i < line_blink_queue.available(); i++)
	{
		LINE_SEQUENCE seq;

        line_blink_queue.peekIndex(&seq, i);

		led_square.set_LineStatus(seq, winner_color, LED_STATUS_ON);
	}
	delay(LED_SHOW_TIME);

	led_square.set_allOff();
	delay(LED_SHOW_TIME);

	for (int32_t i = 0; i < line_blink_queue.available(); i++)
	{
		LINE_SEQUENCE seq;

        line_blink_queue.peekIndex(&seq, i);

		led_square.set_LineStatus(seq, winner_color, LED_STATUS_ON);
	}

	delay(LED_SHOW_TIME);
	
	led_square.set_allOff();
}

void game_tie(void)
{
	delay(LED_SHOW_TIME);
	
	led_square.set_LEDStatus(LED_9, LED_COLOR_A, LED_STATUS_OFF);
	led_square.set_LEDStatus(LED_9, LED_COLOR_B, LED_STATUS_OFF);
	
	delay(LED_SHOW_TIME);
	
	led_square.set_LEDStatus(LED_6, LED_COLOR_A, LED_STATUS_OFF);
	led_square.set_LEDStatus(LED_6, LED_COLOR_B, LED_STATUS_OFF);
	
	led_square.set_LEDStatus(LED_8, LED_COLOR_A, LED_STATUS_OFF);
	led_square.set_LEDStatus(LED_8, LED_COLOR_B, LED_STATUS_OFF);
	
	delay(LED_SHOW_TIME);
	
	led_square.set_LEDStatus(LED_3, LED_COLOR_A, LED_STATUS_OFF);
	led_square.set_LEDStatus(LED_3, LED_COLOR_B, LED_STATUS_OFF);
	
	led_square.set_LEDStatus(LED_5, LED_COLOR_A, LED_STATUS_OFF);
	led_square.set_LEDStatus(LED_5, LED_COLOR_B, LED_STATUS_OFF);
	
	led_square.set_LEDStatus(LED_7, LED_COLOR_A, LED_STATUS_OFF);
	led_square.set_LEDStatus(LED_7, LED_COLOR_B, LED_STATUS_OFF); 
	
	delay(LED_SHOW_TIME);
	
	led_square.set_LEDStatus(LED_2, LED_COLOR_A, LED_STATUS_OFF);
	led_square.set_LEDStatus(LED_2, LED_COLOR_B, LED_STATUS_OFF);
	
	led_square.set_LEDStatus(LED_4, LED_COLOR_A, LED_STATUS_OFF);
	led_square.set_LEDStatus(LED_4, LED_COLOR_B, LED_STATUS_OFF);
	
	delay(LED_SHOW_TIME);
	
	led_square.set_LEDStatus(LED_1, LED_COLOR_A, LED_STATUS_OFF);
	led_square.set_LEDStatus(LED_1, LED_COLOR_B, LED_STATUS_OFF); 
	
	delay(LED_SHOW_TIME);
}

GAME_STATUS game_judeg(LED_COLOR current_color)
{
	if (led_square.get_LineBingo_number(current_color, LED_STATUS_ON))
	{
		sensor_disable();
	
		game_win(current_color);
		game_start();
		
		sensor_enable();

		return GAME_STATUS_WIN;
	}
	else
	{
		LED_SEQUENCE idle_led_seq;
		
		idle_led_seq = led_square.find_IdleLed_Forward(ActiveBlinkLED_Seq);

		if (idle_led_seq != LED_SEQ_TOTAL)
		{
			led_square.set_LEDStatus(idle_led_seq, COLOR_CYCLE(current_color), LED_STATUS_BLINK);

			ActiveBlinkLED_Seq = idle_led_seq;
			ActiveBlinkLED_Color = COLOR_CYCLE(current_color);

			return GAME_STATUS_NEXT_PLAYER;
		}
		else  
		{
		    //no free space
			sensor_disable();
			
			game_tie();
			game_start();
			
			sensor_enable();

			return GAME_STATUS_TIE;
		}		
	}
}

void sensor_disable(void)
{
	button.stopDetect();
	encoder.stopDetect();
}

void sensor_enable(void)
{
	button.startDetect();
	encoder.startDetect();
}

void button_event(uint8_t click_count)
{
	switch (click_count)
	{
			case 1:
			{
				led_square.set_LEDStatus(ActiveBlinkLED_Seq, ActiveBlinkLED_Color, LED_STATUS_ON);

				if (GAME_STATUS_NEXT_PLAYER == game_judeg(ActiveBlinkLED_Color))
				{
					if (GAME_MODE_PLAYER_VS_COMPUTER == GameMode)
					{
						LED_SEQUENCE next_step;

						next_step = led_square.BestNextMove(ActiveBlinkLED_Color);
						
						led_square.set_LEDStatus(ActiveBlinkLED_Seq, ActiveBlinkLED_Color, LED_STATUS_OFF);
						led_square.set_LEDStatus(next_step, ActiveBlinkLED_Color, LED_STATUS_ON);

						game_judeg(ActiveBlinkLED_Color);
					}
				}	
			}
			break;
		
			default:
			break;
	}
}

void encoder_event(ENCODER_RESULT event)
{
	if (ActiveBlinkLED_Seq != LED_SEQ_TOTAL && ActiveBlinkLED_Color != LED_COLOR_TOTAL)
	{
		LED_SEQUENCE current_active_seq = ActiveBlinkLED_Seq;
		LED_COLOR    current_active_color = ActiveBlinkLED_Color;		
		
		switch (event)
		{
			LED_SEQUENCE idle_led_seq;
			
			case ENCODER_CLOCKWISE:
			{
                idle_led_seq = led_square.find_IdleLed_Forward(current_active_seq);

				if (idle_led_seq != LED_SEQ_TOTAL)
				{
					led_square.set_LEDStatus(current_active_seq, current_active_color, LED_STATUS_OFF);
					led_square.set_LEDStatus(idle_led_seq, current_active_color, LED_STATUS_BLINK);

					ActiveBlinkLED_Seq = idle_led_seq; 
					ActiveBlinkLED_Color = current_active_color;					
				}
		
			}
			break;
		
			case ENCODER_COUNTERCLOCKWISE:
			{
				idle_led_seq = led_square.find_IdleLed_Backword(current_active_seq);

				if (idle_led_seq != LED_SEQ_TOTAL)
				{
					led_square.set_LEDStatus(current_active_seq, current_active_color, LED_STATUS_OFF);
					led_square.set_LEDStatus(idle_led_seq, current_active_color, LED_STATUS_BLINK);

					ActiveBlinkLED_Seq = idle_led_seq; 
					ActiveBlinkLED_Color = current_active_color;					
				}
			}
			break;
		
			default:
			break;
		}
	}
}

void setup()
{
  Serial.begin(115200);
  Serial.end();

  led_square.begin();
  button.begin();

  encoder.begin();

  GameMode = (button.pinRead() == false) ? GAME_MODE_PLAYER_VS_COMPUTER : GAME_MODE_PLAYER_VS_PLAYER;
  game_start();
}

void loop()
{	
	if (button.available())
	{
		button_event(button.read());
	}

	if (encoder.available())
	{
		encoder_event(encoder.read());	
	}

	led_square.blinkRoutine();
}

/*
 * @release note
 * | v1.0.0 2023-06-09 - 3lectron3ats : Initial Release
 * #
*/
