/**
  * @file      EE33_EncoderButton.cpp
  * @version   v1.0
  * @author    3lectron3ats <3lectron3ats@gmail.com>
  * @date      2022.06.09
  * @brief     Implement of rotary encoder and button to control moves of LED.
  * @copyright GPL v3.0
  * *************************************************************************************
  * @attention
  * <h2><center>&copy; COPYRIGHT 2023 3lectron3ats </center></h2>
  * *************************************************************************************
  */

#include <EE33_Tic_Tac_Toe.h>
#include <EE33_EncoderButton.h>


#define EVENT_QUEUE_LEN       4

#define ENCODER_BUTTON_DEBUG  0

Button *Button::active_button = NULL;
Encoder *Encoder::active_encoder = NULL;


Button::Button(EXTERNAL_INTERRUPT_ID hw_ext_id, unsigned int multi_click_interval) : Timer_Manager(), External_Interrupt_Manager(), event_queue(sizeof(uint8_t), EVENT_QUEUE_LEN)
{
	ext_id = hw_ext_id;
	timer_id = TIMER_ID_1;
	
	multi_click_interval_ms = multi_click_interval;

	no_debounce = false;
	
	button_click = 0;
	
	button_state = BUTTON_STATE_TOTAL;
}

Button::~Button()
{
	end();
}

bool Button::begin(void)
{
	if (active_button == NULL)
	{
		if (Timer_Manager::begin(timer_id, BUTTON_DEBOUNCE_MS, button_timer_irq))
		{
			if (External_Interrupt_Manager::begin(ext_id, FALLING, button_ext_irq))
			{
				active_button = this;
				_button_state_enter(BUTTON_STATE_IDLE);
				return true;
			}
			
			Timer_Manager::end(timer_id);
		}
	}
	
	return false;
}

bool Button::end(void)
{
	if (active_button)
	{
		Timer_Manager::end(timer_id);
		External_Interrupt_Manager::end(ext_id);
		button_state = BUTTON_STATE_TOTAL;
		active_button = NULL;

		return true;
	}
	
	return false;
}

bool Button::stopDetect(void)
{
	if (active_button)
	{
		return _button_state_enter(BUTTON_STATE_SUSPEND);
	}

	return false;
}

bool Button::startDetect(void)
{
	if (active_button)
	{
		return _button_state_enter(BUTTON_STATE_IDLE);
	}

	return false;
}

bool Button::available(void)
{
	if (event_queue.available())
	{
		return true;
	}

	return false;
}

uint8_t Button::read(void)
{
	uint8_t output = 0;
	
	event_queue.pop(&output);

	return output;
}

void Button::skipDebounce(bool is_skip)
{
	no_debounce = is_skip;
}

bool Button::pinRead(void)
{
	return _pinRead_HighLow(External_Interrupt_Manager::getPin(ext_id));
}

bool Button::_button_state_enter(BUTTON_STATE state)
{
	int reason = 0;

	if (state == BUTTON_STATE_DEBOUNCE_PROCESS && no_debounce)
	{
		reason = -1;
		goto SKIP_BUTTON_STATE;
	}	

	if (state == BUTTON_STATE_CLICK_INTERVAL_PROCESS && !multi_click_interval_ms)
	{
		reason = -2;
		goto SKIP_BUTTON_STATE;		
	}

	switch (state)
	{
		case BUTTON_STATE_IDLE:
			{
				button_click = 0;

				Timer_Manager::stop(timer_id);
				External_Interrupt_Manager::resume(ext_id);
			}
			break;

	    case BUTTON_STATE_DEBOUNCE_PROCESS:
	    	{
	    	    External_Interrupt_Manager::suspend(ext_id);
	    		Timer_Manager::restart(timer_id, (float)BUTTON_DEBOUNCE_MS);
	    	}
			break;

		case BUTTON_STATE_CLICK_INTERVAL_PROCESS:
			{
				Timer_Manager::restart(timer_id, (float)multi_click_interval_ms);
				External_Interrupt_Manager::resume(ext_id);
			}
			break;

		case BUTTON_STATE_SUSPEND:
			{
				External_Interrupt_Manager::suspend(ext_id);
				Timer_Manager::stop(timer_id);
			}
			break;
		
		default:
			{
				reason = -3;
				goto SKIP_BUTTON_STATE;
			}
			break;
	}

	button_state = state;

SKIP_BUTTON_STATE:

#if ENCODER_BUTTON_DEBUG
	Tic_Tac_Toe_print("_button_state_enter: state = ");
	Tic_Tac_Toe_print(state);
	Tic_Tac_Toe_print(" reason =  ");
	Tic_Tac_Toe_println(reason);			
#endif

	if (reason == 0)
	{
		return true;
	}

	return false;
}

