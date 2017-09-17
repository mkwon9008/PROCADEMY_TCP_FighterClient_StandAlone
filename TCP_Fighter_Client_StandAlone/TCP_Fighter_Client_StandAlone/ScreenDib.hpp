#pragma once

#ifndef __SCREEN_DIB_H__
#define __SCREEN_DIB_H__

class CScreenDib
{
protected:
	BITMAPINFO m_stDibInfo;
	BYTE* m_bypBuffer;

	int m_iWidth;
	int m_iHeight;
	int m_iPitch; //4바이트 정렬된 한 줄의 바이트 수.
	int m_iColorBit;
	int m_iBufferSize;

	void CreateDibBuffer(int iWidth, int iHeight, int iColorBit);
	void ReleaseDibBuffer(void);
		

public:
	//Constructor & Destroyer.
			CScreenDib(int iWidth, int iHeight, int iColorBit);
	virtual ~CScreenDib();

	void InitBmSt(void);
	void DrawBuffer(HWND hWnd, int iX = 0, int iY = 0);
	BYTE* GetDibBuffer(void);
	
	int GetWidth(void);
	int GetHeight(void);
	int GetPitch(void);
	
};

#endif