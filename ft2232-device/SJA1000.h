/**
  ******************************************************************************
  * @file    SJA1000.h
  * @author  Khusainov Timur
  * @version 0.0.0.1
  * @date    4.02.2011
  * @brief   SJA1000 CAN-driver via FT2232
  ******************************************************************************
  * @attention
  * <h2><center>&copy; COPYRIGHT 2011 timypik@gmail.com </center></h2>
  ******************************************************************************
  */

#ifndef SJA1000H
#define SJA1000H

#include <stdio.h>
#include <queue>
#include "MCUEmul.h"

#define NOT_STARTED 0 
#define IS_STARTED 1

#define ERROR_NO 0
#define ERROR_DO 1
#define ERROR_BE 2
#define ERROR_FT 4

typedef enum
{
	cmBasicCAN = 0,
	cmPeliCAN = 1,
	cmNone
}tCanMode;

typedef enum
{
	imOperating = 0,
	imReset,
	imListenOnly,
	imNormalMode,
	imSelfTest,
	imNoSelfTest,
	imAcceptanceFilter,
	imNoFilter,
	imSleep,
	imWake,
	imNone
}tInternalMode;

typedef enum 
{
	cs10k,
	cs20k,
	cs50k,
	cs100k,
	cs125k,
	cs250k,
	cs500k,
	cs800k,
	cs1m,
	csNone
}tCanSpeed;

typedef struct
{
	unsigned char Code[4];
	unsigned char Mask[4];
}tCanAcceptance;

typedef struct
{
	unsigned char format;	// Extended/Standard Frame (FF_Bit = 7)
	unsigned char rtr;		// Remote Transmition Bit/Data Frame (RTR_Bit = 6)
	unsigned char len;		// Length of frame, max = 8 
	unsigned int id;		// Frame Identifier
	unsigned char data[8];	// Data Bytes
}tCanFrame;

typedef struct
{
	void *Start;
	void *Done;
}tSyncEvents;

#define TERMINATE_TREAD 1
#define STILL_THREAD 0

template<class T>
struct tThread
{
	void* Handle;
    union
    {
        unsigned long (T:: *Method)(void*);
        unsigned long (__stdcall *Func)(void*);
    }Addr;
	unsigned long Id;
    unsigned char Terminated;
};


class TSJA1000: protected TMCUEmul
{
	private:
		CAutoSection *FOperationAccss;	// Lock & UnLock Critical section
		
		tCanMode FCanMode;
		tInternalMode FInternalMode;
		tCanSpeed FBaud;
		tCanAcceptance *FAcceptance;
		unsigned int FFreqKHz;
		unsigned char FStarted;

        unsigned char FErrors;
		
		tThread<TSJA1000> FInterruptThread;
		unsigned long InterruptHandler(void*);
		
		tThread<TSJA1000> FRecvThread;
		unsigned long ReadFrameHandler(void*);
		void ReadFrameFromDev();
		
		tSyncEvents FRecvEvent;
		
		std::queue<tCanFrame> FRxFrameBuffer;
		std::queue<tCanFrame> FTxFrameBuffer;
	protected:
		void SetupAndStart();
		
		void SetupEnevtAndThread();
		void DestroyEnevtAndThread();
		
		void SetCanMode(tCanMode cMode);
		void SetInternalMode(tInternalMode iMode);
		void SetBaud(tCanSpeed cSpeed);
		void SetAcceptance(tCanAcceptance *acc);
	public:
		TSJA1000():FErrors(ERROR_NO),
        		FStarted(NOT_STARTED),
				FFreqKHz(0),
				FAcceptance(new tCanAcceptance),
				FBaud(csNone),
				FInternalMode(imNone),
				FCanMode(cmNone),
				FOperationAccss(new CAutoSection()),
				TMCUEmul()
                {
                	printf("TSJA1000()\n");

                    memset(&FInterruptThread, 0, sizeof(FInterruptThread));
                    memset(&FRecvThread, 0, sizeof(FRecvThread));
                    memset(&FRecvEvent, 0, sizeof(FRecvEvent));
                };
		virtual ~TSJA1000();
		
		void Open();
		void Close();
        unsigned char IsOpen(){ return TMCUEmul::IsOpen(); };

		unsigned int ConutNewFrame(){ScopeSection(); return FRxFrameBuffer.size(); };
		void ReadFrame(tCanFrame *inFrame);
		void WriteFrame(tCanFrame *outFrame);
		
		__property tCanMode CanMode = {read = FCanMode, write = SetCanMode};
		__property tInternalMode InternalMode = {read = FInternalMode, write = SetInternalMode};
		__property tCanAcceptance *Acceptance = {read = FAcceptance, write = SetAcceptance};
		__property tCanSpeed Baud = {read = FBaud, write = FBaud};
		__property unsigned int ClkFreqKHz = {read = FFreqKHz, write = FFreqKHz};
		__property unsigned char IsStarted = {read = FStarted};
		__property unsigned char Errors = {read = FErrors};
};

