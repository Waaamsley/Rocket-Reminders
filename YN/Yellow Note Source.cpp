#ifndef UNICODE
#define UNICODE
#endif
//including the libraries we require
#include <Windows.h>
#include <stdlib.h> 
#include <stdio.h>
#include <string.h>  
#include <tchar.h>  
#include <fstream>
#include <string>
#include <string>
#include <Uxtheme.h>
#include "resource.h"
#include <iostream>
#include <windows.h>
#include <sqlext.h>
#include <sqltypes.h>
#include <sql.h>
#include <stdlib.h>  
#include <crtdbg.h>  

using namespace std;
//sets the global variables
LRESULT CALLBACK WindowProc(HWND proc, UINT uMsg, WPARAM wParam, LPARAM lparam);
HINSTANCE hInst;
int wmId, wmEvent, counter = 0, counts = 0, len;
const int reminderSize = 10, icon_size = 25;
HWND checkBox[reminderSize], addReminder, del, edit, delcheck, delbox[reminderSize], hwnd, selected;
wchar_t text[256];
wchar_t str[256];
HFONT hFont = CreateFont(17, 0, 0, 0, FW_REGULAR, FALSE, FALSE, FALSE, ANSI_CHARSET,
	OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
	DEFAULT_PITCH | FF_MODERN, TEXT("Tahoma"));
RECT rect;
HBRUSH hBrush;
bool manualQuit = false, checker = false;
HICON hIconAdd = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_hello));
HICON hIconDelete = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_MYICONDELETE));
HICON hIcon1, hIcon2;
char buf[] = "hello";
HANDLE hFile;
string adds;


/**
* save function that opens or creates a file "Save.dat" then adds current reminders to this
* saves on exit or on reminder addition
*/
void save() {
	ofstream outFile;
	outFile.open("Save.dat");
	for (int i = 0; i < counter; i++) {
		GetWindowText(checkBox[i], str, 256);
		wstring ws(str);
		ws += L"\n";
		// your new String
		string gg(ws.begin(), ws.end());
		outFile << gg;
	}

}


/**
* Add function which adds reminders based on the current count position up to 10 reminders per time.
* Also calculates if there is a blank reminder added anywhere to stop addition
*/
void add(HWND hWnd) {
	manualQuit = false;
manual: while (manualQuit == false) {
	if (counter < 10) {
		int count = counter - 1;
		if (counter > 0) {
			size_t low = 256;
			len = (int) SendMessage(checkBox[count], WM_GETTEXTLENGTH, 0, 0);
			GetWindowText(checkBox[counter - 1], text,len);
			if (wcslen(text) == 0) {
				low = 0;
			}
			else {
				for (int i = 0; i < (counter - 1); i++) {
					len = (int) SendMessage(checkBox[count], WM_GETTEXTLENGTH, 0, 0);
					GetWindowText(checkBox[i],text,len);
					if (wcslen(text) == 0) {
						low = wcslen(text);
						i = counter;
					}
				}
			}
			if (low == 0) {
				EnableWindow(hwnd, FALSE);
				int addbox = MessageBox(NULL, L"Do you want to add another blank task?",
					L"Confirmation", MB_YESNO | MB_ICONQUESTION);
				EnableWindow(hwnd, TRUE);
				SetForegroundWindow(hwnd);
				if (addbox == IDNO) {
					manualQuit = true;
					SetFocus(checkBox[counter - 1]);
					goto manual;
				}
			}
		}
		GetWindowText(checkBox[counter], text, 256);
		checkBox[counter] = CreateWindow(TEXT("edit"), TEXT(""), WS_CHILD |
			WS_VISIBLE, 25, 60 + (25 * counter), 230, 17, hWnd, (HMENU)(UINT_PTR)counter, NULL, NULL);
		delbox[counter] = CreateWindow(TEXT("BUTTON"), TEXT(""), WS_CHILD | BS_AUTOCHECKBOX,
			10, 60 + (25 * counter), 15, 17, hWnd, (HMENU)(UINT_PTR)(600 + counter), NULL, NULL);
		SendMessage(checkBox[counter], WM_SETFONT, (WPARAM)hFont, 0);
		SetFocus(checkBox[counter]);
		counter++;
		manualQuit = true;
		checker = true;
	}
	//handles the error if they add more reminders than we currently can handle
	else {
		int msgboxID = MessageBox(
			NULL,
			L"This version currently doesnt support more than 10 reminders",
			L"Rocket Reminders",
			MB_ICONEXCLAMATION | MB_OK);
		manualQuit = true;
	}
}

}
/**
* Opens the file "Save.dat" for adding reminders on program start up
*/
void open() {
	ifstream inFile;
	inFile.open("Save.dat");
	while (getline(inFile, adds)) {
		add(hwnd);
		SetWindowTextA(checkBox[counter - 1], adds.c_str());
	}
}
/**
* Destroys all created windows while app running, clearing the memory of them on exit
*/
void destroy() {
	for (int i = 0; i < counter; i++) {
		DestroyWindow(checkBox[i]);
		DestroyWindow(delbox[i]);
	}
	DestroyIcon(hIconAdd);
	DestroyIcon(hIconDelete);
	DestroyIcon(hIcon1);
	DestroyIcon(hIcon2);
	DeleteObject(hFont);
	DeleteObject(hBrush);
}

