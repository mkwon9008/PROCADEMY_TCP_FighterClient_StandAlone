#include "stdafx.h"
#include "ScreenDib.hpp"


CScreenDib::CScreenDib(int iWidth, int iHeight, int iColorBit)
{
	/*����  ������� �ʱ�ȭ.
	��ũ�� ���� ���� �Լ� ȣ��*/
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
	/*�Է¹��� ������ ������ �ɹ����� ���� ����, 
	BITMAPINFO(BITMAPINFOHEADER)�� �����Ѵ�.
	
	�̹��� ����� ����Ͽ� ���ۿ� �̹��� �����Ҵ�.*/
	//Dib��½� ����� ����ϱ� ���� ���̰��� - �� �Ҵ��ϰ� �ִ�.
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
	//�޸� ����~
	delete[] CScreenDib::m_bypBuffer;
}

void CScreenDib::DrawBuffer(HWND hWnd, int iX, int iY)
{
	/*�Է¹��� hWnd �ڵ��� DC�� �� DC�� X,Y��ġ�� ��ũ�� ���� DIB�� ����Ѵ�.*/
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