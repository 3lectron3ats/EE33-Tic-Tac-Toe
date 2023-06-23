/**
  * @file      EE33_SimplerQueue.cpp
  * @version   v1.0
  * @author    3lectron3ats <3lectron3ats@gmail.com>
  * @date      2022.06.09
  * @brief     Implement of FIFO queue.
  * @copyright GPL v3.0
  * *************************************************************************************
  * @attention
  * <h2><center>&copy; COPYRIGHT 2023 3lectron3ats </center></h2>
  * *************************************************************************************
  */


#ifndef EE33_SIMPLE_QUEUE_H
#define EE33_SIMPLE_QUEUE_H

// Arduino versioning.
#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"	// for digitalRead, digitalWrite, etc
#else
#include "WProgram.h"
#endif

#define SIMPLE_QUEUE_VERSION_MAJOR  1
#define SIMPLE_QUEUE_VERSION_MINOR  0

class SimpleQueue {

public:

/**
	* @brief  queue construction
	* @param  item_size each item size
	* @param  item_number how many item		
	*/
	SimpleQueue(uint32_t item_size, uint32_t item_number);

/**
	* @brief  queue destruction
	*/
	~SimpleQueue();

/**
	* @brief  queue is full or not
	* @retval true queue is full, push will fail
	* @retval false queue is not full			
	*/
	bool isFull(void);

/**
	* @brief  queue is empty or not
	* @retval true queue is empty, pop will fail
	* @retval false queue is not empty			
	*/
	bool isEmpty(void);

/**
	* @brief  number of item existed in queue
	* @return press action count		
	*/
	int32_t available(void);

/**
	* @brief  push item into queue
	* @param  *pItem point of item
	* @retval true push success
	* @retval false push fail
	*/
	bool push(void *pItem);

/**
	* @brief  pop item from queue
	* @param  *pItem point of item
	* @retval true pop success
	* @retval false pop fail
	*/
	bool pop(void *pItem);

/**
	* @brief  peek the index 0 item in queue, but no pop
	* @param  *pItem point of item
	* @retval true peek success
	* @retval false peek fail
	*/
    bool peek(void *pItem);

/**	
	* @brief  peek the specified index item in queue, but not pop
	* @param  *pItem point of item
	* @param  index index of item	
	* @retval true peek success
	* @retval false peek fail
	*/
    bool peekIndex(void *pItem, int32_t index);

/**
	* @brief  reset the queue
	*/		
	void flush();

private:

	/** queue start address */
	uint8_t *pQueue;

    /** queue total size (ItemSize * ItemTotalNumber) */
	int32_t QueueSize;

    /** item size */
	int32_t ItemSize;

	/** item total number */
	int32_t ItemTotalNumber;

    /** item number in queue*/
	int32_t ItemNumber;

    /** index to be pushed of queue*/
	int32_t ItemWriteIdx;

    /** index to be popped of queue*/	
	int32_t ItemReadIdx;
};

#endif  //EE33_SIMPLE_QUEUE_H


/*
 * @release note
 * | v1.0 2023-06-09 - 3lectron3ats : Initial Release
 * #
*/
