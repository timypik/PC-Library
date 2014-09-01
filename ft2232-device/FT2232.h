/**
  ******************************************************************************
  * @file    FT2232.h
  * @author  Khusainov Timur
  * @version 0.0.0.1
  * @date    9.02.2011
  * @brief   FT2232 device base
  ******************************************************************************
  * @attention
  * <h2><center>&copy; COPYRIGHT 2011 timypik@gmail.com </center></h2>
  ******************************************************************************
  */

#ifndef FT2232H
#define FT2232H

//#define FT_DinamicDll
#include <stdio.h>

#ifdef FT_DinamicDll
# pragma message("Dinamic Linkin' FTD2XX.DLL")
# include "ftd2xxd/ftd2xxd.cpp"
# include "ftd2xxd/ftd2xxd.h"
#else
# pragma message("Static Linkin' FTD2XX.DLL")
# pragma comment(lib, "source/ftd2xx/ftd2xx.lib")
# include "ftd2xx/ftd2xx.h"
#endif

#include "ControlCS.h"

#define ScopeSection()	CSectionLock FTSection(this->FOperationAccss)

#define NOT_OPEN	0
#define IS_OPEN		1

typedef enum
{
	chA = 0,
	chB = 1
}tFTDI_Chnl;

typedef struct
{
	void  *Handle;
	unsigned char *dest;
	unsigned long need_read;
	unsigned long now_read;
	unsigned char *src;
    unsigned long need_send;
	unsigned long now_sent;
    unsigned long Status;
}tFTDI_Dev;

class TFT2232
{
	private:
		CAutoSection *FOperationAccss;	// Lock & UnLock Critical section
		
		void *FHandle;			// handle of dev
		unsigned long FStatus;	// return ftd2xx err status
		unsigned char FOpen;	// is dev opened
		tFTDI_Chnl FChannel;	// channal of dev
		#ifdef FT_DinamicDll
		unsigned char DllLoad;
		#endif
	public:
		TFT2232(tFTDI_Chnl ch):FHandle(NULL), 
							FChannel(ch), 
							FOperationAccss(new CAutoSection()), 
							FOpen(NOT_OPEN)
							#ifdef FT_DinamicDll
							,DllLoad(0)
							#endif
							{ 
								#ifdef FT_DinamicDll
                                DllLoad = FT_LoadDll();
								if(!DllLoad)
									MessageBox(NULL, "Error load FTD2XX.DLL", "[Fatal Error]", MB_OK|MB_ICONERROR);
								#endif
								printf("TFT2232()\n"); 
							};
		virtual ~TFT2232();
		
		void Open();
		void Close();
		
		__property void* Handle = {read = FHandle};
		__property unsigned char IsOpen = {read = FOpen};
		__property tFTDI_Chnl Channel = {read = FChannel};
};

#endif // FT2232H