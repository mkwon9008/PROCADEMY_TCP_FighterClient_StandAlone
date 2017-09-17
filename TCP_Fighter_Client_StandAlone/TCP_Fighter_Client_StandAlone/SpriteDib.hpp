#pragma once
#ifndef __SPRITE_DIB_H__
#define __SPRITE_DIB_H__

#include <Windows.h>
#include <stdio.h>

class CSpriteDib
{
public:
	//DIB스프라이트 구조체. 스프라이트 이미지와 사이즈 정보를 가짐.
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

	//sprite 배열 정보.
	int	m_iMaxSprite;
	st_SPRITE* m_stpSprite;

	//투명색으로 사용할 color.
	DWORD m_dwColorKey;
	
public:
	//Constructor, Destroyer.
			 CSpriteDib(int iMaxSprite, DWORD dwColorKey);
	virtual ~CSpriteDib();

	//LoadDibSprite() BMP파일을 읽어서 하나의 프레임으로 저장한다.
	bool LoadDibSprite(int iSpriteIndex, WCHAR* szFileName, int iCenterPointX, int iCenterPointY);
	
	//ReleaseSprite() 해당 스프라이트 해제.
	void ReleaseSprite(int iSpriteIndex);

	//DrawSprite() 특정 메모리 위치에 스프라이트를 출력한다. (ColorKey, Clipping)
	void DrawSprite(int iSpriteIndex, int iDrawX, int iDrawY, BYTE* bypDest, int iDestWidth, int iDestHeight, int iDestPitch, int iDrawLen = 100);

	//DrawImage() 특정 메모리 위치에 이미지를 출력한다. (Clipping)
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