#pragma once
#ifndef __CFRAME_SKIP_H__
#define __CFRAME_SKIP_H__
#define FPS 50


class CFrameSkip
{
	DWORD m_dwBeginFrame;
	DWORD m_dwInterval;
	DWORD m_dwEndFrame;
	DWORD m_dwFrameTick;
	DWORD m_dwFrameSkipInterval;
	int m_iFrameCnt;
public:
	CFrameSkip();
	~CFrameSkip();
	BOOL isFrameSkip(void);
	int getFrameCnt(void);
	void ResetFrameCnt(void);
};

#endif