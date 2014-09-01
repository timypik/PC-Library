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

#ifndef VNC1LH
#define VNC1LH

#include <stdio.h>
#include "RS232_485.h"

#define FW_SIZE 65536

#define F_OK 1
#define F_FAIL 0

#define NOT_SYNC 0
#define IS_SYNC 1

#define I_PF 0x01
#define I_RF 0x04
#define I_VF 0x02

typedef struct
{
	FILE *fPtr;
	unsigned int fSize;
	char *fName;
	unsigned char *fData;
}tVNC1L_ROMFile;

typedef void (__closure *tInfoProc)(unsigned char proc, unsigned char type);

class TVNC1L: protected TRS232_485
{
	private:
		tVNC1L_ROMFile FFrimware;	// file struct
		unsigned char FFileOk;		// file state
		unsigned char FSync;	// state of sync
		tInfoProc FInfoProc;	// update state of operation
		
		unsigned char out_buf[128];
		unsigned char in_buf[129];
	protected:
		unsigned int FileSize(char *fName);			// return file size in byte
		void ReadFrimwareFile(tVNC1L_ROMFile *f);	// fill data buffer from file
		void SetFile(char *Name);

		unsigned char SyncToVNC1L();                // sync with vnc1l; return: -1 - device not opend; 1 - is ok; 0 - fail;
		unsigned char SetupVNC1L(unsigned char st); // prog setup vnc1l; return: -1 - device not opend; 1 - is ok; 0 - fail;

		unsigned char WriteToAddr(unsigned char *Data, unsigned int Addr);
		unsigned int ReadFromAddr(unsigned char *Data, unsigned int Addr);
	public:
		TVNC1L():FSync(NOT_SYNC),
				FFileOk(F_FAIL),
				TRS232_485(chA){ printf("TVNC1L()\n"); FFrimware.fData = NULL; };
		~TVNC1L();

        void Open();
        void Close(){ if (TRS232_485::IsOpen()) SetupVNC1L(0); TRS232_485::Close(); };
        unsigned char IsOpen(){ return TRS232_485::IsOpen(); };

		unsigned int ProgrammFlash();	// return: -1 - device not opend; 0 - is ok; num of error
		unsigned int VerifyFlash();		// return: -1 - device not opend; 0 - is ok; num of error
		void ReadToFile(char *fName);
		
		__property char* Frimware = {read = FFrimware.fName, write = SetFile};
		__property unsigned char IsFileCorrect = {read = FFileOk};
		__property unsigned char IsSync = {read = FSync};
		__property tInfoProc InfoUpdater = {read = FInfoProc, write = FInfoProc};
};

#endif // VNC1LH