bool Button::_pinRead_HighLow(byte pin)
{
	if (pin >= 0 && pin <= 13)
	{
	    return digitalRead(pin) == HIGH ? true : false;
	}

	if (pin >= A0 && pin <= A7)
	{
	    return analogRead(pin) ? true : false;
	}

	return false;
}

void Button::button_timer_irq_handle(void)
{
	Timer_Manager::stop(timer_id);
	
	if (button_state == BUTTON_STATE_DEBOUNCE_PROCESS)
	{
	  if (!_pinRead_HighLow(External_Interrupt_Manager::getPin(ext_id)))	//low active, button press
	  {
	      //valid button debounce
	      button_click++; 
	  }

	  if (!_button_state_enter(BUTTON_STATE_CLICK_INTERVAL_PROCESS))
	  {
	      event_queue.push(&button_click);
	      _button_state_enter(BUTTON_STATE_IDLE);
	  }
	}
	else if (button_state == BUTTON_STATE_CLICK_INTERVAL_PROCESS)
	{	  
	     event_queue.push(&button_click);
	     _button_state_enter(BUTTON_STATE_IDLE);	    
	}
	else
	{
#if ENCODER_BUTTON_DEBUG	
		Tic_Tac_Toe_print("!!! button_timer_irq_handle state wrong: ");
		Tic_Tac_Toe_println(button_state);
#endif		
	}
}

void Button::button_ext_irq_handle(void)
{
	External_Interrupt_Manager::suspend(ext_id);
	
	if (BUTTON_STATE_IDLE == button_state || 
		BUTTON_STATE_CLICK_INTERVAL_PROCESS == button_state)
	{
	    if (!_button_state_enter(BUTTON_STATE_DEBOUNCE_PROCESS))
    	{
    	    button_click++;
			
	   		if (!_button_state_enter(BUTTON_STATE_CLICK_INTERVAL_PROCESS))
   			{
			    event_queue.push(&button_click);
			    _button_state_enter(BUTTON_STATE_IDLE);
   			}
    	}
	}
	else
	{
#if ENCODER_BUTTON_DEBUG	
		Tic_Tac_Toe_print("!!! button_ext_irq_handle state wrong: ");
		Tic_Tac_Toe_println(button_state);
#endif		
	}
}


void Button::button_timer_irq(void)
{
	if (active_button)
	{
		active_button-> button_timer_irq_handle();
	}
}

void Button::button_ext_irq(void)
{
	if (active_button)
	{
		active_button-> button_ext_irq_handle();
	}
}

Encoder::Encoder(EXTERNAL_INTERRUPT_ID hw_ext_id, byte b_pin) : Timer_Manager(), External_Interrupt_Manager(), event_queue(sizeof(ENCODER_RESULT), EVENT_QUEUE_LEN)
{
	ext_id = hw_ext_id;
	
	phase_a_pin = External_Interrupt_Manager::getPin(hw_ext_id);
	phase_b_pin = b_pin;

	timer_id = TIMER_ID_2;
	phase_dectect_ms = ENCODER_PHASE_MS;
	
	encoder_result = ENCODER_NONE;
	encoder_state = ENCODER_STATE_TOTAL;
}

Encoder::~Encoder()
{
	end();
}

bool Encoder::begin(void)
{
	if (active_encoder == NULL)
	{
		if (Timer_Manager::begin(timer_id, ENCODER_PHASE_MS, _encoder_timer_irq))
		{
			if (External_Interrupt_Manager::begin(ext_id, FALLING, _encoder_ext_irq))
			{
				active_encoder = this;
				_encoder_state_enter(ENCODER_STATE_IDLE);
				return true;
			}
			
			Timer_Manager::end(timer_id);
		}
	}
	
	return false;
}

