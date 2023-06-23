/**
  * @file      EE33_SimplerTimer.h
  * @version   v1.0
  * @author    3lectron3ats <3lectron3ats@gmail.com>
  * @date      2022.06.09
  * @brief     Implement of hardware timer control.
  * @copyright GPL v3.0
  * *************************************************************************************
  * @attention
  * <h2><center>&copy; COPYRIGHT 2023 3lectron3ats </center></h2>
  * *************************************************************************************
  */


#ifndef EE33_SIMPLE_TIMER_H
#define EE33_SIMPLE_TIMER_H

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#define SIMPLE_TIMER_VERSION_MAJOR  1
#define SIMPLE_TIMER_VERSION_MINOR  0

typedef void(* P_TIMER_CALLBACK)(void);

typedef enum
{
	TIMER_ID_0 = 0,  /*!< Timer0 */ 
	TIMER_ID_1,      /*!< Timer1 */
	TIMER_ID_2,      /*!< Timer2 */

	TIMER_ID_TOTAL,
} TIMER_ID;


typedef enum 
{
	TIMER_DEPTH_8  = 8,   /*!< Timer0, Timer2 */
	TIMER_DEPTH_16 = 16,  /*!< Timer1*/
	
} TIMER_DEPTH;

typedef enum
{
	TIMER_PRESCALER_1    = 1,     /*!< 16M clock, 1 presclar,    0.0000625ms each tick */
	TIMER_PRESCALER_8    = 8,     /*!< 16M clock, 8 presclar,    0.0005ms each tick */    
	TIMER_PRESCALER_32   = 32,    /*!< 16M clock, 32 presclar,   0.002ms each tick */   
	TIMER_PRESCALER_64   = 64,    /*!< 16M clock, 64 presclar,   0.004ms each tick */  
	TIMER_PRESCALER_128  = 128,   /*!< 16M clock, 128 presclar,  0.008ms each tick */  
	TIMER_PRESCALER_256  = 256,   /*!< 16M clock, 256 presclar,  0.016ms each tick */  
	TIMER_PRESCALER_1024 = 1024,  /*!< 16M clock, 1024 presclar, 0.064ms each tick */  
	
} TIMER_PRESCALER;

typedef struct
{
  TIMER_DEPTH timer_depth;
  TIMER_PRESCALER timer_prescaler;
  bool      is_used;
  bool      is_activated;
  P_TIMER_CALLBACK callFunc;
} T_TIMER;

class Timer_Manager {

public:

/**
	* @brief  construction
	*/
	Timer_Manager();

/**
	* @brief  destruction
	*/
	~Timer_Manager();

/**
	* @brief  register timer
	* @param  id
	* @arg    TIMER_ID_0
	* @arg    TIMER_ID_1
	* @arg    TIMER_ID_2	
	* @param  interval_ms
	* @param  *callFunc	          
	* @retval true timer register successfully.
	* @retval false timer register unsuccessfully.	
	*/	
	bool begin(TIMER_ID id, float interval_ms, void (*callFunc)(void));

/**
	* @brief  unregister timer
	* @param  id
	* @arg    TIMER_ID_0
	* @arg    TIMER_ID_1
	* @arg    TIMER_ID_2	
	* @retval true timer unregister successfully.
	* @retval false timer unregister unsuccessfully.
	*/
	bool end(TIMER_ID id);

/**
	* @brief  timer start counting
	* @param  id	  
	* @retval true timer start successfully.
	* @retval false timer start unsuccessfully.
	*/
	bool start(TIMER_ID id);

/**
	* @brief  timer stop counting
	* @param  id  
	* @retval true timer stop successfully.
	* @retval false timer stop unsuccessfully.
	*/
	bool stop(TIMER_ID id);

/**
	* @brief  timer change the interval
	* @param  id
	* @param  interval_ms	
	* @retval true timer change the interval and start successfully.
	* @retval false timer change the interval and start unsuccessfully.
	*/
	bool restart(TIMER_ID id, float interval_ms);

/**
	* @brief  timer is counting or not
	* @param  id	  
	* @retval true Timer is counting.
	* @retval false Timer is not counting.
	*/
	bool isActivated(TIMER_ID id);

	/** timer interrupt handle */
	static inline void _timer_irq_handle(TIMER_ID id);

private:

/**
	* @brief  timer initialize
	* @param  id
	* @param  interval
	* @retval true Timer initialize successfully.
	* @retval false Timer initialize unsuccessfully.
	*/
	bool _timer_init(TIMER_ID id, float interval);

/**
	* @brief  timer prescaler check
	* @param  id
	* @retval true Timer prescaler is valid.
	* @retval false Timer prescaler is invalid.	
	*/
	bool _timer_prescaler_valid(TIMER_ID id);

/**
	* @brief  calculate the output compare value
	* @param  id
	* @param  interval
	* @param  *count
	* @retval true Comapre value calculate successfully and assign to *count.
	* @retval false Comapre value calculate unsuccessfully.	
	*/
	bool _timer_count_calculate(TIMER_ID id, float interval, unsigned long *count);

/**
	* @brief  timer clock control
	* @param  id
	* @param  enable
	* @arg    true clock enable
	* @arg    false clock disable
	* @retval true Timer clock control successfully.
	* @retval false Timer clock control unsuccessfully.
	*/	
	bool _timer_clock_control(TIMER_ID id, bool enable);
};

#endif  //EE33_SIMPLE_TIMER_H


/*
 * @release note
 * | v1.0 2023-06-09 - 3lectron3ats : Initial Release
 * #
*/
