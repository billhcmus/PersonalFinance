// PersonalFinance.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "PersonalFinance.h"
#include <windowsx.h>
#include "commCtrl.h"
#include "CItem.h"
#include <vector>
#include <fstream>
#include <codecvt>
#define MAX_LOADSTRING 100

#pragma comment(linker,"\"/manifestdependency:type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#pragma comment(lib, "ComCtl32.lib")

// Global Variables:
HWND g_hwnd;
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

// Forward declarations of functions included in this code module:
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

    // TODO: Place code here.

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_PERSONALFINANCE, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_PERSONALFINANCE));

    MSG msg;

    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
			if (!IsDialogMessage(g_hwnd, &msg))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}	
        }
    }

    return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
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
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_PERSONALFINANCE));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_PERSONALFINANCE);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
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
   hInst = hInstance; // Store instance handle in our global variable

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

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
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
// Tọa độ test
int x, y;
// Tọa độ của các groupbox
int grX = 10, grY = 10;
// Mẫu Item cho combobox
const TCHAR* Item[] = { L"Ăn uống", L"Di chuyển", L"Nhà cửa", L"Xe cộ", L"Nhu yếu phẩm", L"Dịch vụ" };
long long MoneyEachType[6];
// Giao diện
// Font
HFONT hFont;
HWND hGroupboxThem;
HWND hTemp; // cho label để setfont
HWND hButtonThem;
HWND hCmbboxLoai;
HWND hEditboxNoiDung;
HWND hEditboxTien;
HWND hViewTongTien;

HWND hGroupboxDanhSach;
HWND hListview;
HWND hGroupboxChart;
// Chọn item combobox
int selItem;
// Số thứ tự của item vào list view
int seqNumItem = 0;
int curSelItem = -1; // item đang chọn ở list view
// vector chứa các loại chi tiêu
vector<CItem*> MyItem;
unsigned long long TongTien = 0;

// Cấu trúc cho biểu đồ
struct PieInfo
{
	float percentage;
	COLORREF color; // màu của 1 phần
	PieInfo() {} // khởi tạo mặc định
	PieInfo(COLORREF c)
	{
		this->color = c;
	}
};

PieInfo thePieInfo[6];

// Hàm tạo 1 list view
HWND CreateAListView(HWND hwndParent, long ID, HINSTANCE hInst,
	long lExtStyle, int x, int y, int nWidth, int nHeight, long lStyle)
{
	HWND hLV = CreateWindowEx(lExtStyle, WC_LISTVIEW, L"List View", WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_TABSTOP | lStyle, x, y, nWidth, nHeight, hwndParent, (HMENU)ID, hInst, NULL);
	// Thêm cột và thông tin cột cho list view
	LVCOLUMN lvCol;
	lvCol.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH;
	
	lvCol.fmt = LVCFMT_LEFT;
	lvCol.cx = 150;
	lvCol.pszText = L"Loại";
	ListView_InsertColumn(hLV, 0, &lvCol);

	lvCol.fmt = LVCFMT_LEFT;
	lvCol.cx = 150;
	lvCol.pszText = L"Tiền";
	ListView_InsertColumn(hLV, 1, &lvCol);

	lvCol.fmt = LVCFMT_LEFT;
	lvCol.cx = 245;
	lvCol.pszText = L"Chi tiết chi tiêu";
	ListView_InsertColumn(hLV, 2, &lvCol);
	return hLV;
}




void WriteListToFile(TCHAR* path)
{
	// Mở file để ghi
	const locale utf8_locale = locale(locale(), new codecvt_utf8<wchar_t>());
	wofstream f(path);
	f.imbue(utf8_locale);
	// Dòng đầu tiên là tổng tiền
	f << TongTien << endl;

	// Ứng với mỗi item ghi thông tin xuống
	for (int i = 0; i < MyItem.size(); i++)
	{
		f << MyItem[i]->LoaiChiTieu << endl;
		f << MyItem[i]->TienChi << endl;
		f << MyItem[i]->ChiTiet << endl;
	}
	f.close();
}

