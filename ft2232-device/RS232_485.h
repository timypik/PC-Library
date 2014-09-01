/**
  ******************************************************************************
  * @file    RS232_485.h
  * @author  Khusainov Timur
  * @version 0.0.0.1
  * @date    28.01.2011
  * @brief   RS232/485 via FT2232
  ******************************************************************************
  * @attention
  * <h2><center>&copy; COPYRIGHT 2011 timypik@gmail.com </center></h2>
  ******************************************************************************
  */

#ifndef RS232_485H
#define RS232_485H

#include "FT2232.h"

typedef enum
{
	brNone = 0,
	br300 = 300,
	br600 = 600,
	br1200 = 1200,
	br2400 = 2400,
	br4800 = 4800,
	br9600 = 9600,
	br14400 = 14400,
	br19200 = 19200,
	br38400 = 38400,
	br57600 = 57600,
	br115200 = 115200,
	br230400 = 230400,
	br460800 = 460800,
	br921600 = 921600,
    br1000000 = 1000000
}tFTDI_Baud;

typedef enum 
{
	db8 = 8,
	db7 = 7
}tFTDI_DataBits;

typedef enum
{
	sp1 = 0,
	sp2 = 1
}tFTDI_StopBits;

typedef enum
{
	pNone = 0,
	pOdd = 1,
	pEven = 2,
	pMark = 3,
	pSpace = 4
}tFTDI_Parity;

typedef enum
{
	fcNone = 0x0000,
	fcRTS_CTS = 0x0100, 
	fcDTR_DSR = 0x0200
}tFTDI_FLowCtrl;

class TRS232_485: protected TFT2232
{
	private:
		CAutoSection *FOperationAccss;	// Lock & UnLock Critical section
		
		tFTDI_Dev FDevice;	
		tFTDI_Baud		FBaud;		// Baudrate of dev
		tFTDI_DataBits	FDataBits;	// Num of data bits
		tFTDI_StopBits	FStopBits;	// Num of stop bits
		tFTDI_Parity	FParity;	// type of parity
		tFTDI_FLowCtrl	FFlowCtrl;	// type of flow control
	protected:
		void SetBaud(tFTDI_Baud baud);
		void SetDataBits(tFTDI_DataBits db);
		void SetStopBits(tFTDI_StopBits sp);
		void SetParity(tFTDI_Parity par);
		void SetFlowControl(tFTDI_FLowCtrl fc);
	public:
		TRS232_485(tFTDI_Chnl ch):	FFlowCtrl(fcNone), 
						FParity(pNone), 
						FStopBits(sp1), 
						FDataBits(db8), 
						FBaud(brNone), 
						FOperationAccss(new CAutoSection()), 
						TFT2232(ch){ printf("TRS232_485()\n"); };
		virtual ~TRS232_485();
		
		void Open(){ TFT2232::Open(); FDevice.Handle = TFT2232::Handle; };
		void Close(){ TFT2232::Close(); };
		unsigned int Write(unsigned char *Data, unsigned int Length);
		unsigned int Read(unsigned char *Data);
		
		unsigned char IsOpen() { return TFT2232::IsOpen; };
		
		__property tFTDI_Baud		Baud = {read = FBaud, write = SetBaud};
		__property tFTDI_DataBits	DataBits = {read = FDataBits, write = SetDataBits};
		__property tFTDI_StopBits	StopBits = {read = FStopBits, write = SetStopBits};
		__property tFTDI_Parity		Parity = {read = FParity, write = SetParity};
		__property tFTDI_FLowCtrl	FlowCtrl = {read = FFlowCtrl, write = SetFlowControl};
};

#endif RS232_485H