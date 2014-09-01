/**
  ******************************************************************************
  * @file    VNC1L.c
  * @author  Khusainov Timur
  * @version 0.0.0.1
  * @date    22.02.2011
  * @brief   VNC1L programmer via FT2232
  ******************************************************************************
  * @attention
  * <h2><center>&copy; COPYRIGHT 2011 timypik@gmail.com </center></h2>
  ******************************************************************************
  */

#include "VNC1L.h"

unsigned int TVNC1L::FileSize(char *fName)
{
	FILE *fp;					// ptr for file
	unsigned int fSize = 0;		// file size 

	fp = fopen(fName,"r");		// open file
	fseek(fp , 0 , SEEK_END);	// seek to end file
	fSize = ftell(fp);			// get tell(position)
	fclose(fp);					// close file
	return fSize;				// retern get file size
}

void TVNC1L::ReadFrimwareFile(tVNC1L_ROMFile *f)
{
	f->fPtr = fopen(f->fName, "rb");	// open file
	
	f->fData = new char[f->fSize];		// alloc memory for buffer
	memset(f->fData, 0, f->fSize);		// clear buffer
	
	fread(f->fData, sizeof(char), FW_SIZE, f->fPtr);	// fill data
	
	fclose(f->fPtr);	// close file 
}

void TVNC1L::SetFile(char *Name)
{
	FFrimware.fSize = FileSize(Name);
	if (FFrimware.fSize == FW_SIZE)
	{
		FFrimware.fName = Name;
		ReadFrimwareFile(&FFrimware);
		FFileOk = F_OK;
	}
	else
		FFileOk = F_FAIL;
}

void TVNC1L::Open()
{
	if (!TRS232_485::IsOpen())		// check device state
	{
		TRS232_485::Open();			// open device
		if (TRS232_485::IsOpen())	// check device state
		{
			TRS232_485::Baud = br115200;
			TRS232_485::DataBits = db8;
			TRS232_485::FlowCtrl = fcRTS_CTS;

			if(SyncToVNC1L() == IS_SYNC)
			{
				SetupVNC1L(1);
				TRS232_485::Baud = br1000000;
				TRS232_485::DataBits = db8;
				TRS232_485::FlowCtrl = fcRTS_CTS;
			}
		}
	}
}

unsigned char TVNC1L::SyncToVNC1L()
{
	unsigned char echo_cmd = 0xFF;
	unsigned char in = 0;
	unsigned char rb = 0;
	
	TRS232_485::Write(&echo_cmd, 1);	// write echo md
	rb = TRS232_485::Read(&in);		// read return value & num of byte 
	
	if (!rb || (in != echo_cmd))		// check return value
		return 0;						// return error
		
	in = 0;
	rb = 0;
	
	TRS232_485::Write(&echo_cmd, 1);	// write echo md
	rb = TRS232_485::Read(&in);		// read return value & num of byte 

	FSync = (!rb || (in != echo_cmd))?(0):(1);	// save state of sync
	
	return FSync;	// return state
}

unsigned char TVNC1L::SetupVNC1L(unsigned char st)
{	
	memset(out_buf, 0, sizeof(out_buf));	// clear buffer
	memset(in_buf, 0, sizeof(in_buf));		// clear buffer
	
	out_buf[0] = 0x01;	// cmd SetVars
	out_buf[1] = 0x70;	// cmd TimerLow
	out_buf[2] = 0x17;	// cmd TimerHigh
	
	out_buf[3] = (st)?(0x03):(0x1A);	// Baud1 value // 1000000 Baud // 115200 Baud
	out_buf[4] = 0;		// Baud2 value
	out_buf[5] = 0;		// Baud3 value

	TRS232_485::Write(out_buf, 6);	// write cmd's
	Sleep(100);						// wait for complate
	TRS232_485::Read(in_buf);		// reade return value
	
	return (in_buf[0] != 0x02)?(0):(1);	// return state
}


unsigned char TVNC1L::WriteToAddr(unsigned char *Data, unsigned int Addr)
{
	unsigned char f_chk = 0;		// check complite operation flag
	
	out_buf[0] = 0x03;				// set cmd write from addr
	out_buf[1] = Addr & 0xFF;		// set high addr
	out_buf[2] = (Addr>>8) & 0xFF;	// set low addr
	out_buf[3] = 0x80;				// set legth data (128)
	TRS232_485::Write(out_buf, 4);	// write and done
	
	TRS232_485::Read(&f_chk);		// read retern value
	
	if (f_chk != 0x02)				// check reten cmd
		return 0;					// operation not done
	
	TRS232_485::Write(Data, 128);	// write data (128)
	f_chk = 0;						// clear check flag
	TRS232_485::Read(&f_chk);		// read retern value
	
	return (f_chk != 0x02)?(0):(1);// retern done/undone
}

