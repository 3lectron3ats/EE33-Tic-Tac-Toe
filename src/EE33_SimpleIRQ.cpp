/**
  * @file      EE33_SimpleIRQ.cpp
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

#include <EE33_Tic_Tac_Toe.h>
#include <EE33_SimpleIRQ.h>

T_EXTERNAL_INTERRUPT ext_irq[EXTERNAL_INTERRUPT_TOTAL] = 
{
	[EXTERNAL_INTERRUPT_D2] = {2,  false, false},
	[EXTERNAL_INTERRUPT_D3] = {3,  false, false},
};


External_Interrupt_Manager::External_Interrupt_Manager(void)
{

}

External_Interrupt_Manager::~External_Interrupt_Manager(void)
{

}

bool External_Interrupt_Manager::begin(EXTERNAL_INTERRUPT_ID id, int mode, void (*callFunc)(void))
{
	if (id < EXTERNAL_INTERRUPT_TOTAL && !ext_irq[id].is_used)
	{
	    ext_irq[id].is_used = true;
		
		pinMode(ext_irq[id].pin, INPUT);
		attachInterrupt(digitalPinToInterrupt(ext_irq[id].pin), callFunc, mode);

		resume(id);
		return true;
	}

	return false;	
}

bool External_Interrupt_Manager::end(EXTERNAL_INTERRUPT_ID id)
{
	if (id < EXTERNAL_INTERRUPT_TOTAL && ext_irq[id].is_used)
	{
		suspend(id);
		
		detachInterrupt(digitalPinToInterrupt(ext_irq[id].pin));

		ext_irq[id].is_used = false;
		return true;
	}

	return false;
}

bool External_Interrupt_Manager::resume(EXTERNAL_INTERRUPT_ID id)
{
	if (id < EXTERNAL_INTERRUPT_TOTAL && ext_irq[id].is_used)
	{
		if (!ext_irq[id].is_activated)
		{
			if (EXTERNAL_INTERRUPT_D2 == id)
			{
				/*	Important, or MCU will jump to INT0 vector immediately.
				*	the flag can be cleared by writing a logical one to it.   
				*/	 

				EIFR  |= _BV(INTF0); 	  
				EIMSK |= _BV(INT0);				
			}

			if (EXTERNAL_INTERRUPT_D3 == id)
			{
				/*	Important, or MCU will jump to INT1 vector immediately.
				*	the flag can be cleared by writing a logical one to it.   
				*/	 
	
				EIFR  |= _BV(INTF1); 	  
				EIMSK |= _BV(INT1);				
			}

			ext_irq[id].is_activated = true;
		}

		return true;
	}

	return false;
}

bool External_Interrupt_Manager::suspend(EXTERNAL_INTERRUPT_ID id)
{
	if (id < EXTERNAL_INTERRUPT_TOTAL && ext_irq[id].is_used)
	{
		if (ext_irq[id].is_activated)
		{    
			if (EXTERNAL_INTERRUPT_D2 == id)
			{
				EIMSK &= ~_BV(INT0);			//disable int0 interrupt
			}

			if (EXTERNAL_INTERRUPT_D3 == id)
			{
				EIMSK &= ~_BV(INT1);			//disable int1 interrupt
			}

			ext_irq[id].is_activated = false;
		}

		return true;
	}

	return false;
}

bool External_Interrupt_Manager::isActivated(EXTERNAL_INTERRUPT_ID id)
{
	if (id < EXTERNAL_INTERRUPT_TOTAL)
	{
		if (ext_irq[id].is_used)
		{
			if (ext_irq[id].is_activated)
			{
				return true;
			}
		}
	}

	return false;
}

byte External_Interrupt_Manager::getPin(EXTERNAL_INTERRUPT_ID id)
{
	if (id < EXTERNAL_INTERRUPT_TOTAL)
	{
		return ext_irq[id].pin;
	}
}

/*
 * @release note
 * | v1.0 2023-06-09 - 3lectron3ats : Initial Release
 * #
*/
