/**
  ******************************************************************************
  * @file    RS232_485.c
  * @author  Khusainov Timur
  * @version 0.0.0.1
  * @date    28.01.2011
  * @brief   RS232/485 via FT2232
  ******************************************************************************
  * @attention
  * <h2><center>&copy; COPYRIGHT 2011 timypik@gmail.com </center></h2>
  ******************************************************************************
  */

#include "RS232_485.h"

void TRS232_485::SetBaud(tFTDI_Baud baud)
{
	ScopeSection();
	
	if((FDevice.Status = FT_SetBaudRate(FDevice.Handle, baud)) == FT_OK)
		FBaud = baud;
	else
		printf("[FT_STATUS] TRS232_485::SetBaud->FT_SetBaudRate: %d\n", FDevice.Status);
}

void TRS232_485::SetDataBits(tFTDI_DataBits db)
{
	ScopeSection();
	
	if((FDevice.Status = FT_SetDataCharacteristics(FDevice.Handle, (unsigned char)db, (unsigned char)FStopBits, (unsigned char)FParity)) == FT_OK)
		FDataBits = db;
	else
		printf("[FT_STATUS] TRS232_485::SetDataBits->FT_SetDataCharacteristics: %d\n", FDevice.Status);
}

void TRS232_485::SetStopBits(tFTDI_StopBits sp)
{
	ScopeSection();
	
	if((FDevice.Status = FT_SetDataCharacteristics(FDevice.Handle, (unsigned char)FDataBits, (unsigned char)sp, (unsigned char)FParity)) == FT_OK)
		FStopBits = sp;
	else
		printf("[FT_STATUS] TRS232_485::SetStopBits->FT_SetDataCharacteristics: %d\n", FDevice.Status);
}

void TRS232_485::SetParity(tFTDI_Parity par)
{
	ScopeSection();
	
	if((FDevice.Status = FT_SetDataCharacteristics(FDevice.Handle, (unsigned char)FDataBits, (unsigned char)FStopBits, (unsigned char)par)) == FT_OK)
		FParity = par;
	else
		printf("[FT_STATUS] TRS232_485::SetParity->FT_SetDataCharacteristics: %d\n", FDevice.Status);
}

void TRS232_485::SetFlowControl(tFTDI_FLowCtrl fc)
{
	ScopeSection();
	
	if((FDevice.Status = FT_SetFlowControl(FDevice.Handle,fc,0,0)) == FT_OK)
		FFlowCtrl = fc;
	else
		printf("[FT_STATUS] TRS232_485::SetFlowControl->FT_SetFlowControl: %d\n", FDevice.Status);
}

unsigned int TRS232_485::Write(unsigned char *Data, unsigned int Length)
{
	ScopeSection();
	
	FDevice.need_send = Length;		// length of data send
	FDevice.src = Data;				// ptr for contrl data
	
	do
	{
		FT_Write(FDevice.Handle, FDevice.src, FDevice.need_send, &FDevice.now_sent);
		
		if (!FDevice.now_sent) // data not send 
			return 0;
		
		if (FDevice.now_sent != FDevice.need_send)	// net all data send
		{
			FDevice.need_send -= FDevice.now_sent;	// calc data need to send
			FDevice.src += FDevice.now_sent;		// change ptr of data
		}
		
	}while(FDevice.now_sent != FDevice.need_send);	// wait while all data send
	
	return FDevice.now_sent;
}

unsigned int TRS232_485::Read(unsigned char *Data)
{
	ScopeSection();
	
	FDevice.now_read = 0;	// reset data counter
	FDevice.dest = Data;	// ptr for data
	
	do
	{
		FDevice.need_read = FDevice.now_read;	// calc in data
		Sleep(50);	// wait some time
		FT_GetQueueStatus(FDevice.Handle, &FDevice.now_read);	// read state of in data
	}while(FDevice.now_read != FDevice.need_read);	// wait while all data recv
	
	do
	{
		FT_Read(FDevice.Handle, FDevice.dest, FDevice.need_read, &FDevice.now_read);
		
		if (!FDevice.now_read)
			return 0;
		
		if (FDevice.now_read != FDevice.need_read)
		{
			FDevice.need_read -= FDevice.now_read;
			FDevice.dest += FDevice.now_read;
		}
	}while(FDevice.now_read != FDevice.need_read);
	
	return FDevice.now_read;
}

TRS232_485::~TRS232_485()
{
	printf("~TRS232_485()\n");
	
	this->Close();
	
	if(this->FOperationAccss)
		delete this->FOperationAccss;
}