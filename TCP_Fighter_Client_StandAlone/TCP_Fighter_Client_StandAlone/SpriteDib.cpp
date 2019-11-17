#include "stdafx.h"
#include "SpriteDib.hpp"

CSpriteDib::CSpriteDib(int iMaxSprite, DWORD dwColorKey)
{
	m_iMaxSprite = iMaxSprite;
	m_dwColorKey = dwColorKey;

	//최대 읽어올 갯수 만큼 미리 할당 받는다.
	m_stpSprite = new st_SPRITE[iMaxSprite];
	memset(m_stpSprite, 0, (sizeof(st_SPRITE) * m_iMaxSprite));
}


CSpriteDib::~CSpriteDib()
{
	int iCnt;

	//전체를 돌면서 모두 지우자.
	for (iCnt = 0; iCnt > m_iMaxSprite; iCnt++)
	{
		ReleaseSprite(iCnt);
	}
}

bool CTestCode
{
	//This is Test Function.
}

//LoadDibSprite() BMP파일을 읽어서 하나의 프레임으로 저장한다.
bool CSpriteDib::LoadDibSprite(int iSpriteIndex, WCHAR* szFileName, int iCenterPointX, int iCenterPointY)
{
	HANDLE hFile;
	DWORD dwRead;
	
	int iPitch;
	//int iImageSize;
	DWORD dwPixelIndexSize = 0, dwFileSize = 0;
	BITMAPFILEHEADER stFileHeader;
	BITMAPINFOHEADER stInfoHeader;


	//비트맵 헤더를 열어 BMP파일 확인.
	hFile = CreateFile(szFileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
		return false;

	//fread_s(&stFileHeader, sizeof(BITMAPFILEHEADER), 1, sizeof(stFileHeader), hFile);
	//파일헤더 읽음.
	if (ReadFile(hFile, &stFileHeader, sizeof(BITMAPFILEHEADER), &dwRead, NULL) == 0)
	{
		CloseHandle(hFile);
		return false;
	}

	//파일헤더 타입 체크.
	if (stFileHeader.bfType != 0x4d42)
	{
		CloseHandle(hFile);
		return false;
	}

	//인포헤더 읽음.
	if (ReadFile(hFile, &stInfoHeader, sizeof(BITMAPINFOHEADER), &dwRead, NULL) == 0)
	{
		CloseHandle(hFile);
		return false;
	}

	//한줄, 한줄의 피치값을 구한다.
	iPitch = (((stInfoHeader.biBitCount >> 3) * stInfoHeader.biWidth) + 3) & ~3;
	
	//스프라이트 구조체에 크기 저장.
	m_stpSprite[iSpriteIndex].iWidth		= stInfoHeader.biWidth;
	m_stpSprite[iSpriteIndex].iHeight		= stInfoHeader.biHeight;
	m_stpSprite[iSpriteIndex].iPitch		= iPitch;
	m_stpSprite[iSpriteIndex].iCenterPointX	= iCenterPointX;
	m_stpSprite[iSpriteIndex].iCenterPointY	= iCenterPointY;

	//파일사이즈 할당.
	dwFileSize = stFileHeader.bfSize;

	//이미지에 대한 전체 크기를 구하고, 메모리 할당.
	if (stInfoHeader.biBitCount <= 8) //st_rgbQuad initialize. only use 8bit BITMAP Image.
		dwPixelIndexSize = dwFileSize - sizeof(BITMAPFILEHEADER) - sizeof(BITMAPINFOHEADER) - sizeof(RGBQUAD); //getDIBSize.
	else
		dwPixelIndexSize = dwFileSize - sizeof(BITMAPFILEHEADER) - sizeof(BITMAPINFOHEADER); //getDIBSize.

	m_stpSprite[iSpriteIndex].bypImage = new BYTE[dwPixelIndexSize];
	
	//이미지 부분은 스프라이트 버퍼로 읽어온다.
	//DIB는 뒤집어져있으므로 이를 다시 뒤집자.
	//임시 버퍼에 읽은 뒤에 뒤집으면서 복사한다.

	BYTE* bypTempBuffer = new BYTE[dwPixelIndexSize];
	BYTE* bypSpriteTemp = m_stpSprite[iSpriteIndex].bypImage;
	BYTE* bypTurnTemp;

	ReadFile(hFile, bypTempBuffer, dwPixelIndexSize, &dwRead, NULL);

	//한줄 한줄 뒤집자.
	bypTurnTemp = bypTempBuffer + iPitch * (stInfoHeader.biHeight - 1);
	for (int iCnt = 0; iCnt < stInfoHeader.biHeight; iCnt++)
	{
		memcpy(bypSpriteTemp, bypTurnTemp, iPitch);
		bypSpriteTemp = bypSpriteTemp + iPitch;
		bypTurnTemp = bypTurnTemp - iPitch;
	}
	delete[] bypTempBuffer;

	CloseHandle(hFile);
	return true;

}

//ReleaseSprite() 해당 스프라이트 해제.
void CSpriteDib::ReleaseSprite(int iSpriteIndex)
{

	//최대 할당 된 스프라이트를 넘어서면 안됨.
	if (m_iMaxSprite <= iSpriteIndex)
		return;

	if (NULL != m_stpSprite[iSpriteIndex].bypImage)
	{
		//삭제 후 초기화.
		delete[] m_stpSprite[iSpriteIndex].bypImage;
		memset(&m_stpSprite[iSpriteIndex], 0, sizeof(st_SPRITE));
	}
}

//DrawSprite() 특정 메모리 위치에 스프라이트를 출력한다. (ColorKey, Clipping)
void CSpriteDib::DrawSprite(int iSpriteIndex, int iDrawX, int iDrawY, BYTE* bypDest, int iDestWidth, int iDestHeight, int iDestPitch, int iDrawLen)
{
	//최대 스프라이트 개수를 초과하거나, 로드되지 않는 스프라이트라면 무시.
	if (m_iMaxSprite <= iSpriteIndex)
		return;

	//NULL체크.
	if (m_stpSprite[iSpriteIndex].bypImage == NULL)
		return;

	st_SPRITE* stpSprite = &m_stpSprite[iSpriteIndex];


	//사이즈 저장.
	int iSpriteWidth = stpSprite->iWidth;
	int iSpriteHeight = stpSprite->iHeight;
	int iCountY, iCountX;

	iSpriteWidth = iSpriteWidth * iDrawLen / 100;

	DWORD* dwpDest = (DWORD*)bypDest;
	DWORD* dwpSprite = (DWORD*)(stpSprite->bypImage);

	//출력 중점으로 좌표 계산.
	iDrawX = iDrawX - stpSprite->iCenterPointX;
	iDrawY = iDrawY - stpSprite->iCenterPointY;

	//상단 클리핑.
	if (0 > iDrawY)
	{
		iSpriteHeight = iSpriteHeight - (-iDrawY);
		dwpSprite = (DWORD*)(stpSprite->bypImage + stpSprite->iPitch * (-iDrawY));

		//위쪽이 짤리는 경우이므로 스프라이트 시작 위치를 아래로 내려준다.
		iDrawY = 0;
	}

	//하단 클리핑
	if (iDestHeight <= iDrawY + stpSprite->iHeight)
	{
		iSpriteHeight -= ((iDrawY + stpSprite->iHeight) - iDestHeight);
	}

	//좌측 클리핑
	if (0 > iDrawX)
	{
		iSpriteWidth = iSpriteWidth - (-iDrawX);
		dwpSprite = dwpSprite + (-iDrawX);

		//왼쪽이 잘리므로 출력 시작위치를 오른쪽으로 민다.
		iDrawX = 0;
	}

	//우측 클리핑
	if (iDestWidth <= iDrawX + stpSprite->iWidth)
	{
		iSpriteWidth -= ((iDrawX + stpSprite->iWidth) - iDestWidth);//////////
	}

	//찍을 그림이 없다면 종료.
	if (iSpriteWidth <= 0 || iSpriteHeight <= 0)
	{
		return;
	}

	//화면에 찍을 위치로 이동한다.
	dwpDest = (DWORD*)(((BYTE*)(dwpDest + iDrawX) + (iDrawY * iDestPitch)));


	BYTE* bypDestOrigin = (BYTE*)dwpDest;
	BYTE* bypSpriteOrigin = (BYTE*)dwpSprite;

	//전체 크기를 돌면서 픽셀마다 투명색 처리를 하며 그림 출력.
	for (iCountY = 0; iSpriteHeight > iCountY; iCountY++)
	{
		for (iCountX = 0; iSpriteWidth > iCountX; iCountX++)
		{
			if (m_dwColorKey != (*dwpSprite & 0x00ffffff)) //ColorKey가 아니면..
			{
				//픽셀 찍기.
				*dwpDest = *dwpSprite;
			}
			//다음 칸 이동.
			dwpDest++;
			dwpSprite++;
		}

		//다음줄로 이동. 화면과 스프라이트 모두..
		bypDestOrigin = bypDestOrigin + iDestPitch;
		bypSpriteOrigin = bypSpriteOrigin + stpSprite->iPitch;

		dwpDest = (DWORD*)bypDestOrigin;
		dwpSprite = (DWORD*)bypSpriteOrigin;

	}
}

//DrawImage() 특정 메모리 위치에 이미지를 출력한다. (Clipping)
void CSpriteDib::DrawImage(int iSpriteIndex, int iDrawX, int iDrawY, BYTE* bypDest, int iDestWidth, int iDestHeight, int iDestPitch, int iDrawLen)
{
	//최대 스프라이트 개수를 초과하거나, 로드되지 않는 스프라이트라면 무시.
	if (m_iMaxSprite <= iSpriteIndex)
		return;

	//NULL체크.
	if (m_stpSprite[iSpriteIndex].bypImage == NULL)
		return;

	st_SPRITE* stpSprite = &m_stpSprite[iSpriteIndex];


	//사이즈 저장.
	int iSpriteWidth = stpSprite->iWidth;
	int iSpriteHeight = stpSprite->iHeight;
	int iCountY;

	iSpriteWidth = iSpriteWidth * iDrawLen / 100;

	DWORD* dwpDest = (DWORD*)bypDest;
	DWORD* dwpSprite = (DWORD*)(stpSprite->bypImage);


	//상단 클리핑.
	if (0 > iDrawY)
	{
		iSpriteHeight = iSpriteHeight - (-iDrawY);
		dwpSprite = (DWORD*)(stpSprite->bypImage + stpSprite->iPitch * (-iDrawY));

		//위쪽이 짤리는 경우이므로 스프라이트 시작 위치를 아래로 내려준다.
		iDrawY = 0;
	}

	//하단 클리핑
	if (iDestHeight < iDrawY + stpSprite->iHeight)
	{
		iSpriteHeight -= ((iDrawY + stpSprite->iHeight) - iDestHeight);
	}

	//좌측 클리핑
	if (0 > iDrawX)
	{
		iSpriteWidth = iSpriteWidth - (-iDrawX);
		dwpSprite = dwpSprite + (-iDrawX);

		//왼쪽이 잘리므로 출력 시작위치를 오른쪽으로 민다.
		iDrawX = 0;
	}

	//우측 클리핑
	if (iDestWidth < iDrawX + stpSprite->iWidth)
	{
		iSpriteWidth -= ((iDrawX + stpSprite->iWidth) - iDestHeight);
	}

	//찍을 그림이 없다면 종료.
	if (iSpriteWidth <= 0 || iSpriteHeight <= 0)
	{
		return;
	}

	dwpDest = (DWORD*)(((BYTE*)(dwpDest + iDrawX) + (iDrawY * iDestPitch)));

	//전체 크기를 돌면서 복사한다.
	for (iCountY = 0; iSpriteHeight > iCountY ; iCountY++)
	{
		memcpy(dwpDest, dwpSprite, iSpriteWidth * 4);

		//다음줄로 이동.. 화면과 스프라이트 모두.
		dwpDest = (DWORD*)((BYTE*)dwpDest + iDestPitch);
		dwpSprite = (DWORD*)((BYTE*)dwpSprite + stpSprite->iPitch);
	}

}


//DrawSprite() 특정 메모리 위치에 스프라이트를 출력한다. (ColorKey, Clipping)
void CSpriteDib::DrawSpriteRED(int iSpriteIndex, int iDrawX, int iDrawY, BYTE* bypDest, int iDestWidth, int iDestHeight, int iDestPitch, int iDrawLen)
{
	//최대 스프라이트 개수를 초과하거나, 로드되지 않는 스프라이트라면 무시.
	if (m_iMaxSprite <= iSpriteIndex)
		return;

	//NULL체크.
	if (m_stpSprite[iSpriteIndex].bypImage == NULL)
		return;

	st_SPRITE* stpSprite = &m_stpSprite[iSpriteIndex];


	//사이즈 저장.
	int iSpriteWidth = stpSprite->iWidth;
	int iSpriteHeight = stpSprite->iHeight;
	int iCountY, iCountX;

	iSpriteWidth = iSpriteWidth * iDrawLen / 100;

	DWORD* dwpDest = (DWORD*)bypDest;
	DWORD* dwpSprite = (DWORD*)(stpSprite->bypImage);

	//출력 중점으로 좌표 계산.
	iDrawX = iDrawX - stpSprite->iCenterPointX;
	iDrawY = iDrawY - stpSprite->iCenterPointY;

	//상단 클리핑.
	if (0 > iDrawY)
	{
		iSpriteHeight = iSpriteHeight - (-iDrawY);
		dwpSprite = (DWORD*)(stpSprite->bypImage + stpSprite->iPitch * (-iDrawY));

		//위쪽이 짤리는 경우이므로 스프라이트 시작 위치를 아래로 내려준다.
		iDrawY = 0;
	}

	//하단 클리핑
	if (iDestHeight <= iDrawY + stpSprite->iHeight)
	{
		iSpriteHeight -= ((iDrawY + stpSprite->iHeight) - iDestHeight);
	}

	//좌측 클리핑
	if (0 > iDrawX)
	{
		iSpriteWidth = iSpriteWidth - (-iDrawX);
		dwpSprite = dwpSprite + (-iDrawX);

		//왼쪽이 잘리므로 출력 시작위치를 오른쪽으로 민다.
		iDrawX = 0;
	}

	//우측 클리핑
	if (iDestWidth <= iDrawX + stpSprite->iWidth)
	{
		iSpriteWidth -= ((iDrawX + stpSprite->iWidth) - iDestWidth);//////////
	}

	//찍을 그림이 없다면 종료.
	if (iSpriteWidth <= 0 || iSpriteHeight <= 0)
	{
		return;
	}

	//화면에 찍을 위치로 이동한다.
	dwpDest = (DWORD*)(((BYTE*)(dwpDest + iDrawX) + (iDrawY * iDestPitch)));


	BYTE* bypDestOrigin = (BYTE*)dwpDest;
	BYTE* bypSpriteOrigin = (BYTE*)dwpSprite;

	//전체 크기를 돌면서 픽셀마다 투명색 처리를 하며 그림 출력.
	for (iCountY = 0; iSpriteHeight > iCountY; iCountY++)
	{
		for (iCountX = 0; iSpriteWidth > iCountX; iCountX++)
		{
			if (m_dwColorKey != (*dwpSprite & 0x00ffffff)) //ColorKey가 아니면..
			{
				//픽셀 찍기.
				//*dwpDest = *dwpSprite;

				*((BYTE*)dwpDest + 0) = *((BYTE*)dwpSprite + 0);
				*((BYTE*)dwpDest + 1) = *((BYTE*)dwpSprite + 1);
				*((BYTE*)dwpDest + 2) = 0xff;
				*((BYTE*)dwpDest + 3) = *((BYTE*)dwpSprite + 3);
			}
			//다음 칸 이동.
			dwpDest++;
			dwpSprite++;
		}

		//다음줄로 이동. 화면과 스프라이트 모두..
		bypDestOrigin = bypDestOrigin + iDestPitch;
		bypSpriteOrigin = bypSpriteOrigin + stpSprite->iPitch;

		dwpDest = (DWORD*)bypDestOrigin;
		dwpSprite = (DWORD*)bypSpriteOrigin;

	}
}



//DrawSprite() 특정 메모리 위치에 스프라이트를 출력한다. (ColorKey, Clipping)
void CSpriteDib::DrawSpriteGREEN(int iSpriteIndex, int iDrawX, int iDrawY, BYTE* bypDest, int iDestWidth, int iDestHeight, int iDestPitch, int iDrawLen)
{
	//최대 스프라이트 개수를 초과하거나, 로드되지 않는 스프라이트라면 무시.
	if (m_iMaxSprite <= iSpriteIndex)
		return;

	//NULL체크.
	if (m_stpSprite[iSpriteIndex].bypImage == NULL)
		return;

	st_SPRITE* stpSprite = &m_stpSprite[iSpriteIndex];


	//사이즈 저장.
	int iSpriteWidth = stpSprite->iWidth;
	int iSpriteHeight = stpSprite->iHeight;
	int iCountY, iCountX;

	iSpriteWidth = iSpriteWidth * iDrawLen / 100;

	DWORD* dwpDest = (DWORD*)bypDest;
	DWORD* dwpSprite = (DWORD*)(stpSprite->bypImage);

	//출력 중점으로 좌표 계산.
	iDrawX = iDrawX - stpSprite->iCenterPointX;
	iDrawY = iDrawY - stpSprite->iCenterPointY;

	//상단 클리핑.
	if (0 > iDrawY)
	{
		iSpriteHeight = iSpriteHeight - (-iDrawY);
		dwpSprite = (DWORD*)(stpSprite->bypImage + stpSprite->iPitch * (-iDrawY));

		//위쪽이 짤리는 경우이므로 스프라이트 시작 위치를 아래로 내려준다.
		iDrawY = 0;
	}

	//하단 클리핑
	if (iDestHeight <= iDrawY + stpSprite->iHeight)
	{
		iSpriteHeight -= ((iDrawY + stpSprite->iHeight) - iDestHeight);
	}

	//좌측 클리핑
	if (0 > iDrawX)
	{
		iSpriteWidth = iSpriteWidth - (-iDrawX);
		dwpSprite = dwpSprite + (-iDrawX);

		//왼쪽이 잘리므로 출력 시작위치를 오른쪽으로 민다.
		iDrawX = 0;
	}

	//우측 클리핑
	if (iDestWidth <= iDrawX + stpSprite->iWidth)
	{
		iSpriteWidth -= ((iDrawX + stpSprite->iWidth) - iDestWidth);//////////
	}

	//찍을 그림이 없다면 종료.
	if (iSpriteWidth <= 0 || iSpriteHeight <= 0)
	{
		return;
	}

	//화면에 찍을 위치로 이동한다.
	dwpDest = (DWORD*)(((BYTE*)(dwpDest + iDrawX) + (iDrawY * iDestPitch)));


	BYTE* bypDestOrigin = (BYTE*)dwpDest;
	BYTE* bypSpriteOrigin = (BYTE*)dwpSprite;

	//전체 크기를 돌면서 픽셀마다 투명색 처리를 하며 그림 출력.
	for (iCountY = 0; iSpriteHeight > iCountY; iCountY++)
	{
		for (iCountX = 0; iSpriteWidth > iCountX; iCountX++)
		{
			if (m_dwColorKey != (*dwpSprite & 0x00ffffff)) //ColorKey가 아니면..
			{
				//픽셀 찍기.
				//*dwpDest = *dwpSprite;

				*((BYTE*)dwpDest + 0) = *((BYTE*)dwpSprite + 0);
				*((BYTE*)dwpDest + 1) = 0x99; 
				*((BYTE*)dwpDest + 2) = *((BYTE*)dwpSprite + 2);
				*((BYTE*)dwpDest + 3) = *((BYTE*)dwpSprite + 3);
			}
			//다음 칸 이동.
			dwpDest++;
			dwpSprite++;
		}

		//다음줄로 이동. 화면과 스프라이트 모두..
		bypDestOrigin = bypDestOrigin + iDestPitch;
		bypSpriteOrigin = bypSpriteOrigin + stpSprite->iPitch;

		dwpDest = (DWORD*)bypDestOrigin;
		dwpSprite = (DWORD*)bypSpriteOrigin;

	}
}


//DrawSprite() 특정 메모리 위치에 스프라이트를 출력한다. (ColorKey, Clipping)
void CSpriteDib::DrawSpriteBLUE(int iSpriteIndex, int iDrawX, int iDrawY, BYTE* bypDest, int iDestWidth, int iDestHeight, int iDestPitch, int iDrawLen)
{
	//최대 스프라이트 개수를 초과하거나, 로드되지 않는 스프라이트라면 무시.
	if (m_iMaxSprite <= iSpriteIndex)
		return;

	//NULL체크.
	if (m_stpSprite[iSpriteIndex].bypImage == NULL)
		return;

	st_SPRITE* stpSprite = &m_stpSprite[iSpriteIndex];


	//사이즈 저장.
	int iSpriteWidth = stpSprite->iWidth;
	int iSpriteHeight = stpSprite->iHeight;
	int iCountY, iCountX;

	iSpriteWidth = iSpriteWidth * iDrawLen / 100;

	DWORD* dwpDest = (DWORD*)bypDest;
	DWORD* dwpSprite = (DWORD*)(stpSprite->bypImage);

	//출력 중점으로 좌표 계산.
	iDrawX = iDrawX - stpSprite->iCenterPointX;
	iDrawY = iDrawY - stpSprite->iCenterPointY;

	//상단 클리핑.
	if (0 > iDrawY)
	{
		iSpriteHeight = iSpriteHeight - (-iDrawY);
		dwpSprite = (DWORD*)(stpSprite->bypImage + stpSprite->iPitch * (-iDrawY));

		//위쪽이 짤리는 경우이므로 스프라이트 시작 위치를 아래로 내려준다.
		iDrawY = 0;
	}

	//하단 클리핑
	if (iDestHeight <= iDrawY + stpSprite->iHeight)
	{
		iSpriteHeight -= ((iDrawY + stpSprite->iHeight) - iDestHeight);
	}

	//좌측 클리핑
	if (0 > iDrawX)
	{
		iSpriteWidth = iSpriteWidth - (-iDrawX);
		dwpSprite = dwpSprite + (-iDrawX);

		//왼쪽이 잘리므로 출력 시작위치를 오른쪽으로 민다.
		iDrawX = 0;
	}

	//우측 클리핑
	if (iDestWidth <= iDrawX + stpSprite->iWidth)
	{
		iSpriteWidth -= ((iDrawX + stpSprite->iWidth) - iDestWidth);//////////
	}

	//찍을 그림이 없다면 종료.
	if (iSpriteWidth <= 0 || iSpriteHeight <= 0)
	{
		return;
	}

	//화면에 찍을 위치로 이동한다.
	dwpDest = (DWORD*)(((BYTE*)(dwpDest + iDrawX) + (iDrawY * iDestPitch)));


	BYTE* bypDestOrigin = (BYTE*)dwpDest;
	BYTE* bypSpriteOrigin = (BYTE*)dwpSprite;

	//전체 크기를 돌면서 픽셀마다 투명색 처리를 하며 그림 출력.
	for (iCountY = 0; iSpriteHeight > iCountY; iCountY++)
	{
		for (iCountX = 0; iSpriteWidth > iCountX; iCountX++)
		{
			if (m_dwColorKey != (*dwpSprite & 0x00ffffff)) //ColorKey가 아니면..
			{
				//픽셀 찍기.
				//*dwpDest = *dwpSprite;

				*((BYTE*)dwpDest + 0) = 0xff; 
				*((BYTE*)dwpDest + 1) = *((BYTE*)dwpSprite + 1);
				*((BYTE*)dwpDest + 2) = *((BYTE*)dwpSprite + 2);
				*((BYTE*)dwpDest + 3) = *((BYTE*)dwpSprite + 3);
			}
			//다음 칸 이동.
			dwpDest++;
			dwpSprite++;
		}

		//다음줄로 이동. 화면과 스프라이트 모두..
		bypDestOrigin = bypDestOrigin + iDestPitch;
		bypSpriteOrigin = bypSpriteOrigin + stpSprite->iPitch;

		dwpDest = (DWORD*)bypDestOrigin;
		dwpSprite = (DWORD*)bypSpriteOrigin;

	}
}


//DrawSprite() 특정 메모리 위치에 스프라이트를 출력한다. (ColorKey, Clipping)
void CSpriteDib::DrawSpriteALPHA50(int iSpriteIndex, int iDrawX, int iDrawY, BYTE* bypDest, int iDestWidth, int iDestHeight, int iDestPitch, int iAlphaVal, int iDrawLen)
{
	//최대 스프라이트 개수를 초과하거나, 로드되지 않는 스프라이트라면 무시.
	if (m_iMaxSprite <= iSpriteIndex)
		return;

	//NULL체크.
	if (m_stpSprite[iSpriteIndex].bypImage == NULL)
		return;

	st_SPRITE* stpSprite = &m_stpSprite[iSpriteIndex];


	//사이즈 저장.
	int iSpriteWidth = stpSprite->iWidth;
	int iSpriteHeight = stpSprite->iHeight;
	int iCountY, iCountX;

	iSpriteWidth = iSpriteWidth * iDrawLen / 100;

	DWORD* dwpDest = (DWORD*)bypDest;
	DWORD* dwpSprite = (DWORD*)(stpSprite->bypImage);

	//출력 중점으로 좌표 계산.
	iDrawX = iDrawX - stpSprite->iCenterPointX;
	iDrawY = iDrawY - stpSprite->iCenterPointY;

	//상단 클리핑.
	if (0 > iDrawY)
	{
		iSpriteHeight = iSpriteHeight - (-iDrawY);
		dwpSprite = (DWORD*)(stpSprite->bypImage + stpSprite->iPitch * (-iDrawY));

		//위쪽이 짤리는 경우이므로 스프라이트 시작 위치를 아래로 내려준다.
		iDrawY = 0;
	}

	//하단 클리핑
	if (iDestHeight <= iDrawY + stpSprite->iHeight)
	{
		iSpriteHeight -= ((iDrawY + stpSprite->iHeight) - iDestHeight);
	}

	//좌측 클리핑
	if (0 > iDrawX)
	{
		iSpriteWidth = iSpriteWidth - (-iDrawX);
		dwpSprite = dwpSprite + (-iDrawX);

		//왼쪽이 잘리므로 출력 시작위치를 오른쪽으로 민다.
		iDrawX = 0;
	}

	//우측 클리핑
	if (iDestWidth <= iDrawX + stpSprite->iWidth)
	{
		iSpriteWidth -= ((iDrawX + stpSprite->iWidth) - iDestWidth);//////////
	}

	//찍을 그림이 없다면 종료.
	if (iSpriteWidth <= 0 || iSpriteHeight <= 0)
	{
		return;
	}

	//화면에 찍을 위치로 이동한다.
	dwpDest = (DWORD*)(((BYTE*)(dwpDest + iDrawX) + (iDrawY * iDestPitch)));


	BYTE* bypDestOrigin = (BYTE*)dwpDest;
	BYTE* bypSpriteOrigin = (BYTE*)dwpSprite;

	//전체 크기를 돌면서 픽셀마다 투명색 처리를 하며 그림 출력.
	for (iCountY = 0; iSpriteHeight > iCountY; iCountY++)
	{
		for (iCountX = 0; iSpriteWidth > iCountX; iCountX++)
		{
			if (m_dwColorKey != (*dwpSprite & 0x00ffffff)) //ColorKey가 아니면..
			{
				//픽셀 찍기.
				//*dwpDest = *dwpSprite;
				int iD = (*((BYTE*)dwpSprite + 0) + *((BYTE*)dwpSprite + 1) + *((BYTE*)dwpSprite + 2)) / iAlphaVal;
				*dwpDest = *(dwpDest + iD) & 0xf0f0f0f0;
				/*
				*((BYTE*)dwpDest + 0) = *((BYTE*)dwpSprite + 0);
				*((BYTE*)dwpDest + 1) = *((BYTE*)dwpSprite + 1);
				*((BYTE*)dwpDest + 2) = *((BYTE*)dwpSprite + 2);
				*((BYTE*)dwpDest + 3) = *((BYTE*)dwpSprite + 3);
				*/
			}
			//다음 칸 이동.
			dwpDest++;
			dwpSprite++;
		}

		//다음줄로 이동. 화면과 스프라이트 모두..
		bypDestOrigin = bypDestOrigin + iDestPitch;
		bypSpriteOrigin = bypSpriteOrigin + stpSprite->iPitch;

		dwpDest = (DWORD*)bypDestOrigin;
		dwpSprite = (DWORD*)bypSpriteOrigin;

	}
}