/**
* main program which create the parent window and sets icons
*/
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR pCmdLine, int nCmdShow) {
	//registering the window class.
	const wchar_t CLASS_NAME[] = L"Yellow Note";
	hBrush = CreateSolidBrush(RGB(255, 244, 168)), (HBRUSH(COLOR_WINDOWFRAME + 1));
	hInst = hInstance;
	HICON hIcon1 = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MYICONLARGE));
	HICON hIcon2 = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MYICONSMALL));
	

	WNDCLASSEX wcex;
	ZeroMemory(&wcex, sizeof(wcex));
	wcex.cbSize = sizeof(wcex);
	wcex.style = CS_HREDRAW | CS_VREDRAW | CS_PARENTDC; //sets window style
	wcex.hbrBackground = hBrush; //paints background yellow
	wcex.lpfnWndProc = WindowProc; //the method to handle window procedures.
	wcex.hInstance = hInstance;	 //instance number.
	wcex.lpszClassName = CLASS_NAME;
	wcex.hIcon = hIcon1;  //sets the icons
	wcex.hIconSm = hIcon2; //sets smaller icons
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW); //adds default cursor
												//error message if window creation fails

	if (!RegisterClassEx(&wcex))
	{
		MessageBox(NULL,
			L"Call to RegisterClassEx failed!",
			L"Yellow Note",
			NULL);
		_CrtDumpMemoryLeaks();
		return 1;
	}
	//creates the window
	 hwnd = CreateWindowEx(
		0,							// Optional window styles
		CLASS_NAME,					// Window Class
		L"Yellow Note",		// Window Text
		WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU,		// Window Style
								//next: size and position
		 GetSystemMetrics(SM_CXSCREEN)-325, 25, 325, 350,
		NULL,	   //parent Window
		NULL,	   // Menu
		hInstance, // Instance Handle
		NULL	   // Additional application data
	);

	if (!hwnd) {
		MessageBox(NULL, NULL, L"Failed to Create Window"
			L"Yellow Note", NULL);
		_CrtDumpMemoryLeaks();
		return 1;
	}
	ShowWindow(hwnd, nCmdShow);
	UpdateWindow(hwnd);
	open();
	
	//message loop:
	MSG msg = {};
	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return 0;

}
/**
* checks if the enable window checkbox is checked, if true disables all created windows
* otherwise enables them.
*/
void checkstate(HWND hwnd, LPARAM lParam) {
	LRESULT chkState = SendMessage(hwnd, BM_GETCHECK, 0, 0);
	if (chkState == BST_CHECKED) {
		for (int i = 0; i < counter; i++) {
			EnableWindow(checkBox[i], TRUE);
		}
	}
	else if (chkState == BST_UNCHECKED) {
		for (int i = 0; i < counter; i++) {
			EnableWindow(checkBox[i], FALSE);
		}
	}
}

