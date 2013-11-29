// mypaint.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "mypaint.h"
#include "CMyListHdc.h"
#include "ListBox.h"
#include "commctrl.h"
#include "Commdlg.h"
#pragma   comment(lib,   "comctl32.lib")


//------ Global Variables:-----------------------------------------------------------------
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name
TCHAR szChildWndClass[MAX_LOADSTRING];
TCHAR szColorWndClass[MAX_LOADSTRING]           =  TEXT("colorbar"); /*********************colorbar*/
HWND sub_hWnd;					//子窗口
int PENSTYLE=FIRST_BUTTON+6 ;		//画笔类型
//int COLOR=FIRST_COLOR;			//画笔颜色
//COLORREF FrontColor;		//前景色
//COLORREF BackColor;			//背景色
brush mybrush;				//结构体mybrush含笔刷的各种参数
bool btn_down=0;
//HCURSOR hCursor;

CMyListHdc g_hdcList; ////////////////////////////////

RECT rcClient; // 客户区矩形

HWND g_hWndListBox; ///////////ListBox句柄


HDC g_hdcMem; /////////////// 内存中的dc 用于缓冲
HBITMAP g_hbitMap; ////////////////内存dc选择的Bitmap
HDC  g_hdcFormer;  ///////////////你上次画的dc 就是如果你要撤销  这个dc上的内容就应该显示到屏幕上
COLORREF g_FrontColor = RGB(0,0,0);  ////////当前前景色开始为黑
COLORREF g_BackColor = RGB(255,255,255)	; /////////当前背景色开始为白
/* 当前的颜色表*/
COLORREF g_colors[28] =             
{
	RGB(0,0,0),
	RGB(128,128,128),
	RGB(128,0,0),
	RGB(128,128,0),
	RGB(0,128,0),
	RGB(0,128,128),
	RGB(0,0,128),
	RGB(128,0,128),
	RGB(128,128,0),
	RGB(0,0,0),
	RGB(0,128,128),
	RGB(0,0,128),
	RGB(128,0,128),
	RGB(128,0,0),
	RGB(255,255,255),
	RGB(192,192,192),
	RGB(255,0,0),
	RGB(255,255,0),
	RGB(0,255,0),
	RGB(0,255,255),
	RGB(0,0,255),
	RGB(255,0,255),
	RGB(192,192,192),
	RGB(0,128,128),
	RGB(192,192,192),
	RGB(192,192,192),
	RGB(128,0,128),
	RGB(128,128,0)
};

/*----------------------------------------------------------------------------------------------*/

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
ATOM              MyChildRegisterClass(HINSTANCE hInstance);
ATOM			MyRegisterClassColorBar(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK  ChildProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	  About(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK      ColorBarWndProc(HWND, UINT, WPARAM, LPARAM); /************************/// 消息处理函数

int APIENTRY _tWinMain(HINSTANCE hInstance,
					   HINSTANCE hPrevInstance,
					   LPTSTR    lpCmdLine,
					   int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// TODO: Place code here.
	MSG msg;
	HACCEL hAccelTable;

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_MYPAINT, szWindowClass, MAX_LOADSTRING);

	wcscpy_s(szChildWndClass , MAX_LOADSTRING , L"childWnd");

	MyRegisterClass(hInstance);
	MyChildRegisterClass(hInstance);
	MyRegisterClassColorBar(hInstance); /******************************************/// 注册窗口类

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_MYPAINT));

	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0))
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
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
//  COMMENTS:
//
//    This function and its usage are only necessary if you want this code
//    to be compatible with Win32 systems prior to the 'RegisterClassEx'
//    function that was added to Windows 95. It is important to call this function
//    so that the application will get 'well formed' small icons associated
//    with it.
//
ATOM MyChildRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= ChildProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MYPAINT));
	wcex.hCursor		= NULL;
	wcex.hbrBackground	= (HBRUSH)::CreateSolidBrush(GetSysColor(COLOR_BTNFACE));
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_MYPAINT);
	wcex.lpszClassName	= szChildWndClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MYPAINT));
	wcex.hCursor		= NULL;
	wcex.hbrBackground	= (HBRUSH)(CreateSolidBrush(GetSysColor(COLOR_BTNFACE)));
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_MYPAINT);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

