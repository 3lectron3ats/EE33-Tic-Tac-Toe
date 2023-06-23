/**
  * @file      EE33_LED.h
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


#ifndef EE33_LED_H
#define EE33_LED_H

// Arduino versioning.
#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"	// for digitalRead, digitalWrite, etc
#else
#include "WProgram.h"
#endif

#define LED_VERSION_MAJOR  1
#define LED_VERSION_MINOR  0

#define MATRIX_3X3_LENGTH            3

#define LED_BLINK_INTERVAL_MS        300

#define SEQUENCE_INCREASE_CYCLE(seq)  (seq != LED_SEQ_MAX)? seq + 1 : LED_SEQ_MIN
#define SEQUENCE_DECREASE_CYCLE(seq)  (seq != LED_SEQ_MIN)? seq - 1 : LED_SEQ_MAX

#define COLOR_CYCLE(color)            (color != LED_COLOR_MAX)? color + 1 : LED_COLOR_MIN

#define INT16_MIN       (1 << 15)
#define INT16_MAX       (0xFFFF >> 1)


typedef enum LED_STATUS
{
	LED_STATUS_OFF,
	LED_STATUS_ON,
	LED_STATUS_BLINK,
};

typedef enum LED_COLOR
{
	LED_COLOR_A = 0,
	LED_COLOR_B,

	LED_COLOR_TOTAL,

	LED_COLOR_MIN = LED_COLOR_A,
	LED_COLOR_MAX = LED_COLOR_B
};

typedef enum LED_SEQUENCE
{
	LED_1 = 0,
	LED_2,
	LED_3,
	LED_4,
	LED_5,
	LED_6,
	LED_7,
	LED_8,
	LED_9,

	LED_SEQ_TOTAL,

    LED_SEQ_MIN = LED_1,	
	LED_SEQ_MAX = LED_9,
};

typedef union
{
  uint16_t d16;
  struct
  {
	  uint16_t m11 : 1; uint16_t m12 : 1; uint16_t m13 : 1;
	  uint16_t m21 : 1; uint16_t m22 : 1; uint16_t m23 : 1;
	  uint16_t m31 : 1; uint16_t m32 : 1; uint16_t m33 : 1;
	  uint16_t rsv : 7;
  };
} T_MATRIX_3X3;

typedef enum PLAYER_TYPE
{
	PLAYER_TYPE_FRIEND,
    PLAYER_TYPE_ENMY,
    
	PLAYER_TYPE_MIN = PLAYER_TYPE_FRIEND,
	PLAYER_TYPE_MAX = PLAYER_TYPE_ENMY,	
};

typedef enum MATRIX_3X3_ELEMENT
{
	MATRIX_3X3_M11 = 0,
	MATRIX_3X3_M12 = 1,
	MATRIX_3X3_M13 = 2,
	MATRIX_3X3_M21 = 3,
	MATRIX_3X3_M22 = 4,
	MATRIX_3X3_M23 = 5,
	MATRIX_3X3_M31 = 6,
	MATRIX_3X3_M32 = 7,
	MATRIX_3X3_M33 = 8,
	
	MATRIX_3X3_ELEMENT_TOTAL,
};

typedef enum LINE_SEQUENCE
{
	LINE_1 = 0,
	LINE_2 = 1,
	LINE_3 = 2,
	LINE_4 = 3,
	LINE_5 = 4,
	LINE_6 = 5,
	LINE_7 = 6,
	LINE_8 = 7,
	
	LINE_SEQ_TOTAL,
};

typedef struct
{
  LED_STATUS status;
  byte       pin;
} SINGLE_LED_T;

typedef struct
{
  LED_SEQUENCE seq[MATRIX_3X3_LENGTH];
} T_LED_LINE;

class LED_SQUARE
{
public:

/**
	* @brief  construction
	* @param  *color_A_pin color A LED pin distribution
	* @param  *color_B_pin color B LED pin distribution
	* @note   pin distribution according to PCB layout
	* @par    sample code
	* @code
    *       byte player_A_pin[] = {0,4,6,8,10,12,A0,A2,A4};
    *       byte player_B_pin[] = {1,5,7,9,11,13,A1,A3,A5};     
    *       LED_SQUARE led_square(player_A_pin, player_B_pin);
    *    
	* @endcode
	*/
	LED_SQUARE(byte *color_A_pin, byte *color_B_pin);

/**
	* @brief  destruction 
	*/
	~LED_SQUARE();

/**
	* @brief  configure all the LED pin as output mode and set all off
	*/
	void begin(void);

/**
	* @brief  stop all LED behavior and reset variable
	*/
	void end(void);

/**
	* @brief  set status of specific LED 
	* @param  LED_seq
	* @param  LED_color
	* @param  status
	*/
	void set_LEDStatus(LED_SEQUENCE LED_seq, LED_COLOR LED_color, LED_STATUS status);

/**
	* @brief  set all LED as OFF status 
	*/
	void set_allOff(void);

/**
	* @brief  set all LED as ON status 
	*/
	void set_allOn(void);

/**
	* @brief  set status of specific LED line 
	* @param  Line_seq
	* @param  LED_color
	* @param  status
	*/
	void set_LineStatus(LINE_SEQUENCE Line_seq, LED_COLOR LED_color, LED_STATUS status);