#define CAN_BASE 0x00

/* address and bit definitions for the Mode & Control Register */
#define ModeControlReg  (CAN_BASE + 0)
#define RM_RR_Bit   0		/* reset mode (request) bit */
#define LOM_Bit     1		/* listen only mode bit */
#define STM_Bit     2		/* self test mode bit */
#define AFM_Bit     3		/* acceptance filter mode bit */
#define SM_Bit      4		/* enter sleep mode bit */

/* address and bit definitions for the Status Register */
#define StatusReg  (CAN_BASE + 2)
#define RBS_Bit     0		/* receive buffer status bit */
#define DOS_Bit     1		/* data overrun status bit */
#define TBS_Bit     2		/* transmit buffer status bit */
#define TCS_Bit     3		/* transmission complete status bit */
#define RS_Bit      4		/* receive status bit */
#define TS_Bit      5		/* transmit status bit */
#define ES_Bit      6		/* error status bit */
#define BS_Bit      7		/* bus status bit */

/* address and bit definitions for the Interrupt Register */
#define InterruptReg (CAN_BASE + 3)
#define RI_Bit      0		/* receive interrupt bit */
#define TI_Bit      1		/* transmit interrupt bit */
#define EI_Bit      2		/* error warning interrupt bit */
#define DOI_Bit     3		/* data overrun interrupt bit */
#define WUI_Bit     4		/* wake-up interrupt bit */
#define EPI_Bit     5		/* error passive interrupt bit */
#define ALI_Bit     6		/* arbitration lost interrupt bit */
#define BEI_Bit     7		/* bus error interrupt bit */

/* address and bit definitions for the Interrupt Enable & Control Register */
#define InterruptEnReg  (CAN_BASE + 4)	/* PeliCAN mode */
#define RIE_Bit     0		/* receive interrupt enable bit */
#define TIE_Bit     1		/* transmit interrupt enable bit */
#define EIE_Bit     2		/* error warning interrupt enable bit */
#define DOIE_Bit    3		/* data overrun interrupt enable bit */
#define WUIE_Bit    4		/* wake-up interrupt enable bit */
#define EPIE_Bit    5		/* error passive interrupt enable bit */
#define ALIE_Bit    6		/* arbitration lost interr. enable bit */
#define BEIE_Bit    7		/* bus error interrupt enable bit */

/* address and bit definitions for the Command Register */
#define CommandReg  (CAN_BASE + 1)
#define TR_Bit      0		/* transmission request bit */
#define AT_Bit      1		/* abort transmission bit */
#define RRB_Bit     2		/* release receive buffer bit */
#define CDO_Bit     3		/* clear data overrun bit */

/* address and bit definitions for the Bus Timing Registers */
#define BusTiming0Reg  (CAN_BASE + 6)
#define BusTiming1Reg  (CAN_BASE + 7)

/* address and bit definitions for the Output Control Register */
#define OutControlReg  (CAN_BASE + 8)
        /* OCMODE1, OCMODE0 */
#define BiPhaseMode 0x00	/* bi-phase output mode */
#define NormalMode  0x02	/* normal output mode */
#define ClkOutMode  0x03	/* clock output mode */
        /* output pin configuration for TX1 */
#define OCPOL1_Bit  5		/* output polarity control bit */
#define Tx1Float    0x00	/* configured as float */
#define Tx1PullDn   0x40	/* configured as pull-down */
#define Tx1PullUp   0x80	/* configured as pull-up */
#define Tx1PshPull  0xC0	/* configured as push/pull */
        /* output pin configuration for TX0 */
#define OCPOL0_Bit  2		/* output polarity control bit */
#define Tx0Float    0x00	/* configured as float */
#define Tx0PullDn   0x08	/* configured as pull-down */
#define Tx0PullUp   0x10	/* configured as pull-up */
#define Tx0PshPull  0x18	/* configured as push/pull */

/* address definitions of Acceptance Code & Mask Registers */
#define AcceptCode0Reg  (CAN_BASE + 16)
#define AcceptCode1Reg  (CAN_BASE + 17)
#define AcceptCode2Reg  (CAN_BASE + 18)
#define AcceptCode3Reg  (CAN_BASE + 19)
#define AcceptMask0Reg  (CAN_BASE + 20)
#define AcceptMask1Reg  (CAN_BASE + 21)
#define AcceptMask2Reg  (CAN_BASE + 22)
#define AcceptMask3Reg  (CAN_BASE + 23)

