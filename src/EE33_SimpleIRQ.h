/**
  * @file      EE33_SimpleIRQ.h
  * @version   v1.0
  * @author    3lectron3ats <3lectron3ats@gmail.com>
  * @date      2022.06.09
  * @brief     Implement of external interrupt.
  * @copyright GPL v3.0
  * *************************************************************************************
  * @attention
  * <h2><center>&copy; COPYRIGHT 2023 3lectron3ats </center></h2>
  * *************************************************************************************
  */


#ifndef EE33_SIMPLE_IRQ_H
#define EE33_SIMPLE_IRQ_H

// Arduino versioning.
#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"	// for digitalRead, digitalWrite, etc
#else
#include "WProgram.h"
#endif

#define SIMPLE_IRQ_VERSION_MAJOR  1
#define SIMPLE_IRQ_VERSION_MINOR  0

typedef enum
{
	EXTERNAL_INTERRUPT_D2 = 0,  /*!< PD2 */
	EXTERNAL_INTERRUPT_D3,      /*!< PD3 */

	EXTERNAL_INTERRUPT_TOTAL,
} EXTERNAL_INTERRUPT_ID;

typedef struct
{
  byte      pin;           /*!< pin number */  
  bool      is_used;       /*!< allocated or not */
  bool      is_activated;  /*!< enabled or not */
} T_EXTERNAL_INTERRUPT;

class External_Interrupt_Manager
{
public:

/**
	* @brief  construction
	*/
	External_Interrupt_Manager();

/**
	* @brief  destruction
	*/
	~External_Interrupt_Manager();	

/**
	* @brief  register external interrupts
	* @param  id
    * @arg EXTERNAL_INTERRUPT_D2: INT0
    * @arg EXTERNAL_INTERRUPT_D3: INT1	
	* @param  mode
    * @arg LOW
    * @arg CHANGE
    * @arg RISING
    * @arg FALLING    
	* @param  *callFunc	          
	* @retval true external interrupt register successfully.
	* @retval false external interrupt register unsuccessfully.	
	*/	
	bool begin(EXTERNAL_INTERRUPT_ID id, int mode, void (*callFunc)(void));

/**
	* @brief  unregister external interrupts
	* @param  id
    * @arg EXTERNAL_INTERRUPT_D2: INT0
    * @arg EXTERNAL_INTERRUPT_D3: INT1		  
	* @retval true external interrupt unregister successfully.
	* @retval false external interrupt unregister unsuccessfully.
	*/	
	bool end(EXTERNAL_INTERRUPT_ID id);	

/**
	* @brief  enable the specific external interrupt
	* @param  id	  
	* @retval true Enable external interrupt successfully.
	* @retval false Enable external interrupt unsuccessfully.	
	*/
	bool resume(EXTERNAL_INTERRUPT_ID id);

/**
	* @brief  disable the specific external interrupt
	* @param  id   
	* @retval true Disable external interrupt successfully.
	* @retval false Disable external interrupt unsuccessfully.
	*/
	bool suspend(EXTERNAL_INTERRUPT_ID id);

/**
	* @brief  external interrupt is enable or not
	* @param  id  
	* @return true if external interrupt is enable, otherwise reuturn false
	* @retval true External interrupt is enable.
	* @retval false External interrupt is disable.	
	*/
	bool isActivated(EXTERNAL_INTERRUPT_ID id);

/**
	* @brief  get the pin number of specific external interrupt
	* @param  id   
	* @return byte pin number
	*/	
	byte getPin(EXTERNAL_INTERRUPT_ID id);
};

#endif  //EE33_SIMPLE_IRQ_H


/*
 * @release note
 * | v1.0 2023-06-09 - 3lectron3ats : Initial Release
 * #
*/
