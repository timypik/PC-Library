/**
  ******************************************************************************
  * @file    SJA1000.c
  * @author  Khusainov Timur
  * @version 0.0.0.1
  * @date    4.02.2011
  * @brief   SJA1000 CAN-driver via FT2232
  ******************************************************************************
  * @attention
  * <h2><center>&copy; COPYRIGHT 2011 timypik@gmail.com </center></h2>
  ******************************************************************************
  */

#include "SJA1000.h"

void TSJA1000::Open()
{
	if (!TMCUEmul::IsOpen())
	{
		TMCUEmul::Open();
		if(!TMCUEmul::IsOpen() || !TMCUEmul::IsConfigured)
			return;
	}
	
	if (TMCUEmul::IsOpen() && TMCUEmul::IsConfigured && !FStarted)
		SetupAndStart();
	
	if(FStarted)
		SetupEnevtAndThread();
}

void TSJA1000::Close()
{
    DestroyEnevtAndThread();

	TMCUEmul::Close();

	while(!FRxFrameBuffer.empty())
		FRxFrameBuffer.pop();

    FStarted = NOT_STARTED;
}

void TSJA1000::SetupAndStart()
{
	tCanAcceptance ac;
	
	TMCUEmul::Write((1<<RM_RR_Bit), ModeControlReg);     // enter to reset mode
	TMCUEmul::Write((1<<CANMode_Bit) | (1<<CBP_Bit)|(1<<ClkOff_Bit), ClockDivideReg); // Disble clkout
	TMCUEmul::Write(0x00, InterruptEnReg); // disable all interrupts

	SetBaud(FBaud); // set speed
	
	TMCUEmul::Write(Tx1Float | Tx0PshPull | NormalMode, OutControlReg);
	
	ac.Code[0] = 0x00;
	ac.Code[1] = 0x00;
	ac.Code[2] = 0x00;
	ac.Code[3] = 0x00;
	ac.Mask[0] = 0xFF;
	ac.Mask[1] = 0xFF;
	ac.Mask[2] = 0xFF;
	ac.Mask[3] = 0xFF;
	SetAcceptance(&ac);
	
	TMCUEmul::Write((1<<BEIE_Bit)|(1<<DOIE_Bit)|(1<<TIE_Bit)|(1<<RIE_Bit), InterruptEnReg);
	TMCUEmul::Write(0x00, ModeControlReg); // enter ro operating mode
	FStarted = IS_STARTED; // save state
}

void TSJA1000::SetCanMode(tCanMode cMode)
{
	unsigned char tr = TMCUEmul::Read(ClockDivideReg);
	
	switch (cMode)
	{
		case cmBasicCAN:
			tr &= ~(1<<CANMode_Bit);
		break;

		case cmPeliCAN:
			tr |= (1<<CANMode_Bit);
		break;
		
		default:
			break;
	}

	TMCUEmul::Write(tr, ModeControlReg);
	FCanMode = cMode; // save state
}

void TSJA1000::SetInternalMode(tInternalMode iMode)
{
	unsigned char tr = TMCUEmul::Read(ModeControlReg);
	
	switch (iMode)
	{
		case imReset:
			tr |= (1<<RM_RR_Bit);
		break;
		
		case imOperating:
			tr &=~(1<<RM_RR_Bit);
		break;
		
		case imListenOnly:
			tr |= (1<<LOM_Bit);
		break;
		
		case imNormalMode:
			tr &=~(1<<LOM_Bit);
		break;
		
		case imSelfTest:
			tr |= (1<<STM_Bit);
		break;
		
		case imNoSelfTest:
			tr &=~(1<<STM_Bit);
		break;
		
		case imAcceptanceFilter:
			tr |= (1<<AFM_Bit);
		break;
		
		case imNoFilter:
			tr &=~(1<<AFM_Bit);
		break;
		
		case imSleep:
			tr |= (1<<SM_Bit);
		break;
		
		case imWake:
			tr &=~(1<<SM_Bit);
		break;
		
		default:
			return;
	}
	
	TMCUEmul::Write(tr, ModeControlReg);
	FInternalMode = iMode; // save state
}

