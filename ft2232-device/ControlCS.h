/**
  ******************************************************************************
  * @file    ControlCS.h
  * @author  Khusainov Timur
  * @version 0.0.0.1
  * @date    12.01.2011
  * @brief   Auto critical section control
  ******************************************************************************
  * @attention
  * <h2><center>&copy; COPYRIGHT 2011 timypik@gmail.com </center></h2>
  ******************************************************************************
  */

#ifndef ControlCSH
#define	ControlCSH

class CAutoSection
{
	private:
		CRITICAL_SECTION m_CS;
	public:
		CAutoSection()
		{
			InitializeCriticalSection(&m_CS);
		}

        LPCRITICAL_SECTION GetCS()
        {
            return &m_CS;
        }

		~CAutoSection()
		{
			DeleteCriticalSection(&m_CS);
		}
};

class CSectionLock
{
	private:
		LPCRITICAL_SECTION	m_pCS;
	public:
		CSectionLock(LPCRITICAL_SECTION CriticalSection):m_pCS(CriticalSection)
		{
			EnterCriticalSection(m_pCS);
		}

        CSectionLock(CAutoSection *needlock):m_pCS(needlock->GetCS())
        {
            EnterCriticalSection(m_pCS);
        }

		~CSectionLock()
		{
			LeaveCriticalSection(m_pCS);
		}
};

#endif // 