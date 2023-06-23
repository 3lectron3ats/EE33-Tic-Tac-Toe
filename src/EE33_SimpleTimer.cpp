/**
  * @file      EE33_SimplerTimer.cpp
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

#include <EE33_Tic_Tac_Toe.h>
#include <EE33_SimpleTimer.h> 

#define TIMER0_RESERVED_FOR_MILLS  1
#define SIMPLE_TIMER_DEBUG  0

#define CLOCK                16000000
#define TIMER_PRESCALER(ID)  timer_resource[ID].timer_prescaler   
#define TICK_MS(ID)          ((1/(float)CLOCK) * (float)TIMER_PRESCALER(ID) * 1000.0f)
#define COUNT_MAX(ID)        bit(timer_resource[ID].timer_depth)

static T_TIMER timer_resource[TIMER_ID_TOTAL] = 
{
  [TIMER_ID_0] = {TIMER_DEPTH_8,  TIMER_PRESCALER_256,  false, false, NULL},    
  [TIMER_ID_1] = {TIMER_DEPTH_16, TIMER_PRESCALER_1024, false, false, NULL},    
  [TIMER_ID_2] = {TIMER_DEPTH_8,  TIMER_PRESCALER_256,  false, false, NULL},
};

Timer_Manager::Timer_Manager()
{

}

Timer_Manager::~Timer_Manager()
{	
	
}

bool Timer_Manager::begin(TIMER_ID id, float interval_ms, void(* callFunc)(void))
{
	if (id < TIMER_ID_TOTAL && !timer_resource[id].is_used)
	{
	
#if (TIMER0_RESERVED_FOR_MILLS == 1)
		if (id == TIMER_ID_0)
		{
			return false;
		}
#endif
		if (_timer_init(id, interval_ms))
		{
			timer_resource[id].is_used = true;
			timer_resource[id].callFunc = callFunc;
			return true;
		}
	}

	return false;
}

bool Timer_Manager::end(TIMER_ID id)
{
	if (id < TIMER_ID_TOTAL && timer_resource[id].is_used)
	{
		if (stop(id))
		{
			timer_resource[id].is_used = false;
			timer_resource[id].callFunc = NULL;
			return true;
		}		
	}

	return false;
}

bool Timer_Manager::_timer_prescaler_valid(TIMER_ID id)	
{
	switch (TIMER_PRESCALER(id))
	{
	case TIMER_PRESCALER_1:
		return true;
	case TIMER_PRESCALER_8:
		return true;
	case TIMER_PRESCALER_32:
	case TIMER_PRESCALER_128:
		{
			if (TIMER_ID_2 == id)
				return true;
		}
	    break;
	case TIMER_PRESCALER_64:
		return true;
	case TIMER_PRESCALER_256:
		return true;
	case TIMER_PRESCALER_1024:
		return true;
	}

#if	SIMPLE_TIMER_DEBUG
	Tic_Tac_Toe_println("_timer_prescaler_valid fail");
#endif
	return false;
}

bool Timer_Manager::_timer_count_calculate(TIMER_ID id, float interval, unsigned long *count)
{
	unsigned long result;

	if (interval > 0.0)
	{
		result = (unsigned long)(interval / TICK_MS(id));
		result = (result > COUNT_MAX(id) - 1) ? (result <= COUNT_MAX(id) - 1) : result;

		if (result)
		{
			*count = result;
			return true;
		}
	}
	
	return false;
}

bool Timer_Manager::_timer_clock_control(TIMER_ID id, bool enable)
{
	switch (id)
	{
		case TIMER_ID_0:
		{
			switch (TIMER_PRESCALER(id))
			{
				case TIMER_PRESCALER_1:
					if (enable)
						TCCR0B |= _BV(CS00);
					else
						TCCR0B &= ~_BV(CS00);
					break;
			
				case TIMER_PRESCALER_8:
					if (enable)
						TCCR0B |= _BV(CS01);
					else
						TCCR0B &= ~_BV(CS01);
					break;
				
				case TIMER_PRESCALER_64:
					if (enable)
						TCCR0B |= _BV(CS01) | _BV(CS00);
					else
						TCCR0B &= ~_BV(CS01) & ~_BV(CS00);
					break;
					
				case TIMER_PRESCALER_256:
					if (enable)
						TCCR0B |= _BV(CS02);
					else
						TCCR0B &= ~_BV(CS02);
					break;
			
				case TIMER_PRESCALER_1024:
					if (enable)
						TCCR0B |= _BV(CS02) | _BV(CS00);
					else
						TCCR0B &= ~_BV(CS02) & ~_BV(CS00);
					break;
					
				default:
					return false;
			}
		}
		break;

		case TIMER_ID_1:
		{
			switch (TIMER_PRESCALER(id))
			{
				case TIMER_PRESCALER_1:
					if (enable)
						TCCR1B |= _BV(CS10);
					else
						TCCR1B &= ~_BV(CS10);
					break;
			
				case TIMER_PRESCALER_8:
					if (enable)				
						TCCR1B |= _BV(CS11);
					else
						TCCR1B &= ~_BV(CS11);
					break;
				
				case TIMER_PRESCALER_64:
					if (enable)				
						TCCR1B |= _BV(CS11) | _BV(CS10);
					else
						TCCR1B &= ~_BV(CS11) & ~_BV(CS10);
					break;
					
				case TIMER_PRESCALER_256:
					if (enable)				
						TCCR1B |= _BV(CS12);
					else
						TCCR1B &= ~_BV(CS12);
					break;
			
				case TIMER_PRESCALER_1024:
					if (enable)				
						TCCR1B |= _BV(CS12) | _BV(CS10);
					else
						TCCR1B &= ~_BV(CS12) & ~_BV(CS10);
					break;
					
				default:
					return false;
			}
		}
		break;
		
		case TIMER_ID_2:
		{
			switch (TIMER_PRESCALER(id))
			{
				case TIMER_PRESCALER_1:
					if (enable)
						TCCR2B |= _BV(CS20);
					else
						TCCR2B &= ~_BV(CS20);
					break;
			
				case TIMER_PRESCALER_8:
					if (enable)				
						TCCR2B |= _BV(CS21);
					else
						TCCR2B &= ~_BV(CS21);
					break;
				
				case TIMER_PRESCALER_32:
					if (enable)				
						TCCR2B |= _BV(CS21) | _BV(CS20);
					else
						TCCR2B &= ~_BV(CS21) & ~_BV(CS20);
					break;	
				
				case TIMER_PRESCALER_64:
					if (enable)
						TCCR2B |= _BV(CS20);
					else
						TCCR2B &= ~_BV(CS20);
					break;

				case TIMER_PRESCALER_128:
					if (enable)				
						TCCR2B |= _BV(CS22) | _BV(CS20);
					else
						TCCR2B &= ~_BV(CS22) & ~_BV(CS20);
					
					break;
				case TIMER_PRESCALER_256:
					if (enable)				
						TCCR2B |= _BV(CS22) | _BV(CS21);
					else
						TCCR2B &= ~_BV(CS22) & ~_BV(CS21);
					break;
			
				case TIMER_PRESCALER_1024:
					if (enable)				
						TCCR2B |= _BV(CS22) | _BV(CS21) | _BV(CS20);
					else
						TCCR2B &= ~_BV(CS22) & ~_BV(CS21) & ~_BV(CS20);
					break;
					
				default:
					return false;
			}
		}

		default:
			return false;
	}

	return true;
}

bool Timer_Manager::_timer_init(TIMER_ID id, float interval)
{
	int reason = 0;
	unsigned long count_reg;
		
	if (!_timer_prescaler_valid(id))
	{
		reason = -1;
		goto fail;
	}

    if (!_timer_count_calculate(id, interval, &count_reg))
	{
		reason = -2;
		goto fail;
	}

	if (TIMER_ID_0 == id)
	{
		TCCR0A = 0;
		TCCR0B = 0;  
		TCNT0 = 0;

		TCCR0A |= _BV(WGM01);    //CTC mode

		OCR0A = count_reg;
	}

	if (TIMER_ID_1 == id)
	{
		TCCR1A = 0;

		TCCR1B = 0;
		TCNT1 = 0;
		TCCR1B |= _BV(WGM12);    //CTC mode

	    OCR1A = count_reg;
	}

	if (TIMER_ID_2 == id)
	{
		TCCR2A = 0;
		TCCR2B = 0;  
		TCNT2 = 0;

		TCCR2A |= _BV(WGM21);    //CTC mode

		OCR2A = count_reg;
	}

	return true;

fail:
#if SIMPLE_TIMER_DEBUG	
	Tic_Tac_Toe_print("_timer_init fail = ");
	Tic_Tac_Toe_println(reason);
#endif	
    return false;

}

bool Timer_Manager::start(TIMER_ID id)
{
	if (id < TIMER_ID_TOTAL && timer_resource[id].is_used)
	{
		if (TIMER_ID_0 == id)
		{
			TCNT0 = 0;
			
			/*	
			*   Important, or MCU will jump to TIMER1_COMPA vector immediately.
			*	OCF1A is automatically cleared when interrupt, or writing logic one to this bit  
			*/	
			TIFR0 |= (1 << OCF0A);    //clear timer0 flag
			TIMSK0 |= (1 << OCIE0A);    //enable timer0 interrupt		
		}

		if (TIMER_ID_1 == id)
		{
			TCNT1 = 0;
			
			/*	
			*   Important, or MCU will jump to TIMER1_COMPA vector immediately.
			*	OCF1A is automatically cleared when interrupt, or writing logic one to this bit  
			*/	
			TIFR1 |= (1 << OCF1A);    //clear timer1 flag
			TIMSK1 |= (1 << OCIE1A);    //enable timer1 interrupt		
		}

		if (TIMER_ID_2 == id)
		{
			TCNT2 = 0;
			
			/*	
			*   Important, or MCU will jump to TIMER2_COMPA vector immediately.
			*	OCF2A is automatically cleared when interrupt, or writing logic one to this bit  
			*/
			TIFR2 |= (1 << OCF2A);    //clear timer2 flag
			TIMSK2 |= (1 << OCIE2A);    //enable timer2 interrupt	
		}

		timer_resource[id].is_activated = true;

		_timer_clock_control(id, true);

		return true;
	}

	return false;
}

