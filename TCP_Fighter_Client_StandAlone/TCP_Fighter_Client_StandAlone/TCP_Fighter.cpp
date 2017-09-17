// TCP_Fighter.cpp : 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
#include "TCP_Fighter.hpp"
#include <timeapi.h>


#define MAX_LOADSTRING 100

// 전역 변수:
HINSTANCE hInst;                                // 현재 인스턴스입니다.
WCHAR szTitle[MAX_LOADSTRING];                  // 제목 표시줄 텍스트입니다.
WCHAR szWindowClass[MAX_LOADSTRING];            // 기본 창 클래스 이름입니다.

#pragma comment (lib, "Winmm.lib")

//사용자 전역 변수
enum GAMESTATE
{
	Title	= 0,
	Game	= 1,
	GameOver= 2
};
HWND hWnd; //임시로 전역으로 뺌.
HWND g_hWnd;
BOOL g_bActiveApp = true; //윈도우 비활성화시 키 프로세스 처리를 위한 BOOL변수.

CBaseObject* g_pPlayerObject;
CFrameSkip g_cFrameSkip;
CSpriteDib g_cSprite(eSPRITE_MAX, 0x00ffffff);
CScreenDib g_cScreenDib(dfSCREEN_WIDTH, dfSCREEN_HEIGHT, dfSCREEN_BIT);

std::list<CBaseObject*>		g_ObjList;

int g_fps;


// 이 코드 모듈에 들어 있는 함수의 정방향 선언입니다.
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

//사용자함수
void Update(GAMESTATE GameState);
void Update_Title(void);
void Update_Game(void);
bool GameInit(void);
void Draw(void);
void KeyProcess(void);
void Action(void);


//WinMain()
int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: 여기에 코드를 입력합니다.

    // 전역 문자열을 초기화합니다.
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_TCP_FIGHTER, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // 응용 프로그램 초기화를 수행합니다.
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_TCP_FIGHTER));

    MSG msg;

	timeBeginPeriod(1);
    // 기본 메시지 루프입니다.
    while (true)
    {
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
			if (msg.message == WM_QUIT)
				break;
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
		else
		{
			//임시.
			Update(Game);
		}
    }
	timeEndPeriod(1);

    return (int) msg.wParam;
}




ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_TCP_FIGHTER));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_TCP_FIGHTER);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}




BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // 인스턴스 핸들을 전역 변수에 저장합니다.

   //HWND hWnd;
   //hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
   //   CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);
   
   hWnd = CreateWindowEx(NULL, szWindowClass, szTitle, WS_SYSMENU | WS_CAPTION | WS_MINIMIZEBOX,
	   CW_USEDEFAULT, CW_USEDEFAULT, dfSCREEN_WIDTH, dfSCREEN_HEIGHT, NULL, NULL, hInst, NULL);


   if (hWnd == NULL)
   {
      return FALSE;
   }

   ShowWindow(hWnd, SW_SHOW);
   UpdateWindow(hWnd);

   SetFocus(hWnd);

   RECT WindowRect;
   WindowRect.top = 0;
   WindowRect.left = 0;
   WindowRect.right = dfSCREEN_WIDTH;
   WindowRect.bottom = dfSCREEN_HEIGHT;

   AdjustWindowRectEx(&WindowRect, GetWindowStyle(hWnd), GetMenu(hWnd) != NULL, GetWindowExStyle(hWnd));

   int iX = (GetSystemMetrics(SM_CXSCREEN) / 2) - (dfSCREEN_WIDTH / 2);
   int iY = (GetSystemMetrics(SM_CYSCREEN) / 2) - (dfSCREEN_HEIGHT / 2);

   MoveWindow(hWnd, iX, iY, WindowRect.right - WindowRect.left, WindowRect.bottom - WindowRect.top, true);

   return TRUE;
}


