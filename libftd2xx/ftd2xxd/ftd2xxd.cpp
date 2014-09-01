#include "ftd2xxd.h"

#define FreeFunc(f) _##f = NULL
#define LoadFunc(func) if ((_##func = (p##func)GetProcAddress(FT_Dll, #func)) == NULL) err++

static pFT_ListDevices _FT_ListDevices = NULL;
static pFT_Open _FT_Open = NULL;
static pFT_Close _FT_Close = NULL;
static pFT_ResetDevice _FT_ResetDevice = NULL;
static pFT_SetUSBParameters _FT_SetUSBParameters = NULL;
static pFT_SetChars _FT_SetChars = NULL;
static pFT_SetTimeouts _FT_SetTimeouts = NULL;
static pFT_SetLatencyTimer _FT_SetLatencyTimer = NULL;
static pFT_SetBitMode _FT_SetBitMode = NULL;
static pFT_SetDivisor _FT_SetDivisor = NULL;
static pFT_SetBaudRate _FT_SetBaudRate = NULL;
static pFT_SetDataCharacteristics _FT_SetDataCharacteristics = NULL;
static pFT_SetFlowControl _FT_SetFlowControl = NULL;
static pFT_Read _FT_Read = NULL;
static pFT_Write _FT_Write = NULL;
static pFT_GetQueueStatus _FT_GetQueueStatus = NULL;
static pFT_Purge _FT_Purge = NULL;

static HINSTANCE FT_Dll = NULL;

unsigned char FT_LoadDll(void)
{
	unsigned char err = 0;
	
	if ((FT_Dll = LoadLibrary(TEXT("ftd2xx"))) == NULL)
		return 0;
	
	LoadFunc(FT_ListDevices);
	LoadFunc(FT_Open);
	LoadFunc(FT_Close);
	LoadFunc(FT_ResetDevice);
	LoadFunc(FT_SetUSBParameters);
	LoadFunc(FT_SetChars);
	LoadFunc(FT_SetTimeouts);
	LoadFunc(FT_SetLatencyTimer);
	LoadFunc(FT_SetBitMode);
	LoadFunc(FT_SetDivisor);
	LoadFunc(FT_SetBaudRate);
	LoadFunc(FT_SetDataCharacteristics);
	LoadFunc(FT_SetFlowControl);
	LoadFunc(FT_Read);
	LoadFunc(FT_Write);
	LoadFunc(FT_GetQueueStatus);
	LoadFunc(FT_Purge);
	
	return (err?0:1);
}

FT_STATUS FT_ListDevices(PVOID pArg1, PVOID pArg2, DWORD Flags){ return _FT_ListDevices(pArg1, pArg2, Flags); };

FT_STATUS FT_Open(int deviceNumber, FT_HANDLE *pHandle){ return _FT_Open(deviceNumber, pHandle); }
FT_STATUS FT_Close(FT_HANDLE ftHandle){ return _FT_Close(ftHandle); }
FT_STATUS FT_ResetDevice(FT_HANDLE ftHandle){ return _FT_ResetDevice(ftHandle); };

FT_STATUS FT_SetUSBParameters(FT_HANDLE ftHandle, ULONG ulInTransferSize, ULONG ulOutTransferSize){ return _FT_SetUSBParameters(ftHandle, ulInTransferSize, ulOutTransferSize); };
FT_STATUS FT_SetChars(FT_HANDLE ftHandle, UCHAR EventChar, UCHAR EventCharEnabled, UCHAR ErrorChar, UCHAR ErrorCharEnabled){ return _FT_SetChars(ftHandle, EventChar, EventCharEnabled, ErrorChar, ErrorCharEnabled); };
FT_STATUS FT_SetTimeouts(FT_HANDLE ftHandle, ULONG ReadTimeout, ULONG WriteTimeout){ return _FT_SetTimeouts(ftHandle, ReadTimeout, WriteTimeout); };
FT_STATUS FT_SetLatencyTimer(FT_HANDLE ftHandle, UCHAR ucLatency){ return _FT_SetLatencyTimer(ftHandle, ucLatency); };
FT_STATUS FT_SetBitMode(FT_HANDLE ftHandle, UCHAR ucMask, UCHAR ucEnable){ return _FT_SetBitMode(ftHandle, ucMask, ucEnable); };
FT_STATUS FT_SetDivisor(FT_HANDLE ftHandle, USHORT Divisor){ return _FT_SetDivisor(ftHandle, Divisor); };

FT_STATUS FT_SetBaudRate(FT_HANDLE ftHandle, ULONG BaudRate){ return _FT_SetBaudRate(ftHandle, BaudRate); };
FT_STATUS FT_SetDataCharacteristics(FT_HANDLE ftHandle, UCHAR WordLength, UCHAR StopBits, UCHAR Parity){ return _FT_SetDataCharacteristics(ftHandle, WordLength, StopBits, Parity); };
FT_STATUS FT_SetFlowControl(FT_HANDLE ftHandle, USHORT FlowControl, UCHAR XonChar, UCHAR XoffChar){ return _FT_SetFlowControl(ftHandle, FlowControl, XonChar, XoffChar); };

FT_STATUS FT_Read(FT_HANDLE ftHandle, LPVOID lpBuffer, DWORD dwBytesToRead, LPDWORD lpBytesReturned){ return _FT_Read(ftHandle, lpBuffer, dwBytesToRead, lpBytesReturned); };
FT_STATUS FT_Write(FT_HANDLE ftHandle, LPVOID lpBuffer, DWORD dwBytesToWrite, LPDWORD lpBytesWritten){ return _FT_Write(ftHandle, lpBuffer, dwBytesToWrite, lpBytesWritten); };
FT_STATUS FT_GetQueueStatus(FT_HANDLE ftHandle, DWORD *dwRxBytes){ return _FT_GetQueueStatus(ftHandle, dwRxBytes); };
FT_STATUS FT_Purge(FT_HANDLE ftHandle, ULONG Mask){ return _FT_Purge(ftHandle, Mask); };

void FT_FreeDll(void)
{
	if(FT_Dll)
	{
		FreeLibrary(FT_Dll);
		FT_Dll = NULL;
	}
	
	FreeFunc(FT_ListDevices);
	FreeFunc(FT_Open);
	FreeFunc(FT_Close);
	FreeFunc(FT_ResetDevice);
	FreeFunc(FT_SetUSBParameters);
	FreeFunc(FT_SetChars);
	FreeFunc(FT_SetTimeouts);
	FreeFunc(FT_SetLatencyTimer);
	FreeFunc(FT_SetBitMode);
	FreeFunc(FT_SetDivisor);
	FreeFunc(FT_SetBaudRate);
	FreeFunc(FT_SetDataCharacteristics);
	FreeFunc(FT_SetFlowControl);
	FreeFunc(FT_Read);
	FreeFunc(FT_Write);
	FreeFunc(FT_GetQueueStatus);
	FreeFunc(FT_Purge);
}