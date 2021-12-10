#define _CRT_SECURE_NO_WARNINGS
//#include <Codes.h>
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
#include "Codes.h"





using namespace std;

//Size of controls
#define WND_HEIGHT 1500
#define WND_WIDTH 850
#define WND_BETWEEN 30
#define WND_OUTER 290
#define WND_INNER 380
#define WND_FIELD_HEIGHT 700

#define COM_SETTINGS 18
#define COM_STOPRESUME 60

#define SCAN_COM 19
#define COM1  20
#define COM2  21
#define COM3  22
#define COM4  23
#define COM5  24
#define COM6  25
#define COM7  26
#define COM8  27
#define COM9  28
#define COM10 29
#define CONNECT_COM 30

#define SEND_EDIT 50
#define SEND_BUT 51
#define CODE_CLEAR 52





#define BUT_1 1000
#define BUT_2 1001
#define BUT_3 1002





#define MENU_EXIT 31 
#define UP_MENU_SCAN 32
#define UP_MENU_SAVE 33


#define MENU_ABOUT 40


HANDLE hSerial;
DCB dcbSerialParams = { 0 };
HANDLE threadReadingCom;


HANDLE hComSettings;
HANDLE hCode;
HANDLE hCodeInfo;
HANDLE hCodePresets;

HANDLE hComPorts;
HANDLE hComStopResume;
bool isStop = FALSE;
HANDLE hCodeClear;

HANDLE hCom1, hCom2, hCom3, hCom4, hCom5, hCom6, hCom7, hCom8, hCom9, hCom10;
BOOL ports[11] = { false };

HANDLE hSendEdit;
HANDLE hSendButton;

//
HMENU hMenu;
DWORD dwSize;
DWORD dwBytesWritten;
BOOL iRet;
DWORD Data[512];
char dataChar[2000];
int numOfLine = 0;
char bufwr[255];
char command[8];

int mailLength = 0;
char mail[255];
HWND hEdit;

DWORD from;
DWORD length;
DWORD to;
DWORD data;
DWORD chkSumm;
//

LPCTSTR sPortName = L"COM7";



void SetControls(HWND);



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

void PrintCode()
{
	

	char CodesInfo[255] = { 0 };
	strcat(CodesInfo, IBUSDevices[Data[0]]);
	strcat(CodesInfo, " to ");
	strcat(CodesInfo, IBUSDevices[Data[2]]);
	strcat(CodesInfo, "; ");
	strcat(CodesInfo, IBUSMessages[Data[3]]);
	strcat(CodesInfo, "\r\n");
	strcat(CodesInfo, "\r\n");
	strcat(CodesInfo, "---------------------------------------------------");
	strcat(CodesInfo, "\r\n");


	numOfLine += 7;

	strcat(dataChar, CodesInfo);

	//SetWindowTextA((HWND)hCode, dataChar);

	int index = GetWindowTextLength((HWND)hCode);
	SetFocus((HWND)hCode); // set focus
	SendMessageA((HWND)hCode, EM_SETSEL, (WPARAM)index, (LPARAM)index); // set selection - end of text
	SendMessageA((HWND)hCode, EM_REPLACESEL, 0, (LPARAM)dataChar); // append!

	*dataChar = '\0';

	//SendMessageA((HWND)hCode, EM_SETSEL, 0, -1); //Select all. 
	//SendMessageA((HWND)hCode, EM_SETSEL, -1, -1);//Unselect and stay at the end pos
	//SendMessageA((HWND)hCode, EM_SCROLLCARET, 0, 0); //Set scrollcaret to the current Pos
	//SendMessage((HWND)hCode, EM_LINESCROLL, 0, numOfLine);
	
}

