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


#define WND_HEIGHT 500
#define WND_WIDTH 600

#define BUT_1 1000
#define BUT_2 1001
#define BUT_3 1002





#define MENU_EXIT 31 
#define UP_MENU_SCAN 32
#define UP_MENU_SAVE 33


#define MENU_ABOUT 30

HMENU hMenu;
HANDLE hSerial;
DCB dcbSerialParams = { 0 };



//
DWORD dwSize;
DWORD dwBytesWritten;
BOOL iRet;
DWORD Data[255];
char dataChar[255];
HWND hEdit;

DWORD from;
DWORD length;
DWORD to;
DWORD data;
DWORD chkSumm;
//

LPCTSTR sPortName = L"COM7";


//Не нужно---------------------------
OPENFILENAME ofn;       // структура станд. блока диалога
char szFile[260];       // буфер для имени файла
//-----------------------------------


void SetControls(HWND);
void MainMenu(HWND hWnd);



void ConnectRequest()
{

}

//void SerialRead()
//{
//	unsigned long BytesIterated = 0;
//
//	if (ReadFile(hSerial, Buffer, 5, &BytesIterated, NULL)) {
//		//SetWindowTextA(hEdit, Buffer);
//	}
//}

void StrToHex()
{
	
}

char* unsigned_to_hex_string(unsigned x, char* dest, size_t size) {
	snprintf(dest, size, "%X", x);
	return dest;
}

DWORD WINAPI ReadCOM(CONST LPVOID lpParam)
{
	while (true)
	{
		const int READ_TIME = 1;
		OVERLAPPED sync = { 0 };
		int reuslt = 0;
		unsigned long wait = 0, read = 0, state = 0;
		string str;


		/* Создаем объект синхронизации */
		sync.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

		/* Устанавливаем маску на события порта */
		if (SetCommMask(hSerial, EV_RXCHAR)) {
			/* Связываем порт и объект синхронизации*/
			WaitCommEvent(hSerial, &state, &sync);
			/* Начинаем ожидание данных*/
			wait = WaitForSingleObject(sync.hEvent, READ_TIME);
			/* Данные получены */
			if (wait == WAIT_OBJECT_0) {
				/* Начинаем чтение данных */
				char dst[255] = { 0 };



				ReadFile(hSerial, &Data[0], 1, &read, &sync);
				ReadFile(hSerial, &Data[1], 1, &read, &sync);
				for (int i = 0; i < Data[1]; i++)
				{
					ReadFile(hSerial, &Data[i+2], 1, &read, &sync);
				}
				int len = Data[1] + 2;

				char oneByte[1] = { 0 };
				for (int i = 0; i < len; i++)
				{					
					unsigned_to_hex_string(Data[i], oneByte, (sizeof(unsigned) * CHAR_BIT + 3) / 4 + 1);
					if (oneByte[1] == '\0')
					{
						dst[i * 2] = '0';
						dst[i * 2 + 1] = oneByte[0];
					}
					else
					{
						dst[i * 2] = oneByte[0];
						dst[i * 2 + 1] = oneByte[1];
					}

				}
				dst[len * 2 ] = '\r';
				dst[len * 2 + 2] = '\n';


				//sprintf(dst, "%d", Data);
				//std::string sourceStrByte = "e8";
				//unsigned char result = (unsigned char)strtol(Buffer, NULL, 16);


				//StrToHex();

				//str = string(dst, sizeof(dst));
				strcat(dataChar, dst);
				//strcat(dataChar, " ");
				SetWindowTextA(hEdit, dataChar);

				if (strlen(dataChar) > 400)
				{
					*dataChar = '\0';
				}
				



				//MessageBoxA(NULL, &dst, NULL, NULL);
				/* Ждем завершения операции чтения */
				wait = WaitForSingleObject(sync.hEvent, READ_TIME);
				/* Если все успешно завершено, узнаем какой объем данных прочитан */
				if (wait == WAIT_OBJECT_0)
					if (GetOverlappedResult(hSerial, &sync, &read, FALSE))
						reuslt = read;
			}
		}
		CloseHandle(sync.hEvent);
	}
	return 0;
}