void TSJA1000::SetBaud(tCanSpeed cSpeed)
{
	unsigned char BT0 = 0, BT1 = 0;
	
	switch (cSpeed)
	{
		/*case cs10k:
			BT0 = 0x5d;
			BT1 = 0x2f;
		break;

		case cs20k:
			BT0 = 0x71;
			BT1 = 0x21;
		break;
        */
		case cs50k:
			BT0 = 0x49;
			BT1 = 0x54;
		break;
        /*
		case cs100k:
			BT0 = 0x49;
			BT1 = 0x21;
		break;*/

		case cs125k:
			BT0 = 0x42;
			BT1 = 0x76;
		break;

		case cs250k:
			BT0 = 0x41;
			BT1 = 0x27;
		break;

		case cs500k:
			BT0 = 0x41;
			BT1 = 0x21;
		break;

		case cs1m:
			BT0 = 0x40;
			BT1 = 0x21;
		break;
		
		default:
			return;
	}
	
	TMCUEmul::Write(BT0, BusTiming0Reg);
	TMCUEmul::Write(BT1, BusTiming1Reg);
	FBaud = cSpeed; // save state
}

void TSJA1000::SetAcceptance(tCanAcceptance *acc)
{
	TMCUEmul::Write(acc->Code[0], AcceptCode0Reg);
	TMCUEmul::Write(acc->Code[1], AcceptCode1Reg);
	TMCUEmul::Write(acc->Code[2], AcceptCode2Reg);
	TMCUEmul::Write(acc->Code[3], AcceptCode3Reg);
	
	TMCUEmul::Write(acc->Mask[0], AcceptMask0Reg);
	TMCUEmul::Write(acc->Mask[1], AcceptMask1Reg);
	TMCUEmul::Write(acc->Mask[2], AcceptMask2Reg);
	TMCUEmul::Write(acc->Mask[3], AcceptMask3Reg);
	
	*FAcceptance = *acc;
}

void TSJA1000::ReadFrame(tCanFrame *inFrame)
{
	ScopeSection();
	
	*inFrame = FRxFrameBuffer.front();
	FRxFrameBuffer.pop();
}

void TSJA1000::ReadFrameFromDev()
{
	unsigned char FrameInfo;
	tCanFrame tf;

    memset(tf.data, 0, 8);
	
	FrameInfo = TMCUEmul::Read(RxFrameInfo);
	
	tf.format = FrameInfo & (1<<FF_Bit) ? 1 : 0;
	tf.rtr = FrameInfo & (1<<RTR_Bit) ? 1 : 0;
	tf.len = FrameInfo & 0xff;
	
	if (!tf.format)
	{
		tf.id = ((unsigned int)TMCUEmul::Read(RxId1) << 3) & 0x7F8;
		tf.id |= ((unsigned int)TMCUEmul::Read(RxId2) >> 5) & 0x07;

        for(unsigned char i = 0, rxbuf = RxBuffer1; i < tf.len; i++, rxbuf++)
        	tf.data[i] = TMCUEmul::Read(rxbuf);
	}
	else
	{
		tf.id = ((unsigned int) TMCUEmul::Read(RxId1) << 21) & 0x1FE00000;
		tf.id |= ((unsigned int) TMCUEmul::Read(RxId2) << 13) & 0x1FE000;
		tf.id |= ((unsigned int) TMCUEmul::Read(RxId3) << 5) & 0x1FFE;
		tf.id |= ((unsigned int) TMCUEmul::Read(RxId4) >> 3) & 0x1F;

        for(unsigned char i = 0, rxbuf = RxBuffer1Ex; i < tf.len; i++, rxbuf++)
        	tf.data[i] = TMCUEmul::Read(rxbuf);
	}
	
	TMCUEmul::Write((1<<RRB_Bit), CommandReg); // release receive buffer

	ScopeSection();
	FRxFrameBuffer.push(tf);
}

void TSJA1000::WriteFrame(tCanFrame *outFrame)
{
	
}

