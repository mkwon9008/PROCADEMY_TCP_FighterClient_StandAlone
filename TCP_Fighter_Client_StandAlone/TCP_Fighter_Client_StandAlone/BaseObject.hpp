#pragma once
#ifndef __CBASE_OBJECT_H__
#define __CBASE_OBJECT_H__

class CBaseObject
{
protected:

	BOOL m_bEndFrame;
	DWORD m_dwActionInput;
	int m_iCurX;
	int m_iCurY;
	int m_iDelayCount;
	int m_iFrameDelay;
	int m_iObjectID;
	int m_iObjectType;
	int m_iOldX;
	int m_iOldY;
	int m_iSpriteEnd;
	int m_iSpriteNow;
	int m_iSpriteStart;
	int m_iSpriteMax;
	
public:
	CBaseObject();
	~CBaseObject();

	virtual DWORD Action(DWORD dwParam) = 0;
	virtual DWORD Draw(BYTE* bypDest, int iDestWidth, int iDestHeight, int iDestPitch) = 0;

	void ActionInput(DWORD dwParam);

	int isEndFrame(void);
	void NextFrame(void);

	int GetCurX(void);
	int GetCurY(void);
	int GetObjectID(void);
	int GetObjectType(void);
	int GetOldX(void);
	int GetOldY(void);
	int GetSprite(void);
	
	void SetCurPosition(int iX, int iY);
	void SetObjectID(int iObjectID);
	void SetObjectType(int iObjectType);
	void SetOldPosition(int iX, int iY);
	void SetPosition(int iX, int iY);
	void SetSprite(int iSpriteStart, int iSpriteMax, int iFrameDelay);
	
};

struct YposSort
{
	//연산자 () 재정의. struct를 선언해서 class밖에서 따로 선언해줘야 한다.
	bool operator() (CBaseObject* pCBaseObject1, CBaseObject* pCBaseObject2) const
	{
		if (pCBaseObject1->GetCurY() < pCBaseObject2->GetCurY())
			return true;

		return false;
	}
	//사용할때는 g_listObj.sort(YposSort()); 이렇게 선언해줘야함.
};
#endif