unsigned int TVNC1L::ReadFromAddr(unsigned char *Data, unsigned int Addr)
{
	out_buf[0] = 0x02;				// set set cmd read from addr
	out_buf[1] = Addr & 0xFF;		// set high addr
	out_buf[2] = (Addr>>8) & 0xFF;	// set low addr
	out_buf[3] = 0x80;				// set legth data (128)
	TRS232_485::Write(out_buf, 4);	// write and done
	
	return (TRS232_485::Read(Data));// read & retern lengh
}

unsigned int TVNC1L::ProgrammFlash()
{
	unsigned int FlashPage;		// num of flash page
	unsigned int err_count = 0;	// num of write error
	unsigned char pack[128];	// package to send
	
	if (!TRS232_485::IsOpen())		// check device state
	{
		this->Open();			// open device
		if (!TRS232_485::IsOpen() || FSync != IS_SYNC || !FFrimware.fData)	// check device state
			return -1;					// return with error
	}
	
	for (FlashPage = 0; FlashPage < FW_SIZE; FlashPage+=128)
	{
		memset(pack, 0, 128);	// clean buffer
		
		for (int i = 0; i < 128; i++)
			pack[i] = FFrimware.fData[i+FlashPage];// fill buffer
		
		if (!WriteToAddr(pack, FlashPage))	// write & check whit error
			err_count++;					// inc error counter
			
		if (FInfoProc)
			FInfoProc(((FlashPage/65536.0)*100), I_PF);
	}
	
	if (FInfoProc)
		FInfoProc(100, I_PF);
	
	return err_count;	// retern num of write error
}

void TVNC1L::ReadToFile(char *fName)
{
	unsigned int FlashPage;			// num of flash page
	unsigned int rb = 0;			// num of read data
	
	if (!TRS232_485::IsOpen())		// check device state
	{
		this->Open();			// open device
		if (!TRS232_485::IsOpen() || FSync != IS_SYNC)	// check device state
			return;					// return with error
	}
	
	FILE *file = fopen(fName,"wb");// open file
	
	for (FlashPage = 0; FlashPage < FW_SIZE; FlashPage+=128)
	{
		memset(in_buf, 0, sizeof(in_buf));		// clear buffer
		rb = ReadFromAddr(in_buf, FlashPage);	// read data & get num of read data
		
		for (unsigned int i = 1; i < rb; i++)	// skip first byte
				fwrite(&in_buf[i], 1, 1, file);// write data to file
		
		if (FInfoProc)
			FInfoProc(((FlashPage/65536.0)*100), I_RF);
	}
	
	if (FInfoProc)
		FInfoProc(100, I_RF);
	
	fclose(file);		// close file
}

unsigned int TVNC1L::VerifyFlash()
{
	unsigned int FlashPage;			// num of flash page
	unsigned int rb = 0;			// num of read data
	unsigned int err_count = 0;
	
	if (!TRS232_485::IsOpen())		// check device state
	{
		this->Open();			// open device
		if (!TRS232_485::IsOpen() || FSync != IS_SYNC || !FFrimware.fData)	// check device state
			return -1;					// return with error
	}
	
	for (FlashPage = 0; FlashPage < FW_SIZE; FlashPage+=128)
	{
		memset(in_buf, 0, sizeof(in_buf));		// clear buffer
		rb = ReadFromAddr(in_buf, FlashPage);	// read data & get num of read data
		
		for (unsigned int i = 1; i < rb; i++)	// skip first byte
				if (FFrimware.fData[(i-1)+FlashPage] != in_buf[i])	// compare data
					err_count++;	// inc of error
		
		if (FInfoProc)
			FInfoProc(((FlashPage/65536.0)*100), I_VF);
	}
	
	if (FInfoProc)
		FInfoProc(100, I_VF);
	
	return err_count;	// return num of error
}

TVNC1L::~TVNC1L()
{
	printf("~TVNC1L()\n");
	
	if (TRS232_485::IsOpen())
		SetupVNC1L(0);
	
	if (FFrimware.fData)
		delete FFrimware.fData;
		
	TRS232_485::Close();
}