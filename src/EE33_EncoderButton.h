/**
  * @file      EE33_EncoderButton.h
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


#ifndef EE33_ENCODER_BUTTON_H
#define EE33_ENCODER_BUTTON_H

// Arduino versioning.
#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"	// for digitalRead, digitalWrite, etc
#else
#include "WProgram.h"
#endif

#include <EE33_SimpleTimer.h>
#include <EE33_SimpleIRQ.h>
#include <EE33_SimpleQueue.h>

#define ENCODER_BUTTON_VERSION_MAJOR  1
#define ENCODER_BUTTON_VERSION_MINOR  0

#define BUTTON_DEBOUNCE_MS        30
#define BUTTON_CLICK_INTERVAL_MS  400
#define ENCODER_PHASE_MS          1.5  

/*! encoder action */
typedef enum
{
	ENCODER_NONE,             /*!< encoder no action */
	ENCODER_CLOCKWISE,        /*!< encoder clockwise rotation */
	ENCODER_COUNTERCLOCKWISE, /*!< encoder counterclockwise rotation */
} ENCODER_RESULT;

class Button: virtual public Timer_Manager, virtual public External_Interrupt_Manager
{
public:

	/*! state of buttone state machine */
	typedef enum
	{
		BUTTON_STATE_IDLE,                   /*!< button ready to detect */                   
		BUTTON_STATE_DEBOUNCE_PROCESS,       /*!< button is debouncing */ 
		BUTTON_STATE_CLICK_INTERVAL_PROCESS, /*!< button multi-click processing */
		BUTTON_STATE_SUSPEND,                /*!< button is not detecting */       
	
		BUTTON_STATE_TOTAL,                  /*!< button is not create or released */
	} BUTTON_STATE;

/**
	* @brief  construction
	* @param  hw_ext_id
    *   Board Nano(Uno, Mini, other 328-based) external interrupts pin supported.
    *    @arg EXTERNAL_INTERRUPT_D2: INT0
    *    @arg EXTERNAL_INTERRUPT_D3: INT1    
	* @param  multi_click_interval	
	*/
	Button(EXTERNAL_INTERRUPT_ID hw_ext_id, unsigned int multi_click_interval);

/**
	* @brief  destruction
	*/
    ~Button();

/**
  	* @brief  button create			  
	* @retval true Button create successfully.
	* @retval false Button create unsuccessfully.
	*/	
	bool begin(void);

/**
	* @brief  button release 		  
	* @retval true Button release successfully.
	* @retval false Button release unsuccessfully.
	*/
	bool end(void);

/**
	* @brief  button start to detect the press action 		  
	* @retval true Button start to detect successfully.
	* @retval false Button start to detect unsuccessfully.	
	*/
	bool startDetect(void);

/**
	* @brief  button stop to detect the press action 	  
	* @retval true Button stop to detect successfully.
	* @retval false Button stop to detect unsuccessfully.
	*/
	bool stopDetect(void);

/**
	* @brief  action of button be triggered 	  
	* @retval true There is button action existed, get the action by \a read function.
	* @retval false There is no button action existed.
	* @see read()
	*/	
	bool available(void);

/**
	* @brief  read the count of press action 	  
	* @return press action count
	* @see available()
	*/	
	uint8_t read(void);	

/**
	* @brief  enable debounce or not 	  
	* @param  is_skip
    *    @arg true: disable debounce
    *    @arg flase: enable debounce
	*/
	void skipDebounce(bool is_skip);

/**
	* @brief  read the status of button pin 	  
	* @retval Ture button pin High level
	* @retval False button pin Low level
	*/
	bool pinRead(void);

private:

	/** pointer of button which is active */
	static Button *active_button;

    /**	HW external interrupt ID be allocated by button */	
	EXTERNAL_INTERRUPT_ID ext_id;

	/**	HW timer ID be allocated by button */
	TIMER_ID timer_id;

	/** multi-click detect interval by user defined */
	unsigned int multi_click_interval_ms;

    /** button debounce execute or not */
	bool no_debounce;
	
	/** button state */
	BUTTON_STATE button_state;

	/** button click count */
    uint8_t button_click;

