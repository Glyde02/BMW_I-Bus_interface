#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include <stdlib.h>
#include <string.h>
#include <tchar.h>

#include <stdio.h>
#include <string.h>
#include <commctrl.h>
#include <tchar.h>

#include <iostream>
#include <strsafe.h>



using namespace std;


#define WND_HEIGHT 1700
#define WND_WIDTH 1025

#define BUT_1 1000
#define BUT_2 1001
#define BUT_3 1002





#define MENU_EXIT 31 
#define UP_MENU_SCAN 32
#define UP_MENU_SAVE 33


#define MENU_ABOUT 30

HMENU hMenu;
HANDLE hSerial;

LPCTSTR sPortName = L"COM1";


//Не нужно---------------------------
OPENFILENAME ofn;       // структура станд. блока диалога
char szFile[260];       // буфер для имени файла
HANDLE hf;              // дескриптор файла
char FileExtension[][6] = { ".jpg", ".bmp", ".jpg", ".jp2", ".png", ".tif" };
//-----------------------------------


void SetControls(HWND);
void MainMenu(HWND hWnd);



//original - MainImage
//current - CurrImage
//new - BuffImage


void ReadCOM()
{
	DWORD iSize;
	//char sReceivedChar;
	
	TCHAR* receivedMessage = { 0 };

	while (true)
	{
		ReadFile(hSerial, receivedMessage, 1, &iSize, 0);  // получаем 1 байт
		if (iSize > 0)   // если что-то принято, выводим
			//cout << sReceivedChar;
			MessageBox(NULL, receivedMessage, NULL, NULL);
	}
}

LRESULT CALLBACK WindowProcedure(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	UINT code;
	LPNMUPDOWN lpnmud;
	int BufValue;
	int bufGamma;
	HDC hDc;
	HFONT hfont;
	PAINTSTRUCT ps;

	switch (uMsg) {
		
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		
		case MENU_EXIT:
			MessageBeep(MB_OK);
			if (MessageBoxW(hWnd, L"Are you sure?", L"Exit", MB_YESNO) == IDYES)
			{
				DestroyWindow(hWnd);
				DeleteFile(L"buf.bmp");
			}
			break;
		case MENU_ABOUT:
			MessageBoxW(NULL, L"   The program provides features for image processing.\n\
The program was developed by a second-year student of POIT.\t\t\t\t\t\t Kondratskiy Anton Andreevich.", L"Info", MB_OK);
			break;
		case BUT_1:
			hSerial = CreateFile(sPortName, GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
			if (hSerial == INVALID_HANDLE_VALUE)
			{
				if (GetLastError() == ERROR_FILE_NOT_FOUND)
				{
					MessageBox(NULL, L"no", NULL, NULL);
					//cout << "serial port does not exist.\n";
				}
				MessageBox(NULL, L"no2", NULL, NULL);

				//cout << "some other error occurred.\n";
			}

			DCB dcbSerialParams = { 0 };
			dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
			if (!GetCommState(hSerial, &dcbSerialParams))
			{
				cout << "getting state error\n";
			}
			dcbSerialParams.BaudRate = CBR_9600;
			dcbSerialParams.ByteSize = 8;
			dcbSerialParams.StopBits = ONESTOPBIT;
			dcbSerialParams.Parity = EVENPARITY;

			if (!SetCommState(hSerial, &dcbSerialParams))
			{
				cout << "error setting serial port state\n";
			}

			char data[] = "I-Bus"; 
			DWORD dwSize = sizeof(data);   
			DWORD dwBytesWritten;   

			BOOL iRet = WriteFile(hSerial, data, dwSize, &dwBytesWritten, NULL);

			ReadCOM();

			break;
		}




		break;
	case WM_PAINT:




		BeginPaint(hWnd, &ps);
		hDc = GetDC(hWnd);
		hfont = CreateFont(20, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, RUSSIAN_CHARSET, OUT_OUTLINE_PRECIS,
			CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, FIXED_PITCH, TEXT("Times New Roman"));
		SelectObject(hDc, hfont);
		SetBkMode(hDc, TRANSPARENT);
		
		DeleteObject(hfont);
		EndPaint(hWnd, &ps);
		break;

	case WM_CREATE:
		MainMenu(hWnd);
		SetControls(hWnd);
		break;
	case WM_CLOSE:
		DestroyWindow(hWnd);

		MessageBeep(MB_OK);
		/*if (MessageBoxW(hWnd, L"Are you sure?", L"Exit", MB_YESNO) == IDYES)
		{
			DestroyWindow(hWnd);
		}
		*/break;
	case WM_DESTROY:
		PostQuitMessage(0);
		return DefWindowProcW(hWnd, uMsg, wParam, lParam);
		break;
	default:
		return DefWindowProcW(hWnd, uMsg, wParam, lParam);
	}
	return 0;
}



INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, INT nCmdShow)
{
	WNDCLASSEX wcex;
	HWND hWnd;
	MSG msg;
	memset(&wcex, 0, sizeof(WNDCLASSEX));
	
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = 0;
	wcex.lpfnWndProc = WindowProcedure;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = 0;
	wcex.hCursor = LoadCursor(0, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = L"MyWindowClass";
	wcex.hIconSm = 0;

	RegisterClassEx(&wcex);
	hWnd = CreateWindowExW(0, L"MyWindowClass", L"I-Bus Checker",
		WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_VISIBLE,
		0, 0, WND_HEIGHT, WND_WIDTH, 0, 0, hInstance, NULL);

	// Инициализация структуры OPENFILENAME
	ZeroMemory(&ofn, sizeof(OPENFILENAME));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = hWnd;
	ofn.lpstrFile = (LPWSTR)szFile;
	ofn.nMaxFile = sizeof(szFile);
	//ofn.lpstrFilter = (LPWSTR)L"All images type\0*.bmp;*.dib;*.jpg;*.jpeg;*.jpe;*.jp2;*.png;*.tif;*.tiff\0\0";
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = NULL;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;




	while (GetMessage(&msg, 0, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return 0;
}

//Вывод диалогового окна для сохранения файла
bool ShowSaveDialog(HWND hWndOwner, char* fileName, char* title, char* initialDir, char* filter)
{
	OPENFILENAME ofn;
	ZeroMemory(&ofn, sizeof(OPENFILENAME));

	char szFile[MAX_PATH];
	ZeroMemory(szFile, MAX_PATH);

	// Инициализируем OPENFILENAME
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = hWndOwner;
	ofn.lpstrFile = (LPWSTR)szFile;
	ofn.nMaxFile = MAX_PATH;
	ofn.lpstrFilter = (LPCWSTR)filter;
	ofn.nFilterIndex = 0;
	ofn.lpstrTitle = (LPCWSTR)title;
	ofn.lpstrInitialDir = (LPCWSTR)initialDir;
	ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT;


	if (GetSaveFileName(&ofn))
	{
		//strcpy(fileName, (LPWSTR)ofn.lpstrFile);
		lstrcpyW((LPWSTR)fileName, ofn.lpstrFile);
		return 1;
	}
	return 0;
}


void MainMenu(HWND hWnd)
{
	hMenu = CreateMenu();
	HMENU hFileMenu = CreateMenu();
	HMENU hAboutMenu = CreateMenu();
	//Menu File:	
	AppendMenuW(hMenu, MF_POPUP, (UINT_PTR)hFileMenu, L"Menu");
	AppendMenuW(hFileMenu, MF_STRING, UP_MENU_SCAN, L"Open...");
	AppendMenuW(hFileMenu, MF_STRING, UP_MENU_SAVE, L"Save...");
	AppendMenuW(hFileMenu, MF_SEPARATOR, (UINT_PTR)NULL, NULL);
	AppendMenuW(hFileMenu, MF_STRING, MENU_EXIT, L"Exit");

	// Menu About:
	AppendMenuW(hMenu, MF_POPUP, (UINT_PTR)hAboutMenu, L"About");
	AppendMenuW(hAboutMenu, MF_STRING, MENU_ABOUT, L"Info");


	

	SetMenu(hWnd, hMenu);
}

// Create controls
void SetControls(HWND hWnd)
{


	CreateWindowW(L"Button", L"button1", WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON, 100, WND_WIDTH - 135, 200,
		50, hWnd, (HMENU)BUT_1, NULL, NULL);

	CreateWindowW(L"button", L"button2", WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON, 400, WND_WIDTH - 135, 200,
		50, hWnd, (HMENU)BUT_2, NULL, NULL);

	CreateWindowW(L"button", L"button3", WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON, 650, WND_WIDTH - 135, 200,
		50, hWnd, (HMENU)BUT_3, NULL, NULL);


}


