/**
  * @file      EE33_SimplerQueue.h
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

#include <EE33_Tic_Tac_Toe.h>
#include <EE33_SimpleQueue.h> 

SimpleQueue::SimpleQueue(uint32_t item_size, uint32_t item_number)
{
    uint32_t malloc_size;

	malloc_size = item_size * item_number;
	
	pQueue = NULL;

	if (malloc_size)
	{
	pQueue = (uint8_t *)malloc(item_size * item_number);

	if (pQueue)
	{
		ItemSize = item_size;
		ItemTotalNumber = item_number;
		QueueSize = ItemSize * ItemTotalNumber;

		flush();
	}
}
}

SimpleQueue::~SimpleQueue()
{	
	if (pQueue)
	{
		free(pQueue);
		pQueue = NULL;
	}
}

bool SimpleQueue::isFull()
{
	if (pQueue)
	{
		if (ItemNumber == ItemTotalNumber)
			return true;
	}

	return false;
}

bool SimpleQueue::isEmpty()
{
	if (pQueue)
	{
		if (ItemNumber == 0)
			return true;
	}

	return false;
}

int32_t SimpleQueue::available()
{
	if (pQueue)
	{
		return ItemNumber;
	}

	return 0;
}

bool SimpleQueue::push(void *pItem)
{	
	if (pQueue)
	{
		if (!isFull())
		{
			uint8_t *pQueue_Write;
			pQueue_Write = pQueue + ItemSize * ItemWriteIdx;
			
			memcpy((void *)pQueue_Write, pItem, ItemSize);

			ItemWriteIdx = (++ItemWriteIdx == ItemTotalNumber) ? 0 : ItemWriteIdx;

			ItemNumber++;

			return true;
		}
	}

	return false;
}


bool SimpleQueue::pop(void *pItem)
{	
	if (pQueue)
	{
		if (!isEmpty())
		{
			uint8_t *pQueue_Read;
			pQueue_Read = pQueue + ItemSize * ItemReadIdx;
			
			memcpy(pItem, (void *)pQueue_Read, ItemSize);

			ItemReadIdx = (++ItemReadIdx == ItemTotalNumber) ? 0 : ItemReadIdx;

			ItemNumber--;

			return true;
		}
	}

	return false;
}


bool SimpleQueue::peekIndex(void *pItem, int32_t index)
{
	if (pQueue)
	{
		if (index < ItemNumber)
		{
			uint8_t *pQueue_Read;		
			int32_t next_read_index;

			next_read_index = ((ItemReadIdx + index) >= ItemTotalNumber) ? (ItemReadIdx + index - ItemTotalNumber) : (ItemReadIdx + index);
			pQueue_Read = pQueue + ItemSize * next_read_index;

			memcpy(pItem, (void *)pQueue_Read, ItemSize);

			return true;
		}
	}

	return false;
}


bool SimpleQueue::peek(void *pItem)
{
	return peekIndex(pItem, 0);
}


void SimpleQueue::flush(void)
{
	ItemNumber = 0;
	ItemWriteIdx = 0;
	ItemReadIdx = 0;
}


/*
 * @release note
 * | v1.0 2023-06-09 - 3lectron3ats : Initial Release
 * #
*/