//runs the methods based on current state
LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	PAINTSTRUCT ps;
	HDC hdc;
	hdc = BeginPaint(hWnd, &ps);

	switch (uMsg) {
		//destroys the window
	case WM_DESTROY:
		save();
		destroy();
		PostQuitMessage(0);
		return 0;
		//paints the windows
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
		break;

		//creates the required windows and sets their font.
	case WM_CREATE:
		addReminder = CreateWindow(TEXT("button"), TEXT("add"), WS_VISIBLE | WS_CHILD | BS_ICON,
			10, 15, 35, 35, hWnd, (HMENU)501, NULL, NULL);
		del = CreateWindow(TEXT("button"), TEXT("Delete"),  WS_VISIBLE | WS_CHILD |BS_ICON,
			50, 15, 35, 35,hWnd, (HMENU)502, NULL, NULL);
		edit = CreateWindowEx(0, TEXT("Button"), L"Enable Editing?", WS_CHILD | WS_VISIBLE | 
			WS_CLIPSIBLINGS | BS_AUTOCHECKBOX | BS_TEXT | WS_GROUP |
			WS_TABSTOP,100,23,140,25, hWnd, (HMENU)503, hInst, 0);
		delcheck = CreateWindow(TEXT("button"), TEXT("Delete Selected?"), WS_CHILD | BS_ICON,
			50, 15, 35, 35, hWnd, (HMENU)504, NULL, NULL);
		selected = CreateWindowEx(0, TEXT("Button"), L"Select All?", WS_CHILD |
			WS_CLIPSIBLINGS | BS_AUTOCHECKBOX | BS_TEXT | WS_GROUP |
			WS_TABSTOP, 100, 23, 140, 25, hWnd, (HMENU)505, hInst, 0);

		SendMessage(delcheck, BM_SETIMAGE, (WPARAM)IMAGE_ICON, (LPARAM)hIconDelete);
		SendMessage(addReminder, BM_SETIMAGE, (WPARAM)IMAGE_ICON, (LPARAM)hIconAdd);
		SendMessage(del, BM_SETIMAGE, (WPARAM)IMAGE_ICON, (LPARAM)hIconDelete);
		SendMessage(edit, WM_SETFONT, (WPARAM)hFont, 0);
		SendMessage(selected, WM_SETFONT, (WPARAM)hFont, 0);
		SendMessage(edit, BM_SETCHECK, TRUE, NULL);
		break;

		//takes the commands for currennt wParam
	case WM_COMMAND:
		wmId = LOWORD(wParam);
		wmEvent = HIWORD(wParam);

		if (wmId == 501) {
			if (wmEvent == BN_CLICKED) {
				add(hwnd);
			}
		}

		//handles the addition of the reminders
		if (wmId == 502) {
			if (wmEvent == BN_CLICKED) {
				SendMessage(selected, BM_SETCHECK, BST_UNCHECKED, 0);
				ShowWindow(delcheck, SW_SHOW);
				for (int i = 0; i < counter; i++) {
					ShowWindow(delbox[i], SW_SHOW);
					EnableWindow(checkBox[i], FALSE);
				}
				EnableWindow(addReminder, FALSE);
				ShowWindow(edit, SW_HIDE);
				ShowWindow(selected, SW_SHOW);
				EnableWindow(del, FALSE);
			}
		}


		//handles enable editing activation/ deactivation
		if (wmId == 503) {
			if (wmEvent == BN_CLICKED) {
				checkstate(edit, lParam);
			}
		}
		// handles the deletion and reorders the textbox
		if (wmId == 504) {
			BOOL yes = false;
			for (int z = 0; z < counter; z++) {
				BOOL isChecked = IsDlgButtonChecked(hwnd, z + 600);
				if (isChecked) {
					DestroyWindow(checkBox[z]);
					DestroyWindow(delbox[z]);
					checkBox[z] = (HWND)-1;
					yes = true;
				}
			}
			int count = 0;

			for (int z = 0; z < counter; z++) {
				if (checkBox[z] != (HWND)-1 && yes && count != z) {
					GetWindowText(checkBox[z], text, 256);
					checkBox[count] = CreateWindow(TEXT("edit"), text, WS_CHILD |
						WS_VISIBLE, 25, 60 + (25 * count), 230, 17, hWnd, (HMENU)(UINT_PTR)count, NULL, NULL);
					SendMessage(checkBox[count], WM_SETFONT, (WPARAM)hFont, 0);
					delbox[count] = CreateWindow(TEXT("BUTTON"), TEXT(""), WS_CHILD | BS_AUTOCHECKBOX,
						10, 60 + (25 * count), 15, 17, hWnd, (HMENU)(UINT_PTR)(600 + count), NULL, NULL);
					count++;
					DestroyWindow(checkBox[z]);
					DestroyWindow(delbox[z]);
				}
				else if (checkBox[z] != (HWND)-1 && yes && count == z) {
					count++;
				}
			}
			if (yes) {
				counter = count;
			}

			for (int i = 0; i < counter; i++) {
				EnableWindow(checkBox[i], TRUE);
				ShowWindow(delbox[i], SW_HIDE);
			}
			ShowWindow(delcheck, SW_HIDE);
			EnableWindow(addReminder, TRUE);
			ShowWindow(edit, SW_SHOW);
			EnableWindow(del, TRUE);
			checkstate(edit, lParam);
			save();
		}

		//disables everything during the delete phase, adds select all checkbox and check box next to text boxes
			if (wmId == 505) {
				BOOL isChecked = IsDlgButtonChecked(hwnd, 505);
				if (isChecked) {
					for (int i = 0; i < counter; i++) {
						SendMessage(delbox[i], BM_SETCHECK, BST_CHECKED, 0);


					}
				}
				else if(isChecked == false){
					for (int i = 0; i < counter; i++) {
						SendMessage(delbox[i], BM_SETCHECK, BST_UNCHECKED, 0);
						ShowWindow(delbox[i], SW_SHOW);
					}
				}
			}
			//colours the checkboxes transparent
			case WM_CTLCOLORBTN:
			case WM_CTLCOLORSTATIC:
			SetBkColor((HDC)wParam, RGB(0, 0, 0));
			SetBkMode((HDC)wParam, TRANSPARENT);
			return (LRESULT)GetStockObject(HOLLOW_BRUSH);
	//sets textbox colour
	case WM_CTLCOLOREDIT:
	
			hdc = (HDC)wParam;
			SetBkColor(hdc, RGB(255, 255, 204));
			return (LRESULT)GetStockObject(DC_BRUSH);
		
		break;

	}
	//controls disabling checkbox if new reminder is added
	if (checker && counts == 1) {
		checkstate(edit, lParam);
		checker = false;
		counts = 0;
	}
	if (checker && counts < 1) {
		counts++;
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}  