void TSJA1000::SetupEnevtAndThread()
{
    ScopeSection();
    
	FRecvEvent.Start = CreateEvent(NULL, FALSE, FALSE, NULL);	// wait RI (start nonsignaled)
	FRecvEvent.Done = CreateEvent(NULL, FALSE, FALSE, NULL);	// wait frame read (start signaled)
	
	FInterruptThread.Terminated = STILL_THREAD;
	FInterruptThread.Addr.Method = &TSJA1000::InterruptHandler;
	FInterruptThread.Handle = CreateThread(NULL, 0, FInterruptThread.Addr.Func, this, 0, &FInterruptThread.Id);
	
	FRecvThread.Terminated = STILL_THREAD;
	FRecvThread.Addr.Method = &TSJA1000::ReadFrameHandler;
	FRecvThread.Handle = CreateThread(NULL, 0, FRecvThread.Addr.Func, this, 0, &FRecvThread.Id);
}

void TSJA1000::DestroyEnevtAndThread()
{
    ScopeSection();

    if (!FRecvThread.Terminated)
    {
    	FRecvThread.Terminated = TERMINATE_TREAD;
    	SetEvent(FRecvEvent.Start);
        if (WaitForSingleObject(FRecvThread.Handle, 1000) != WAIT_OBJECT_0)
	        TerminateThread(FRecvThread.Handle, STILL_ACTIVE);
    }

    if (FRecvThread.Handle)
    {
    	CloseHandle(FRecvThread.Handle);
    	FRecvThread.Handle = NULL;
    }

    if(!FInterruptThread.Terminated)
    {
    	FInterruptThread.Terminated = TERMINATE_TREAD;
    	SetEvent(FRecvEvent.Done);
    	if (WaitForSingleObject(FInterruptThread.Handle, 1000) != WAIT_OBJECT_0)
        	TerminateThread(FRecvThread.Handle, STILL_ACTIVE);
    }

    if(FInterruptThread.Handle)
    {
    	CloseHandle(FInterruptThread.Handle);
    	FInterruptThread.Handle = NULL;
    }

    CloseHandle(FRecvEvent.Start);
    FRecvEvent.Start = NULL;
    
    CloseHandle(FRecvEvent.Done);
    FRecvEvent.Done = NULL;
}

unsigned long TSJA1000::ReadFrameHandler(void*)
{
	while(!FRecvThread.Terminated)
	{
		WaitForSingleObject(FRecvEvent.Start, INFINITE);	// wait input frame
        if(FRecvThread.Terminated)
        	break;
		ReadFrameFromDev();	// read frame to buffer
		SetEvent(FRecvEvent.Done);	// set even to done read
	}
	
	return 1;
}

unsigned long TSJA1000::InterruptHandler(void*)
{
	unsigned char RegState = 0x00, IoState = 0x00;
	
	while(!FInterruptThread.Terminated)
	{
        IoState = TMCUEmul::ReadIO();
		if(!((IoState>>1) & 0x01))
		{
			RegState = TMCUEmul::Read(InterruptReg);
            FErrors = ERROR_NO;
			
			if((RegState>>BEI_Bit) & 0x01) // bus error interrupt
			{
            	FErrors |= ERROR_BE;
			}

			if((RegState>>DOI_Bit) & 0x01)	// data overrun interrupt
			{
				FErrors |= ERROR_DO;	
			}
			
			if((RegState>>RI_Bit) & 0x01) // receive interrupt
			{
                SetEvent(FRecvEvent.Start); // set event to start read frame
				WaitForSingleObject(FRecvEvent.Done, INFINITE); // wait while read done
			}
			
			if((RegState>>TI_Bit) & 0x01) // tramsmit interrupt
			{
				
			}
			
			RegState = 0x00;
		}
        else
        	if (IoState == 0x7F)
            	FErrors |= ERROR_FT;
        Sleep(0);
	}
	
	return 1;
}

TSJA1000::~TSJA1000()
{
	printf("~TSJA1000()\n");

	this->Close();

	if(FAcceptance)
		delete FAcceptance;

	if(this->FOperationAccss)
		delete this->FOperationAccss;
}