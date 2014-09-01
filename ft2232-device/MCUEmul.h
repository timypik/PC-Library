/**
  ******************************************************************************
  * @file    MCUEmul.h
  * @author  Khusainov Timur
  * @version 0.0.0.1
  * @date    4.03.2011
  * @brief   Emulate MCU intrface via FT2232
  ******************************************************************************
  * @attention
  * <h2><center>&copy; COPYRIGHT 2011 timypik@gmail.com </center></h2>
  ******************************************************************************
  */


#ifndef McuEmulH
#define McuEmulH

#include <stdio.h>
#include "FT2232.h"

#define NOT_CONFIGURED 0
#define IS_CONFIGURED 1

class TMCUEmul: protected TFT2232
{
	private:
		CAutoSection *FOperationAccss;	// Lock & UnLock Critical section
		
		tFTDI_Dev FDevice;
		unsigned char FConfigured;
		
		unsigned char in_buf[5];
		unsigned char out_buf[3];
	protected:
		void SetpuAndSync();
		unsigned char ReadIO();
	public:
		TMCUEmul():FConfigured(NOT_CONFIGURED),
				FOperationAccss(new CAutoSection()), 
				TFT2232(chA){ printf("TMCUEmul()\n"); };
		virtual ~TMCUEmul();
		
		void Open();
		void Close(){ TFT2232::Close(); };
		
		void Write(unsigned char Data, unsigned short Addr);
		unsigned char Read(unsigned short Addr);
		
		unsigned char IsOpen() { return TFT2232::IsOpen; };
		__property unsigned char IsConfigured = {read = FConfigured};
		__property unsigned char IOState = {read = ReadIO};
};

#endif // McuEmulH