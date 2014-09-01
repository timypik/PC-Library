/**
  ******************************************************************************
  * @file    FT2232.c
  * @author  Khusainov Timur
  * @version 0.0.0.1
  * @date    9.02.2011
  * @brief   FT2232 device base
  ******************************************************************************
  * @attention
  * <h2><center>&copy; COPYRIGHT 2011 timypik@gmail.com </center></h2>
  ******************************************************************************
  */

#include "FT2232.h"

void TFT2232::Open()
{
	#ifdef FT_DinamicDll 
		if(!DllLoad)
			return;
	#endif
	
	if(FOpen == IS_OPEN && FHandle)
		return;
	
	ScopeSection();		// Lock section
	
	if((FStatus = (FT_Open((int)FChannel, &FHandle))) == FT_OK)
	{
		FT_ResetDevice(FHandle);					// HW reset dev
		FT_Purge(FHandle, FT_PURGE_RX|FT_PURGE_TX);	// Clear in/out buffers
		FT_SetUSBParameters(FHandle, 65536, 65535);	// Set USB request transfer sizes to 64K
		FT_SetChars(FHandle, false, 0, false, 0);	// Disable event and error characters
		FT_SetTimeouts(FHandle, 5000, 5000);		// Sets the read and write timeouts in milliseconds
		FT_SetLatencyTimer(FHandle, 16);			// Set the latency timer (default is 16mS)
		FT_SetBitMode(FHandle, 0x00, 0x00);			// Set the reset bit mode
		
		Sleep(50); // wait apply
		
		FOpen = IS_OPEN;
	}
	else
		printf("[FT_STATUS] TFT2232::Open->FT_Open: %d\n", FStatus);
}

void TFT2232::Close()
{
	if (FOpen == NOT_OPEN && !FHandle)
		return;
	
	ScopeSection();		// Lock section
	
	if((FStatus = (FT_SetBitMode(FHandle, 0x00, 0x00))) == FT_OK)	// Set the reset bit mode
	{
		if((FStatus = (FT_Close(FHandle))) == FT_OK)	// Close dev
		{
			FHandle = NULL;
			FOpen = NOT_OPEN;
		}
		else
			printf("[FT_STATUS] TFT2232::Close->FT_Close: %d\n", FStatus);
	}
	else
		printf("[FT_STATUS] TFT2232::Close->FT_SetBitMode: %d\n", FStatus);
}

TFT2232::~TFT2232()
{
	printf("~TFT2232()\n");
	
	#ifdef FT_DinamicDll
	FT_FreeDll();
	#endif
	
	this->Close();
	
	if(FOperationAccss)
		delete FOperationAccss;
}