bool Encoder::end(void)
{
	if (active_encoder)
	{
		Timer_Manager::end(timer_id);
		External_Interrupt_Manager::end(ext_id);
		encoder_result = ENCODER_NONE;
		encoder_state = ENCODER_STATE_TOTAL;
		active_encoder = NULL;

		return true;
	}
	
	return false;

}

bool Encoder::stopDetect(void)
{
	if (active_encoder)
	{
		return _encoder_state_enter(ENCODER_STATE_SUSPEND);
	}

	return false;
}

bool Encoder::startDetect(void)
{
	if (active_encoder)
	{
		return _encoder_state_enter(ENCODER_STATE_IDLE);
	}

	return false;
}

bool Encoder::available(void)
{
	if (event_queue.available())
	{
		return true;
	}

	return false;
}

ENCODER_RESULT Encoder::read(void)
{
	ENCODER_RESULT value;

	event_queue.pop(&value);

	return value;
}

bool Encoder::_encoder_state_enter(ENCODER_STATE state)
{
	int reason = 0;
	
	switch (state)
	{
		case ENCODER_STATE_IDLE:
			{
				encoder_result = ENCODER_NONE;

				Timer_Manager::stop(timer_id);
				External_Interrupt_Manager::resume(ext_id);
			}
			break;

		case ENCODER_STATE_PHASE_DETECT:
			{
				External_Interrupt_Manager::suspend(ext_id);
				Timer_Manager::start(timer_id);
			}
			break;

		case ENCODER_STATE_SUSPEND:
			{
				Timer_Manager::stop(timer_id);
				External_Interrupt_Manager::suspend(ext_id);
			}
			break;
		
		default:
			{
				reason = -1;
				goto SKIP_ENCODER_STATE;
			}
			break;
	}

	encoder_state = state;

SKIP_ENCODER_STATE:
	
#if ENCODER_BUTTON_DEBUG
	Tic_Tac_Toe_print("_encoder_state_enter: state = ");
	Tic_Tac_Toe_print(state);
	Tic_Tac_Toe_print(" reason =  ");
	Tic_Tac_Toe_println(reason);	
#endif
	
	if (reason == 0)
	{
		return true;
	}

	return false;
}


bool Encoder::_pinRead_HighLow(byte pin)
{
	if (pin >= 0 && pin <= 13)
	{
	    return digitalRead(pin) == HIGH ? true : false;
	}

	if (pin >= A0 && pin <= A7)
	{
	    return analogRead(pin) ? true : false;
	}

	return false;
}

void Encoder::_encoder_timer_irq_handle(void)
{
	Timer_Manager::stop(timer_id);

	if (encoder_state == ENCODER_STATE_PHASE_DETECT)
	{
    	if (_pinRead_HighLow(phase_b_pin))
    	{
      		encoder_result = ENCODER_COUNTERCLOCKWISE;
    	}
    	else
    	{
      		encoder_result = ENCODER_CLOCKWISE;
    	}
		
		event_queue.push(&encoder_result);
		_encoder_state_enter(ENCODER_STATE_IDLE);
	}
	else
	{
#if ENCODER_BUTTON_DEBUG	
		Tic_Tac_Toe_print("!!! _encoder_timer_irq_handle state wrong: ");
		Tic_Tac_Toe_println(encoder_state);
#endif	
	}
}

void Encoder::_encoder_ext_irq_handle(void)
{
	External_Interrupt_Manager::suspend(ext_id);

	if (encoder_state == ENCODER_STATE_IDLE)
	{
		_encoder_state_enter(ENCODER_STATE_PHASE_DETECT);
	}
	else
	{
#if ENCODER_BUTTON_DEBUG	
		Tic_Tac_Toe_print("!!! _encoder_ext_irq_handle state wrong: ");
		Tic_Tac_Toe_println(encoder_state);
#endif			
	}
}

static void Encoder::_encoder_timer_irq(void)
{
	if (active_encoder)
	{
		active_encoder->_encoder_timer_irq_handle();
	}
}

static void Encoder::_encoder_ext_irq(void)
{
	if (active_encoder)
	{
		active_encoder->_encoder_ext_irq_handle();
	}
}


/*
 * @release note
 * | v1.0 2023-06-09 - 3lectron3ats : Initial Release
 * #
*/
