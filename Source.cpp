#ifndef UNICODE
#define UNICODE
#endif
//including the libraries we require
#include <Windows.h>
#include <stdlib.h> 
#include <stdio.h>
#include <string.h>  
#include <tchar.h>  
#include <sstream>
#include <string>
#include <Uxtheme.h>
#include "resource.h"

//sets the global variables
using namespace std;
LRESULT CALLBACK WindowProc(HWND proc, UINT uMsg, WPARAM wParam, LPARAM lparam);
HINSTANCE hInst;
int wmId, wmEvent,counter = 0;
const int reminderSize = 10;
HWND checkBox[reminderSize], textBox, addReminder, del, prioritise;
int checkBoxDelete[reminderSize] = {11,11,11,11,11,11,11,11,11,11};
wchar_t text[256];
wchar_t blank[256];
boolean priority[10];
boolean prior = true; 

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR pCmdLine, int nCmdShow) {
	//registering the window class.
	const wchar_t CLASS_NAME[] = L"Rocket Reminders";
	hInst = hInstance;
	//resource files to be used.
	HICON hIcon1 = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MYICONLARGE));
	HICON hIcon2 = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MYICONSMALL));

	WNDCLASSEX wcex;
	ZeroMemory(&wcex, sizeof(wcex));
	//sets all the default values of the parent window
	wcex.cbSize = sizeof(wcex);
	wcex.style = CS_HREDRAW | CS_VREDRAW | CS_PARENTDC; //sets window style
	wcex.hbrBackground = CreateSolidBrush(RGB(255, 255, 204)), (HBRUSH(COLOR_WINDOWFRAME + 1)); //paints background yellow
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
			L"Win32 Guided Tour",
			NULL);
		return 1;
	}

	//creates the window
	HWND hwnd = CreateWindowEx(
		0,							// Optional window styles
		CLASS_NAME,					// Window Class
		L"Rocket Reminders",		// Window Text
		WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX,		// Window Style
									//next: size and position
		0, 0, 325, 350,
		NULL,	   //parent Window
		NULL,	   // Menu
		hInstance, // Instance Handle
		NULL	   // Additional application data
	);

	if (hwnd == NULL) {
		MessageBox(NULL,
			_T("Call to CreateWindow failed!"),
			_T("Win32 Guided Tour"),
			NULL);

		return 1;
	}

	ShowWindow(hwnd, nCmdShow);
	UpdateWindow(hwnd);

	//message loop:
	MSG msg = {};
	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return 0;
}
//runs the methods based on current state
LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	PAINTSTRUCT ps;
	HDC hdc;
	//font to be used.
	HFONT hFont = CreateFont(17, 0, 0, 0, FW_REGULAR, FALSE, FALSE, FALSE, ANSI_CHARSET,
		OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
		DEFAULT_PITCH | FF_MODERN, TEXT("Tahoma"));
	RECT rect;
	switch (uMsg) {

		//destroys the window
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

		//paints the windows
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
		break;

		//moves the boxes when the parent window resizes.
	case WM_MOVE:
		if (SIZE_MAXIMIZED || SIZE_RESTORED) {
			GetClientRect(hWnd, &rect);
		}
		MoveWindow(textBox, 10, rect.bottom - 80, 180, 25, TRUE);
		MoveWindow(addReminder, 200, rect.bottom - 80, 100, 25, TRUE);
		MoveWindow(del, 10, rect.bottom - 45, 140, 25, TRUE);
		//MoveWindow(prioritise, 160, rect.bottom - 45, 140, 25, TRUE);
		break;

		//creates all the required windows
	case WM_CREATE:
		textBox = CreateWindow(TEXT("edit"), TEXT(""), WS_CHILD |
			WS_VISIBLE | WS_BORDER, 10, 225, 180, 25, hWnd, (HMENU)500, NULL, NULL);
		SendMessage(textBox, WM_SETFONT, (WPARAM)hFont, 0);//creates window for adding text

		addReminder = CreateWindow(TEXT("button"), TEXT("Add Reminder"),
			WS_VISIBLE | WS_CHILD,
			200, 225, 100, 25,
			hWnd, (HMENU)501, NULL, NULL);
		del = CreateWindow(TEXT("button"), TEXT("Delete Selected?"), WS_VISIBLE | WS_CHILD, 
			10, 260, 140, 25, hWnd, (HMENU)502, NULL, NULL);

		/**prioritise = CreateWindow(TEXT("button"), TEXT("Prioritise selected?"), WS_VISIBLE | WS_CHILD, 
			160, 260, 140, 25, hWnd, (HMENU)503, NULL, NULL);*/

		SendMessage(addReminder, WM_SETFONT, (WPARAM)hFont, 0);
		SendMessage(del, WM_SETFONT, (WPARAM)hFont, 0);//creates the button to add the reminders
		//SendMessage(prioritise, WM_SETFONT, (WPARAM)hFont, 0);
		break;

	case WM_COMMAND:


		wmId = LOWORD(wParam);
		wmEvent = HIWORD(wParam);

		//keeps track of checked reminders for latter manipulation.
		for (int i = 0; i < 10; i++) {
			if (wmId == i) {
				if (wmEvent == BN_CLICKED) {
					LRESULT chkState = SendMessage((HWND)lParam, BM_GETCHECK, 0, 0);
					if (chkState == BST_CHECKED) {
						checkBoxDelete[i] = i;
						priority[i] = true;
					}
				}
			}
		}

		//handles the addition of the reminders
		if (wmId == 501) {
			if (wmEvent == BN_CLICKED) {
				if (counter < 10) {
					GetWindowText(textBox, text, 256);
					checkBox[counter] = CreateWindowEx(0, TEXT("Button"), text, WS_CHILD | WS_VISIBLE |
						WS_CLIPSIBLINGS | BS_AUTOCHECKBOX | BS_TEXT | WS_GROUP |
						WS_TABSTOP, 10, 20 * (counter + 1), 230, 17, hWnd, (HMENU)counter, hInst, 0);
					SendMessage(checkBox[counter], WM_SETFONT, (WPARAM)hFont, 0);
					SetWindowText(textBox, blank);
					counter++;
				}
				//handles the error if they add more reminders than we currently can handle
				else {
					int msgboxID = MessageBox(
						NULL,
						L"This version currently doesnt support more than 10 reminders",
						L"Rocket Reminders",
						MB_ICONEXCLAMATION | MB_OK);
				}
			}
		}
		// handles the deletetion of all selected reminders.
		if (wmId == 502) {
			for (int z = 0; z < counter; z++) {
				if (checkBoxDelete[z] == z) {
					counter--;
					DestroyWindow(checkBox[z]);
					for (int j = z; j < counter; j++) {
						GetWindowText(checkBox[j + 1], text, 256);
						if (checkBoxDelete[j+1] == 11) {
							checkBoxDelete[j] = 11;
						}
						else {
							checkBoxDelete[j] = j;
							checkBoxDelete[j + 1] = 11;
						}
						checkBox[j] = CreateWindowEx(0, TEXT("Button"), text, WS_CHILD | WS_VISIBLE |
							WS_CLIPSIBLINGS | BS_AUTOCHECKBOX | BS_TEXT | WS_GROUP |
							WS_TABSTOP, 10, 20 * (j + 1), 230, 17, hWnd, (HMENU)j, hInst, 0);
						SendMessage(checkBox[j], WM_SETFONT, (WPARAM)hFont, 0);
						DestroyWindow(checkBox[j + 1]);
					}
					z = z - 1;
				}
			}
			for (int j = 0; j < 10; j++) {
				checkBoxDelete[j] = 11;
			}
		}
		//not yet implemented.
		if (wmId = 503) {
			prior = true;
		}
		break;
		//makes the checkbox background transparent
	case WM_CTLCOLORSTATIC:
		SetBkColor((HDC)wParam, RGB(0, 0, 0));
		SetBkMode((HDC)wParam, TRANSPARENT);
		/**if (prior = true){
			for (int i = 0; i < counter; i++) {
				if (priority[i]) {
					GetWindowText(checkBox[i], text, 256);
					DestroyWindow(checkBox[i]);
					checkBox[i] = GetDlgItem(checkBox[i], i);
					SetWindowTheme(checkBox[i], NULL, NULL);
					checkBox[i] = CreateWindowEx(0, TEXT("Button"), text, WS_CHILD | WS_VISIBLE |
						WS_CLIPSIBLINGS | BS_AUTOCHECKBOX | BS_TEXT | WS_GROUP |
						WS_TABSTOP, 10, 20 * (i + 1), 230, 17, hWnd, (HMENU)hFont, hInst, 0);
					SetTextColor((HDC)checkBox[i], RGB(255, 0, 0));
				}
				priority[i] = false;
			}
			prior = false;
		}*/
		return(LRESULT)GetStockObject(HOLLOW_BRUSH);
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}