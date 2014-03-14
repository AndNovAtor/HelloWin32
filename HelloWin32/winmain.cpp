#include <windows.h>
#include <windowsx.h>
#include <stdlib.h>
#include <string>
#include <tchar.h>
#include "resource.h"

static TCHAR szWindowClass[] = _T("win32app");
static TCHAR szTitle[] = _T("My first WinAPI App");
HINSTANCE hInst;
HWND hDlgAbout = NULL;

const int WND_WIDTH = 600;
const int WND_HEIGHT = 400;

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

HWND initWindow(HINSTANCE hInstance) {
    WNDCLASS wc;
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszMenuName = MAKEINTRESOURCE(IDR_MENU1);
    wc.lpszClassName = szWindowClass;
    if (!RegisterClass(&wc)) {
        MessageBox(NULL, _T("Call to RegisterClassEx failed!"), _T("Error in My first WinAPI App"), MB_OK);

        return 0;
    }

    // The parameters to CreateWindow explained:
    // szWindowClass: the name of the application
    // szTitle: the text that appears in the title bar
    // WS_OVERLAPPEDWINDOW: the type of window to create
    // CW_USEDEFAULT, CW_USEDEFAULT: initial position (x, y)
    // 500, 100: initial size (width, length)
    // NULL: the parent of this window
    // NULL: this application dows not have a menu bar
    // hInstance: the first parameter from WinMain
    // NULL: not used in this application
    return CreateWindow(
        szWindowClass,
        szTitle,
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        WND_WIDTH, WND_HEIGHT,
        NULL,
        NULL,
        hInstance,
        NULL
        );
}


enum MyColor {
    COL_BLACK,
    COL_GREEN,
    COL_RED,
    COL_WHITE,

    _N_COLORS
};

const COLORREF colors[_N_COLORS] = { RGB(0, 0, 0), RGB(0, 255, 0), RGB(255, 0, 0), RGB(255,255,255) };
HPEN pens[_N_COLORS];
HBRUSH brushes[_N_COLORS];
HFONT fonts[_N_COLORS];

int menuPenColor[_N_COLORS];
int menuBrushColor[_N_COLORS];
int menuFontColor[_N_COLORS];

// Current state
MyColor currentPenColor = COL_BLACK;
MyColor currentBrushColor = COL_WHITE;
MyColor currentFontColor = COL_BLACK;

HFONT hFontDef = NULL;
HFONT hFontCur = NULL;
HFONT hFontSome = CreateFont(20, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_OUTLINE_PRECIS,
    CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, VARIABLE_PITCH, TEXT("Arial Black"));
HBRUSH hBrushCur = NULL;
HBRUSH hBrushDef = NULL;
HBRUSH hBrushSolidDef = CreateSolidBrush(RGB(255,255,255));

HPEN hPenCur = NULL;
HPEN hPenDef = NULL;
HPEN hPenSolid = CreatePen(PS_SOLID, 0, RGB(0, 0, 0));


int curXPos = 0;
int curYPos = 0;
RECT coordRect = { WND_WIDTH - 100, 5, WND_WIDTH, 25 };

void initColors() {
    for (int i = 0; i < _N_COLORS; ++i) {
        pens[i] = CreatePen(PS_SOLID, 2, colors[i]);
        brushes[i] = CreateSolidBrush(colors[i]);
        //fonts[i] = CreateFont()
    }
    menuPenColor[COL_BLACK] = ID_PEN_BLACK;
    menuPenColor[COL_GREEN] = ID_PEN_GREEN;
    menuPenColor[COL_RED] = ID_PEN_RED;
    menuPenColor[COL_WHITE] = ID_PEN_WHITE;
    menuBrushColor[COL_BLACK] = ID_BRUSH_S_BLACK;
    menuBrushColor[COL_GREEN] = ID_BRUSH_S_GREEN;
    menuBrushColor[COL_RED] = ID_BRUSH_S_RED;
    menuBrushColor[COL_WHITE] = ID_BRUSH_S_WHITE;
    menuFontColor[COL_BLACK] = ID_FONT_BLACK;
    menuFontColor[COL_GREEN] = ID_FONT_GREEN;
    menuFontColor[COL_RED] = ID_FONT_RED;
    menuFontColor[COL_WHITE] = ID_FONT_WHITE;
}

int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow) {
    
    UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

    hInst = hInstance; // Store instance handle in our global variable
    
    HWND hWnd = initWindow(hInstance);
    if (hWnd == 0) {
        MessageBox(NULL,
            _T("Call to CreateWindow failed!"),
            _T("Win32 Guided Tour"),
            NULL);

        return 1;
    }
    initColors();
    // The parameters to ShowWindow explained:
    // hWnd: the value returned from CreateWindow
    // nCmdShow: the fourth parameter from WinMain
    ShowWindow(hWnd,
        nCmdShow);
    UpdateWindow(hWnd);


    HACCEL hAccelTable;
    hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDR_ACCELERATOR1));

    // Main message loop:
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int)msg.wParam;
}