/* address definitions of the Rx-Buffer
#define RxFrameInfo (CAN_BASE + 16)
#define RxBuffer1   (CAN_BASE + 17)
#define RxBuffer2   (CAN_BASE + 18)
#define RxBuffer3   (CAN_BASE + 19)
#define RxBuffer4   (CAN_BASE + 20)
#define RxBuffer5   (CAN_BASE + 21)
#define RxBuffer6   (CAN_BASE + 22)
#define RxBuffer7   (CAN_BASE + 23)
#define RxBuffer8   (CAN_BASE + 24)
#define RxBuffer9   (CAN_BASE + 25)
#define RxBuffer10  (CAN_BASE + 26)
#define RxBuffer11  (CAN_BASE + 27)
#define RxBuffer12  (CAN_BASE + 28)     */

/* address definitions of the Rx-Buffer  */
#define FF_Bit    7		/* Frame Format Bit */
#define RTR_Bit   6		/* Remote Transmission Request Bit */
#define RxFrameInfo (CAN_BASE + 16)
#define RxId1   (CAN_BASE + 17)
#define RxId2   (CAN_BASE + 18)
#define RxBuffer1   (CAN_BASE + 19)
#define RxBuffer2   (CAN_BASE + 20)
#define RxBuffer3   (CAN_BASE + 21)
#define RxBuffer4   (CAN_BASE + 22)
#define RxBuffer5   (CAN_BASE + 23)
#define RxBuffer6   (CAN_BASE + 24)
#define RxBuffer7   (CAN_BASE + 25)
#define RxBuffer8   (CAN_BASE + 26)

#define RxId3   (CAN_BASE + 19)
#define RxId4   (CAN_BASE + 20)
#define RxBuffer1Ex   (CAN_BASE + 21)
#define RxBuffer2Ex   (CAN_BASE + 22)
#define RxBuffer3Ex   (CAN_BASE + 23)
#define RxBuffer4Ex   (CAN_BASE + 24)
#define RxBuffer5Ex   (CAN_BASE + 25)
#define RxBuffer6Ex   (CAN_BASE + 26)
#define RxBuffer7Ex   (CAN_BASE + 27)
#define RxBuffer8Ex   (CAN_BASE + 28)

/* address definitions of the Tx-Buffer */
  /* write only addresses */
#define TxFrameInfo (CAN_BASE + 16)
#define TxBuffer1  (CAN_BASE + 17)
#define TxBuffer2  (CAN_BASE + 18)
#define TxBuffer3  (CAN_BASE + 19)
#define TxBuffer4  (CAN_BASE + 20)
#define TxBuffer5  (CAN_BASE + 21)
#define TxBuffer6  (CAN_BASE + 22)
#define TxBuffer7  (CAN_BASE + 23)
#define TxBuffer8  (CAN_BASE + 24)
#define TxBuffer9  (CAN_BASE + 25)
#define TxBuffer10  (CAN_BASE + 26)
#define TxBuffer11  (CAN_BASE + 27)
#define TxBuffer12  (CAN_BASE + 28)
  /* read only addresses */
#define TxFrameInfoRd  (CAN_BASE + 96)
#define TxBufferRd1  (CAN_BASE + 97)
#define TxBufferRd2  (CAN_BASE + 98)
#define TxBufferRd3  (CAN_BASE + 99)
#define TxBufferRd4  (CAN_BASE + 100)
#define TxBufferRd5  (CAN_BASE + 101)
#define TxBufferRd6  (CAN_BASE + 102)
#define TxBufferRd7  (CAN_BASE + 103)
#define TxBufferRd8  (CAN_BASE + 104)
#define TxBufferRd9  (CAN_BASE + 105)
#define TxBufferRd10  (CAN_BASE + 106)
#define TxBufferRd11  (CAN_BASE + 107)
#define TxBufferRd12  (CAN_BASE + 108)

/* address and bit definitions for the Clock Divider Register */
#define ClockDivideReg  (CAN_BASE + 31)
#define DivBy1      0x07	/* CLKOUT = oscillator frequency */
#define DivBy2      0x00	/* CLKOUT = 1/2 oscillator frequency */
#define DivBy4      0x01	/* CLKOUT = 1/4 oscillator frequency */
#define DivBy6      0x02	/* CLKOUT = 1/6 oscillator frequency */
#define DivBy8      0x03	/* CLKOUT = 1/8 oscillator frequency */
#define DivBy10     0x04	/* CLKOUT = 1/10 oscillator frequency */
#define DivBy12     0x05	/* CLKOUT = 1/12 oscillator frequency */
#define DivBy14     0x06	/* CLKOUT = 1/14 oscillator frequency */
#define ClkOff_Bit  3		/* clock off bit,control of the CLK OUT pin */
#define RXINTEN_Bit 5		/* pin TX1 used for receive interrupt */
#define CBP_Bit     6		/* CAN comparator bypass control bit */
#define CANMode_Bit 7		/* CAN mode definition bit */

#endif // SJA1000H