	/** queue for storage of button event */
	SimpleQueue event_queue;

/**
	* @brief  entry point of button state machine
	* @param  state	
	* @retval true Switching the state is successful.
	* @retval false Switching the state is unsuccessful.
	*/	
	bool _button_state_enter(BUTTON_STATE state);

/**
	* @brief  read the status of pin 
	* @param  pin
	* @retval HIGH The pin is HIGH level.
	* @retval LOW The pin is LOW level.	
	*/	
    bool _pinRead_HighLow(byte pin);

/**
	* @brief timer callback function that button object registered   
	*/
	static void button_timer_irq(void);

/**
	* @brief handle button timer callback function   
	*/
	void button_timer_irq_handle(void);

/**
	* @brief external interrupt callback function that button object registered	 
	*/
	static void button_ext_irq(void);

/**
	* @brief handle button external interrupt callback function   
	*/
	void button_ext_irq_handle(void);

};

class Encoder: virtual public Timer_Manager, virtual public External_Interrupt_Manager
{
public:

	/*! state of encoder state machine */
	typedef enum
	{
		ENCODER_STATE_IDLE,         /*!< encoder ready to detect */  
		ENCODER_STATE_PHASE_DETECT, /*!< encoder is judging */ 
		ENCODER_STATE_SUSPEND,      /*!< encoder is not detecting */

		ENCODER_STATE_TOTAL,        /*!< encoder is not create or released */
	} ENCODER_STATE;

/**
   * @brief  construction
   * @param  hw_ext_id
   *   Board Nano(Uno, Mini, other 328-based) external interrupts pin supported.
   *    @arg EXTERNAL_INTERRUPT_D2: INT0
   *    @arg EXTERNAL_INTERRUPT_D3: INT1   
   * @param  b_pin  
   */
   Encoder(EXTERNAL_INTERRUPT_ID hw_ext_id, byte b_pin);

/**
	* @brief  destruction 
  */
   ~Encoder();

/**
   * @brief  encoder create			 
   * @retval true Encoder create successfully.
   * @retval false Encoder create unsuccessfully.
   */
	bool begin(void);

/**
	* @brief  encoder release		  
	* @retval true Encoder release successfully.
	* @retval false Encoder release unsuccessfully.
	*/
	bool end(void);

/**
	* @brief  encoder start to detect the clockwise/counterclockwise action  		  
	* @retval true Encoder start to detect successfully.
	* @retval false Encoder start to detect unsuccessfully.	
	*/	
	bool startDetect(void);

/**
	* @brief  encoder stop to detect the clockwise/counterclockwise action  		  
	* @retval true Encoder stop to detect successfully.
	* @retval false Encoder stop to detect unsuccessfully.	
	*/	
	bool stopDetect(void);

/**
	* @brief  action of encoder be triggered 	  
	* @retval true There is encoder action existed, get the action by \a read function.
	* @retval false There is no encoder action existed.
	* @see read()
	*/
	bool available(void);

/**
	* @brief  read the encoder action 	  
	* @return encoder action
	* @arg ENCODER_CLOCKWISE
	* @arg ENCODER_COUNTERCLOCKWISE
	* @see available()
	*/		
	ENCODER_RESULT read(void);	

private:

	/** pointer of encoder which is active */
	static Encoder *active_encoder;

	/** HW external interrupt ID be allocated by encoder */
	EXTERNAL_INTERRUPT_ID ext_id;

	/**	HW timer ID be allocated by encoder */
	TIMER_ID timer_id;

	/** encoder phase A pin number */
	byte phase_a_pin;

	/** encoder phase B pin number */
	byte phase_b_pin;

	/** encoder phase A and phase B detect interval */
	unsigned int phase_dectect_ms;

	/** encoder state */
	ENCODER_STATE encoder_state;

	/** encoder action */
	ENCODER_RESULT encoder_result;

	/** queue for storage of encoder event */
	SimpleQueue event_queue;

/**
	* @brief  entry point of encoder state machine
	* @param  state
	* @retval true Switching the state is successful.
	* @retval false Switching the state is unsuccessful.
	*/
	bool _encoder_state_enter(ENCODER_STATE state);

/**
	* @brief  read the status of pin 
	* @param  pin
	* @retval HIGH The pin is HIGH level.
	* @retval LOW The pin is LOW level. 
	*/	
	bool _pinRead_HighLow(byte pin);

/**
	* @brief timer callback function that encoder object registered	 
	*/
	static void _encoder_timer_irq(void);

/**
	* @brief handle encoder timer callback function	 
	*/
	void _encoder_timer_irq_handle(void);

/**
	* @brief external interrupt callback function that encoder object registered  
	*/
	static void _encoder_ext_irq(void);

/**
	* @brief handle encoder external interrupt callback function   
	*/
	void _encoder_ext_irq_handle(void);
};

#endif  //EE33_ENCODER_BUTTON_H

/*
 * @release note
 * | v1.0 2023-06-09 - 3lectron3ats : Initial Release
 * #
*/