inline void redrawWindow(HWND hWnd, LPRECT rect = NULL){
    InvalidateRect(hWnd, rect, true);
    UpdateWindow(hWnd);
}

template <size_t SIZE>
void setMenuChecks(HWND hWnd, int (&menuItemsColor)[SIZE], MyColor currentColor) {
    for (int i = 0; i < SIZE; ++i) {
        UINT checked = (i == currentColor) ? MF_CHECKED : MF_UNCHECKED;
        CheckMenuItem(GetMenu(hWnd), menuItemsColor[i], checked);
    }
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam){
    PAINTSTRUCT ps;
    HDC hdc;
    TCHAR greeting[] = _T("Hello, World!");
    int wmId, wmEvent;

    switch (message) {
    case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
        switch (wmId) {
        case ID_PEN_BLACK:
            currentPenColor = COL_BLACK;
            redrawWindow(hWnd);
            setMenuChecks(hWnd, menuPenColor, currentPenColor);
            break;
        case ID_PEN_GREEN:
            currentPenColor = COL_GREEN;
            redrawWindow(hWnd);
            setMenuChecks(hWnd, menuPenColor, currentPenColor);
            break;
        case ID_PEN_RED:
            currentPenColor = COL_RED;
            redrawWindow(hWnd);
            setMenuChecks(hWnd, menuPenColor, currentPenColor);
            break;
        case ID_PEN_WHITE:
            currentPenColor = COL_WHITE;
            redrawWindow(hWnd);
            setMenuChecks(hWnd, menuPenColor, currentPenColor);
            break;
        case ID_BRUSH_S_BLACK:
            currentBrushColor = COL_BLACK;
            redrawWindow(hWnd);
            setMenuChecks(hWnd, menuBrushColor, currentBrushColor);
            break;
        case ID_BRUSH_S_GREEN:
            currentBrushColor = COL_GREEN;
            redrawWindow(hWnd);
            setMenuChecks(hWnd, menuBrushColor, currentBrushColor);
            break;
        case ID_BRUSH_S_RED:
            currentBrushColor = COL_RED;
            redrawWindow(hWnd);
            setMenuChecks(hWnd, menuBrushColor, currentBrushColor);
            break;
        case ID_BRUSH_S_WHITE:
            currentBrushColor = COL_WHITE;
            redrawWindow(hWnd);
            setMenuChecks(hWnd, menuBrushColor, currentBrushColor);
            break;
        case ID_HELP:
            MessageBox(hWnd,
                _T("This is the short help"),
                _T("Help"),
                NULL);
            break;
        case ID_HELP_DIALOG:
            if (!IsWindow(hDlgAbout)) {
                hDlgAbout = CreateDialog(hInst, MAKEINTRESOURCE(IDD_DIALOG1), hWnd, About);
                ShowWindow(hDlgAbout, SW_SHOW);
            }
            break;
        case ID_EXIT:
            PostQuitMessage(0);
            break;
		default:
            return DefWindowProc(hWnd, message, wParam, lParam);
		}
    case WM_PAINT: {

        hdc = BeginPaint(hWnd, &ps);
        
        SelectObject(hdc, pens[currentPenColor]);
        SelectObject(hdc, brushes[currentBrushColor]);
        //SelectObject(hdc, brushes[currentBrushColor]);
        //SelectObject(hdc, fonts[currentFontColor]);
        //SetTextColor(hdc, fonts[currentColor]);
        TextOut(hdc,
            100, 100,
            greeting, _tcslen(greeting));

        Rectangle(hdc, 10, 10, 50, 50);

        hFontDef = (HFONT)GetCurrentObject(hdc, OBJ_FONT);
        const int BUF_SIZE = 50;
        TCHAR coordStr[BUF_SIZE];// , tmp[BUF_SIZE];
        _stprintf_s(coordStr, _T("(%d; %d)"), curXPos, curYPos);
        //_stprintf_s(tmp, _T("%d"), GetTextAlign(hdc));
        //FillRect(hdc, &coordRect, GetSysColorBrush(COLOR_WINDOW));
        SelectObject(hdc, hFontSome);
        TextOut(hdc, 50, 50, _T("lala"), _tcslen(_T("lala")));
        SelectObject(hdc, hFontDef);
        TextOut(hdc, coordRect.left, coordRect.top, coordStr, _tcslen(coordStr));
        //TextOut(hdc, 5, 200, tmp, _tcslen(tmp));
        EndPaint(hWnd, &ps);
        break; }
    case WM_LBUTTONDOWN:
    case WM_MOUSEMOVE:
        curXPos = GET_X_LPARAM(lParam);
        curYPos = GET_Y_LPARAM(lParam);
        redrawWindow(hWnd, &coordRect);

        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
        break;
    }
    return 0;
}

INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message) {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}