/**
	* @brief  get status of specific LED 
	* @param  LED_seq
	* @param  LED_color
	* @return LED_STATUS
	*/
	LED_STATUS get_LEDStatus(LED_SEQUENCE LED_seq, LED_COLOR LED_color);

/**
	* @brief  find the sequence of next(sequence increase) idle LED(status OFF)
	* @param  LED_now current LED sequence
	* @return result be founded, LED_SEQ_TOTAL means than result is invalid.
	*/	
	LED_SEQUENCE find_IdleLed_Forward(LED_SEQUENCE LED_now);

/**
	* @brief  find the sequence of last(sequence decrease) idle LED(status OFF)
	* @param  LED_now current LED sequence
	* @return result be founded, LED_SEQ_TOTAL means than result is invalid.
	*/	
	LED_SEQUENCE find_IdleLed_Backword(LED_SEQUENCE LED_now);

/**
	* @brief check all the LEDs in line match the specify condition  
	* @param LineSeq 	
	* @param LED_color
	* @param status
	* @retval true LEDs in line match the specify condition
	* @retval false LEDs in line not match the specify condition	
	*/
	bool Is_LineBingo(LINE_SEQUENCE LineSeq, LED_COLOR LED_color, LED_STATUS status);

/**
	* @brief count the number of line match the specify condition
	* @param LED_color	
	* @param status
	* @return the number of bingo line 	
	*/
	uint8_t get_LineBingo_number(LED_COLOR LED_color, LED_STATUS status);

/**
	* @brief  drive blink LED work, it needs to be execute in loop 
	* @par    sample code
	* @code
    *       loop 
    *       {
    *           blinkRoutine();     
    *       }
    *    
	* @endcode
	*/
	void blinkRoutine(void);

/**
	* @brief according to current status, calculate the next step  
	* @param LED_color color of now player 	
	* @return next position	
	*/
	LED_SEQUENCE BestNextMove(LED_COLOR LED_color);

/**
	* @brief print LED distribution(for debug) 	
	*/
    void print_matrix(void);

private:

/**
	* @brief  configure all the LED pin as output mode
	*/
    void _allPin_OutputMode(void);

/**
	* @brief  control LED pin as ON or OFF
	* @param  pin_number 
	* @param  status
	* @arg    HIGH
	* @arg    LOW
	*/
	void _controlLED(byte pin_number, uint8_t status);

/**
	* @brief  find the sequence of next/last idle LED(status OFF)
	* @param  LED_now current LED sequence
	* @param  is_next
	* @arg    ture: next sequence
	* @arg    false: last sequence	
    * @return idle LED sequence, LED_SEQ_TOTAL means did not found 
	*/
	LED_SEQUENCE _find_IdleLed(LED_SEQUENCE LED_now, bool is_next);

/**
	* @brief  count bit 1
	* @param  n
	* @return total bit 1 in number 	
	*/
	uint8_t _minmax_popcount(uint32_t n);
	
/**
	* @brief  calculate minmax score
	* @param  MatrixMap_Friend
	* @param  MatrixMap_Enmy	
	* @return minmax score 	
	*/	
	int16_t _minmax_cal_score(T_MATRIX_3X3 MatrixMap_Friend, T_MATRIX_3X3 MatrixMap_Enmy);

/**
	* @brief according to current friend and enmy status, calculate the next step  
	* @param MatrixMap_Friend map of friend
	* @param MatrixMap_Enmy map of enmy
	* @param WhoseTurn next step of friend or enmy		 	
	* @return next position	
	*/
	MATRIX_3X3_ELEMENT _BestNextMove_Wrapper(T_MATRIX_3X3 MatrixMap_Friend, T_MATRIX_3X3 MatrixMap_Enmy, PLAYER_TYPE WhoseTurn);

/**
	* @brief according to current friend and enmy status, calculate the score  
	* @param MatrixMap_Friend map of friend
	* @param MatrixMap_Enmy map of enmy
	* @param alpha_score
	* @param beta_score
	* @param WhoseTurn 			 	
	* @return score	of current state in WhoseTurn
	*/
	int16_t _minmax(T_MATRIX_3X3 MatrixMap_Friend, T_MATRIX_3X3 MatrixMap_Enmy, int16_t alpha_score, int16_t beta_score, PLAYER_TYPE WhoseTurn);

    void _print_matrix(T_MATRIX_3X3 MatrixMap_Friend, T_MATRIX_3X3 MatrixMap_Enmy);

	void _print_matrix_row(uint8_t friend_enable, uint8_t enmy_enable);

	/** LED information */	
	SINGLE_LED_T LED_data[LED_SEQ_TOTAL][LED_COLOR_TOTAL];

	/** blinking LED status */
	uint8_t LED_blink_status;

	/** LED bitmap */
	T_MATRIX_3X3 MatrixBitmap[LED_COLOR_TOTAL];

	/** blinking LED millis accumulation */
    unsigned long LED_blink_millis_accumulation;
};

#endif  //EE33_LED_H


/*
 * @release note
 * | v1.0 2023-06-09 - 3lectron3ats : Initial Release
 * #
*/
