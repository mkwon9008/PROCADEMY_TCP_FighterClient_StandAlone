#include "stdafx.h"
#include "FrameSkip.hpp"


CFrameSkip::CFrameSkip()
{
	//dwBeginFrame 시작시간 측정.
	m_dwBeginFrame = (DWORD)GetTickCount64();
	m_dwInterval = 1000 / FPS;
	m_dwEndFrame = 0;
	m_dwFrameTick = 0;
	m_dwFrameSkipInterval = 0;
	m_iFrameCnt = 0;
}


CFrameSkip::~CFrameSkip()
{
}

BOOL CFrameSkip::isFrameSkip(void)
{
	//dwEndFrame Update()함수 끝난시간 측정.
	
	m_dwEndFrame = (DWORD)GetTickCount64();

	//끝난시간 - 시작시간 = 걸린시간.
	m_dwFrameTick = m_dwEndFrame - m_dwBeginFrame;
	
	WCHAR test[20];
	wsprintf(test, L"%d\n", m_dwFrameTick);
	OutputDebugStringW(test);

	//연산에 걸리는 속도가 20ms보다 많으면
	if (m_dwFrameTick > m_dwInterval)
	{
		//dwFrameSkipInterval에 dwFrameTick - dwInterval 만큼 저장.
		m_dwFrameSkipInterval = m_dwFrameSkipInterval + (m_dwFrameTick - m_dwInterval);
	}

	//dwInterval=20ms. 걸린시간이 20ms보다 적으면.
	if (m_dwInterval > m_dwFrameTick)
	{
		//차이나는 만큼 슬립.
		Sleep(m_dwInterval - m_dwFrameTick);

		//슬립한 시간을 빼고 다시 시간 구함.
		m_dwEndFrame = (DWORD)GetTickCount64();
	} 

	//다음 루프를 위해 저장.
	m_dwBeginFrame = m_dwEndFrame; 
	

	if (m_dwFrameSkipInterval > 20)
	{
		//dwFrameSkipInterval 에서 20ms 만큼 빼주고.
		m_dwFrameSkipInterval = m_dwFrameSkipInterval - 20;

		//프레임 스킵.
		m_iFrameCnt++;
		return true;
	}
	//프레임 카운트 증가.
	m_iFrameCnt++;

	return false;
}

int CFrameSkip::getFrameCnt(void)
{
	return m_iFrameCnt;
}

void CFrameSkip::ResetFrameCnt(void)
{
	m_iFrameCnt = 0;
}