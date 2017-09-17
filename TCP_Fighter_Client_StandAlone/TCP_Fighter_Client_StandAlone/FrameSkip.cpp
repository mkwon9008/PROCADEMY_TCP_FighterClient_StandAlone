#include "stdafx.h"
#include "FrameSkip.hpp"


CFrameSkip::CFrameSkip()
{
	//dwBeginFrame ���۽ð� ����.
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
	//dwEndFrame Update()�Լ� �����ð� ����.
	
	m_dwEndFrame = (DWORD)GetTickCount64();

	//�����ð� - ���۽ð� = �ɸ��ð�.
	m_dwFrameTick = m_dwEndFrame - m_dwBeginFrame;
	
	WCHAR test[20];
	wsprintf(test, L"%d\n", m_dwFrameTick);
	OutputDebugStringW(test);

	//���꿡 �ɸ��� �ӵ��� 20ms���� ������
	if (m_dwFrameTick > m_dwInterval)
	{
		//dwFrameSkipInterval�� dwFrameTick - dwInterval ��ŭ ����.
		m_dwFrameSkipInterval = m_dwFrameSkipInterval + (m_dwFrameTick - m_dwInterval);
	}

	//dwInterval=20ms. �ɸ��ð��� 20ms���� ������.
	if (m_dwInterval > m_dwFrameTick)
	{
		//���̳��� ��ŭ ����.
		Sleep(m_dwInterval - m_dwFrameTick);

		//������ �ð��� ���� �ٽ� �ð� ����.
		m_dwEndFrame = (DWORD)GetTickCount64();
	} 

	//���� ������ ���� ����.
	m_dwBeginFrame = m_dwEndFrame; 
	

	if (m_dwFrameSkipInterval > 20)
	{
		//dwFrameSkipInterval ���� 20ms ��ŭ ���ְ�.
		m_dwFrameSkipInterval = m_dwFrameSkipInterval - 20;

		//������ ��ŵ.
		m_iFrameCnt++;
		return true;
	}
	//������ ī��Ʈ ����.
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