int StringToWString(std::wstring& ws, const std::string& s)
{
	std::wstring wsTmp(s.begin(), s.end());

	ws = wsTmp;

	return 0;
}

LRESULT CALLBACK WindowProcedure(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
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
			}
			break;
		case MENU_ABOUT:
			MessageBoxW(NULL, L"   The program provides features for communicate with i-bus protocol in BMW e38, e39, e53.\n\
The program was developed by a third-year student of POIT BSUIR.\t\t\t\t\t\t Kondratskiy Anton.", L"Info", MB_OK);
			break;
		case BUT_1:
		{
			hSerial = CreateFile(sPortName, GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, 0);
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

			//dcbSerialParams = { 0 };
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

			COMMTIMEOUTS serialTimeouts;
			serialTimeouts.ReadIntervalTimeout = 1;
			serialTimeouts.ReadTotalTimeoutConstant = 1;
			serialTimeouts.ReadTotalTimeoutMultiplier = 1;
			serialTimeouts.WriteTotalTimeoutConstant = 1;
			serialTimeouts.WriteTotalTimeoutMultiplier = 1;

			if (!SetCommTimeouts(hSerial, &serialTimeouts))
			{
				cout << "error Set timeouts\n";
			}

			//SerialRead();

			char data[] = "I-Bus";
			dwSize = sizeof(data);
			dwBytesWritten;

			//iRet = WriteFile(hSerial, data, dwSize, &dwBytesWritten, NULL);

			//ReadCOM();

			/*string str = ReadCOM();
			wstring wstr;
			StringToWString(wstr, str);

			hDc = GetDC(hWnd);

			TextOutW(hDc, 50, 20, wstr.c_str(), 25);
			TextOut(hDc, 300, 20, L"BLUR", 4);*/

			break; }

		case BUT_2:
		{
			//DCB dcbSerialParams = { 0 };
			/*dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
			if (!GetCommState(hSerial, &dcbSerialParams))
			{
				cout << "getting state error\n";
			}
			dcbSerialParams.BaudRate = CBR_9600;
			dcbSerialParams.ByteSize = 8;
			dcbSerialParams.StopBits = ONESTOPBIT;
			dcbSerialParams.Parity = EVENPARITY;*/

			/*if (!SetCommState(hSerial, &dcbSerialParams))
			{
				cout << "error setting serial port state\n";
			}*/

			char data2[] = "I-Bus";
			dwSize = sizeof(data2);
			dwBytesWritten;

			iRet = WriteFile(hSerial, data2, dwSize, &dwBytesWritten, NULL);

			//ReadCOM();

			break; 
		}
		case BUT_3:
		{
			//DCB dcbSerialParams = { 0 };
			/*dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
			if (!GetCommState(hSerial, &dcbSerialParams))
			{
				cout << "getting state error\n";
			}
			dcbSerialParams.BaudRate = CBR_9600;
			dcbSerialParams.ByteSize = 8;
			dcbSerialParams.StopBits = ONESTOPBIT;
			dcbSerialParams.Parity = EVENPARITY;*/

			/*if (!SetCommState(hSerial, &dcbSerialParams))
			{
				cout << "error setting serial port state\n";
			}*/

			HANDLE thread = CreateThread(NULL, 0, ReadCOM, NULL, 0, NULL);

			//ReadCOM();

			//char data2[] = "I-Bus";
			//dwSize = sizeof(data2);
			//dwBytesWritten;

			//iRet = WriteFile(hSerial, data2, dwSize, &dwBytesWritten, NULL);

			//ReadCOM();

			break;
		}

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


	CreateWindowW(L"Button", L"button1", WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON, 300, 100, 200,
		50, hWnd, (HMENU)BUT_1, NULL, NULL);

	CreateWindowW(L"button", L"button2", WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON, 300, 200, 200,
		50, hWnd, (HMENU)BUT_2, NULL, NULL);

	CreateWindowW(L"button", L"button3", WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON, 300, 300, 200,
		50, hWnd, (HMENU)BUT_3, NULL, NULL);

	hEdit = CreateWindowW(L"Static", L"Edit control", WS_VISIBLE | WS_CHILD | SS_LEFT, 5, 5, 320, 520, hWnd, NULL, NULL, NULL);




}


