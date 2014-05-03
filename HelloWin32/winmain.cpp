#include <windows.h>
#include <windowsx.h>
#include <stdlib.h>
#include <string>
#include <tchar.h>
#include "resource.h"
#include <commdlg.h>
#include <map>

static TCHAR szWindowClass[] = _T("win32app");
static TCHAR szTitle[] = _T("My first WinAPI App");
HINSTANCE hInst;
HWND hDlgAbout = NULL;

const int WND_WIDTH = 600;
const int WND_HEIGHT = 400;

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK About(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK dialog2Func(HWND, UINT, WPARAM, LPARAM);

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
//HBRUSH brushes[_N_COLORS];
//COLORREF fonts[_N_COLORS];

const int brushSolidCode = HS_DIAGCROSS + 1;

const MyColor DEFAULT_PEN_COL = COL_BLACK;
const int DEFAULT_BRUSH_COL = COL_WHITE;
const int DEFAULT_BRUSH_STYLE = brushSolidCode;
const MyColor DEFAULT_FONT_COL = COL_BLACK;

int penColorMenuItems[_N_COLORS];
int brushColorMenuItems[_N_COLORS];
int brushStyleMenuItems[brushSolidCode+1];
int fontColorMenuItems[_N_COLORS];

// Current state
unsigned int currentBrushStyle = DEFAULT_BRUSH_STYLE;
MyColor currentPenColor = DEFAULT_PEN_COL;
COLORREF currentBrushColor = colors[DEFAULT_BRUSH_COL];
MyColor currentFontColor = DEFAULT_FONT_COL;

COLORREF currentFontCOLORREF = RGB(0, 0, 0);
HBRUSH currentBrush; //!!! !Not init! But init in _tWinMain in calling "initColors" functions
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

void inits() {
    for (int i = 0; i < _N_COLORS; ++i) {
        pens[i] = CreatePen(PS_SOLID, 2, colors[i]);
        //brushes[i] = CreateSolidBrush(colors[i]);
        //fonts[i] = CreateFont()
    }
    currentBrush = CreateSolidBrush(colors[COL_WHITE]);
    
    penColorMenuItems[COL_BLACK] = ID_PEN_BLACK;
    penColorMenuItems[COL_GREEN] = ID_PEN_GREEN;
    penColorMenuItems[COL_RED] = ID_PEN_RED;
    penColorMenuItems[COL_WHITE] = ID_PEN_WHITE;

    brushColorMenuItems[COL_BLACK] = ID_BRUSH_C_BLACK;
    brushColorMenuItems[COL_GREEN] = ID_BRUSH_C_GREEN;
    brushColorMenuItems[COL_RED] = ID_BRUSH_C_RED;
    brushColorMenuItems[COL_WHITE] = ID_BRUSH_C_WHITE;    

    fontColorMenuItems[COL_BLACK] = ID_FONT_BLACK;
    fontColorMenuItems[COL_GREEN] = ID_FONT_GREEN;
    fontColorMenuItems[COL_RED] = ID_FONT_RED;
    fontColorMenuItems[COL_WHITE] = ID_FONT_WHITE;

    brushStyleMenuItems[brushSolidCode] = ID_BRUSH_S_SOLID;
    brushStyleMenuItems[HS_HORIZONTAL] = ID_BRUSH_S_HORIZONTAL;
    brushStyleMenuItems[HS_VERTICAL] = ID_BRUSH_S_VERTICAL;
    brushStyleMenuItems[HS_FDIAGONAL] = ID_BRUSH_S_FDIAGONAL;
    brushStyleMenuItems[HS_BDIAGONAL] = ID_BRUSH_S_BDIAGONAL;
    brushStyleMenuItems[HS_CROSS] = ID_BRUSH_S_CROSS;
    brushStyleMenuItems[HS_DIAGCROSS] = ID_BRUSH_S_DIAGCROSS;
}

CHOOSECOLOR cc;                 // common dialog box structure
const COLORREF WH = RGB(255, 255, 255);
static COLORREF acrCustClr[16] = { WH, WH, WH, WH, WH, WH, WH, WH, WH, WH, WH, WH, WH, WH, WH, WH }; // array of custom colors 
//static DWORD rgbCurrent;        // initial color selection

template <size_t SIZE>
void setMenuChecks(HWND hWnd, int(&menuItemsColor)[SIZE], int currentColor) {
    for (int i = 0; i < SIZE; ++i) {
        UINT checked = (i == currentColor) ? MF_CHECKED : MF_UNCHECKED;
        CheckMenuItem(GetMenu(hWnd), menuItemsColor[i], checked);
    }
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


    inits();

    setMenuChecks(hWnd, brushColorMenuItems, DEFAULT_BRUSH_COL);
    setMenuChecks(hWnd, brushStyleMenuItems, DEFAULT_BRUSH_STYLE);
    setMenuChecks(hWnd, penColorMenuItems, DEFAULT_PEN_COL);
    setMenuChecks(hWnd, fontColorMenuItems, DEFAULT_FONT_COL);

    // Initialize CHOOSECOLOR 
    ZeroMemory(&cc, sizeof(cc));
    cc.lStructSize = sizeof(cc);
    cc.hwndOwner = hWnd;
    cc.lpCustColors = (LPDWORD)acrCustClr;
    //cc.rgbResult = rgbCurrent;
    cc.rgbResult = RGB(255,255,255);
    cc.Flags = /*CC_FULLOPEN |*/ CC_RGBINIT;

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


//bool chooseBrushFromArr = true;
bool choosePenFromArr = true;
bool chooseFontFromArr = true;

const std::map<int, MyColor> menuId2Color = { { ID_BRUSH_C_BLACK, COL_BLACK},
                                                { ID_BRUSH_C_GREEN, COL_GREEN},
                                                { ID_BRUSH_C_RED, COL_RED },
                                                { ID_BRUSH_C_WHITE, COL_WHITE}
}; // idToColor[ID_RED] = COL_RED

const std::map<int, int> menuId2Brush = { { ID_BRUSH_S_SOLID, brushSolidCode },
                                          {ID_BRUSH_S_HORIZONTAL, HS_HORIZONTAL},
                                          {ID_BRUSH_S_VERTICAL, HS_VERTICAL},
                                          {ID_BRUSH_S_FDIAGONAL, HS_FDIAGONAL},
                                          {ID_BRUSH_S_BDIAGONAL, HS_BDIAGONAL},
                                          {ID_BRUSH_S_CROSS, HS_CROSS},
                                          {ID_BRUSH_S_DIAGCROSS, HS_DIAGCROSS},
}; // idToColor[ID_BRUSH_S_HORIZONTAL] = HS_HORIZONTAL

void resetBrush();

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam){
    PAINTSTRUCT ps;
    HDC hdc;
    TCHAR greeting[] = _T("Hello, World!");
    int wmId, wmEvent;

    switch (message) {
    case WM_COMMAND:
        wmId = LOWORD(wParam);
        wmEvent = HIWORD(wParam);
        if (menuId2Color.count(wmId) > 0) {
            MyColor tmpMyColor = menuId2Color.at(wmId);
            currentBrushColor = colors[tmpMyColor];
            //currentBrushColor = colors[menuId2Color[wmId]];
            setMenuChecks(hWnd, brushColorMenuItems, tmpMyColor);
            resetBrush();
            redrawWindow(hWnd);
        } else {
            if (menuId2Brush.count(wmId) > 0) {
                int tmpBrushStyleCode = menuId2Brush.at(wmId);
                currentBrushStyle = tmpBrushStyleCode;
                setMenuChecks(hWnd, brushStyleMenuItems, tmpBrushStyleCode);
                resetBrush();
                redrawWindow(hWnd);
                
            } else {

                switch (wmId) {
                case ID_PEN_BLACK:
                    currentPenColor = COL_BLACK;
                    redrawWindow(hWnd);
                    setMenuChecks(hWnd, penColorMenuItems, currentPenColor);
                    break;
                case ID_PEN_GREEN:
                    currentPenColor = COL_GREEN;
                    redrawWindow(hWnd);
                    setMenuChecks(hWnd, penColorMenuItems, currentPenColor);
                    break;
                case ID_PEN_RED:
                    currentPenColor = COL_RED;
                    redrawWindow(hWnd);
                    setMenuChecks(hWnd, penColorMenuItems, currentPenColor);
                    break;
                case ID_PEN_WHITE:
                    currentPenColor = COL_WHITE;
                    redrawWindow(hWnd);
                    setMenuChecks(hWnd, penColorMenuItems, currentPenColor);
                    break;
                /*case ID_BRUSH_C_BLACK:
                    currentBrushColor = colors[COL_BLACK];
                    redrawWindow(hWnd);
                    setMenuChecks(hWnd, brushColorMenuItems, COL_BLACK);
                    break;
                case ID_BRUSH_C_GREEN:
                    currentBrushColor = colors[COL_GREEN];
                    redrawWindow(hWnd);
                    setMenuChecks(hWnd, brushColorMenuItems, COL_GREEN);
                    break;
                case ID_BRUSH_C_RED:
                    currentBrushColor = colors[COL_RED];
                    redrawWindow(hWnd);
                    setMenuChecks(hWnd, brushColorMenuItems, COL_RED);
                    break;
                case ID_BRUSH_C_WHITE:
                    currentBrushColor = colors[COL_WHITE];
                    redrawWindow(hWnd);
                    setMenuChecks(hWnd, brushColorMenuItems, COL_WHITE);
                    break;*/
                case ID_BRUSH_C_CHOOSECOLOR:
                    if (ChooseColor(&cc) == TRUE) {
                        currentBrushColor = cc.rgbResult;
                        //rgbCurrent = cc.rgbResult;
                        //chooseBrushFromArr = false;

                    }
                    for (int i = 0; i < sizeof(brushColorMenuItems) / sizeof(int); ++i) {
                        CheckMenuItem(GetMenu(hWnd), brushColorMenuItems[i], MF_UNCHECKED);
                    }
                    resetBrush();
                    redrawWindow(hWnd);
                    break;

                /*case ID_BRUSH_S_SOLID:
                    currentBrushStyle = brushSolidCode;
                    break;
                case ID_BRUSH_S_HORIZONTAL:
                    currentBrushStyle = HS_HORIZONTAL;
                    break;
                case ID_BRUSH_S_VERTICAL:
                    currentBrushStyle = HS_VERTICAL;
                    break;
                case ID_BRUSH_S_FDIAGONAL:
                    currentBrushStyle = HS_FDIAGONAL;
                    break;
                case ID_BRUSH_S_BDIAGONAL:
                    currentBrushStyle = HS_BDIAGONAL;
                    break;
                case ID_BRUSH_S_CROSS:
                    currentBrushStyle = HS_CROSS;
                    break;
                case ID_BRUSH_S_DIAGCROSS:
                    currentBrushStyle = HS_DIAGCROSS;*/

                case ID_FONT_BLACK:
                    currentFontColor = COL_BLACK;
                    redrawWindow(hWnd);
                    setMenuChecks(hWnd, fontColorMenuItems, currentFontColor);
                    break;
                case ID_FONT_GREEN:
                    currentFontColor = COL_GREEN;
                    redrawWindow(hWnd);
                    setMenuChecks(hWnd, fontColorMenuItems, currentFontColor);
                    break;
                case ID_FONT_RED:
                    currentFontColor = COL_RED;
                    redrawWindow(hWnd);
                    setMenuChecks(hWnd, fontColorMenuItems, currentFontColor);
                    break;
                case ID_FONT_WHITE:
                    currentFontColor = COL_WHITE;
                    redrawWindow(hWnd);
                    setMenuChecks(hWnd, fontColorMenuItems, currentFontColor);
                    break;
                case ID_FONT_CHOOSECOLOR:
                    if (ChooseColor(&cc) == TRUE) {
                        currentFontCOLORREF = cc.rgbResult;
                        //rgbCurrent = cc.rgbResult;
                        //chooseBrushFromArr = false;
                    }
                    for (int i = 0; i < sizeof(brushColorMenuItems) / sizeof(int); ++i) {
                        CheckMenuItem(GetMenu(hWnd), brushColorMenuItems[i], MF_UNCHECKED);
                    }
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
                    //case ID_DIALOG:
                    //DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOG2), hwnd, dialog2Func);
                    //break;
                case ID_EXIT:
                    PostQuitMessage(0);
                    break;
                default:
                    return DefWindowProc(hWnd, message, wParam, lParam);
                }
            }
        }
    case WM_PAINT: {

        hdc = BeginPaint(hWnd, &ps);
        
        SelectObject(hdc, pens[currentPenColor]);
        /*if (currentBrushStyle < brushSolidCode) {
            SelectObject(hdc, brushes[currentBrushColor]);
        } else {
            SelectObject(hdc, hBrushCur);
            chooseBrushFromArr = true;
        }*/
        SelectObject(hdc, currentBrush);
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
        SetTextColor(hdc, colors[currentFontColor]);
        TextOut(hdc, 50, 50, _T("lala"), _tcslen(_T("lala")));
        SelectObject(hdc, hFontDef);
        SetTextColor(hdc, colors[COL_BLACK]);
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

void resetBrush() {
    DeleteObject(currentBrush);
    if (currentBrushStyle < brushSolidCode) {
        currentBrush = CreateHatchBrush(currentBrushStyle, currentBrushColor);
    } else {
        currentBrush = CreateSolidBrush(currentBrushColor);
    }
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

INT_PTR CALLBACK dialog2Func(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
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
