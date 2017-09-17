#include "stdafx.h"
#include "ScreenDib.hpp"


CScreenDib::CScreenDib(int iWidth, int iHeight, int iColorBit)
{
	/*각종  멤버변수 초기화.
	스크린 버퍼 생성 함수 호출*/
	InitBmSt();
	CreateDibBuffer(iWidth,iHeight,iColorBit);
}


CScreenDib::~CScreenDib()
{
	ReleaseDibBuffer();
}

void CScreenDib::InitBmSt(void)
{
	CScreenDib::m_stDibInfo.bmiHeader.biSize			= 0;
	CScreenDib::m_stDibInfo.bmiHeader.biBitCount		= 0;
	CScreenDib::m_stDibInfo.bmiHeader.biHeight			= 0;
	CScreenDib::m_stDibInfo.bmiHeader.biWidth			= 0;
	CScreenDib::m_stDibInfo.bmiHeader.biPlanes			= 0;
	CScreenDib::m_stDibInfo.bmiHeader.biXPelsPerMeter	= 0;
	CScreenDib::m_stDibInfo.bmiHeader.biYPelsPerMeter	= 0;
	CScreenDib::m_stDibInfo.bmiHeader.biClrUsed			= 0;
	CScreenDib::m_stDibInfo.bmiHeader.biClrImportant	= 0;
	CScreenDib::m_stDibInfo.bmiHeader.biCompression		= 0;
	CScreenDib::m_stDibInfo.bmiHeader.biSizeImage		= 0;

	CScreenDib::m_stDibInfo.bmiColors->rgbReserved	= 0;
	CScreenDib::m_stDibInfo.bmiColors->rgbRed		= 0;
	CScreenDib::m_stDibInfo.bmiColors->rgbGreen		= 0;
	CScreenDib::m_stDibInfo.bmiColors->rgbBlue		= 0;

	CScreenDib::m_iWidth		= 0;
	CScreenDib::m_iHeight		= 0;
	CScreenDib::m_iPitch		= 0;
	CScreenDib::m_iColorBit		= 0;
	CScreenDib::m_iBufferSize	= 0;

	CScreenDib::m_bypBuffer = 0;
}


void CScreenDib::CreateDibBuffer(int iWidth, int iHeight, int iColorBit)
{
	/*입력받은 인자의 정보로 맴버변수 정보 셋팅, 
	BITMAPINFO(BITMAPINFOHEADER)를 셋팅한다.
	
	이미지 사이즈를 계산하여 버퍼용 이미지 동적할당.*/
	//Dib출력시 뒤집어서 출력하기 위해 높이값을 - 로 할당하고 있다.
	m_stDibInfo.bmiHeader.biSize			= sizeof(BITMAPINFOHEADER);
	m_stDibInfo.bmiHeader.biBitCount		= iColorBit;
	m_stDibInfo.bmiHeader.biHeight			= -iHeight;
	m_stDibInfo.bmiHeader.biWidth			= iWidth;
	m_stDibInfo.bmiHeader.biPlanes			= 1;

	if (iColorBit <= 8)
	{
		m_stDibInfo.bmiColors->rgbReserved = 0x00;
		m_stDibInfo.bmiColors->rgbRed = 0x00;
		m_stDibInfo.bmiColors->rgbGreen = 0x00;
		m_stDibInfo.bmiColors->rgbBlue = 0x00;
	}

	CScreenDib::m_iHeight = iHeight;
	CScreenDib::m_iWidth = iWidth;
	CScreenDib::m_iColorBit = iColorBit;
	CScreenDib::m_iPitch = (((m_iColorBit >> 3) * m_iWidth) + 3) & ~3;
	CScreenDib::m_iBufferSize = (m_iPitch * m_iHeight);
	CScreenDib::m_bypBuffer = new BYTE[m_iBufferSize];

}
void CScreenDib::ReleaseDibBuffer(void)
{
	//메모리 해제~
	delete[] CScreenDib::m_bypBuffer;
}

void CScreenDib::DrawBuffer(HWND hWnd, int iX, int iY)
{
	/*입력받은 hWnd 핸들의 DC를 얻어서 DC의 X,Y위치에 스크린 버퍼 DIB를 출력한다.*/
	HDC hdc = GetDC(hWnd);
	StretchDIBits	(hdc, iX, iY, m_iWidth, m_iHeight,
							0, 0, m_iWidth, m_iHeight,
					(LPBITMAPINFO)m_bypBuffer, (BITMAPINFO*)&m_stDibInfo.bmiHeader, DIB_RGB_COLORS, SRCCOPY);
	ReleaseDC(hWnd, hdc);
}
BYTE* CScreenDib::GetDibBuffer(void)
{
	return m_bypBuffer;
}

int CScreenDib::GetWidth(void)
{
	return m_iWidth;
}
int CScreenDib::GetHeight(void)
{
	return m_iHeight;
}
int CScreenDib::GetPitch(void)
{
	return m_iPitch;
}