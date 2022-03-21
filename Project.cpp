// Project.cpp : 애플리케이션에 대한 진입점을 정의합니다.
//

#include "framework.h"
#include "Project.h"

#include <mmsystem.h>
#pragma comment(lib,"winmm.lib")
#include <stdlib.h>

#define MAX_LOADSTRING 100

// 전역 변수:
HINSTANCE hInst;                                // 현재 인스턴스입니다.
WCHAR szTitle[MAX_LOADSTRING];                  // 제목 표시줄 텍스트입니다.
WCHAR szWindowClass[MAX_LOADSTRING];            // 기본 창 클래스 이름입니다.

// 이 코드 모듈에 포함된 함수의 선언을 전달합니다:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

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
    LoadStringW(hInstance, IDC_PROJECT, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // 애플리케이션 초기화를 수행합니다:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_PROJECT));

    MSG msg;

    // 기본 메시지 루프입니다:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}

//
//  함수: MyRegisterClass()
//
//  용도: 창 클래스를 등록합니다.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_PROJECT));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_PROJECT);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   함수: InitInstance(HINSTANCE, int)
//
//   용도: 인스턴스 핸들을 저장하고 주 창을 만듭니다.
//
//   주석:
//
//        이 함수를 통해 인스턴스 핸들을 전역 변수에 저장하고
//        주 프로그램 창을 만든 다음 표시합니다.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // 인스턴스 핸들을 전역 변수에 저장합니다.

   HWND hWnd = CreateWindowW(szWindowClass, L"201807065_박진택", WS_OVERLAPPED | WS_MINIMIZEBOX | WS_SYSMENU,
       50, 50, 1200, 700, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  함수: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  용도: 주 창의 메시지를 처리합니다.
//
//  WM_COMMAND  - 애플리케이션 메뉴를 처리합니다.
//  WM_PAINT    - 주 창을 그립니다.
//  WM_DESTROY  - 종료 메시지를 게시하고 반환합니다.
//
//

CONST INT g_scafNum = 46;               // 발판의 개수를 상수로 선언
CONST INT g_obsNum = 35;                // 장애물의 개수를 상수로 선언

RECT g_Window;                          // 전체 창크기를 지정하여 영역을 구분
RECT g_me;                              // 플레이어
RECT g_scaf[g_scafNum];                 // 플레이어가 밟을 수 있는 발판
RECT g_drop[g_scafNum];                 // 플레이어가 발판에서 떨어지는 영역을 처리
RECT g_bottom;                          // 기본 제공 발판
RECT g_Nowme;                           // 현재 플레이어의 위치
RECT g_CrashBottom;                     // 점프 후 바닥과의 충돌영역
RECT g_CrashScaf;                       // 발판과의 충돌영역
RECT g_CrashDrop;                       // 떨어지는 처리 충돌영역
RECT g_obs[(g_obsNum * 3)];             // 장애물 모서리마다 사각형 설정
RECT g_CrashObs;                        // 장애물과 플레이어의 충돌영역

RECT g_Finish[2];                       // 출발지점과 도착지점 사각형
RECT g_CrashFinish;                     // 출발지점과 도착지점 사각형 충돌영역
INT g_ProgressFinish;                   // 도착지점의 좌표를 저장

WCHAR Progress_Text[128];               // 진행바 텍스트
RECT Progress_Rect;                     // 진행바 사각형
RECT Window_ProgressBar;                // 전체 진행바 사각형
INT Progress_Increase;                  // 진행바 증가치

BOOL g_isJump = FALSE;                  // 점프 중인지 확인(이중점프 금지)
BOOL g_Drop = FALSE;                    // 떨어지는지 확인
BOOL g_flag = TRUE;                     // 게임진행 여부 판정(FALSE 일 경우 게임 오버)

INT g_JumpPower = 300;                  // 점프의 힘 조절
INT g_Gravity = 4;                      // 점프 후 내려오는 속도(중력) 조절
INT g_JumpHeight = 0;                   // 점프 높이

//  발판의 좌표 설정
extern INT g_scafRectLeft[g_scafNum];
extern INT g_scafRectTop[g_scafNum];
extern INT g_scafRectRight[g_scafNum];

// 장애물의 좌표 설정
extern INT g_obsRectLeft[(g_obsNum * 3)];
extern INT g_obsRectTop[(g_obsNum * 3)];

// 장애물 현재위치, 장애물 움직이는 속도 조절
RECT g_NowObs[(g_obsNum * 3)];

// 점프 설정 함수
void Jump() {

    // 점프중이라면
    if (g_isJump) {

        // 점프 중 바닥과 닿으면 변화 값 초기상태로 복구
        if (IntersectRect(&g_CrashBottom, &g_bottom, &g_Nowme)) {
            g_JumpPower = 300;
            g_JumpHeight = 0;
            g_me.bottom = g_CrashBottom.top;
            g_me.top = g_me.bottom - 50;
            g_isJump = FALSE;
            return;
        }

        // 점프 높이와 점프 힘을 감소 및 중력 증가
        g_JumpHeight -= g_JumpPower * 0.04;
        g_JumpPower -= g_Gravity * 4;
    }
}

void me_Drop() {

    if (g_Drop == TRUE) {
        //  점프 상태면 떨어짐 처리 안함
        if (g_isJump == TRUE) {
            g_Drop = FALSE;
        }
        else {
            // 낙하 상태로 전환
            g_JumpPower = 2;
            g_Gravity = 4;
            g_JumpHeight += g_JumpPower * 4;
            g_JumpPower += g_Gravity * 4;

            // 바닥과 닿으면 점프 값 초기상태로 전환하고 플레이어 위치 조정
            if (IntersectRect(&g_CrashBottom, &g_bottom, &g_Nowme)) {
                g_JumpPower = 300;
                g_JumpHeight = 0;
                g_Gravity = 4;
                g_me.bottom = g_CrashBottom.top;
                g_me.top = g_me.bottom - 50;
                g_isJump = FALSE;
                g_Drop = FALSE;
                return;
            }
        }
    }
}

//  진행방향으로 이동하는 함수
void Move_Timer(HWND hWnd) {

    for (int i = 0; i < g_scafNum; i++) {
        // 발판 진행방향으로 이동
        if (g_scaf[i].right >= g_Window.left && g_flag == TRUE) {
            g_scaf[i].left -= 5;
            g_scaf[i].right -= 5;
            g_drop[i].left -= 5;
            g_drop[i].right -= 5;
        }

        // 현재 플레이어의 위치를 저장
        g_Nowme.left = g_me.left;
        g_Nowme.top = g_me.top + g_JumpHeight;
        g_Nowme.right = g_me.right;
        g_Nowme.bottom = g_me.bottom + g_JumpHeight;

        // 플레이어가 발판에 닿는 경우
        if (IntersectRect(&g_CrashScaf, &g_Nowme, &g_scaf[i]) && g_flag == TRUE) {

            // 점프값 초기화, 점프 및 낙하 상태 초기화
            g_JumpPower = 300;
            g_JumpHeight = 0;
            g_Gravity = 4;
            g_isJump = FALSE;
            g_Drop = FALSE;

            //  플레이어가 발판 측면에 닿았을 때 게임오버
            if ((g_CrashScaf.left + 10) > g_CrashScaf.right && g_scaf[i].right > g_me.right) {
                // 음악 정지
                PlaySound(NULL, 0, 0);
                MessageBox(hWnd, L"발판 측면에 충돌", L"죽음", MB_OK);
                g_flag = FALSE;
                return;
            }

            //  플레이어가 발판위로 올라가면 플레이어의 위치를 발판위로 변경
            if (g_CrashScaf.bottom > g_scaf[i].top) {
                g_me.bottom = g_CrashScaf.top;
                g_me.top = g_me.bottom - 50;
            }
        }
        //  플레이어가 발판끝에서 점프하지 않고 떨어지는 처리
        if (IntersectRect(&g_CrashDrop, &g_Nowme, &g_drop[i])) {
            g_Drop = TRUE;
        }
    }

    // 플레이어가 장애물과 충돌했는지
    for (int j = 0; j < (g_obsNum * 3); j++) {
        if (g_obs[j].right >= g_Window.left && g_flag == TRUE) {
            g_obs[j].left -= 5;
            g_obs[j].right -= 5;
        }

        // 현재 장애물의 위치
        g_NowObs[j].left = g_obs[j].left;
        g_NowObs[j].top = g_obs[j].top;
        g_NowObs[j].right = g_obs[j].right;
        g_NowObs[j].bottom = g_obs[j].bottom;

        // 플레이어가 장애물과 충돌하는 경우
        if (IntersectRect(&g_CrashObs, &g_Nowme, &g_NowObs[j]) && g_flag == TRUE) {
            // 음악 정지
            PlaySound(NULL, 0, 0);
            MessageBox(hWnd, L"장애물과 충돌", L"죽음", MB_OK);
            g_flag = FALSE;
            return;
        }
    }
    // 플레이어가 도착지점와 충돌하는 경우
    if (IntersectRect(&g_CrashFinish, &g_Finish[0], &g_Finish[1]) && g_flag == TRUE) {
        MessageBox(hWnd, L"스테이지 클리어!", L"클리어", MB_OK);
        g_flag = FALSE;
        return;
    }
    else {
        // 플레이어가 도착하지 않으면 진행바 증가
        if (g_flag == TRUE) {
            g_Finish[1].left -= 5;
            g_Finish[1].right -= 5;
            Progress_Increase = _wtoi(Progress_Text);
        }
    }
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        // 메뉴 선택을 구문 분석합니다:
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

    case WM_CREATE:
        // 표시할 윈도우 크기 설정
        g_Window.left = 0;
        g_Window.top = 0;
        g_Window.right = 1200;
        g_Window.bottom = 700;

        // 플레이어 크기 설정
        g_me.left = 300;
        g_me.top = 450;
        g_me.right = g_me.left + 50;
        g_me.bottom = g_me.top + 50;

        // 기본제공 바닥 크기 설정
        g_bottom.left = 0;
        g_bottom.top = 500;
        g_bottom.right = g_bottom.left + 1200;
        g_bottom.bottom = g_bottom.top + 200;

        // 발판과 낙하영역 설정
        for (int i = 0; i < g_scafNum; i++) {
            g_scaf[i].left = g_scafRectLeft[i];
            g_scaf[i].top = g_scafRectTop[i];
            g_scaf[i].right = g_scaf[i].left + g_scafRectRight[i];
            g_scaf[i].bottom = g_bottom.top;

            g_drop[i].left = g_scaf[i].right + 30;
            g_drop[i].top = g_scaf[i].top - 1;
            g_drop[i].right = g_drop[i].left + 5;
            g_drop[i].bottom = g_drop[i].top + 5;
        }

        // 장애물 및 판정영역 설정
        for (int j = 0; j < (g_obsNum * 3); j++) {
            g_obs[j].left = g_obsRectLeft[j];
            g_obs[j].top = g_obsRectTop[j] - 1;
            g_obs[j].right = g_obs[j].left + 1;
            g_obs[j].bottom = g_obs[j].top + 1;
        }

        // 출발지점 및 도착지점 영역 설정
        g_Finish[0].left = g_me.left;
        g_Finish[0].top = g_me.top;
        g_Finish[0].right = g_me.right;
        g_Finish[0].bottom = g_me.bottom;

        g_Finish[1].left = 29500;
        g_Finish[1].top = 0;
        g_Finish[1].right = g_Finish[1].left + 300;
        g_Finish[1].bottom = g_Window.bottom;

        g_ProgressFinish = g_Finish[1].left;

        // 진행바 설정
        Progress_Rect.left = 600;
        Progress_Rect.top = 10;
        Progress_Rect.right = Progress_Rect.left + 1;
        Progress_Rect.bottom = Progress_Rect.top + 20;

        Window_ProgressBar.left = Progress_Rect.left;
        Window_ProgressBar.top = Progress_Rect.top;
        Window_ProgressBar.right = Progress_Rect.left + 100;
        Window_ProgressBar.bottom = Progress_Rect.bottom;

        // 음악 재생 부분
        PlaySound(L"Music.wav", 0, SND_ASYNC | SND_NODEFAULT);
        break;

    case WM_KEYDOWN:
        switch (wParam) {
        // 점프키 입력 부분
        case VK_UP:
        case VK_SPACE:
            // 게임오버 상태와 낙하 상태 확인
            if(g_flag == TRUE && g_Drop == FALSE)
                g_isJump = TRUE;
            break;
        }
        break;

    case WM_PAINT:
    {
        static HDC hdc, MemDC, tmpDC;
        static HBITMAP BackBit, oldBackBit;
        static RECT bufferRT;
        PAINTSTRUCT ps;

        hdc = BeginPaint(hWnd, &ps);
        GetClientRect(hWnd, &bufferRT);
        MemDC = CreateCompatibleDC(hdc);
        BackBit = CreateCompatibleBitmap(hdc, bufferRT.right, bufferRT.bottom);
        oldBackBit = (HBITMAP)SelectObject(MemDC, BackBit);
        PatBlt(MemDC, 0, 0, bufferRT.right, bufferRT.bottom, WHITENESS);
        tmpDC = hdc;
        hdc = MemDC;
        MemDC = tmpDC;

        // TODO: 여기에 그리기 코드를 추가합니다.

        // 표시 속도 조절
        Sleep(10);

        // 전체 게임 창 그리기
        Rectangle(hdc, g_Window.left, g_Window.top, g_Window.right, g_Window.bottom);
        
        // 바닥 그리기
        Rectangle(hdc, g_bottom.left, g_bottom.top, g_bottom.right, g_bottom.bottom);

        // 플레이어 그리기
        Rectangle(hdc, g_me.left, g_me.top + g_JumpHeight, g_me.right, g_me.bottom + g_JumpHeight);

        // 진행도 표시
        wsprintfW(Progress_Text, L"%d %s", (100 - ((g_Finish[1].left - g_Finish[0].right) / (g_ProgressFinish / 100))), L"%");
        TextOut(hdc, 750, 10, Progress_Text, lstrlenW(Progress_Text));
        Rectangle(hdc, Window_ProgressBar.left, Window_ProgressBar.top, Window_ProgressBar.right, Window_ProgressBar.bottom);
        FillRect(hdc, &Window_ProgressBar, CreateSolidBrush(RGB(0, 0, 0)));
        Rectangle(hdc, Progress_Rect.left, Progress_Rect.top, Progress_Rect.right + Progress_Increase, Progress_Rect.bottom);

        // 발판 그리기
        for (int i = 0; i < g_scafNum; i++) {
            Rectangle(hdc, g_scaf[i].left, g_scaf[i].top, g_scaf[i].right, g_scaf[i].bottom);
        }

        // 장애물 그리기
        for (int j = 1; j < (g_obsNum * 3); (j += 3)) {
            MoveToEx(hdc, g_NowObs[j].left, g_NowObs[j].top, NULL);
            LineTo(hdc, g_NowObs[j - 1].left, g_NowObs[j - 1].top);
            MoveToEx(hdc, g_NowObs[j].left, g_NowObs[j].top, NULL);
            LineTo(hdc, g_NowObs[j + 1].left, g_NowObs[j + 1].top);
        }

        // 함수
        Jump();
        me_Drop();
        Move_Timer(hWnd);

        InvalidateRect(hWnd, NULL, FALSE);
        
        // End TODO

        tmpDC = hdc;
        hdc = MemDC;
        MemDC = tmpDC;
        GetClientRect(hWnd, &bufferRT);
        BitBlt(hdc, 0, 0, bufferRT.right, bufferRT.bottom, MemDC, 0, 0, SRCCOPY);
        SelectObject(MemDC, oldBackBit);
        DeleteObject(BackBit);
        DeleteDC(MemDC);
        EndPaint(hWnd, &ps);
        break;

    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
    }
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