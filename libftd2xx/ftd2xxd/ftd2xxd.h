#ifndef FTD2XXDH
#define FTD2XXDH
/*
	FT_ListDevices
	
	FT_Open
	FT_Close
	FT_ResetDevice
	
	FT_SetUSBParameters
	FT_SetChars
	FT_SetTimeouts
	FT_SetLatencyTimer
	FT_SetBitMode
	FT_SetDivisor

	FT_SetBaudRate
	FT_SetDataCharacteristics
	FT_SetFlowControl

	FT_Read
	FT_Write
	FT_GetQueueStatus
	FT_Purge
*/

//
// FT_ListDevices Flags (used in conjunction with FT_OpenEx Flags
//

#include <windows.h>

#define FT_LIST_NUMBER_ONLY			0x80000000
#define FT_LIST_BY_INDEX			0x40000000
#define FT_LIST_ALL					0x20000000

#define FT_LIST_MASK (FT_LIST_NUMBER_ONLY|FT_LIST_BY_INDEX|FT_LIST_ALL)

//
// Purge rx and tx buffers
//
#define FT_PURGE_RX			1
#define FT_PURGE_TX			2


typedef PVOID	FT_HANDLE;
typedef ULONG	FT_STATUS;
typedef ULONG	FT_DEVICE;


enum {
	FT_OK,
	FT_INVALID_HANDLE,
	FT_DEVICE_NOT_FOUND,
	FT_DEVICE_NOT_OPENED,
	FT_IO_ERROR,
	FT_INSUFFICIENT_RESOURCES,
	FT_INVALID_PARAMETER,
	FT_INVALID_BAUD_RATE,

	FT_DEVICE_NOT_OPENED_FOR_ERASE,
	FT_DEVICE_NOT_OPENED_FOR_WRITE,
	FT_FAILED_TO_WRITE_DEVICE,
	FT_EEPROM_READ_FAILED,
	FT_EEPROM_WRITE_FAILED,
	FT_EEPROM_ERASE_FAILED,
	FT_EEPROM_NOT_PRESENT,
	FT_EEPROM_NOT_PROGRAMMED,
	FT_INVALID_ARGS,
	FT_NOT_SUPPORTED,
	FT_OTHER_ERROR,
	FT_DEVICE_LIST_NOT_READY,
};

enum {
	FT_DEVICE_BM,
	FT_DEVICE_AM,
	FT_DEVICE_100AX,
	FT_DEVICE_UNKNOWN,
	FT_DEVICE_2232C,
	FT_DEVICE_232R,
	FT_DEVICE_2232H,
	FT_DEVICE_4232H
};

#define FT_SUCCESS(status) ((status) == FT_OK)

typedef FT_STATUS __stdcall (*pFT_ListDevices)(PVOID pArg1, PVOID pArg2, DWORD Flags);

typedef FT_STATUS __stdcall (*pFT_Open)(int deviceNumber, FT_HANDLE *pHandle);
typedef FT_STATUS __stdcall (*pFT_Close)(FT_HANDLE ftHandle);
typedef FT_STATUS __stdcall (*pFT_ResetDevice)(FT_HANDLE ftHandle);

typedef FT_STATUS __stdcall (*pFT_SetUSBParameters)(FT_HANDLE ftHandle, ULONG ulInTransferSize, ULONG ulOutTransferSize);
typedef FT_STATUS __stdcall (*pFT_SetChars)(FT_HANDLE ftHandle, UCHAR EventChar, UCHAR EventCharEnabled, UCHAR ErrorChar, UCHAR ErrorCharEnabled);
typedef FT_STATUS __stdcall (*pFT_SetTimeouts)(FT_HANDLE ftHandle, ULONG ReadTimeout, ULONG WriteTimeout);
typedef FT_STATUS __stdcall (*pFT_SetLatencyTimer)(FT_HANDLE ftHandle, UCHAR ucLatency);
typedef FT_STATUS __stdcall (*pFT_SetBitMode)(FT_HANDLE ftHandle, UCHAR ucMask, UCHAR ucEnable);
typedef FT_STATUS __stdcall (*pFT_SetDivisor)(FT_HANDLE ftHandle, USHORT Divisor);

typedef FT_STATUS __stdcall (*pFT_SetBaudRate)(FT_HANDLE ftHandle, ULONG BaudRate);
typedef FT_STATUS __stdcall (*pFT_SetDataCharacteristics)(FT_HANDLE ftHandle, UCHAR WordLength, UCHAR StopBits, UCHAR Parity);
typedef FT_STATUS __stdcall (*pFT_SetFlowControl)(FT_HANDLE ftHandle, USHORT FlowControl, UCHAR XonChar, UCHAR XoffChar);

typedef FT_STATUS __stdcall (*pFT_Write)(FT_HANDLE ftHandle, LPVOID lpBuffer, DWORD dwBytesToWrite, LPDWORD lpBytesWritten);
typedef FT_STATUS __stdcall (*pFT_Read)(FT_HANDLE ftHandle, LPVOID lpBuffer, DWORD dwBytesToRead, LPDWORD lpBytesReturned);
typedef FT_STATUS __stdcall (*pFT_GetQueueStatus)(FT_HANDLE ftHandle, DWORD *dwRxBytes);
typedef FT_STATUS __stdcall (*pFT_Purge)(FT_HANDLE ftHandle, ULONG Mask);

unsigned char FT_LoadDll(void);
void FT_FreeDll(void);

FT_STATUS FT_ListDevices(PVOID pArg1, PVOID pArg2, DWORD Flags);

FT_STATUS FT_Open(int deviceNumber, FT_HANDLE *pHandle);
FT_STATUS FT_Close(FT_HANDLE ftHandle);
FT_STATUS FT_ResetDevice(FT_HANDLE ftHandle);

FT_STATUS FT_SetUSBParameters(FT_HANDLE ftHandle, ULONG ulInTransferSize, ULONG ulOutTransferSize);
FT_STATUS FT_SetChars(FT_HANDLE ftHandle, UCHAR EventChar, UCHAR EventCharEnabled, UCHAR ErrorChar, UCHAR ErrorCharEnabled);
FT_STATUS FT_SetTimeouts(FT_HANDLE ftHandle, ULONG ReadTimeout, ULONG WriteTimeout);
FT_STATUS FT_SetLatencyTimer(FT_HANDLE ftHandle, UCHAR ucLatency);
FT_STATUS FT_SetBitMode(FT_HANDLE ftHandle, UCHAR ucMask, UCHAR ucEnable);
FT_STATUS FT_SetDivisor(FT_HANDLE ftHandle, USHORT Divisor);

FT_STATUS FT_SetBaudRate(FT_HANDLE ftHandle, ULONG BaudRate);
FT_STATUS FT_SetDataCharacteristics(FT_HANDLE ftHandle, UCHAR WordLength, UCHAR StopBits, UCHAR Parity);
FT_STATUS FT_SetFlowControl(FT_HANDLE ftHandle, USHORT FlowControl, UCHAR XonChar, UCHAR XoffChar);

FT_STATUS FT_Read(FT_HANDLE ftHandle, LPVOID lpBuffer, DWORD dwBytesToRead, LPDWORD lpBytesReturned);
FT_STATUS FT_Write(FT_HANDLE ftHandle, LPVOID lpBuffer, DWORD dwBytesToWrite, LPDWORD lpBytesWritten);
FT_STATUS FT_GetQueueStatus(FT_HANDLE ftHandle, DWORD *dwRxBytes);
FT_STATUS FT_Purge(FT_HANDLE ftHandle, ULONG Mask);

#endif 