bool Timer_Manager::stop(TIMER_ID id)
{
	if (id < TIMER_ID_TOTAL && timer_resource[id].is_used)
	{
		if (TIMER_ID_0 == id)
		{
			TIMSK0 &= ~(1 << OCIE0A);    //disable timer0 interrupt		
		}

		if (TIMER_ID_1 == id)
		{
			TIMSK1 &= ~(1 << OCIE1A);    //disable timer1 interrupt		
		}

		if (TIMER_ID_2 == id)
		{
			TIMSK2 &= ~(1 << OCIE2A);	 //disable timer2 interrupt
		}

		timer_resource[id].is_activated = false;
		_timer_clock_control(id, false);

		return true;
	}

	return false;
}

bool Timer_Manager::restart(TIMER_ID id, float interval_ms)
{
	if (id < TIMER_ID_TOTAL && timer_resource[id].is_used)
	{
	    unsigned long count_reg;
		
	    if (_timer_count_calculate(id, interval_ms, &count_reg))
		{
			if (timer_resource[id].is_activated)
			{
				stop(id);
			}

			if (TIMER_ID_0 == id)
			{
				OCR0A = count_reg;
			}

			if (TIMER_ID_1 == id)
			{
			    OCR1A = count_reg;
			}

			if (TIMER_ID_2 == id)
			{
				OCR2A = count_reg;
			}	

			start(id);

			return true;
		}
	}

	return false;
}

bool Timer_Manager::isActivated(TIMER_ID id)
{
	if (id < TIMER_ID_TOTAL && timer_resource[id].is_used)
	{
		if (timer_resource[id].is_activated)
		{
			return true;
		}
	}

	return false;
}

static inline void Timer_Manager::_timer_irq_handle(TIMER_ID id)
{
	if (id < TIMER_ID_TOTAL && timer_resource[id].is_used)
	{
		if (timer_resource[id].callFunc)
		{
			timer_resource[id].callFunc();
		}
	}
}

ISR(TIMER0_COMPA_vect)
{
    Timer_Manager::_timer_irq_handle(TIMER_ID_0);
}

ISR(TIMER1_COMPA_vect)
{
    Timer_Manager::_timer_irq_handle(TIMER_ID_1);
}

ISR(TIMER2_COMPA_vect)
{
	Timer_Manager::_timer_irq_handle(TIMER_ID_2);
}


/*
 * @release note
 * | v1.0 2023-06-09 - 3lectron3ats : Initial Release
 * #
*/
