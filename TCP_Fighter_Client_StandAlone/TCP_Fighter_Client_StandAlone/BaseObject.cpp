#include "stdafx.h"
#include "BaseObject.hpp"


CBaseObject::CBaseObject()
{
	//g_cSprite = new CSpriteDib(7, 0x00ffffff);
	
}


CBaseObject::~CBaseObject()
{
}

void CBaseObject::ActionInput(DWORD dwParam)
{
	m_dwActionInput = dwParam;
}

int CBaseObject::isEndFrame(void)
{
	return m_bEndFrame;
}
void CBaseObject::NextFrame(void)
{
	//����� ��������Ʈ�� �ִ��� üũ.
	if (0 > m_iSpriteStart)
	{
		return;
	}
	
	//������ ������ ���� �Ѿ�� ���� ���������� �Ѿ��.
	m_iDelayCount++;

	if (m_iDelayCount >= m_iFrameDelay)
	{
		m_iDelayCount = 0;
		//�ִϸ��̼� ������ �̵��Ǹ� ó������ ������.
		m_iSpriteNow++;

		if (m_iSpriteNow > m_iSpriteMax)
		{
			m_iSpriteNow = m_iSpriteStart;
			m_bEndFrame = true;
		}
	}
}

int CBaseObject::GetCurX(void)
{
	return m_iCurX;
}
int CBaseObject::GetCurY(void)
{
	return m_iCurY;
}
int CBaseObject::GetObjectID(void)
{
	return m_iObjectID;
}
int CBaseObject::GetObjectType(void)
{
	return m_iObjectType;
}
int CBaseObject::GetOldX(void)
{
	return m_iOldX;
}
int CBaseObject::GetOldY(void)
{
	return m_iOldY;
}
int CBaseObject::GetSprite(void)
{
	return m_iSpriteNow;
}


void CBaseObject::SetCurPosition(int iX, int iY)
{
	CBaseObject::m_iCurX = iX;
	CBaseObject::m_iCurY = iY;
}

void CBaseObject::SetOldPosition(int iX, int iY)
{
	CBaseObject::m_iOldX = iX;
	CBaseObject::m_iOldY = iY;
}

void CBaseObject::SetPosition(int iX, int iY)
{

}

void CBaseObject::SetObjectID(int iObjectID)
{
	CBaseObject::m_iObjectID = iObjectID;
}

void CBaseObject::SetObjectType(int iObjectType)
{
	CBaseObject::m_iObjectType = iObjectType;
}

void CBaseObject::SetSprite(int iSpriteStart, int iSpriteMax, int iFrameDelay)
{
	m_iSpriteStart = iSpriteStart;
	m_iSpriteMax = iSpriteMax;
	m_iFrameDelay = iFrameDelay;
	m_iDelayCount = 0;
	m_iSpriteNow = iSpriteStart;
	
	m_bEndFrame = false;
	
}
