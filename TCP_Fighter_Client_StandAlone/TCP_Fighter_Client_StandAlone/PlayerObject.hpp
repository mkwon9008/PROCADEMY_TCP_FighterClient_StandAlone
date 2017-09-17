#pragma once
#ifndef __CPLAYER_OBJECT_H__
#define __CPLAYER_OBJECT_H__

#include "BaseObject.hpp"
#include "DefineEnum.hpp"
#include "SpriteDib.hpp"

class CPlayerObject : public CBaseObject
{
private:
	BOOL m_bPlayerCharacter;
	char m_chHP;
	DWORD m_dwActionCur;
	DWORD m_dwActionOld;
	int m_iDirCur;
	int m_iDirOld;

public:
	CPlayerObject();
	~CPlayerObject();

	DWORD Draw(BYTE* bypDest, int iDestWidth, int iDestHeight, int iDestPitch);
	
	virtual DWORD Action(DWORD dwParam);
	void ActionProc(void);
	void SetActionStand(void);
	void InputActionProc(void);

	int GetDirection(void);
	char GetHP(void);

	BOOL IsPlayer(void);

	void SetActionAttack1(DWORD Attack1);
	void SetActionAttack2(DWORD Attack2);
	void SetActionAttack3(DWORD Attack3);
	void SetActionMove(void);
	void SetDirection(int iDirCur);
	void SetHP(void);
};

extern CSpriteDib g_cSprite;
#endif