void LoadListFromFile(TCHAR* path)
{
	const locale utf8_locale = locale(locale(), new codecvt_utf8<wchar_t>());
	wfstream f;
	f.imbue(utf8_locale);
	f.open(path, ios::in);

	wstring buffer;
	if (f.is_open())
	{
		// Đọc tổng số tiền
		getline(f, buffer);
		TongTien = _wtoi64(buffer.c_str());

		// Lấy danh sách ra
		while (getline(f, buffer))
		{
			CItem* it = new CItem();
			// Lấy loại
			wcscpy_s(it->LoaiChiTieu, buffer.c_str());
			// Lấy tiền chi ra
			getline(f, buffer);
			it->TienChi = _wtoi64(buffer.c_str());
			// Lấy chi tiết 
			getline(f, buffer);
			it->ChiTiet = buffer;

			// Đưa vào list
			MyItem.push_back(it);
		}
	}
	f.close();
}

void AddItemToListViewFromFile(TCHAR* path)
{
	LoadListFromFile(L"info.txt");
	// Số lượng item trong MyItem được cập nhật lại
	LV_ITEM lvItem;
	for (int i = 0; i < MyItem.size(); i++)
	{
		// Cột 1
		lvItem.mask = LVIF_TEXT;
		lvItem.iItem = seqNumItem++;
		lvItem.iSubItem = 0;
		lvItem.pszText = MyItem[i]->LoaiChiTieu;
		ListView_InsertItem(hListview, &lvItem);

		// Cột 2
		lvItem.mask = LVIF_TEXT;
		lvItem.iSubItem = 1;
		TCHAR buff[30];
		wsprintf(buff, L"%I64u", MyItem[i]->TienChi);
		lvItem.pszText = buff;
		ListView_SetItem(hListview, &lvItem);

		// Cột 3
		lvItem.mask = LVIF_TEXT;
		lvItem.iSubItem = 2;
		lvItem.pszText = (WCHAR*)MyItem[i]->ChiTiet.c_str();
		ListView_SetItem(hListview, &lvItem);
	}
}


void drawPieChart(HDC hdc)
{
	TCHAR buf[30];
	wsprintf(buf, L"%I64u", TongTien);
	// Cập nhật label tổng tiền
	SetWindowText(hViewTongTien, buf);
	// Tính tổng tiền mỗi loại
	for (int i = 0; i < 6; i++)
	{
		// Reset
		MoneyEachType[i] = 0;
	}

	for (int i = 0; i < MyItem.size(); i++)
	{
		// Tính lại
		MoneyEachType[MyItem[i]->getType()] += MyItem[i]->TienChi;
	}
	int countType = 6;
	int margin = 50;
	int width = 420;
	int height = 420;

	RECT rect;
	rect.left = margin / 2;
	rect.top = grY + 120 + margin / 2;
	rect.right = width;
	rect.bottom = grY + 120 + height;
	
	HPEN pen = CreatePen(PS_SOLID, 2, RGB(0, 0, 255)); // bút có nét màu
	SelectObject(hdc, pen);
	Ellipse(hdc, rect.left, rect.top, rect.right, rect.bottom);

	double xCenter = (rect.left + rect.right) / 2;
	double yCenter = (rect.top + rect.bottom) / 2;
	static const double PI = 3.1415926535;
	double radius = (rect.bottom - rect.top) / 2; // bán kính

	thePieInfo[0].color = RGB(0, 255, 0);
	thePieInfo[1].color = RGB(255, 0, 0);
	thePieInfo[2].color = RGB(0, 0, 255);
	thePieInfo[3].color = RGB(128, 128, 128);
	thePieInfo[4].color = RGB(255, 255, 0);
	thePieInfo[5].color = RGB(0, 255, 255);

	// Tính phần trăm mỗi loại
	if (TongTien != 0)
	{
		for (int i = 0; i < countType; i++)
		{
			thePieInfo[i].percentage = ((MoneyEachType[i] * 100.0) / TongTien);
		}

		double percent = 0;
		for (int i = 0; i < countType; i++)
		{
			percent += thePieInfo[i].percentage;

			if (percent > 100)
				percent = 100;
			double angle = (percent / 100) * 360; // góc ở tâm đơn vị độ
			double x = radius * cos(angle * PI / 180);
			double y = radius * sin(angle * PI / 180);
			MoveToEx(hdc, xCenter, yCenter, NULL);
			LineTo(hdc, xCenter + x, yCenter + y);
		}
		radius = radius / 2;
		percent = 0;
		for (int i = 0; i < countType; i++)
		{
			percent += thePieInfo[i].percentage;
			if (percent > 100)
				percent = 100;
			double angle = (percent - thePieInfo[i].percentage / 2) / 100 * 360;
			double x = radius * cos(angle * PI / 180);
			double y = radius * sin(angle * PI / 180);
			HBRUSH br;
			br = CreateSolidBrush(thePieInfo[i].color);
			SelectObject(hdc, br);
			FloodFill(hdc, xCenter + x, yCenter + y, RGB(0, 0, 255)); // tham số cuối là màu biên để đổ
		}
	}
	// Chú thích
	int xPos = 445;
	int yPos = 170;
	for (int i = 0; i < countType; i++)
	{
		HPEN pen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0)); // tạo bút màu đen
		HBRUSH hbr = CreateSolidBrush(thePieInfo[i].color); // tạo chổi màu từng loại
		SelectObject(hdc, pen);
		Rectangle(hdc, xPos, yPos, xPos + 30, yPos + 25);
		RECT rect;
		rect.left = xPos;
		rect.top = yPos;
		rect.right = xPos + 30;
		rect.bottom = yPos + 25;
		FillRect(hdc, &rect, hbr);
		hTemp = CreateWindow(L"Static", Item[i], WS_CHILD | WS_VISIBLE, rect.right + 5, rect.top + 5, 100, 30, g_hwnd, NULL, hInst, NULL);
		SendMessage(hTemp, WM_SETFONT, WPARAM(hFont), TRUE);
		yPos += 40;
	}
}