void WriteCOM()
{
	DWORD temp, signal; //temp - переменная-заглушка
	OVERLAPPED sync = { 0 };
	bool fl = false;

	sync.hEvent = CreateEvent(NULL, true, true, NULL); //создать событие
	WriteFile(hSerial, mail, mailLength, &temp, &sync); //записать байты в порт (перекрываемая
	int j = GetLastError();
	signal = WaitForSingleObject(sync.hEvent, INFINITE); //приостановить поток, пока не завершится

		if ((signal == WAIT_OBJECT_0) && (GetOverlappedResult(hSerial, &sync, &temp, true))) fl = true; //если
		else fl = false;

	
	CloseHandle(sync.hEvent); //перед выходом из потока закрыть объект-событие
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
				dst[len * 2] = '\r';
				dst[len * 2 + 1] = '\n';

				strcat(dataChar, dst);


				//sprintf(dst, "%d", Data);
				//std::string sourceStrByte = "e8";
				//unsigned char result = (unsigned char)strtol(Buffer, NULL, 16);


				//StrToHex();

				//str = string(dst, sizeof(dst));
				
				//strcat(dataChar, " ");
				PrintCode();
				

				if (strlen(dataChar) > 30000)
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

void GetActivePorts()
{
	int r = 0;
	HKEY hkey = NULL;
	//Открываем раздел реестра, в котором хранится иинформация о COM портах
	r = RegOpenKeyEx(HKEY_LOCAL_MACHINE, TEXT("HARDWARE\\DEVICEMAP\\SERIALCOMM\\"), 0, KEY_READ, &hkey);
	if (r != ERROR_SUCCESS)
		return;

	unsigned long CountValues = 0, MaxValueNameLen = 0, MaxValueLen = 0;
	//Получаем информацию об открытом разделе реестра
	RegQueryInfoKey(hkey, NULL, NULL, NULL, NULL, NULL, NULL, &CountValues, &MaxValueNameLen, &MaxValueLen, NULL, NULL);
	++MaxValueNameLen;
	//Выделяем память
	TCHAR* bufferName = NULL, * bufferData = NULL;
	bufferName = (TCHAR*)malloc(MaxValueNameLen * sizeof(TCHAR));
	if (!bufferName)
	{
		RegCloseKey(hkey);
		return;
	}
	bufferData = (TCHAR*)malloc((MaxValueLen + 1) * sizeof(TCHAR));
	if (!bufferData)
	{
		free(bufferName);
		RegCloseKey(hkey);
		return;
	}

	unsigned long NameLen, type, DataLen;
	//Цикл перебора параметров раздела реестра
	for (unsigned int i = 0; i < CountValues; i++)
	{
		NameLen = MaxValueNameLen;
		DataLen = MaxValueLen;
		r = RegEnumValue(hkey, i, bufferName, &NameLen, NULL, &type, (LPBYTE)bufferData, &DataLen);
		if ((r != ERROR_SUCCESS) || (type != REG_SZ))
			continue;

		//int j = 0; 
		//char s = bufferData[3];
		ports[(int)(bufferData[3] - '0')] = TRUE;
		
	}
	//Освобождаем память
	free(bufferName);
	free(bufferData);
	//Закрываем раздел реестра
	RegCloseKey(hkey);
}

void Connecting()
{
	hSerial = CreateFileW(sPortName, GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, 0, 0);
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

	dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
	if (!GetCommState(hSerial, &dcbSerialParams))
	{
		cout << "getting state error\n";
	}
	dcbSerialParams.BaudRate = CBR_9600;
	dcbSerialParams.ByteSize = 8;
	dcbSerialParams.StopBits = ONESTOPBIT;
	dcbSerialParams.Parity = EVENPARITY;
	dcbSerialParams.fNull = FALSE;

	if (!SetCommState(hSerial, &dcbSerialParams))
	{
		cout << "error setting serial port state\n";
	}

	COMMTIMEOUTS serialTimeouts;

	serialTimeouts.ReadIntervalTimeout = 0xFFFFFFFF;
	serialTimeouts.ReadTotalTimeoutConstant = 0;
	serialTimeouts.ReadTotalTimeoutMultiplier = 0;

	/*serialTimeouts.ReadIntervalTimeout = 5000;
	serialTimeouts.ReadTotalTimeoutConstant = 5000;
	serialTimeouts.ReadTotalTimeoutMultiplier = 1000;*/
	serialTimeouts.WriteTotalTimeoutConstant = 5000;
	serialTimeouts.WriteTotalTimeoutMultiplier = 1000;

	if (!SetCommTimeouts(hSerial, &serialTimeouts))
	{
		cout << "error Set timeouts\n";
	}

}

void EnableActivePorts()
{
	if (ports[0])
		EnableWindow((HWND)hCom1, TRUE);
	if (ports[1])
		EnableWindow((HWND)hCom1, TRUE);
	if (ports[2])
		EnableWindow((HWND)hCom2, TRUE);
	if (ports[3])
		EnableWindow((HWND)hCom3, TRUE);
	if (ports[4])
		EnableWindow((HWND)hCom4, TRUE);
	if (ports[5])
		EnableWindow((HWND)hCom5, TRUE);
	if (ports[6])
		EnableWindow((HWND)hCom6, TRUE);
	if (ports[7])
		EnableWindow((HWND)hCom7, TRUE);
	if (ports[8])
		EnableWindow((HWND)hCom8, TRUE);
	if (ports[9])
		EnableWindow((HWND)hCom9, TRUE);
	if (ports[10])
		EnableWindow((HWND)hCom10, TRUE);
}

LRESULT CALLBACK WindowProcedure(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	HDC hDc;
	HFONT hfont;
	PAINTSTRUCT ps;

	switch (uMsg) {
		
	case WM_COMMAND:

		switch (LOWORD(wParam)) {
		
		case SCAN_COM:
			GetActivePorts();
			EnableActivePorts();
			break;
		case CONNECT_COM:
			if (IsDlgButtonChecked(hWnd, COM1))
				sPortName = L"COM1";
			if (IsDlgButtonChecked(hWnd, COM2))
				sPortName = L"COM2";
			if (IsDlgButtonChecked(hWnd, COM3))
				sPortName = L"COM3";
			if (IsDlgButtonChecked(hWnd, COM4))
				sPortName = L"COM4";
			if (IsDlgButtonChecked(hWnd, COM5))
				sPortName = L"COM5";
			if (IsDlgButtonChecked(hWnd, COM6))
				sPortName = L"COM6";
			if (IsDlgButtonChecked(hWnd, COM7))
				sPortName = L"COM7";
			if (IsDlgButtonChecked(hWnd, COM8))
				sPortName = L"COM8";
			if (IsDlgButtonChecked(hWnd, COM9))
				sPortName = L"COM9";
			if (IsDlgButtonChecked(hWnd, COM10))
				sPortName = L"COM10";

			Connecting();
			threadReadingCom = CreateThread(NULL, 0, ReadCOM, NULL, 0, NULL);
			break;
		
		case COM_STOPRESUME:
			if (!isStop)
			{
				SuspendThread(threadReadingCom);
			}
			else
			{
				ResumeThread(threadReadingCom);
			}
			isStop = !isStop;
			break;
		case CODE_CLEAR:
			SetWindowText((HWND)hCode, L"");
			break;
		case SEND_BUT:
		{
			//get Mail
			int len = SendMessage((HWND)hSendEdit, WM_GETTEXTLENGTH, 0, 0);
			mailLength = len / 2;
			char* buffer = new char[len*2];
			SendMessage((HWND)hSendEdit, WM_GETTEXT, len*2, (LPARAM)buffer);

			char bufferHex[255] = { 0 };

			int currPos = 0;
			for (int i = 0; i < len ; i++)
			{
				if (*(buffer + i * 2) != ' ') {
					bufferHex[currPos] = *(buffer + i * 2);
					currPos++;
				}
					
			}

			mailLength = currPos / 2;

			for (int i = 0; i < mailLength ; i++)
			{
				char strByte[2];
				strByte[0] = bufferHex[i*2];


				strByte[1] = bufferHex[i*2+1];


				int g = (int)strtol(strByte, NULL, 16);

				mail[i] = char(g);
				
			}

			BYTE chkSum = 0x00;
			for (int i = 0; i < mailLength ; i++)
			{
				chkSum = chkSum ^ mail[i];
			}
			mailLength++;
			mail[mailLength-1] = char(chkSum);


			SuspendThread(threadReadingCom);

			PurgeComm(hSerial, PURGE_TXCLEAR);
			WriteCOM();

			ResumeThread(threadReadingCom);
		}
			break; 


		case MENU_EXIT:
			//MessageBeep(MB_OK);
			if (MessageBoxW(hWnd, L"Are you sure?", L"Exit", MB_YESNO) == IDYES)
			{
				DestroyWindow(hWnd);
			}
			break;
		case MENU_ABOUT:
			MessageBoxW(NULL, L"   The program provides features for communicate with i-bus protocol in BMW e38, e39, e53.\n\
The program was developed by a third-year student of POIT BSUIR.\t\t\t\t\t\t Kondratskiy Anton.", L"Info", MB_OK);
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
		SetControls(hWnd);
		break;
	case WM_CLOSE:
		DestroyWindow(hWnd);

		//MessageBeep(MB_OK);
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


	while (GetMessage(&msg, 0, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
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
	AppendMenuW(hFileMenu, MF_STRING, MENU_EXIT, L"Exit");

	// Menu About:
	AppendMenuW(hMenu, MF_POPUP, (UINT_PTR)hAboutMenu, L"About");
	AppendMenuW(hAboutMenu, MF_STRING, MENU_ABOUT, L"Info");	

	SetMenu(hWnd, hMenu);
}

// Create controls
void SetControls(HWND hWnd)
{

	hComSettings = CreateWindowW(L"Static", L"", WS_CHILD | WS_VISIBLE | SS_GRAYRECT, 1*WND_BETWEEN,						   10, WND_OUTER, WND_FIELD_HEIGHT, 
		hWnd, NULL, NULL, NULL);
	hCode		 = CreateWindowW(L"Edit", L"", WS_CHILD | WS_VISIBLE | SS_LEFT | WS_VSCROLL | WS_BORDER | ES_READONLY | ES_MULTILINE | ES_AUTOVSCROLL, 2*WND_BETWEEN + WND_OUTER,			   10, WND_INNER*2+WND_BETWEEN, WND_FIELD_HEIGHT, hWnd, NULL, NULL, NULL);

	hCodePresets = CreateWindowW(L"Static", L"", WS_CHILD | WS_VISIBLE | SS_GRAYRECT, 4*WND_BETWEEN + WND_OUTER + 2*WND_INNER, 10, WND_OUTER, WND_FIELD_HEIGHT, hWnd, NULL, NULL, NULL);
	
	//SetWindowLongPtr((HWND)hComSettings, GWLP_WNDPROC | GWL_EXSTYLE, (LONG_PTR)BtnProc);

	CreateWindowW(L"button", L"Scan...", WS_CHILD | WS_VISIBLE, 52, 22, 50, 20, (HWND)hWnd, (HMENU)SCAN_COM, NULL, NULL);
	hComPorts = CreateWindowW(L"Button", L"Com ports:", WS_CHILD | WS_VISIBLE | BS_GROUPBOX , 50, 50, WND_OUTER - 40, 285, (HWND)hWnd, NULL, NULL, NULL);
	hCom1 = CreateWindowW(L"button", L"Com 1", WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON, 60, 80 + 0 * 25, WND_OUTER - 60, 20, (HWND)hWnd, (HMENU)COM1, NULL, NULL);
	hCom2 = CreateWindowW(L"button", L"Com 2", WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON, 60, 80 + 1 * 25, WND_OUTER - 60, 20, (HWND)hWnd, (HMENU)COM2, NULL, NULL);
	hCom3 = CreateWindowW(L"button", L"Com 3", WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON, 60, 80 + 2 * 25, WND_OUTER - 60, 20, (HWND)hWnd, (HMENU)COM3, NULL, NULL);
	hCom4 = CreateWindowW(L"button", L"Com 4", WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON, 60, 80 + 3 * 25, WND_OUTER - 60, 20, (HWND)hWnd, (HMENU)COM4, NULL, NULL);
	hCom5 = CreateWindowW(L"button", L"Com 5", WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON, 60, 80 + 4 * 25, WND_OUTER - 60, 20, (HWND)hWnd, (HMENU)COM5, NULL, NULL);
	hCom6 = CreateWindowW(L"button", L"Com 6", WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON, 60, 80 + 5 * 25, WND_OUTER - 60, 20, (HWND)hWnd, (HMENU)COM6, NULL, NULL);
	hCom7 = CreateWindowW(L"button", L"Com 7", WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON, 60, 80 + 6 * 25, WND_OUTER - 60, 20, (HWND)hWnd, (HMENU)COM7, NULL, NULL);
	hCom8 = CreateWindowW(L"button", L"Com 8", WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON, 60, 80 + 7 * 25, WND_OUTER - 60, 20, (HWND)hWnd, (HMENU)COM8, NULL, NULL);
	hCom9 = CreateWindowW(L"button", L"Com 9", WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON, 60, 80 + 8 * 25, WND_OUTER - 60, 20, (HWND)hWnd, (HMENU)COM9, NULL, NULL);
	hCom10 = CreateWindowW(L"button", L"Com 10", WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON, 60, 80 + 9 * 25, WND_OUTER - 60, 20, (HWND)hWnd, (HMENU)COM10, NULL, NULL);
	CreateWindowW(L"button", L"Connect to ...", WS_CHILD | WS_VISIBLE, 52, WND_FIELD_HEIGHT - 40, 100, 20, (HWND)hWnd, (HMENU)CONNECT_COM, NULL, NULL);

	EnableWindow((HWND)hCom1, FALSE);
	EnableWindow((HWND)hCom2, FALSE);
	EnableWindow((HWND)hCom3, FALSE);
	EnableWindow((HWND)hCom4, FALSE);
	EnableWindow((HWND)hCom5, FALSE);
	EnableWindow((HWND)hCom6, FALSE);
	EnableWindow((HWND)hCom7, FALSE);
	EnableWindow((HWND)hCom8, FALSE);
	EnableWindow((HWND)hCom9, FALSE);
	EnableWindow((HWND)hCom10, FALSE);

	hComStopResume = CreateWindowW(L"Button", L"Stop/Resume", WS_CHILD | WS_VISIBLE | SS_LEFT, 1 * WND_BETWEEN, 10 + WND_FIELD_HEIGHT + 20, WND_OUTER / 2 - 20, 30, hWnd, (HMENU)COM_STOPRESUME, NULL, NULL);
	hCodeClear = CreateWindowW(L"Button", L"Clear", WS_CHILD | WS_VISIBLE | SS_LEFT, 1 * WND_BETWEEN + WND_OUTER / 2 + 20, 10 + WND_FIELD_HEIGHT + 20, WND_OUTER / 2 - 20, 30, hWnd, (HMENU)CODE_CLEAR, NULL, NULL);

	hSendEdit = CreateWindowW(L"Edit", L"C0066831400000DF", WS_CHILD | WS_VISIBLE | SS_LEFT | WS_BORDER, 2 * WND_BETWEEN + WND_OUTER, 10 + WND_FIELD_HEIGHT + 20, (4 * WND_BETWEEN + WND_OUTER + 2 * WND_INNER - 180)-(2 * WND_BETWEEN + WND_OUTER), 30, hWnd, NULL, NULL, NULL);
	hSendButton = CreateWindowW(L"Button", L"Send command->", WS_CHILD | WS_VISIBLE | SS_LEFT, 4 * WND_BETWEEN + WND_OUTER + 2 * WND_INNER - 180, 10 + WND_FIELD_HEIGHT + 20, 150, 30, hWnd, (HMENU)SEND_BUT, NULL, NULL);


	CreateWindowW(L"Button", L"button1", WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON, 300, 100, 200,
		50, hWnd, (HMENU)BUT_1, NULL, NULL);

	CreateWindowW(L"button", L"button2", WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON, 300, 200, 200,
		50, hWnd, (HMENU)BUT_2, NULL, NULL);

	CreateWindowW(L"button", L"button3", WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON, 300, 300, 200,
		50, hWnd, (HMENU)BUT_3, NULL, NULL);

	//hEdit = CreateWindowW(L"Static", L"Edit control", WS_VISIBLE | WS_CHILD | SS_LEFT, 5, 5, 320, 520, hWnd, NULL, NULL, NULL);




}


