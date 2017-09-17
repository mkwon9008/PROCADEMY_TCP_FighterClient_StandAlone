#include "stdafx.h"
#include "SpriteDib.hpp"

CSpriteDib::CSpriteDib(int iMaxSprite, DWORD dwColorKey)
{
	m_iMaxSprite = iMaxSprite;
	m_dwColorKey = dwColorKey;

	//�ִ� �о�� ���� ��ŭ �̸� �Ҵ� �޴´�.
	m_stpSprite = new st_SPRITE[iMaxSprite];
	memset(m_stpSprite, 0, (sizeof(st_SPRITE) * m_iMaxSprite));
}


CSpriteDib::~CSpriteDib()
{
	int iCnt;

	//��ü�� ���鼭 ��� ������.
	for (iCnt = 0; iCnt > m_iMaxSprite; iCnt++)
	{
		ReleaseSprite(iCnt);
	}
}


//LoadDibSprite() BMP������ �о �ϳ��� ���������� �����Ѵ�.
bool CSpriteDib::LoadDibSprite(int iSpriteIndex, WCHAR* szFileName, int iCenterPointX, int iCenterPointY)
{
	HANDLE hFile;
	DWORD dwRead;
	
	int iPitch;
	//int iImageSize;
	DWORD dwPixelIndexSize = 0, dwFileSize = 0;
	BITMAPFILEHEADER stFileHeader;
	BITMAPINFOHEADER stInfoHeader;


	//��Ʈ�� ����� ���� BMP���� Ȯ��.
	hFile = CreateFile(szFileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
		return false;

	//fread_s(&stFileHeader, sizeof(BITMAPFILEHEADER), 1, sizeof(stFileHeader), hFile);
	//������� ����.
	if (ReadFile(hFile, &stFileHeader, sizeof(BITMAPFILEHEADER), &dwRead, NULL) == 0)
	{
		CloseHandle(hFile);
		return false;
	}

	//������� Ÿ�� üũ.
	if (stFileHeader.bfType != 0x4d42)
	{
		CloseHandle(hFile);
		return false;
	}

	//������� ����.
	if (ReadFile(hFile, &stInfoHeader, sizeof(BITMAPINFOHEADER), &dwRead, NULL) == 0)
	{
		CloseHandle(hFile);
		return false;
	}

	//����, ������ ��ġ���� ���Ѵ�.
	iPitch = (((stInfoHeader.biBitCount >> 3) * stInfoHeader.biWidth) + 3) & ~3;
	
	//��������Ʈ ����ü�� ũ�� ����.
	m_stpSprite[iSpriteIndex].iWidth		= stInfoHeader.biWidth;
	m_stpSprite[iSpriteIndex].iHeight		= stInfoHeader.biHeight;
	m_stpSprite[iSpriteIndex].iPitch		= iPitch;
	m_stpSprite[iSpriteIndex].iCenterPointX	= iCenterPointX;
	m_stpSprite[iSpriteIndex].iCenterPointY	= iCenterPointY;

	//���ϻ����� �Ҵ�.
	dwFileSize = stFileHeader.bfSize;

	//�̹����� ���� ��ü ũ�⸦ ���ϰ�, �޸� �Ҵ�.
	if (stInfoHeader.biBitCount <= 8) //st_rgbQuad initialize. only use 8bit BITMAP Image.
		dwPixelIndexSize = dwFileSize - sizeof(BITMAPFILEHEADER) - sizeof(BITMAPINFOHEADER) - sizeof(RGBQUAD); //getDIBSize.
	else
		dwPixelIndexSize = dwFileSize - sizeof(BITMAPFILEHEADER) - sizeof(BITMAPINFOHEADER); //getDIBSize.

	m_stpSprite[iSpriteIndex].bypImage = new BYTE[dwPixelIndexSize];
	
	//�̹��� �κ��� ��������Ʈ ���۷� �о�´�.
	//DIB�� �������������Ƿ� �̸� �ٽ� ������.
	//�ӽ� ���ۿ� ���� �ڿ� �������鼭 �����Ѵ�.

	BYTE* bypTempBuffer = new BYTE[dwPixelIndexSize];
	BYTE* bypSpriteTemp = m_stpSprite[iSpriteIndex].bypImage;
	BYTE* bypTurnTemp;

	ReadFile(hFile, bypTempBuffer, dwPixelIndexSize, &dwRead, NULL);

	//���� ���� ������.
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

//ReleaseSprite() �ش� ��������Ʈ ����.
void CSpriteDib::ReleaseSprite(int iSpriteIndex)
{

	//�ִ� �Ҵ� �� ��������Ʈ�� �Ѿ�� �ȵ�.
	if (m_iMaxSprite <= iSpriteIndex)
		return;

	if (NULL != m_stpSprite[iSpriteIndex].bypImage)
	{
		//���� �� �ʱ�ȭ.
		delete[] m_stpSprite[iSpriteIndex].bypImage;
		memset(&m_stpSprite[iSpriteIndex], 0, sizeof(st_SPRITE));
	}
}

//DrawSprite() Ư�� �޸� ��ġ�� ��������Ʈ�� ����Ѵ�. (ColorKey, Clipping)
void CSpriteDib::DrawSprite(int iSpriteIndex, int iDrawX, int iDrawY, BYTE* bypDest, int iDestWidth, int iDestHeight, int iDestPitch, int iDrawLen)
{
	//�ִ� ��������Ʈ ������ �ʰ��ϰų�, �ε���� �ʴ� ��������Ʈ��� ����.
	if (m_iMaxSprite <= iSpriteIndex)
		return;

	//NULLüũ.
	if (m_stpSprite[iSpriteIndex].bypImage == NULL)
		return;

	st_SPRITE* stpSprite = &m_stpSprite[iSpriteIndex];


	//������ ����.
	int iSpriteWidth = stpSprite->iWidth;
	int iSpriteHeight = stpSprite->iHeight;
	int iCountY, iCountX;

	iSpriteWidth = iSpriteWidth * iDrawLen / 100;

	DWORD* dwpDest = (DWORD*)bypDest;
	DWORD* dwpSprite = (DWORD*)(stpSprite->bypImage);

	//��� �������� ��ǥ ���.
	iDrawX = iDrawX - stpSprite->iCenterPointX;
	iDrawY = iDrawY - stpSprite->iCenterPointY;

	//��� Ŭ����.
	if (0 > iDrawY)
	{
		iSpriteHeight = iSpriteHeight - (-iDrawY);
		dwpSprite = (DWORD*)(stpSprite->bypImage + stpSprite->iPitch * (-iDrawY));

		//������ ©���� ����̹Ƿ� ��������Ʈ ���� ��ġ�� �Ʒ��� �����ش�.
		iDrawY = 0;
	}

	//�ϴ� Ŭ����
	if (iDestHeight <= iDrawY + stpSprite->iHeight)
	{
		iSpriteHeight -= ((iDrawY + stpSprite->iHeight) - iDestHeight);
	}

	//���� Ŭ����
	if (0 > iDrawX)
	{
		iSpriteWidth = iSpriteWidth - (-iDrawX);
		dwpSprite = dwpSprite + (-iDrawX);

		//������ �߸��Ƿ� ��� ������ġ�� ���������� �δ�.
		iDrawX = 0;
	}

	//���� Ŭ����
	if (iDestWidth <= iDrawX + stpSprite->iWidth)
	{
		iSpriteWidth -= ((iDrawX + stpSprite->iWidth) - iDestWidth);//////////
	}

	//���� �׸��� ���ٸ� ����.
	if (iSpriteWidth <= 0 || iSpriteHeight <= 0)
	{
		return;
	}

	//ȭ�鿡 ���� ��ġ�� �̵��Ѵ�.
	dwpDest = (DWORD*)(((BYTE*)(dwpDest + iDrawX) + (iDrawY * iDestPitch)));


	BYTE* bypDestOrigin = (BYTE*)dwpDest;
	BYTE* bypSpriteOrigin = (BYTE*)dwpSprite;

	//��ü ũ�⸦ ���鼭 �ȼ����� ����� ó���� �ϸ� �׸� ���.
	for (iCountY = 0; iSpriteHeight > iCountY; iCountY++)
	{
		for (iCountX = 0; iSpriteWidth > iCountX; iCountX++)
		{
			if (m_dwColorKey != (*dwpSprite & 0x00ffffff)) //ColorKey�� �ƴϸ�..
			{
				//�ȼ� ���.
				*dwpDest = *dwpSprite;
			}
			//���� ĭ �̵�.
			dwpDest++;
			dwpSprite++;
		}

		//�����ٷ� �̵�. ȭ��� ��������Ʈ ���..
		bypDestOrigin = bypDestOrigin + iDestPitch;
		bypSpriteOrigin = bypSpriteOrigin + stpSprite->iPitch;

		dwpDest = (DWORD*)bypDestOrigin;
		dwpSprite = (DWORD*)bypSpriteOrigin;

	}
}

//DrawImage() Ư�� �޸� ��ġ�� �̹����� ����Ѵ�. (Clipping)
void CSpriteDib::DrawImage(int iSpriteIndex, int iDrawX, int iDrawY, BYTE* bypDest, int iDestWidth, int iDestHeight, int iDestPitch, int iDrawLen)
{
	//�ִ� ��������Ʈ ������ �ʰ��ϰų�, �ε���� �ʴ� ��������Ʈ��� ����.
	if (m_iMaxSprite <= iSpriteIndex)
		return;

	//NULLüũ.
	if (m_stpSprite[iSpriteIndex].bypImage == NULL)
		return;

	st_SPRITE* stpSprite = &m_stpSprite[iSpriteIndex];


	//������ ����.
	int iSpriteWidth = stpSprite->iWidth;
	int iSpriteHeight = stpSprite->iHeight;
	int iCountY;

	iSpriteWidth = iSpriteWidth * iDrawLen / 100;

	DWORD* dwpDest = (DWORD*)bypDest;
	DWORD* dwpSprite = (DWORD*)(stpSprite->bypImage);


	//��� Ŭ����.
	if (0 > iDrawY)
	{
		iSpriteHeight = iSpriteHeight - (-iDrawY);
		dwpSprite = (DWORD*)(stpSprite->bypImage + stpSprite->iPitch * (-iDrawY));

		//������ ©���� ����̹Ƿ� ��������Ʈ ���� ��ġ�� �Ʒ��� �����ش�.
		iDrawY = 0;
	}

	//�ϴ� Ŭ����
	if (iDestHeight < iDrawY + stpSprite->iHeight)
	{
		iSpriteHeight -= ((iDrawY + stpSprite->iHeight) - iDestHeight);
	}

	//���� Ŭ����
	if (0 > iDrawX)
	{
		iSpriteWidth = iSpriteWidth - (-iDrawX);
		dwpSprite = dwpSprite + (-iDrawX);

		//������ �߸��Ƿ� ��� ������ġ�� ���������� �δ�.
		iDrawX = 0;
	}

	//���� Ŭ����
	if (iDestWidth < iDrawX + stpSprite->iWidth)
	{
		iSpriteWidth -= ((iDrawX + stpSprite->iWidth) - iDestHeight);
	}

	//���� �׸��� ���ٸ� ����.
	if (iSpriteWidth <= 0 || iSpriteHeight <= 0)
	{
		return;
	}

	dwpDest = (DWORD*)(((BYTE*)(dwpDest + iDrawX) + (iDrawY * iDestPitch)));

	//��ü ũ�⸦ ���鼭 �����Ѵ�.
	for (iCountY = 0; iSpriteHeight > iCountY ; iCountY++)
	{
		memcpy(dwpDest, dwpSprite, iSpriteWidth * 4);

		//�����ٷ� �̵�.. ȭ��� ��������Ʈ ���.
		dwpDest = (DWORD*)((BYTE*)dwpDest + iDestPitch);
		dwpSprite = (DWORD*)((BYTE*)dwpSprite + stpSprite->iPitch);
	}

}


//DrawSprite() Ư�� �޸� ��ġ�� ��������Ʈ�� ����Ѵ�. (ColorKey, Clipping)
void CSpriteDib::DrawSpriteRED(int iSpriteIndex, int iDrawX, int iDrawY, BYTE* bypDest, int iDestWidth, int iDestHeight, int iDestPitch, int iDrawLen)
{
	//�ִ� ��������Ʈ ������ �ʰ��ϰų�, �ε���� �ʴ� ��������Ʈ��� ����.
	if (m_iMaxSprite <= iSpriteIndex)
		return;

	//NULLüũ.
	if (m_stpSprite[iSpriteIndex].bypImage == NULL)
		return;

	st_SPRITE* stpSprite = &m_stpSprite[iSpriteIndex];


	//������ ����.
	int iSpriteWidth = stpSprite->iWidth;
	int iSpriteHeight = stpSprite->iHeight;
	int iCountY, iCountX;

	iSpriteWidth = iSpriteWidth * iDrawLen / 100;

	DWORD* dwpDest = (DWORD*)bypDest;
	DWORD* dwpSprite = (DWORD*)(stpSprite->bypImage);

	//��� �������� ��ǥ ���.
	iDrawX = iDrawX - stpSprite->iCenterPointX;
	iDrawY = iDrawY - stpSprite->iCenterPointY;

	//��� Ŭ����.
	if (0 > iDrawY)
	{
		iSpriteHeight = iSpriteHeight - (-iDrawY);
		dwpSprite = (DWORD*)(stpSprite->bypImage + stpSprite->iPitch * (-iDrawY));

		//������ ©���� ����̹Ƿ� ��������Ʈ ���� ��ġ�� �Ʒ��� �����ش�.
		iDrawY = 0;
	}

	//�ϴ� Ŭ����
	if (iDestHeight <= iDrawY + stpSprite->iHeight)
	{
		iSpriteHeight -= ((iDrawY + stpSprite->iHeight) - iDestHeight);
	}

	//���� Ŭ����
	if (0 > iDrawX)
	{
		iSpriteWidth = iSpriteWidth - (-iDrawX);
		dwpSprite = dwpSprite + (-iDrawX);

		//������ �߸��Ƿ� ��� ������ġ�� ���������� �δ�.
		iDrawX = 0;
	}

	//���� Ŭ����
	if (iDestWidth <= iDrawX + stpSprite->iWidth)
	{
		iSpriteWidth -= ((iDrawX + stpSprite->iWidth) - iDestWidth);//////////
	}

	//���� �׸��� ���ٸ� ����.
	if (iSpriteWidth <= 0 || iSpriteHeight <= 0)
	{
		return;
	}

	//ȭ�鿡 ���� ��ġ�� �̵��Ѵ�.
	dwpDest = (DWORD*)(((BYTE*)(dwpDest + iDrawX) + (iDrawY * iDestPitch)));


	BYTE* bypDestOrigin = (BYTE*)dwpDest;
	BYTE* bypSpriteOrigin = (BYTE*)dwpSprite;

	//��ü ũ�⸦ ���鼭 �ȼ����� ����� ó���� �ϸ� �׸� ���.
	for (iCountY = 0; iSpriteHeight > iCountY; iCountY++)
	{
		for (iCountX = 0; iSpriteWidth > iCountX; iCountX++)
		{
			if (m_dwColorKey != (*dwpSprite & 0x00ffffff)) //ColorKey�� �ƴϸ�..
			{
				//�ȼ� ���.
				//*dwpDest = *dwpSprite;

				*((BYTE*)dwpDest + 0) = *((BYTE*)dwpSprite + 0);
				*((BYTE*)dwpDest + 1) = *((BYTE*)dwpSprite + 1);
				*((BYTE*)dwpDest + 2) = 0xff;
				*((BYTE*)dwpDest + 3) = *((BYTE*)dwpSprite + 3);
			}
			//���� ĭ �̵�.
			dwpDest++;
			dwpSprite++;
		}

		//�����ٷ� �̵�. ȭ��� ��������Ʈ ���..
		bypDestOrigin = bypDestOrigin + iDestPitch;
		bypSpriteOrigin = bypSpriteOrigin + stpSprite->iPitch;

		dwpDest = (DWORD*)bypDestOrigin;
		dwpSprite = (DWORD*)bypSpriteOrigin;

	}
}



//DrawSprite() Ư�� �޸� ��ġ�� ��������Ʈ�� ����Ѵ�. (ColorKey, Clipping)
void CSpriteDib::DrawSpriteGREEN(int iSpriteIndex, int iDrawX, int iDrawY, BYTE* bypDest, int iDestWidth, int iDestHeight, int iDestPitch, int iDrawLen)
{
	//�ִ� ��������Ʈ ������ �ʰ��ϰų�, �ε���� �ʴ� ��������Ʈ��� ����.
	if (m_iMaxSprite <= iSpriteIndex)
		return;

	//NULLüũ.
	if (m_stpSprite[iSpriteIndex].bypImage == NULL)
		return;

	st_SPRITE* stpSprite = &m_stpSprite[iSpriteIndex];


	//������ ����.
	int iSpriteWidth = stpSprite->iWidth;
	int iSpriteHeight = stpSprite->iHeight;
	int iCountY, iCountX;

	iSpriteWidth = iSpriteWidth * iDrawLen / 100;

	DWORD* dwpDest = (DWORD*)bypDest;
	DWORD* dwpSprite = (DWORD*)(stpSprite->bypImage);

	//��� �������� ��ǥ ���.
	iDrawX = iDrawX - stpSprite->iCenterPointX;
	iDrawY = iDrawY - stpSprite->iCenterPointY;

	//��� Ŭ����.
	if (0 > iDrawY)
	{
		iSpriteHeight = iSpriteHeight - (-iDrawY);
		dwpSprite = (DWORD*)(stpSprite->bypImage + stpSprite->iPitch * (-iDrawY));

		//������ ©���� ����̹Ƿ� ��������Ʈ ���� ��ġ�� �Ʒ��� �����ش�.
		iDrawY = 0;
	}

	//�ϴ� Ŭ����
	if (iDestHeight <= iDrawY + stpSprite->iHeight)
	{
		iSpriteHeight -= ((iDrawY + stpSprite->iHeight) - iDestHeight);
	}

	//���� Ŭ����
	if (0 > iDrawX)
	{
		iSpriteWidth = iSpriteWidth - (-iDrawX);
		dwpSprite = dwpSprite + (-iDrawX);

		//������ �߸��Ƿ� ��� ������ġ�� ���������� �δ�.
		iDrawX = 0;
	}

	//���� Ŭ����
	if (iDestWidth <= iDrawX + stpSprite->iWidth)
	{
		iSpriteWidth -= ((iDrawX + stpSprite->iWidth) - iDestWidth);//////////
	}

	//���� �׸��� ���ٸ� ����.
	if (iSpriteWidth <= 0 || iSpriteHeight <= 0)
	{
		return;
	}

	//ȭ�鿡 ���� ��ġ�� �̵��Ѵ�.
	dwpDest = (DWORD*)(((BYTE*)(dwpDest + iDrawX) + (iDrawY * iDestPitch)));


	BYTE* bypDestOrigin = (BYTE*)dwpDest;
	BYTE* bypSpriteOrigin = (BYTE*)dwpSprite;

	//��ü ũ�⸦ ���鼭 �ȼ����� ����� ó���� �ϸ� �׸� ���.
	for (iCountY = 0; iSpriteHeight > iCountY; iCountY++)
	{
		for (iCountX = 0; iSpriteWidth > iCountX; iCountX++)
		{
			if (m_dwColorKey != (*dwpSprite & 0x00ffffff)) //ColorKey�� �ƴϸ�..
			{
				//�ȼ� ���.
				//*dwpDest = *dwpSprite;

				*((BYTE*)dwpDest + 0) = *((BYTE*)dwpSprite + 0);
				*((BYTE*)dwpDest + 1) = 0x99; 
				*((BYTE*)dwpDest + 2) = *((BYTE*)dwpSprite + 2);
				*((BYTE*)dwpDest + 3) = *((BYTE*)dwpSprite + 3);
			}
			//���� ĭ �̵�.
			dwpDest++;
			dwpSprite++;
		}

		//�����ٷ� �̵�. ȭ��� ��������Ʈ ���..
		bypDestOrigin = bypDestOrigin + iDestPitch;
		bypSpriteOrigin = bypSpriteOrigin + stpSprite->iPitch;

		dwpDest = (DWORD*)bypDestOrigin;
		dwpSprite = (DWORD*)bypSpriteOrigin;

	}
}


//DrawSprite() Ư�� �޸� ��ġ�� ��������Ʈ�� ����Ѵ�. (ColorKey, Clipping)
void CSpriteDib::DrawSpriteBLUE(int iSpriteIndex, int iDrawX, int iDrawY, BYTE* bypDest, int iDestWidth, int iDestHeight, int iDestPitch, int iDrawLen)
{
	//�ִ� ��������Ʈ ������ �ʰ��ϰų�, �ε���� �ʴ� ��������Ʈ��� ����.
	if (m_iMaxSprite <= iSpriteIndex)
		return;

	//NULLüũ.
	if (m_stpSprite[iSpriteIndex].bypImage == NULL)
		return;

	st_SPRITE* stpSprite = &m_stpSprite[iSpriteIndex];


	//������ ����.
	int iSpriteWidth = stpSprite->iWidth;
	int iSpriteHeight = stpSprite->iHeight;
	int iCountY, iCountX;

	iSpriteWidth = iSpriteWidth * iDrawLen / 100;

	DWORD* dwpDest = (DWORD*)bypDest;
	DWORD* dwpSprite = (DWORD*)(stpSprite->bypImage);

	//��� �������� ��ǥ ���.
	iDrawX = iDrawX - stpSprite->iCenterPointX;
	iDrawY = iDrawY - stpSprite->iCenterPointY;

	//��� Ŭ����.
	if (0 > iDrawY)
	{
		iSpriteHeight = iSpriteHeight - (-iDrawY);
		dwpSprite = (DWORD*)(stpSprite->bypImage + stpSprite->iPitch * (-iDrawY));

		//������ ©���� ����̹Ƿ� ��������Ʈ ���� ��ġ�� �Ʒ��� �����ش�.
		iDrawY = 0;
	}

	//�ϴ� Ŭ����
	if (iDestHeight <= iDrawY + stpSprite->iHeight)
	{
		iSpriteHeight -= ((iDrawY + stpSprite->iHeight) - iDestHeight);
	}

	//���� Ŭ����
	if (0 > iDrawX)
	{
		iSpriteWidth = iSpriteWidth - (-iDrawX);
		dwpSprite = dwpSprite + (-iDrawX);

		//������ �߸��Ƿ� ��� ������ġ�� ���������� �δ�.
		iDrawX = 0;
	}

	//���� Ŭ����
	if (iDestWidth <= iDrawX + stpSprite->iWidth)
	{
		iSpriteWidth -= ((iDrawX + stpSprite->iWidth) - iDestWidth);//////////
	}

	//���� �׸��� ���ٸ� ����.
	if (iSpriteWidth <= 0 || iSpriteHeight <= 0)
	{
		return;
	}

	//ȭ�鿡 ���� ��ġ�� �̵��Ѵ�.
	dwpDest = (DWORD*)(((BYTE*)(dwpDest + iDrawX) + (iDrawY * iDestPitch)));


	BYTE* bypDestOrigin = (BYTE*)dwpDest;
	BYTE* bypSpriteOrigin = (BYTE*)dwpSprite;

	//��ü ũ�⸦ ���鼭 �ȼ����� ����� ó���� �ϸ� �׸� ���.
	for (iCountY = 0; iSpriteHeight > iCountY; iCountY++)
	{
		for (iCountX = 0; iSpriteWidth > iCountX; iCountX++)
		{
			if (m_dwColorKey != (*dwpSprite & 0x00ffffff)) //ColorKey�� �ƴϸ�..
			{
				//�ȼ� ���.
				//*dwpDest = *dwpSprite;

				*((BYTE*)dwpDest + 0) = 0xff; 
				*((BYTE*)dwpDest + 1) = *((BYTE*)dwpSprite + 1);
				*((BYTE*)dwpDest + 2) = *((BYTE*)dwpSprite + 2);
				*((BYTE*)dwpDest + 3) = *((BYTE*)dwpSprite + 3);
			}
			//���� ĭ �̵�.
			dwpDest++;
			dwpSprite++;
		}

		//�����ٷ� �̵�. ȭ��� ��������Ʈ ���..
		bypDestOrigin = bypDestOrigin + iDestPitch;
		bypSpriteOrigin = bypSpriteOrigin + stpSprite->iPitch;

		dwpDest = (DWORD*)bypDestOrigin;
		dwpSprite = (DWORD*)bypSpriteOrigin;

	}
}


//DrawSprite() Ư�� �޸� ��ġ�� ��������Ʈ�� ����Ѵ�. (ColorKey, Clipping)
void CSpriteDib::DrawSpriteALPHA50(int iSpriteIndex, int iDrawX, int iDrawY, BYTE* bypDest, int iDestWidth, int iDestHeight, int iDestPitch, int iAlphaVal, int iDrawLen)
{
	//�ִ� ��������Ʈ ������ �ʰ��ϰų�, �ε���� �ʴ� ��������Ʈ��� ����.
	if (m_iMaxSprite <= iSpriteIndex)
		return;

	//NULLüũ.
	if (m_stpSprite[iSpriteIndex].bypImage == NULL)
		return;

	st_SPRITE* stpSprite = &m_stpSprite[iSpriteIndex];


	//������ ����.
	int iSpriteWidth = stpSprite->iWidth;
	int iSpriteHeight = stpSprite->iHeight;
	int iCountY, iCountX;

	iSpriteWidth = iSpriteWidth * iDrawLen / 100;

	DWORD* dwpDest = (DWORD*)bypDest;
	DWORD* dwpSprite = (DWORD*)(stpSprite->bypImage);

	//��� �������� ��ǥ ���.
	iDrawX = iDrawX - stpSprite->iCenterPointX;
	iDrawY = iDrawY - stpSprite->iCenterPointY;

	//��� Ŭ����.
	if (0 > iDrawY)
	{
		iSpriteHeight = iSpriteHeight - (-iDrawY);
		dwpSprite = (DWORD*)(stpSprite->bypImage + stpSprite->iPitch * (-iDrawY));

		//������ ©���� ����̹Ƿ� ��������Ʈ ���� ��ġ�� �Ʒ��� �����ش�.
		iDrawY = 0;
	}

	//�ϴ� Ŭ����
	if (iDestHeight <= iDrawY + stpSprite->iHeight)
	{
		iSpriteHeight -= ((iDrawY + stpSprite->iHeight) - iDestHeight);
	}

	//���� Ŭ����
	if (0 > iDrawX)
	{
		iSpriteWidth = iSpriteWidth - (-iDrawX);
		dwpSprite = dwpSprite + (-iDrawX);

		//������ �߸��Ƿ� ��� ������ġ�� ���������� �δ�.
		iDrawX = 0;
	}

	//���� Ŭ����
	if (iDestWidth <= iDrawX + stpSprite->iWidth)
	{
		iSpriteWidth -= ((iDrawX + stpSprite->iWidth) - iDestWidth);//////////
	}

	//���� �׸��� ���ٸ� ����.
	if (iSpriteWidth <= 0 || iSpriteHeight <= 0)
	{
		return;
	}

	//ȭ�鿡 ���� ��ġ�� �̵��Ѵ�.
	dwpDest = (DWORD*)(((BYTE*)(dwpDest + iDrawX) + (iDrawY * iDestPitch)));


	BYTE* bypDestOrigin = (BYTE*)dwpDest;
	BYTE* bypSpriteOrigin = (BYTE*)dwpSprite;

	//��ü ũ�⸦ ���鼭 �ȼ����� ����� ó���� �ϸ� �׸� ���.
	for (iCountY = 0; iSpriteHeight > iCountY; iCountY++)
	{
		for (iCountX = 0; iSpriteWidth > iCountX; iCountX++)
		{
			if (m_dwColorKey != (*dwpSprite & 0x00ffffff)) //ColorKey�� �ƴϸ�..
			{
				//�ȼ� ���.
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
			//���� ĭ �̵�.
			dwpDest++;
			dwpSprite++;
		}

		//�����ٷ� �̵�. ȭ��� ��������Ʈ ���..
		bypDestOrigin = bypDestOrigin + iDestPitch;
		bypSpriteOrigin = bypSpriteOrigin + stpSprite->iPitch;

		dwpDest = (DWORD*)bypDestOrigin;
		dwpSprite = (DWORD*)bypSpriteOrigin;

	}
}