/////************************************************************注册COLORBAR窗口类/////////
ATOM		MyRegisterClassColorBar(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;
	ZeroMemory(&wcex,sizeof(wcex) );
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style			= CS_DBLCLKS;
	wcex.lpfnWndProc	= ColorBarWndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MYPAINT));
	wcex.hCursor		= NULL;
	wcex.hbrBackground  = (HBRUSH)::CreateSolidBrush(RGB(255,255,255));

	wcex.lpszMenuName	= NULL;
	wcex.lpszClassName	= szColorWndClass;
	wcex.hIconSm		= NULL;

	return RegisterClassEx(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	HWND hWnd;

	hInst = hInstance; // Store instance handle in our global variable
	hWnd = CreateWindow(szWindowClass, szTitle,WS_OVERLAPPEDWINDOW|WS_VSCROLL|WS_HSCROLL,CW_USEDEFAULT, 0,PAINTRECT_WIDTH+100,PAINTRECT_HEIGHT+130,
		NULL, NULL, hInstance, NULL);
	if (!hWnd)
	{
		return FALSE;
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	return TRUE;
}


//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;
	RECT rect = {0,0,PAINTRECT_WIDTH,PAINTRECT_HEIGHT};  //////////////////客户区大小

	static HBRUSH hbGray = ::CreateSolidBrush( GetSysColor(COLOR_BTNFACE) );
	static RECT   rectList   =  {0,0,40,80};
	LPDRAWITEMSTRUCT drawItem = NULL;   //DrawItem
	int current;  ////////当前 ListBox 的选择
	TCHAR ttt[100];
	int currenttool;

	switch (message)
	{
	case WM_WINDOWPOSCHANGED:
		UpdateWindow(hWnd);
		break;
	case WM_CREATE:


		BITMAP bi1;
		HIMAGELIST himl1;
		HWND hWndBar1,hWndBar2;
		HBITMAP hBitmapTemp1;
		hBitmapTemp1 = LoadBitmap(hInst , MAKEINTRESOURCE(IDB_BITMAP1));
		GetObject(hBitmapTemp1 , sizeof(BITMAP) , &bi1);

		sub_hWnd =  CreateWindow(szChildWndClass, NULL,0|WS_VISIBLE |WS_CHILD |WS_BORDER,
			50, 50, PAINTRECT_WIDTH, PAINTRECT_HEIGHT, hWnd, NULL, hInst, NULL);

		TBBUTTON tButtons1[BUTTON_COUNT];
		for(int i = 0 ; i < BUTTON_COUNT ; ++i)
		{
			tButtons1[i].iBitmap = i ;                       // bitmap index
			tButtons1[i].idCommand = FIRST_BUTTON + i;              // command id
			tButtons1[i].fsState = TBSTATE_ENABLED;      //button state
			tButtons1[i].fsStyle = BTNS_CHECKGROUP | TBSTYLE_FLAT;            // Button style 有按下时的显示状态
			ZeroMemory(tButtons1[i].bReserved , sizeof(BYTE) * 2);
			tButtons1[i].iString = 0;
			tButtons1[i].dwData = 0L;
		}


		himl1=ImageList_Create(16,16,ILC_COLOR32|ILC_MASK,BUTTON_COUNT,0);
		//加入事先作好的工具条位图IDB_BITMAP 
		for (int i=0;i<BUTTON_COUNT;i++)
		{
			HBITMAP hBitmapTemp = LoadBitmap(hInst , MAKEINTRESOURCE(IDB_BITMAP1 + i));
			ImageList_Add(himl1 , hBitmapTemp , NULL);
		}

		GetClientRect(hWnd,&rcClient);

		hWndBar1 = CreateWindowEx(NULL , 
			TOOLBARCLASSNAME ,
			NULL , 
			WS_VISIBLE | WS_CHILD |CCS_VERT | WS_BORDER|CCS_NORESIZE | TBSTYLE_WRAPABLE , 
			0,50,48,/*rcClient.bottom*/ 183, 
			hWnd , 
			NULL , 
			hInst , 
			NULL);//create toolbar
		int ret1;

		ret1 = SendMessage(hWndBar1 , TB_SETIMAGELIST , 0, (LPARAM)himl1);
		ret1 = SendMessage(hWndBar1, TB_BUTTONSTRUCTSIZE, (WPARAM)sizeof(TBBUTTON), 0);
		ret1 = SendMessage(hWndBar1, TB_ADDBUTTONS, BUTTON_COUNT, (LPARAM)&tButtons1);
		::SendMessage(hWndBar1,TB_CHECKBUTTON,FIRST_BUTTON+6,MAKELONG(TRUE,0) ); //打开画板时,默认为选择铅笔 


		hWndBar2 = CreateWindowEx(NULL,szColorWndClass,NULL,WS_CHILD | WS_VISIBLE |WS_BORDER ,2,0 + 2,290,48,hWnd,NULL,hInst,NULL);
		ShowWindow(hWndBar2,SW_SHOW);
		UpdateWindow(hWndBar2);

		g_hWndListBox = CreateWindowEx(WS_EX_CLIENTEDGE,TEXT("LISTBOX"),NULL,WS_CHILDWINDOW|LBS_OWNERDRAWFIXED|LBS_NOTIFY|WS_VISIBLE ,2,240,40,100,hWnd,(HMENU)0xff00,hInst,NULL);

		break;
	case WM_INITMENUPOPUP: ///////////能否撤销 重复
		wmId = LOWORD(lParam);
		if(wmId == 1) //Edit
		{

			::EnableMenuItem((HMENU)wParam,IDM_UNDO,(g_hdcList.bCanUndo())?MF_ENABLED:MF_GRAYED);
			::EnableMenuItem((HMENU)wParam,IDM_REDO,g_hdcList.bCanRestore()?MF_ENABLED:MF_GRAYED);

		}

		break;
	case WM_DRAWITEM:  ///////////////////////画那个ListBox
		drawItem = NULL;   //DrawItem
		drawItem = (DRAWITEMSTRUCT*)lParam;
		current = SendMessage(g_hWndListBox,LB_GETCURSEL,0,0);
		mybrush.MyStyle = current;
		FillRect(drawItem->hDC,&rect,hbGray);
		TCHAR ttt[100];
		switch(PENSTYLE-FIRST_BUTTON)
		{
		case 2:      //////////Eraser
			DrawEraser(drawItem->hDC,current);
			break;   
		case 5:     ///////////////////Zoom
			DrawZoom(drawItem->hDC,current);
			break;
		case 7:    ////////Brush
			DrawBrush(drawItem->hDC,current);
			break;
		case 8:    ////////喷抢
			DrawGun(drawItem->hDC,current);
			break;
		case 10:  ///////////Line
			DrawLine(drawItem->hDC,current);
			break;
		case 12:
		case 13:
		case 14:
		case 15:  /////////矩形
			DrawRect(drawItem->hDC,current);
			break;
		}			
		break;

	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// Parse the menu selections:/
		///////////////////处理ListBox

		switch (wmId)
		{
			//------------------TOOLBAR1消息处理:-----------------------
		case FIRST_BUTTON:
			PENSTYLE=FIRST_BUTTON;
			SendMessage(g_hWndListBox,LB_RESETCONTENT,0,0);
			break;
		case FIRST_BUTTON+1:
			PENSTYLE=FIRST_BUTTON+1;
			SendMessage(g_hWndListBox,LB_RESETCONTENT,0,0);
			break;
		case FIRST_BUTTON+2:        ///Eraser
			PENSTYLE=FIRST_BUTTON+2;
			SendMessage(g_hWndListBox,LB_RESETCONTENT,0,0);
			SendMessage(g_hWndListBox,LB_ADDSTRING,0,0);
			SendMessage(g_hWndListBox,LB_ADDSTRING,0,1);
			SendMessage(g_hWndListBox,LB_ADDSTRING,0,2);
			SendMessage(g_hWndListBox,LB_ADDSTRING,0,3);
			SendMessage(g_hWndListBox,LB_SETITEMHEIGHT, 0,16);
			SendMessage(g_hWndListBox,LB_SETCURSEL,0,0);
			mybrush.MyStyle = 0;
			break;
		case FIRST_BUTTON+3:
			PENSTYLE=FIRST_BUTTON+3;
			SendMessage(g_hWndListBox,LB_RESETCONTENT,0,0);
			break;
		case FIRST_BUTTON+4:
			PENSTYLE=FIRST_BUTTON+4;
			SendMessage(g_hWndListBox,LB_RESETCONTENT,0,0);
			break;
		case FIRST_BUTTON+5:
			PENSTYLE=FIRST_BUTTON+5;
			SendMessage(g_hWndListBox,LB_RESETCONTENT,0,0);
			SendMessage(g_hWndListBox,LB_ADDSTRING,0,0);
			SendMessage(g_hWndListBox,LB_ADDSTRING,0,1);
			SendMessage(g_hWndListBox,LB_ADDSTRING,0,2);
			SendMessage(g_hWndListBox,LB_ADDSTRING,0,3);
			SendMessage(g_hWndListBox,LB_SETITEMHEIGHT, 0,16);
			SendMessage(g_hWndListBox,LB_SETCURSEL,0,0);
			mybrush.MyStyle = 0;
			break;
		case FIRST_BUTTON+6:
			PENSTYLE=FIRST_BUTTON+6;
			SendMessage(g_hWndListBox,LB_RESETCONTENT,0,0);
			break;
		case FIRST_BUTTON+7: /////////刷子
			PENSTYLE=FIRST_BUTTON+7;
			SendMessage(g_hWndListBox,LB_RESETCONTENT,0,0);
			SendMessage(g_hWndListBox,LB_ADDSTRING,0,0);
			SendMessage(g_hWndListBox,LB_ADDSTRING,0,1);
			SendMessage(g_hWndListBox,LB_ADDSTRING,0,2);
			SendMessage(g_hWndListBox,LB_ADDSTRING,0,3);
			SendMessage(g_hWndListBox,LB_SETITEMHEIGHT, 0,16);
			SendMessage(g_hWndListBox,LB_SETCURSEL,0,0);
			mybrush.MyStyle = 0;
			break;
		case FIRST_BUTTON+8: /////////喷抢
			PENSTYLE=FIRST_BUTTON+8;
			SendMessage(g_hWndListBox,LB_RESETCONTENT,0,0);
			SendMessage(g_hWndListBox,LB_ADDSTRING,0,0);
			SendMessage(g_hWndListBox,LB_ADDSTRING,0,1);
			SendMessage(g_hWndListBox,LB_ADDSTRING,0,2);
			SendMessage(g_hWndListBox,LB_SETITEMHEIGHT, 0,25);
			SendMessage(g_hWndListBox,LB_SETCURSEL,0,0);
			mybrush.MyStyle = 0;
			break;
		case FIRST_BUTTON+9:  
			PENSTYLE=FIRST_BUTTON+9;
			SendMessage(g_hWndListBox,LB_RESETCONTENT,0,0);
			break;
		case FIRST_BUTTON+10: //////////直线
			PENSTYLE=FIRST_BUTTON+10;
			SendMessage(g_hWndListBox,LB_RESETCONTENT,0,0);
			SendMessage(g_hWndListBox,LB_ADDSTRING,0,0);
			SendMessage(g_hWndListBox,LB_ADDSTRING,0,1);
			SendMessage(g_hWndListBox,LB_ADDSTRING,0,2);
			SendMessage(g_hWndListBox,LB_ADDSTRING,0,3);
			SendMessage(g_hWndListBox,LB_ADDSTRING,0,4);
			SendMessage(g_hWndListBox,LB_SETITEMHEIGHT, 0,12);
			SendMessage(g_hWndListBox,LB_SETCURSEL,0,0);
			mybrush.MyStyle = 0;
			break;
		case FIRST_BUTTON+11:
			PENSTYLE=FIRST_BUTTON+11;
			SendMessage(g_hWndListBox,LB_RESETCONTENT,0,0);
			break;
		case FIRST_BUTTON+12: ///////////矩形
			PENSTYLE=FIRST_BUTTON+12;
			SendMessage(g_hWndListBox,LB_RESETCONTENT,0,0);
			SendMessage(g_hWndListBox,LB_ADDSTRING,0,0);
			SendMessage(g_hWndListBox,LB_ADDSTRING,0,1);
			SendMessage(g_hWndListBox,LB_ADDSTRING,0,2);
			SendMessage(g_hWndListBox,LB_SETITEMHEIGHT, 0,18);
			SendMessage(g_hWndListBox,LB_SETCURSEL,0,0);
			mybrush.MyStyle = 0;
			break;
		case FIRST_BUTTON+13:
			PENSTYLE=FIRST_BUTTON+13;
			SendMessage(g_hWndListBox,LB_RESETCONTENT,0,0);
			SendMessage(g_hWndListBox,LB_ADDSTRING,0,0);
			SendMessage(g_hWndListBox,LB_ADDSTRING,0,1);
			SendMessage(g_hWndListBox,LB_ADDSTRING,0,2);
			SendMessage(g_hWndListBox,LB_SETITEMHEIGHT, 0,18);
			SendMessage(g_hWndListBox,LB_SETCURSEL,0,0);
			mybrush.MyStyle = 0;
			break;
		case FIRST_BUTTON+14:
			PENSTYLE=FIRST_BUTTON+14;
			SendMessage(g_hWndListBox,LB_RESETCONTENT,0,0);
			SendMessage(g_hWndListBox,LB_ADDSTRING,0,0);
			SendMessage(g_hWndListBox,LB_ADDSTRING,0,1);
			SendMessage(g_hWndListBox,LB_ADDSTRING,0,2);
			SendMessage(g_hWndListBox,LB_SETITEMHEIGHT, 0,18);
			SendMessage(g_hWndListBox,LB_SETCURSEL,0,0);
			mybrush.MyStyle = 0;
			break;
		case FIRST_BUTTON+15:
			SendMessage(g_hWndListBox,LB_RESETCONTENT,0,0);
			SendMessage(g_hWndListBox,LB_ADDSTRING,0,0);
			SendMessage(g_hWndListBox,LB_ADDSTRING,0,1);
			SendMessage(g_hWndListBox,LB_ADDSTRING,0,2);
			SendMessage(g_hWndListBox,LB_SETITEMHEIGHT, 0,18);
			SendMessage(g_hWndListBox,LB_SETCURSEL,0,0);
			mybrush.MyStyle = 0;
			PENSTYLE=FIRST_BUTTON+15;
			break;
		case FIRST_BUTTON+16:
			PENSTYLE=FIRST_BUTTON+16;
			break;
			//-------------------TOOLBAR2消息处理:-----------------------

			//----------------------------------------------------------
		case IDM_UNDO:  ////////////撤销
			if(g_hdcList.bCanUndo()  )
			{
				g_hdcList.Undo(rect);       //////////////rect没用

				g_hdcList.Top(hWnd,g_hdcMem);     ////////////////将最近的图像拷到内存中
				g_hdcFormer = g_hdcList.GetLast(); ////////////////这个你记住就行了
				///////////////////////////////刷新屏幕
				hdc = GetDC(sub_hWnd);
				BitBlt(hdc,0,0,PAINTRECT_WIDTH,PAINTRECT_HEIGHT,g_hdcMem,0,0,SRCCOPY);
				ReleaseDC(sub_hWnd,hdc);

			}
			break;
		case IDM_REDO:
			/////////////////////重复
			if(g_hdcList.bCanRestore()  )
			{
				g_hdcList.Restore(rect);    ////////////////////同上

				g_hdcList.Top(hWnd,g_hdcMem);     ////////////////将最近的图像拷到内存中
				g_hdcFormer = g_hdcList.GetLast(); ////////////////这个你记住就行了
				///////////////////////////////刷新屏幕
				hdc = GetDC(sub_hWnd);
				BitBlt(hdc,0,0,PAINTRECT_WIDTH,PAINTRECT_HEIGHT,g_hdcMem,0,0,SRCCOPY);
				ReleaseDC(sub_hWnd,hdc);
			}
			break;
		//case IDM_CLEAR:

		//	break;
		
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_PAINT:

		hdc = BeginPaint(hWnd, &ps);
		// TODO: Add any drawing code here...
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

LRESULT CALLBACK ChildProc(HWND hWnd , UINT message , WPARAM wParam , LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;		 /////////////////////// 屏幕的dc
	POINT point;   /////////////////记录鼠标的当前位置
	static POINT pointStart; /////////////////记录图形的起点


	RECT rect = {0,0,PAINTRECT_WIDTH,PAINTRECT_HEIGHT}; ////////////////////////屏幕的大小

	switch (message)
	{
	case WM_CREATE:
		hdc = GetDC(hWnd); //////////////////获得屏幕的dc
		g_hdcMem = CreateCompatibleDC(hdc);  /////创建内存中的dc
		g_hbitMap = CreateCompatibleBitmap(hdc,PAINTRECT_WIDTH,PAINTRECT_HEIGHT); /////////////////默认为PAINTRECT_WIDTH * PAINTRECT_HEIGHT 大小
		SelectObject(g_hdcMem,g_hbitMap);          //////////////////////把位图选入dc中
		ReleaseDC(hWnd,hdc);                   ////////////////////////释放屏幕的dc
		////////////////////////////////////初始化g_hInst
		////////////////////////////////////在内存中的dc 画一个 白色矩形 
		SelectObject(g_hdcMem,GetStockObject(NULL_PEN) );
		SelectObject(g_hdcMem,GetStockObject(WHITE_BRUSH) );
		Rectangle(g_hdcMem,0,0,rect.right,rect.bottom);
		SelectObject(g_hdcMem,GetStockObject(BLACK_PEN) );
		SelectObject(g_hdcMem,GetStockObject(WHITE_BRUSH) );
		g_hdcList.push(hWnd,g_hdcMem,rect); //////////////////////////////////将这个图形保存起来 rect没什么用的
		g_hdcFormer = g_hdcList.GetLast();  	/////////////hdcFormer就是上次你画好的东西 现在是一个白的矩形

		break;
		//case WM_NCHITTEST:
		//UpdateWindow(hWnd);
		//break;


	case WM_LBUTTONDOWN: //////////////////////////////////////////////////
		btn_down=1;
		///////////////////////记下矩形的起点 存入PointFrom中:

		mybrush.PointFrom.x=LOWORD(lParam);
		mybrush.PointFrom.y=HIWORD(lParam);
		break;
	case WM_MOUSEMOVE:  /////////////////////////////////////
		if (PENSTYLE==FIRST_BUTTON+3){SetCursor(LoadCursor(hInst,MAKEINTRESOURCE(IDC_CURSOR2)));}
		else if (PENSTYLE==FIRST_BUTTON+4)
		{SetCursor(LoadCursor(hInst,MAKEINTRESOURCE(IDC_CURSOR3)));}
		else if (PENSTYLE==FIRST_BUTTON+5)
		{SetCursor(LoadCursor(hInst,MAKEINTRESOURCE(IDC_CURSOR4)));}
		else if (PENSTYLE==FIRST_BUTTON+6)
		{SetCursor(LoadCursor(hInst,MAKEINTRESOURCE(IDC_CURSOR5)));}
		else if (PENSTYLE==FIRST_BUTTON+7)
		{SetCursor(LoadCursor(hInst,MAKEINTRESOURCE(IDC_CURSOR6)));}
		else if (PENSTYLE==FIRST_BUTTON+8)
		{SetCursor(LoadCursor(hInst,MAKEINTRESOURCE(IDC_CURSOR7)));}
		else if (PENSTYLE<FIRST_BUTTON+2||PENSTYLE>FIRST_BUTTON+8)
		{SetCursor(LoadCursor(hInst,MAKEINTRESOURCE(IDC_CURSOR1)));}
		///////////////////首先判断左键时候按下
		if(!btn_down)
			return 0;
		else
		{
			switch (PENSTYLE-FIRST_BUTTON)            //switch画笔类型
			{
			case 6:						//铅笔
				//BitBlt(g_hdcMem,0,0,PAINTRECT_WIDTH,PAINTRECT_HEIGHT,g_hdcFormer,0,0,SRCCOPY);
				mybrush.PointTo.x=LOWORD(lParam);
				mybrush.PointTo.y=HIWORD(lParam);
				//////////////hdc=GetDC(sub_hWnd);
				SelectObject(g_hdcMem,CreatePen(PS_SOLID,1,g_FrontColor));  //设置铅笔颜色
				MoveToEx(g_hdcMem,mybrush.PointFrom.x,mybrush.PointFrom.y,NULL);
				LineTo(g_hdcMem,mybrush.PointTo.x,mybrush.PointTo.y);
				mybrush.PointFrom=mybrush.PointTo;
				hdc = GetDC(hWnd);
				BitBlt(hdc,0,0,PAINTRECT_WIDTH,PAINTRECT_HEIGHT,g_hdcMem,0,0,SRCCOPY);
				ReleaseDC(hWnd,hdc);
				break;
			case 10:					//画直线
				BitBlt(g_hdcMem,0,0,PAINTRECT_WIDTH,PAINTRECT_HEIGHT,g_hdcFormer,0,0,SRCCOPY);
				/////////////////////////得到当先的点 
				mybrush.PointTo.x=LOWORD(lParam);
				mybrush.PointTo.y=HIWORD(lParam);
				/////////////////////////////在 内存中的dc画图
				SelectObject(g_hdcMem,CreatePen(PS_SOLID,1,g_FrontColor));         //画笔类型与颜色设置
				MoveToEx(g_hdcMem,mybrush.PointFrom.x,mybrush.PointFrom.y,NULL);
				LineTo(g_hdcMem,mybrush.PointTo.x,mybrush.PointTo.y);
				///////////////////////////////贴到屏幕上
				hdc = GetDC(hWnd);
				BitBlt(hdc,0,0,PAINTRECT_WIDTH,PAINTRECT_HEIGHT,g_hdcMem,0,0,SRCCOPY);
				ReleaseDC(hWnd,hdc);
				break;
			case 12:						//画矩形
				//--------------------先把上次画的东西拷入内存中的dc:
				BitBlt(g_hdcMem,0,0,PAINTRECT_WIDTH,PAINTRECT_HEIGHT,g_hdcFormer,0,0,SRCCOPY);
				/////////////////////////得到当先的点 
				mybrush.PointTo.x=LOWORD(lParam);
				mybrush.PointTo.y=HIWORD(lParam);
				/////////////////////////////在 内存中的dc画图
				SelectObject(g_hdcMem,GetStockObject(NULL_BRUSH)  );  //矩形内部透明
				SelectObject(g_hdcMem,CreatePen(PS_SOLID,1,g_FrontColor));
				Rectangle(g_hdcMem,mybrush.PointFrom.x,mybrush.PointFrom.y,mybrush.PointTo.x,mybrush.PointTo.y);
				///////////////////////////////贴到屏幕上
				hdc = GetDC(hWnd);
				BitBlt(hdc,0,0,PAINTRECT_WIDTH,PAINTRECT_HEIGHT,g_hdcMem,0,0,SRCCOPY);
				ReleaseDC(hWnd,hdc);
				break;
			case 14:						//画椭圆
				//------------先把上次画的东西拷入内存中的dc:
				BitBlt(g_hdcMem,0,0,PAINTRECT_WIDTH,PAINTRECT_HEIGHT,g_hdcFormer,0,0,SRCCOPY);
				mybrush.PointTo.x=LOWORD(lParam);  //当前点
				mybrush.PointTo.y=HIWORD(lParam);
				SelectObject(g_hdcMem,GetStockObject(NULL_BRUSH)  );  //内部透明
				SelectObject(g_hdcMem,CreatePen(PS_SOLID,1,g_FrontColor));
				Ellipse(g_hdcMem,mybrush.PointFrom.x,mybrush.PointFrom.y,mybrush.PointTo.x,mybrush.PointTo.y);
				//-----------显示在屏幕上:
				hdc = GetDC(hWnd);
				BitBlt(hdc,0,0,PAINTRECT_WIDTH,PAINTRECT_HEIGHT,g_hdcMem,0,0,SRCCOPY);
				ReleaseDC(hWnd,hdc);
				break;
			case 15:					//画圆角矩形
				BitBlt(g_hdcMem,0,0,PAINTRECT_WIDTH,PAINTRECT_HEIGHT,g_hdcFormer,0,0,SRCCOPY);
				mybrush.PointTo.x=LOWORD(lParam);
				mybrush.PointTo.y=HIWORD(lParam);
				SelectObject(g_hdcMem,GetStockObject(NULL_BRUSH) );///////////////
				SelectObject(g_hdcMem,CreatePen(PS_SOLID,1,g_FrontColor));
				RoundRect(g_hdcMem,mybrush.PointFrom.x,mybrush.PointFrom.y,mybrush.PointTo.x,mybrush.PointTo.y,
					(mybrush.PointTo.x-mybrush.PointFrom.x)/4,(mybrush.PointTo.y-mybrush.PointFrom.y)/4);
				hdc = GetDC(hWnd);
				BitBlt(hdc,0,0,PAINTRECT_WIDTH,PAINTRECT_HEIGHT,g_hdcMem,0,0,SRCCOPY);
				ReleaseDC(hWnd,hdc);
				break;

			}
			break;
		}
		break;

	case WM_LBUTTONUP: //////////////////////////////////////画完了 当前画的东西就被保存起来
		g_hdcList.push(hWnd,g_hdcMem,rect);          //////////////////rect这个参数没什么用随便添一个
		g_hdcFormer = g_hdcList.GetLast();						////////////然后再让 hdcFormer 变成你现在所画的东西
		mybrush.PointTo.x = (short) LOWORD (lParam) ;
		mybrush.PointTo.y = (short) HIWORD (lParam) ;
		btn_down = 0;

		break;
	case WM_PAINT:
		//if (PENSTYLE<FIRST_BUTTON||PENSTYLE>FIRST_BUTTON+15){break;} 
		hdc = BeginPaint(sub_hWnd, &ps);
		BitBlt(hdc,0,0,PAINTRECT_WIDTH,PAINTRECT_HEIGHT,g_hdcMem,0,0,SRCCOPY);
		//// TODO: Add any drawing code here...
		////画线:
		//MoveToEx(hdc,0,40,NULL);
		//LineTo(hdc,100,300);
		////SetPixel在指定的x和y坐标设置像素:
		//SetPixel(hdc,80,80,BLACK_BRUSH);

		EndPaint(sub_hWnd,&ps);


		//else {break;}

		break;

		//		//wmId    = LOWORD(wParam);
		//		//wmEvent = HIWORD(wParam);
		//		//// Parse the menu selections:
		//		//switch (wmId)
		//		//{
		//		//	case MOUSE_MOVED:
		//		//}
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// Message handler for about box.
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
////********************************colorbar窗口过程 *************/

LRESULT CALLBACK      ColorBarWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) /************************/// 消息处理函数
{
	HDC hdc;
	static HDC hdcMem;
	static HBITMAP bitmap;
	PAINTSTRUCT ps;

	HPEN hp ;
	HBRUSH hbf;
	HBRUSH hbb; 
	HPEN hptmp;
	RECT rectToDraw;
	HBRUSH hbtmp;
	RECT rectcolor;  //Judge which rect mouse belongs
	int x ;
	int	y ;
	int colorFill;
	DWORD rgbCurrent;
	COLORREF acrCustClr[16] = {0}; // array of custom colors 
	CHOOSECOLOR cc;
	switch(message)
	{
	case WM_CREATE:
		hdc = GetDC(hWnd);
		bitmap = LoadBitmap(hInst,MAKEINTRESOURCE(IDB_COLOR));
		hdcMem = CreateCompatibleDC(hdc);
		SelectObject(hdcMem,bitmap);
		ReleaseDC(hWnd,hdc);
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd,&ps);

		for(int i = 0;i < 28; i++)
		{
			hbtmp = CreateSolidBrush( g_colors[i] );
			rectToDraw.left = 32 + (i %14) * 16 ;
			rectToDraw.right = rectToDraw.left + 13 ;
			rectToDraw.top = (i>=14)?24:8;
			rectToDraw.bottom = rectToDraw.top + 13;
			FillRect(hdcMem,&rectToDraw,hbtmp);
			DeleteObject(hbtmp);
		}
	
		//DrawCurrent Front color and bk color
		hp = (HPEN)GetStockObject(NULL_PEN);
		hbf = CreateSolidBrush(g_FrontColor);
		hbb = CreateSolidBrush(g_BackColor);
		hptmp = (HPEN)SelectObject(hdcMem,hp);
		hbtmp = (HBRUSH)SelectObject(hdcMem,hbf);
		Rectangle(hdcMem,6  ,14,18 ,26);
		SelectObject(hdcMem,hbb);
		Rectangle(hdcMem,19,21,25,28);
		Rectangle(hdcMem,13,27,25,33);
		SelectObject(hdcMem,hptmp);
		SelectObject(hdcMem,hbtmp);
		DeleteObject(hp);
		DeleteObject(hbf);
		DeleteObject(hbb);
		DeleteObject(hptmp);
		DeleteObject(hbtmp);
		BitBlt(hdc,0+10+5,0,254+10,48,hdcMem,0,0,SRCCOPY);
		EndPaint(hWnd,&ps);
		break;

	case WM_LBUTTONDOWN:
		x = LOWORD(lParam);
		y = HIWORD(lParam);
		x-=10;
		colorFill = -1;
		for(int i = 0;i<=27;i++)
		{
			rectcolor.left = 33+ (i %14) * 16 ;
			rectcolor.right = rectcolor.left + 14 ;
			rectcolor.top = (i>=14)?22:8;
			rectcolor.bottom = rectcolor.top + 14;
			if( x >=rectcolor.left && x<=rectcolor.right && y>=rectcolor.top && y<=rectcolor.bottom)
			{
				colorFill = i;
				g_FrontColor = g_colors[i];  //改变前景色
				break;
			}
		}
		if(colorFill != -1)
		{		
			hdc = GetDC(hWnd);
			hbtmp = (HBRUSH)SelectObject(hdcMem,CreateSolidBrush(g_colors[colorFill]) );
			hptmp = (HPEN) SelectObject(hdcMem,GetStockObject(NULL_PEN) );
			Rectangle(hdcMem,6  ,14,18 ,26);
			DeleteObject( SelectObject(hdcMem,hbtmp) );
			DeleteObject( SelectObject(hdcMem,hptmp) );
			BitBlt(hdc,0+15,0,254 +10,48,hdcMem,0,0,SRCCOPY);
			ReleaseDC(hWnd,hdc);
		}
		return 0;

		break;
	case WM_RBUTTONDOWN:
		x = LOWORD(lParam);
		y = HIWORD(lParam);
		x-=10;
		//y-=5;
		colorFill = -1;
		for(int i = 0;i<=27;i++)
		{
			rectcolor.left = 33+ (i %14) * 16 ;
			rectcolor.right = rectcolor.left + 14 ;
			rectcolor.top = (i>=14)?22:8;
			rectcolor.bottom = rectcolor.top + 14;
			if( x >=rectcolor.left && x<=rectcolor.right && y>=rectcolor.top && y<=rectcolor.bottom)
			{
				colorFill = i;
				g_BackColor = g_colors[i];
				break;
			}
		}
	if(colorFill != -1)
	{		
		hdc = GetDC(hWnd);
		hbtmp = (HBRUSH)SelectObject(hdcMem,CreateSolidBrush(g_colors[colorFill]) );
		hptmp = (HPEN) SelectObject(hdcMem,GetStockObject(NULL_PEN) );
		Rectangle(hdcMem,19,21,25,28);
		Rectangle(hdcMem,13,27,25,33);
		DeleteObject( SelectObject(hdcMem,hbtmp) );
		DeleteObject( SelectObject(hdcMem,hptmp) );
		BitBlt(hdc,0+15,0,254+10,48,hdcMem,0,0,SRCCOPY);
		ReleaseDC(hWnd,hdc);
	}

	break;
	case WM_LBUTTONDBLCLK:
		x = LOWORD(lParam);
		y = HIWORD(lParam);
		x-=10;
		//y-=5;
		colorFill = -1;

		for(int i = 0;i<=27;i++)
		{
			rectcolor.left = 33+ (i %14) * 16 ;
			rectcolor.right = rectcolor.left + 14 ;
			rectcolor.top = (i>=14)?22:8;
			rectcolor.bottom = rectcolor.top + 14;
			if( x >=rectcolor.left && x<=rectcolor.right && y>=rectcolor.top && y<=rectcolor.bottom)
			{
				colorFill = i;
				break;
			}
		}

		if(colorFill != -1)
		{
			rgbCurrent = g_colors[colorFill];
			ZeroMemory(acrCustClr,sizeof(COLORREF)*16);
			ZeroMemory(&cc,sizeof(cc) );
			cc.lStructSize = sizeof(cc);
			cc.hwndOwner = hWnd;
			cc.lpCustColors = (LPDWORD) acrCustClr;
			cc.rgbResult = rgbCurrent;
			cc.Flags =   CC_RGBINIT | CC_FULLOPEN ;
			cc.lpfnHook =NULL;
			::ChooseColor(&cc);
			if(cc.rgbResult != g_colors[colorFill]) //color Changed
			{
				g_colors[colorFill]  = cc.rgbResult;
				g_FrontColor = cc.rgbResult;
				InvalidateRect(hWnd,NULL,TRUE);
			}
		}
		break;
	case WM_RBUTTONDBLCLK:
		x = LOWORD(lParam);
		y = HIWORD(lParam);
		x-=10;
		//y-=5;
		colorFill = -1;
	
		for(int i = 0;i<=27;i++)
		{
			rectcolor.left = 33+ (i %14) * 16 ;
			rectcolor.right = rectcolor.left + 14 ;
			rectcolor.top = (i>=14)?22:8;
			rectcolor.bottom = rectcolor.top + 14;
			if( x >=rectcolor.left && x<=rectcolor.right && y>=rectcolor.top && y<=rectcolor.bottom)
			{
				colorFill = i;
				break;
			}
		}

		if(colorFill != -1)
		{
			rgbCurrent = g_colors[colorFill];
			ZeroMemory(acrCustClr,sizeof(COLORREF)*16);
			ZeroMemory(&cc,sizeof(cc) );
			cc.lStructSize = sizeof(cc);
			cc.hwndOwner = hWnd;
			cc.lpCustColors = (LPDWORD) acrCustClr;
			cc.rgbResult = rgbCurrent;
			cc.Flags =   CC_RGBINIT | CC_FULLOPEN ;
			cc.lpfnHook = NULL;
			::ChooseColor(&cc);
			if(cc.rgbResult != g_colors[colorFill]) //color Changed
			{
		
				g_colors[colorFill]  = cc.rgbResult;
				g_BackColor = cc.rgbResult;

				InvalidateRect(hWnd,NULL,TRUE);
			}
		}
		break;
}
return DefWindowProc(hWnd,message,wParam,lParam);
}