//WinProcedure()
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {

	case WM_CREATE :
		{
			//윈도우 만들때 초기화 함수 호출.
			if (!GameInit())
				break;
			
			SetTimer(hWnd, 1, 1000, NULL);
		}
		break;

	case WM_TIMER:
		{
			WCHAR buffer[30];
			g_fps = g_cFrameSkip.getFrameCnt();
			wsprintf(buffer, L"FPS : %d", g_fps);
			SetWindowText(hWnd, buffer);
			
			g_cFrameSkip.ResetFrameCnt();	
		}
		break;
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // 메뉴 선택을 구문 분석합니다.
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: 여기에 hdc를 사용하는 그리기 코드를 추가합니다.	
			
			
            EndPaint(hWnd, &ps);
        }
        break;

	case WM_ACTIVATEAPP:
		{
			//윈도우 비활성화 상태일때 키보드 입력처리를 위한 bool변수.
			g_bActiveApp = (BOOL)wParam; //wParam에 TRUE가 들어오면 활성화.
			break;
		}
		break;

    case WM_DESTROY:
		{
			PostQuitMessage(0);
			//KillTimer(hWnd, NULL);
		}
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// 정보 대화 상자의 메시지 처리기입니다.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}


void Draw(void)
{
	BYTE* bypDest = g_cScreenDib.GetDibBuffer();
	int iDestWidth = g_cScreenDib.GetWidth();
	int iDestHeight = g_cScreenDib.GetHeight();
	int iDestPitch = g_cScreenDib.GetPitch();


	//맵 출력.
	g_cSprite.DrawImage(0, 0, 0, bypDest, iDestWidth, iDestHeight, iDestPitch);
	

	
	//캐릭터 출력.
	for (std::list<CBaseObject*>::iterator iter = g_ObjList.begin(); iter != g_ObjList.end(); ++iter)
	{
		g_ObjList.sort(YposSort());
		(*iter)->Draw(bypDest, iDestWidth, iDestHeight, iDestPitch);
	}
	


}

void KeyProcess(void)
{
	if (g_pPlayerObject == NULL)
		return;

	DWORD dwAction;

	if ((GetAsyncKeyState(VK_UP) & 0x8000) && (GetAsyncKeyState(VK_LEFT) & 0x8000))
	{
		dwAction = dfACTION_MOVE_LU;
	}

	else if ((GetAsyncKeyState(VK_UP) & 0x8000) && (GetAsyncKeyState(VK_RIGHT) & 0x8000))
	{
		dwAction = dfACTION_MOVE_RU;
	}

	else if ((GetAsyncKeyState(VK_DOWN) & 0x8000) && (GetAsyncKeyState(VK_RIGHT) & 0x8000))
	{
		dwAction = dfACTION_MOVE_RD;
	}	

	else if ((GetAsyncKeyState(VK_DOWN) & 0x8000) && (GetAsyncKeyState(VK_LEFT) & 0x8000))
	{
		dwAction = dfACTION_MOVE_LD;
	}

	else if (GetAsyncKeyState(VK_LEFT) & 0x8000)
	{
		dwAction = dfACTION_MOVE_LL;
	}

	else if (GetAsyncKeyState(VK_RIGHT) & 0x8000)
	{
		dwAction = dfACTION_MOVE_RR;
	}

	else if (GetAsyncKeyState(VK_UP) & 0x8000)
	{
		dwAction = dfACTION_MOVE_UU;
	}

	else if (GetAsyncKeyState(VK_DOWN) & 0x8000)
	{
		dwAction = dfACTION_MOVE_DD;
	}

	else if (GetAsyncKeyState(0x5A) & 0x8000)
	{
		dwAction = dfACTION_ATTACK1;
	}

	else if (GetAsyncKeyState(0x58) & 0x8000)
	{
		dwAction = dfACTION_ATTACK2;
	}

	else if (GetAsyncKeyState(0x43) & 0x8000)
	{
		dwAction = dfACTION_ATTACK3;
	}

	else
	{
		dwAction = dfACTION_STAND;
	}
	
	g_pPlayerObject->ActionInput(dwAction);
}

