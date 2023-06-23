/**
  * @file      EE33_Tic_Tac_Toe.h
  * @version   v1.0.0
  * @author    3lectron3ats <3lectron3ats@gmail.com>
  * @date      2022.06.09
  * @brief     General usage for Tic-Tac-Toe project.
  * @copyright GPL v3.0
  * *************************************************************************************
  * @attention
  * <h2><center>&copy; COPYRIGHT 2023 3lectron3ats </center></h2>
  * *************************************************************************************
  */

#ifndef EE33_TIC_TAC_TOE_H
#define EE33_TIC_TAC_TOE_H

#define TIC_TAC_TOE_VERSION_MAJOR   1
#define TIC_TAC_TOE_VERSION_MIDDLE  0
#define TIC_TAC_TOE_VERSION_MINOR   0


#if defined(__AVR_ATmega328P__)

#else
  #error “This library only supports boards with an ATmega328P processor.”
#endif

#define Tic_Tac_Toe_println(...)   \
    {                              \
      UCSR0B |= (1<<TXEN0);        \
      Serial.println(__VA_ARGS__); \
      Serial.flush();              \
      UCSR0B &= ~(1<<TXEN0);       \
    }


#define Tic_Tac_Toe_print(...)     \
    {                              \
      UCSR0B |= (1<<TXEN0);        \
      Serial.print(__VA_ARGS__);   \
      Serial.flush();              \
      UCSR0B &= ~(1<<TXEN0);       \
    }

typedef enum GAME_MODE
{
	  GAME_MODE_PLAYER_VS_PLAYER,
    GAME_MODE_PLAYER_VS_COMPUTER,
};

typedef enum GAME_STATUS
{
    GAME_STATUS_WIN,
    GAME_STATUS_NEXT_PLAYER,
    GAME_STATUS_TIE,
};

#endif


/*
 * @release note
 * | v1.0.0 2023-06-09 - 3lectron3ats : Initial Release
 * #
*/
