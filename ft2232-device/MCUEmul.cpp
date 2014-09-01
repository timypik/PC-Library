/**
  ******************************************************************************
  * @file    MCUEmul.c
  * @author  Khusainov Timur
  * @version 0.0.0.1
  * @date    4.03.2011
  * @brief   Emulate MCU intrface via FT2232
  ******************************************************************************
  * @attention
  * <h2><center>&copy; COPYRIGHT 2011 timypik@gmail.com </center></h2>
  ******************************************************************************
  */

#include "MCUEmul.h"

void TMCUEmul::Open()
{
	if (!TFT2232::IsOpen)
	{
		TFT2232::Open(); 
		if (!TFT2232::IsOpen)
			return;
		
		FDevice.Handle = TFT2232::Handle;
	}
	
	if (FConfigured != IS_CONFIGURED)
		SetpuAndSync();
}

void TMCUEmul::SetpuAndSync()
{
	ScopeSection();
	
	if((FDevice.Status = FT_SetBitMode(FDevice.Handle, 0x00, 0x08)) != FT_OK)
		printf("[FT_STATUS] TMCUEmul::SetpuAndSync()->FT_SetBitMode: %d\n", FDevice.Status);
	
	out_buf[0] = 0xAA;
	
	if((FDevice.Status = FT_Write(FDevice.Handle, out_buf, 1, &FDevice.now_sent)) != FT_OK)
		printf("[FT_STATUS] TMCUEmul::SetpuAndSync()->FT_Write: %d\n", FDevice.Status);

	do
	{
		Sleep(100);
		if((FDevice.Status = FT_GetQueueStatus(FDevice.Handle, &FDevice.need_read)) != FT_OK)
			printf("[FT_STATUS] TMCUEmul::SetpuAndSync()->FT_GetQueueStatus: %d\n", FDevice.Status);
	} while ((FDevice.need_read == 0) && (FDevice.Status == FT_OK));
	
	if((FDevice.Status = FT_Read(FDevice.Handle, in_buf, FDevice.need_read, &FDevice.now_read)) != FT_OK)
		printf("[FT_STATUS] TMCUEmul::SetpuAndSync()->FT_Read: %d\n", FDevice.Status);
		
	for (unsigned char i = 0; i < FDevice.now_read - 1; i++)
		if ((in_buf[i] != 0xFA) && (in_buf[i+1] != 0xAA))	
			return;
	
	FConfigured = IS_CONFIGURED;
	
	Sleep(20);
	
	out_buf[0] = 0x82;
    out_buf[1] = 0x00;
    out_buf[2] = 0x00;
	
	if((FDevice.Status = FT_Write(FDevice.Handle, out_buf, 3, &FDevice.now_sent)) != FT_OK)
		printf("[FT_STATUS] TMCUEmul::SetpuAndSync()->FT_Write: %d\n", FDevice.Status);
}

unsigned char TMCUEmul::ReadIO()
{
	ScopeSection();
	
	in_buf[0] = 0x00;
	
	out_buf[0] = 0x83;	// read I/O Lines
	out_buf[1] = 0x87;	// send immediate
	
	
	if((FDevice.Status = FT_Write(FDevice.Handle, out_buf, 2, &FDevice.now_sent)) != FT_OK)
		printf("[FT_STATUS] TMCUEmul::ReadIO()->FT_Write: %d\n", FDevice.Status);
		
	do
	{
		if((FDevice.Status = FT_GetQueueStatus(FDevice.Handle, &FDevice.need_read)) != FT_OK)
			printf("[FT_STATUS] TMCUEmul::ReadIO()->FT_GetQueueStatus: %d\n", FDevice.Status);
	} while ((FDevice.need_read == 0) && (FDevice.Status == FT_OK));
	
	if((FDevice.Status = FT_Read(FDevice.Handle, in_buf, FDevice.need_read, &FDevice.now_read)) != FT_OK)
		printf("[FT_STATUS] TMCUEmul::ReadIO()->FT_Read: %d\n", FDevice.Status);

    return (FDevice.Status != FT_OK) ? 0x7F : in_buf[0];
}

void TMCUEmul::Write(unsigned char Data, unsigned short Addr)
{
	ScopeSection();
	FDevice.need_send = 0;	// clear counter
	
	if (Addr >> 8)
	{
		out_buf[FDevice.need_send++] = 0x93;				// write to extended address
		out_buf[FDevice.need_send++] = (Addr>>8) & 0xFF;	// addr high byte
		out_buf[FDevice.need_send++] = Addr & 0xFF;		// addr low byte
	}
	else
	{
		out_buf[FDevice.need_send++] = 0x92;			// write to std address
		out_buf[FDevice.need_send++] = Addr & 0xFF;	// addr byte
	}
	out_buf[FDevice.need_send++] = Data;	// set data
	
	if((FDevice.Status = FT_Write(FDevice.Handle, out_buf, FDevice.need_send, &FDevice.now_sent)) != FT_OK)
		printf("[FT_STATUS] TMCUEmul::Write()->FT_Write: %d\n", FDevice.Status);
}

unsigned char TMCUEmul::Read(unsigned short Addr)
{
	ScopeSection();
	FDevice.need_send = 0;	// clear counter
	in_buf[0] = 0x00;		// clear input buffer
	
	if (Addr >> 8)
	{
		out_buf[FDevice.need_send++] = 0x91;				// read from extended addres
		out_buf[FDevice.need_send++] = (Addr>>8) & 0xFF;	// addr high byte
		out_buf[FDevice.need_send++] = Addr & 0xFF;		// addr low byte
	}
	else
	{
		out_buf[FDevice.need_send++] = 0x90;			// read from std addr
		out_buf[FDevice.need_send++] = Addr & 0xFF;	// addr byte
	}
	
	out_buf[FDevice.need_send++] = 0x87;	// send immediate
	
	if((FDevice.Status = FT_Write(FDevice.Handle, out_buf, FDevice.need_send, &FDevice.now_sent)) != FT_OK)
		printf("[FT_STATUS] TMCUEmul::Read()->FT_Write: %d\n", FDevice.Status);
	
	do
	{
		if((FDevice.Status = FT_GetQueueStatus(FDevice.Handle, &FDevice.need_read)) != FT_OK)
			printf("[FT_STATUS] TMCUEmul::Read()->FT_GetQueueStatus: %d\n", FDevice.Status);
	} while ((FDevice.need_read == 0) && (FDevice.Status == FT_OK));
	
	if((FDevice.Status = FT_Read(FDevice.Handle, in_buf, FDevice.need_read, &FDevice.now_read)) != FT_OK)
		printf("[FT_STATUS] TMCUEmul::Read()->FT_Read: %d\n", FDevice.Status);
	
    return in_buf[0];
}

TMCUEmul::~TMCUEmul()
{
	printf("~TMCUEmul()\n");
	
	TFT2232::Close();
	
	if(this->FOperationAccss)
		delete this->FOperationAccss;
}