void Action(void)
{
	for (std::list<CBaseObject*>::iterator iter = g_ObjList.begin(); iter != g_ObjList.end(); ++iter)
	{
		(*iter)->Action(NULL);
	}
}

//게임타이틀 처리.
void Update_Title(void)
{

}

//게임 프로시저 처리.
void Update_Game(void)
{
	if (g_bActiveApp)
	{
		KeyProcess();
	}
		
	Action();

	if (!g_cFrameSkip.isFrameSkip())
	{
		Draw();
	}

	Sleep(0);

	//Dib를 화면으로 플립.
	g_cScreenDib.DrawBuffer(hWnd, 0, 0);

}

//업데이트문.
void Update(GAMESTATE GameState)
{
	switch (GameState)
	{
	case Title:
		{
			Update_Title();
			break;
		}

	case Game:
		{
			Update_Game();
			break;
		}
	}
}

bool GameInit(void)
{

	//Map Sprite.
	if (!g_cSprite.LoadDibSprite(eMAP, L"SpriteData\\_Map.bmp", 0, 0))
		return false;

	//StandLeft Sprite.
	if (!g_cSprite.LoadDibSprite(ePLAYER_STAND_L01, L"SpriteData\\Stand_L_01.bmp", 71, 90))
		return false;
	if (!g_cSprite.LoadDibSprite(ePLAYER_STAND_L02, L"SpriteData\\Stand_L_02.bmp", 71, 90))
		return false;
	if (!g_cSprite.LoadDibSprite(ePLAYER_STAND_L03, L"SpriteData\\Stand_L_02.bmp", 71, 90))
		return false;
	if (!g_cSprite.LoadDibSprite(ePLAYER_STAND_L04, L"SpriteData\\Stand_L_03.bmp", 71, 90))
		return false;
	if (!g_cSprite.LoadDibSprite(ePLAYER_STAND_L05, L"SpriteData\\Stand_L_03.bmp", 71, 90))
		return false;

	//StandRight Sprite.
	if (!g_cSprite.LoadDibSprite(ePLAYER_STAND_R01, L"SpriteData\\Stand_R_01.bmp", 71, 90))
		return false;
	if (!g_cSprite.LoadDibSprite(ePLAYER_STAND_R02, L"SpriteData\\Stand_R_02.bmp", 71, 90))
		return false;
	if (!g_cSprite.LoadDibSprite(ePLAYER_STAND_R03, L"SpriteData\\Stand_R_02.bmp", 71, 90))
		return false;
	if (!g_cSprite.LoadDibSprite(ePLAYER_STAND_R04, L"SpriteData\\Stand_R_03.bmp", 71, 90))
		return false;
	if (!g_cSprite.LoadDibSprite(ePLAYER_STAND_R05, L"SpriteData\\Stand_R_03.bmp", 71, 90))
		return false;

	//MoveLeft Sprite.
	if (!g_cSprite.LoadDibSprite(ePLAYER_MOVE_L01, L"SpriteData\\Move_L_01.bmp", 71, 90))
		return false;
	if (!g_cSprite.LoadDibSprite(ePLAYER_MOVE_L02, L"SpriteData\\Move_L_02.bmp", 71, 90))
		return false;
	if (!g_cSprite.LoadDibSprite(ePLAYER_MOVE_L03, L"SpriteData\\Move_L_03.bmp", 71, 90))
		return false;
	if (!g_cSprite.LoadDibSprite(ePLAYER_MOVE_L04, L"SpriteData\\Move_L_04.bmp", 71, 90))
		return false;
	if (!g_cSprite.LoadDibSprite(ePLAYER_MOVE_L05, L"SpriteData\\Move_L_05.bmp", 71, 90))
		return false;
	if (!g_cSprite.LoadDibSprite(ePLAYER_MOVE_L06, L"SpriteData\\Move_L_06.bmp", 71, 90))
		return false;
	if (!g_cSprite.LoadDibSprite(ePLAYER_MOVE_L07, L"SpriteData\\Move_L_07.bmp", 71, 90))
		return false;
	if (!g_cSprite.LoadDibSprite(ePLAYER_MOVE_L08, L"SpriteData\\Move_L_08.bmp", 71, 90))
		return false;
	if (!g_cSprite.LoadDibSprite(ePLAYER_MOVE_L09, L"SpriteData\\Move_L_09.bmp", 71, 90))
		return false;
	if (!g_cSprite.LoadDibSprite(ePLAYER_MOVE_L10, L"SpriteData\\Move_L_10.bmp", 71, 90))
		return false;
	if (!g_cSprite.LoadDibSprite(ePLAYER_MOVE_L11, L"SpriteData\\Move_L_11.bmp", 71, 90))
		return false;
	if (!g_cSprite.LoadDibSprite(ePLAYER_MOVE_L12, L"SpriteData\\Move_L_12.bmp", 71, 90))
		return false;

	//MoveRight Sprite.
	if (!g_cSprite.LoadDibSprite(ePLAYER_MOVE_R01, L"SpriteData\\Move_R_01.bmp", 71, 90))
		return false;
	if (!g_cSprite.LoadDibSprite(ePLAYER_MOVE_R02, L"SpriteData\\Move_R_02.bmp", 71, 90))
		return false;
	if (!g_cSprite.LoadDibSprite(ePLAYER_MOVE_R03, L"SpriteData\\Move_R_03.bmp", 71, 90))
		return false;
	if (!g_cSprite.LoadDibSprite(ePLAYER_MOVE_R04, L"SpriteData\\Move_R_04.bmp", 71, 90))
		return false;
	if (!g_cSprite.LoadDibSprite(ePLAYER_MOVE_R05, L"SpriteData\\Move_R_05.bmp", 71, 90))
		return false;
	if (!g_cSprite.LoadDibSprite(ePLAYER_MOVE_R06, L"SpriteData\\Move_R_06.bmp", 71, 90))
		return false;
	if (!g_cSprite.LoadDibSprite(ePLAYER_MOVE_R07, L"SpriteData\\Move_R_07.bmp", 71, 90))
		return false;
	if (!g_cSprite.LoadDibSprite(ePLAYER_MOVE_R08, L"SpriteData\\Move_R_08.bmp", 71, 90))
		return false;
	if (!g_cSprite.LoadDibSprite(ePLAYER_MOVE_R09, L"SpriteData\\Move_R_09.bmp", 71, 90))
		return false;
	if (!g_cSprite.LoadDibSprite(ePLAYER_MOVE_R10, L"SpriteData\\Move_R_10.bmp", 71, 90))
		return false;
	if (!g_cSprite.LoadDibSprite(ePLAYER_MOVE_R11, L"SpriteData\\Move_R_11.bmp", 71, 90))
		return false;
	if (!g_cSprite.LoadDibSprite(ePLAYER_MOVE_R12, L"SpriteData\\Move_R_12.bmp", 71, 90))
		return false;

	//Attack1Left Sprite.
	if (!g_cSprite.LoadDibSprite(ePLAYER_ATTACK1_L01, L"SpriteData\\Attack1_L_01.bmp", 71, 90))
		return false;
	if (!g_cSprite.LoadDibSprite(ePLAYER_ATTACK1_L02, L"SpriteData\\Attack1_L_02.bmp", 71, 90))
		return false;
	if (!g_cSprite.LoadDibSprite(ePLAYER_ATTACK1_L03, L"SpriteData\\Attack1_L_03.bmp", 71, 90))
		return false;
	if (!g_cSprite.LoadDibSprite(ePLAYER_ATTACK1_L04, L"SpriteData\\Attack1_L_04.bmp", 71, 90))
		return false;

	//Attack1Right Sprite.
	if (!g_cSprite.LoadDibSprite(ePLAYER_ATTACK1_R01, L"SpriteData\\Attack1_R_01.bmp", 71, 90))
		return false;
	if (!g_cSprite.LoadDibSprite(ePLAYER_ATTACK1_R02, L"SpriteData\\Attack1_R_02.bmp", 71, 90))
		return false;
	if (!g_cSprite.LoadDibSprite(ePLAYER_ATTACK1_R03, L"SpriteData\\Attack1_R_03.bmp", 71, 90))
		return false;
	if (!g_cSprite.LoadDibSprite(ePLAYER_ATTACK1_R04, L"SpriteData\\Attack1_R_04.bmp", 71, 90))
		return false;

	//Attack2Left Sprite.
	if (!g_cSprite.LoadDibSprite(ePLAYER_ATTACK2_L01, L"SpriteData\\Attack2_L_01.bmp", 71, 90))
		return false;
	if (!g_cSprite.LoadDibSprite(ePLAYER_ATTACK2_L02, L"SpriteData\\Attack2_L_02.bmp", 71, 90))
		return false;
	if (!g_cSprite.LoadDibSprite(ePLAYER_ATTACK2_L03, L"SpriteData\\Attack2_L_03.bmp", 71, 90))
		return false;
	if (!g_cSprite.LoadDibSprite(ePLAYER_ATTACK2_L04, L"SpriteData\\Attack2_L_04.bmp", 71, 90))
		return false;
	
	//Attack2Right Sprite.
	if (!g_cSprite.LoadDibSprite(ePLAYER_ATTACK2_R01, L"SpriteData\\Attack2_R_01.bmp", 71, 90))
		return false;
	if (!g_cSprite.LoadDibSprite(ePLAYER_ATTACK2_R02, L"SpriteData\\Attack2_R_02.bmp", 71, 90))
		return false;
	if (!g_cSprite.LoadDibSprite(ePLAYER_ATTACK2_R03, L"SpriteData\\Attack2_R_03.bmp", 71, 90))
		return false;
	if (!g_cSprite.LoadDibSprite(ePLAYER_ATTACK2_R04, L"SpriteData\\Attack2_R_04.bmp", 71, 90))
		return false;

	//Attack3Left Sprite.
	if (!g_cSprite.LoadDibSprite(ePLAYER_ATTACK3_L01, L"SpriteData\\Attack3_L_01.bmp", 71, 90))
		return false;
	if (!g_cSprite.LoadDibSprite(ePLAYER_ATTACK3_L02, L"SpriteData\\Attack3_L_02.bmp", 71, 90))
		return false;
	if (!g_cSprite.LoadDibSprite(ePLAYER_ATTACK3_L03, L"SpriteData\\Attack3_L_03.bmp", 71, 90))
		return false;
	if (!g_cSprite.LoadDibSprite(ePLAYER_ATTACK3_L04, L"SpriteData\\Attack3_L_04.bmp", 71, 90))
		return false;
	if (!g_cSprite.LoadDibSprite(ePLAYER_ATTACK3_L05, L"SpriteData\\Attack3_L_05.bmp", 71, 90))
		return false;
	if (!g_cSprite.LoadDibSprite(ePLAYER_ATTACK3_L06, L"SpriteData\\Attack3_L_06.bmp", 71, 90))
		return false;

	//Attack3Right Sprite.
	if (!g_cSprite.LoadDibSprite(ePLAYER_ATTACK3_R01, L"SpriteData\\Attack3_R_01.bmp", 71, 90))
		return false;
	if (!g_cSprite.LoadDibSprite(ePLAYER_ATTACK3_R02, L"SpriteData\\Attack3_R_02.bmp", 71, 90))
		return false;
	if (!g_cSprite.LoadDibSprite(ePLAYER_ATTACK3_R03, L"SpriteData\\Attack3_R_03.bmp", 71, 90))
		return false;
	if (!g_cSprite.LoadDibSprite(ePLAYER_ATTACK3_R04, L"SpriteData\\Attack3_R_04.bmp", 71, 90))
		return false;
	if (!g_cSprite.LoadDibSprite(ePLAYER_ATTACK3_R05, L"SpriteData\\Attack3_R_05.bmp", 71, 90))
		return false;
	if (!g_cSprite.LoadDibSprite(ePLAYER_ATTACK3_R06, L"SpriteData\\Attack3_R_06.bmp", 71, 90))
		return false;

	//EffectSpark Sprite.
	if (!g_cSprite.LoadDibSprite(eEFFECT_SPARK_01, L"SpriteData\\xSpark_1.bmp", 70, 70))
		return false;
	if (!g_cSprite.LoadDibSprite(eEFFECT_SPARK_02, L"SpriteData\\xSpark_2.bmp", 70, 70))
		return false;
	if (!g_cSprite.LoadDibSprite(eEFFECT_SPARK_03, L"SpriteData\\xSpark_3.bmp", 70, 70))
		return false;
	if (!g_cSprite.LoadDibSprite(eEFFECT_SPARK_04, L"SpriteData\\xSpark_4.bmp", 70, 70))
		return false;

	//GUAGE_HP Sprite.
	if (!g_cSprite.LoadDibSprite(eGUAGE_HP, L"SpriteData\\HPGuage.bmp", 0, 0))
		return false;

	//SHADOW Sprite.
	if (!g_cSprite.LoadDibSprite(eSHADOW, L"SpriteData\\Shadow.bmp", 32, 4))
		return false;


	g_pPlayerObject = new CPlayerObject();

	//g_pPlayerObject-> 좌표 및 기타 정보, 능력치 셋팅..
	g_pPlayerObject->SetObjectType(eTYPE_PLAYER);
	g_pPlayerObject->SetObjectID(1);
	g_pPlayerObject->SetCurPosition(200, 400);
	g_pPlayerObject->SetSprite(ePLAYER_STAND_R01, ePLAYER_STAND_R_MAX, dfDELAY_STAND);

	//리스트 삽입 (g_pPlayerObject)
	g_ObjList.push_back(g_pPlayerObject);

	
	//pPlayer2 생성.
	CBaseObject* pPlayer2 = new CPlayerObject();
	pPlayer2->SetObjectType(eTYPE_PLAYER);
	pPlayer2->SetObjectID(2);
	pPlayer2->SetCurPosition(200, 200);
	pPlayer2->SetSprite(ePLAYER_STAND_R01, ePLAYER_STAND_R_MAX, dfDELAY_STAND);

	//리스트 삽입 (pPlayer2)
	g_ObjList.push_back(pPlayer2);


	//pPlayer3 생성.
	CBaseObject* pPlayer3 = new CPlayerObject();
	pPlayer3->SetObjectType(eTYPE_PLAYER);
	pPlayer3->SetObjectID(3);
	pPlayer3->SetCurPosition(400, 200);
	pPlayer3->SetSprite(ePLAYER_STAND_L01, ePLAYER_STAND_L_MAX, dfDELAY_STAND);

	//리스트 삽입 (pPlayer3)
	g_ObjList.push_back(pPlayer3);


	//pPlayer4 생성.
	CBaseObject* pPlayer4 = new CPlayerObject();
	pPlayer4->SetObjectType(eTYPE_PLAYER);
	pPlayer4->SetObjectID(4);
	pPlayer4->SetCurPosition(400, 400);
	pPlayer4->SetSprite(ePLAYER_STAND_L01, ePLAYER_STAND_L_MAX, dfDELAY_STAND);

	//리스트 삽입 (pPlayer4)
	g_ObjList.push_back(pPlayer4);

	
	//pPlayer5 생성.
	CBaseObject* pPlayer5 = new CPlayerObject();
	pPlayer5->SetObjectType(eTYPE_PLAYER);
	pPlayer5->SetObjectID(5);
	pPlayer5->SetCurPosition(100, 100);
	pPlayer5->SetSprite(ePLAYER_STAND_R01, ePLAYER_STAND_R_MAX, dfDELAY_STAND);

	//리스트 삽입 (pPlayer5)
	g_ObjList.push_back(pPlayer5);
	


	return true;
}