#pragma once
#ifndef __SPRITE_DIB_H__
#define __SPRITE_DIB_H__

#include <Windows.h>
#include <stdio.h>

class CSpriteDib
{
public:
	//DIB��������Ʈ ����ü. ��������Ʈ �̹����� ������ ������ ����.
	struct st_SPRITE
	{
		BYTE*	bypImage;
		int		iWidth;
		int		iHeight;
		int		iPitch;

		int		iCenterPointX;
		int		iCenterPointY;
	};


protected:

	//sprite �迭 ����.
	int	m_iMaxSprite;
	st_SPRITE* m_stpSprite;

	//��������� ����� color.
	DWORD m_dwColorKey;
	
public:
	//Constructor, Destroyer.
			 CSpriteDib(int iMaxSprite, DWORD dwColorKey);
	virtual ~CSpriteDib();

	//LoadDibSprite() BMP������ �о �ϳ��� ���������� �����Ѵ�.
	bool LoadDibSprite(int iSpriteIndex, WCHAR* szFileName, int iCenterPointX, int iCenterPointY);
	
	//ReleaseSprite() �ش� ��������Ʈ ����.
	void ReleaseSprite(int iSpriteIndex);

	//DrawSprite() Ư�� �޸� ��ġ�� ��������Ʈ�� ����Ѵ�. (ColorKey, Clipping)
	void DrawSprite(int iSpriteIndex, int iDrawX, int iDrawY, BYTE* bypDest, int iDestWidth, int iDestHeight, int iDestPitch, int iDrawLen = 100);

	//DrawImage() Ư�� �޸� ��ġ�� �̹����� ����Ѵ�. (Clipping)
	void DrawImage(int iSpriteIndex, int iDrawX, int iDrawY, BYTE* bypDest, int iDestWidth, int iDestHeight, int iDestPitch, int iDrawLen = 100);

	//RED SPRITE.
	void DrawSpriteRED(int iSpriteIndex, int iDrawX, int iDrawY, BYTE* bypDest, int iDestWidth, int iDestHeight, int iDestPitch, int iDrawLen = 100);
	
	//GREEN SPRITE.
	void DrawSpriteGREEN(int iSpriteIndex, int iDrawX, int iDrawY, BYTE* bypDest, int iDestWidth, int iDestHeight, int iDestPitch, int iDrawLen = 100);

	//BLUE SPRITE.
	void DrawSpriteBLUE(int iSpriteIndex, int iDrawX, int iDrawY, BYTE* bypDest, int iDestWidth, int iDestHeight, int iDestPitch, int iDrawLen = 100);

	//ALPHA SPRITE.
	void CSpriteDib::DrawSpriteALPHA50(int iSpriteIndex, int iDrawX, int iDrawY, BYTE* bypDest, int iDestWidth, int iDestHeight, int iDestPitch, int iAlphaVal = 5, int iDrawLen=100);
};

#endif