void AddItemToListView(HWND hWnd)
{
	bool check;
	TCHAR* buffer;
	buffer = NULL;
	int lenCmbLoai, lenTien, lenChitiet;
	CItem* OneItem = new CItem();

	lenCmbLoai = GetWindowTextLength(hCmbboxLoai);
	check = true;
	if (lenCmbLoai == 0)
	{
		MessageBox(hWnd, L"Bạn chưa chọn loại chi tiêu", L"Thông báo", MB_OK);
		check = false;
	}
	lenTien = GetWindowTextLength(hEditboxTien);
	if (lenTien == 0)
	{
		MessageBox(hWnd, L"Bạn chưa nhập số tiền", L"Thông báo", MB_OK);
		check = false;
	}

	lenChitiet = GetWindowTextLength(hEditboxNoiDung);
	// Thêm vào list view
	if (check == true)
	{
		LV_ITEM lvItem;
		// Cột 1
		lvItem.mask = LVIF_TEXT;
		lvItem.iItem = seqNumItem++;
		lvItem.iSubItem = 0;
		lvItem.pszText = (LPWSTR)Item[selItem];
		ListView_InsertItem(hListview, &lvItem);
		wcscpy_s(OneItem->LoaiChiTieu, Item[selItem]);
		// Cột 2
		buffer = new TCHAR[lenTien + 1];
		GetWindowText(hEditboxTien, buffer, lenTien + 1);
		lvItem.mask = LVIF_TEXT;
		lvItem.iSubItem = 1;
		lvItem.pszText = buffer;
		ListView_SetItem(hListview, &lvItem);
		OneItem->TienChi = _wtoi64(buffer);
		TongTien += OneItem->TienChi;
		// Cột 3
		buffer = new TCHAR[lenChitiet + 1];
		GetWindowText(hEditboxNoiDung, buffer, lenChitiet + 1);
		lvItem.mask = LVIF_TEXT;
		lvItem.iSubItem = 2;
		lvItem.pszText = buffer;
		ListView_SetItem(hListview, &lvItem);
		OneItem->ChiTiet = buffer;

		// đưa vào vector
		MyItem.push_back(OneItem);

		// clear
		SetWindowText(hCmbboxLoai, L"");
		SetWindowText(hEditboxNoiDung, L"");
		SetWindowText(hEditboxTien, L"");
	}
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CREATE:
		InitCommonControls();
		SetWindowText(hWnd, L"Personal Finance");
		g_hwnd = hWnd;
		// get system font
		LOGFONT lf;
		GetObject(GetStockObject(DEFAULT_GUI_FONT), sizeof(LOGFONT), &lf);
		hFont = CreateFont(lf.lfHeight, lf.lfWidth,
			lf.lfEscapement, lf.lfOrientation, lf.lfWeight,
			lf.lfItalic, lf.lfUnderline, lf.lfStrikeOut, lf.lfCharSet,
			lf.lfOutPrecision, lf.lfClipPrecision, lf.lfQuality,
			lf.lfPitchAndFamily, lf.lfFaceName);
		hFont = CreateFont(20, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, L"Calibri");

		// Group 1
		hGroupboxThem = CreateWindow(L"Button", L"Thêm loại chi tiêu", WS_CHILD | WS_VISIBLE | BS_GROUPBOX, grX, grY, 580, 120, hWnd, NULL, hInst, NULL);
		SendMessage(hGroupboxThem, WM_SETFONT, WPARAM(hFont), TRUE);
		// Label 1
		hTemp = CreateWindow(L"Static", L"Loại chi tiêu", WS_CHILD | WS_VISIBLE, grX + 20, grY + 30, 80, 30, hWnd, NULL, hInst, NULL);
		// combobox
		hCmbboxLoai = CreateWindow(L"Combobox", L"", WS_CHILD | WS_VISIBLE | CBS_DROPDOWN | WS_TABSTOP, grX + 20, grY + 70, 125, 30, hWnd, NULL, hInst, NULL);
		SendMessage(hTemp, WM_SETFONT, WPARAM(hFont), TRUE);
		SendMessage(hCmbboxLoai, WM_SETFONT, WPARAM(hFont), TRUE);
		// Khởi tạo combobox
		for (int i = 0; i < 6; i++)
		{
			SendMessage(hCmbboxLoai, CB_ADDSTRING, 0, (LPARAM)Item[i]);
		}

		// label 2
		hTemp = CreateWindow(L"Static", L"Nội dung", WS_CHILD | WS_VISIBLE, grX + 180, grY + 30, 60, 30, hWnd, NULL, hInst, NULL);
		// textbox
		hEditboxNoiDung = CreateWindow(L"Edit", L"", WS_CHILD | WS_VISIBLE | WS_BORDER | WS_TABSTOP, grX + 180, grY + 70, 125, 30, hWnd, NULL, hInst, NULL);
		SendMessage(hTemp, WM_SETFONT, WPARAM(hFont), TRUE);
		SendMessage(hEditboxNoiDung, WM_SETFONT, WPARAM(hFont), TRUE);

		// label 3
		hTemp = CreateWindow(L"Static", L"Số tiền", WS_CHILD | WS_VISIBLE, grX + 350, grY + 30, 60, 30, hWnd, NULL, hInst, NULL);
		hEditboxTien = CreateWindow(L"Edit", L"", WS_CHILD | WS_VISIBLE | WS_BORDER | WS_TABSTOP | ES_NUMBER, grX + 350, grY + 70, 125, 30, hWnd, NULL, hInst, NULL);
		SendMessage(hTemp, WM_SETFONT, WPARAM(hFont), TRUE);
		SendMessage(hEditboxTien, WM_SETFONT, WPARAM(hFont), TRUE);
		
		// button Them
		hButtonThem = CreateWindow(L"Button", L"Thêm", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | WS_TABSTOP, grX + 500, grY + 70, 60, 30, hWnd, (HMENU)IDC_BTN_THEM, hInst, NULL);
		SendMessage(hButtonThem, WM_SETFONT, WPARAM(hFont), TRUE);
		
		// Group 2
		hGroupboxDanhSach = CreateWindow(L"Button", L"Danh sách chi tiêu", WS_CHILD | WS_VISIBLE | BS_GROUPBOX | WS_TABSTOP, grX + 585, grY, 580, 550, hWnd, NULL, hInst, NULL);
		SendMessage(hGroupboxDanhSach, WM_SETFONT, WPARAM(hFont), TRUE);
		// List view chứa dữ liệu
		hListview = CreateAListView(hWnd, NULL, hInst, WS_EX_CLIENTEDGE, grX + 600, grY + 30, 550, 500, LVS_REPORT | LVS_ICON | LVS_EDITLABELS | LVS_SHOWSELALWAYS);
		SendMessage(hListview, WM_SETFONT, WPARAM(hFont), TRUE);

		// Group 3
		hGroupboxChart = CreateWindow(L"Button", L"Biểu đồ thống kê", WS_CHILD | WS_VISIBLE | BS_GROUPBOX, grX, grY + 130, 580, 420, hWnd, NULL, hInst, NULL);
		SendMessage(hGroupboxChart, WM_SETFONT, WPARAM(hFont), TRUE);
		hTemp = CreateWindow(L"Static", L"Tổng Tiền", WS_CHILD | WS_VISIBLE, grX + 440, grY + 440, 70, 30, g_hwnd, NULL, hInst, NULL);
		SendMessage(hTemp, WM_SETFONT, WPARAM(hFont), TRUE);
		hViewTongTien = CreateWindow(L"Static", L"", WS_CHILD | WS_VISIBLE, grX + 442, grY + 470, 100, 30, g_hwnd, NULL, hInst, NULL);
		SendMessage(hViewTongTien, WM_SETFONT, WPARAM(hFont), TRUE);
		// Khởi tạo ban đầu cho chương trình
		AddItemToListViewFromFile(L"info.txt");
		InvalidateRect(hWnd, NULL, FALSE);

		break;
	case WM_CTLCOLORSTATIC:
		SetBkMode((HDC)wParam, TRANSPARENT);
		return (LRESULT)GetStockObject(NULL_BRUSH);
	case WM_NOTIFY:
		NMHDR* notifyMess;
		notifyMess = (NMHDR*)lParam;
		switch (notifyMess->code)
		{
		case NM_CLICK:
			if (notifyMess->hwndFrom == hListview) {
				curSelItem = ListView_GetNextItem(hListview, -1, LVNI_FOCUSED);
			}
			break;
		case LVN_KEYDOWN:
			if (LPNMLVKEYDOWN(lParam)->wVKey == VK_DELETE && (curSelItem >= 0)) {
				
				ListView_DeleteItem(hListview, curSelItem);

				TongTien -= MyItem[curSelItem]->TienChi;
				seqNumItem--;
				MyItem.erase(MyItem.begin() + curSelItem);

				InvalidateRect(hWnd, NULL, TRUE);
			}
			break;
		}
		break;
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
			int wmIdH = HIWORD(wParam);
			switch (wmIdH)
			{
			case CBN_SELCHANGE: // có sự thay đổi ở combobox
				selItem = SendMessage(hCmbboxLoai, CB_GETCURSEL, 0, 0);
				SetFocus(hWnd);
				break;
			}
            // Parse the menu selections:
            switch (wmId)
            {
			case IDC_BTN_THEM:
				AddItemToListView(hWnd);
				InvalidateRect(hWnd, NULL, TRUE);
				break;
			case ID_EDIT_CLEARALL:
				for (int i = 0; i < MyItem.size(); i++)
				{
					delete MyItem[i];
				}
				MyItem.clear();
				TongTien = 0;
				seqNumItem = 0;
				ListView_DeleteAllItems(hListview);
				InvalidateRect(hWnd, NULL, TRUE);
				break;
			case ID_EDIT_UNDO:
				if (MyItem.size() > 0)
				{
					ListView_DeleteItem(hListview, seqNumItem - 1);
					TongTien -= MyItem[seqNumItem - 1]->TienChi;
					MyItem.erase(MyItem.begin() + seqNumItem - 1);
					seqNumItem--;
					InvalidateRect(hWnd, NULL, TRUE);
				}
				break;
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
				WriteListToFile(L"info.txt");
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
            // TODO: Add any drawing code that uses hdc here...
			
				drawPieChart(hdc);
			
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
		WriteListToFile(L"info.txt");
        PostQuitMessage(0);
        break;
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
