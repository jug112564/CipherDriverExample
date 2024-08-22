#include "Events.h"

HWND edit1;
HWND edit2;
HWND edit3;

void CreateTestDriverWindows(HWND hWnd)
{
	CreateWindow(TEXT("button"), TEXT("open device"), WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON | BS_MULTILINE,
		50, 10,
		100, 25,
		hWnd, (HMENU)(1), GetModuleHandle(NULL), NULL);
	CreateWindow(TEXT("button"), TEXT("close device"), WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON | BS_MULTILINE,
		200, 10,
		100, 25,
		hWnd, (HMENU)(2), GetModuleHandle(NULL), NULL);
	CreateWindow(TEXT("button"), TEXT("send"), WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON | BS_MULTILINE,
		480, 50,
		50, 25,
		hWnd, (HMENU)(3), GetModuleHandle(NULL), NULL);
	CreateWindow(TEXT("button"), TEXT("recv"), WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON | BS_MULTILINE,
		480, 100,
		50, 25,
		hWnd, (HMENU)(4), GetModuleHandle(NULL), NULL);
	edit1 = CreateWindow(TEXT("edit"), NULL, WS_BORDER | WS_CHILD | WS_VISIBLE,
		150, 50, 
		300, 25, 
		hWnd, (HMENU)(5), GetModuleHandle(NULL), NULL);	
	edit2 = CreateWindow(TEXT("edit"), NULL, WS_BORDER | WS_CHILD | WS_VISIBLE,
		150, 100, 
		300, 25, 
		hWnd, (HMENU)(6), GetModuleHandle(NULL), NULL);	
	edit3 = CreateWindow(TEXT("edit"), NULL, WS_BORDER | WS_CHILD | WS_VISIBLE,
		150, 150,
		300, 25,
		hWnd, (HMENU)(7), GetModuleHandle(NULL), NULL);

	SetWindowText(edit1, TEXT("Input Data Here"));
	SetWindowText(edit2, TEXT("No value received"));
	EnableWindow(edit2, FALSE);	
	SetWindowText(edit3, TEXT("No value received"));
	EnableWindow(edit3, FALSE);
}

void PaintEvent(HWND hWnd, HDC hdc, PAINTSTRUCT* ps)
{
	hdc = BeginPaint(hWnd, ps);
	TextOut(hdc, 20, 50, TEXT("Input Send Data"), 15);
	TextOut(hdc, 20, 100, TEXT("Receive Reult"), 13);
	TextOut(hdc, 20, 150, TEXT("Decryption Result"), 17);
	EndPaint(hWnd, &ps);
}
//testing make device
HANDLE Button1Event(HWND hWnd)
{
	HANDLE devicehandle = CreateFile(L"\\\\.\\SampleCiperDevicelink1", GENERIC_ALL, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_SYSTEM, 0);
	if (devicehandle == INVALID_HANDLE_VALUE)
	{
		DWORD error = GetLastError();
		WCHAR errorMessage[256];

		FormatMessage(
			FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL,
			error,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			errorMessage,
			(sizeof(errorMessage) / sizeof(WCHAR)),
			NULL
		);

		MessageBox(hWnd, errorMessage, L"Open Error", MB_OK);
	}
	else
	{
		MessageBox(hWnd, L"valid device", L"Open Success", MB_OK);
	}
	
	return devicehandle;
}

//testing close device
void Button2Event(HANDLE devicehandle, HWND hWnd)
{
	if (devicehandle != INVALID_HANDLE_VALUE)
	{
		CloseHandle(devicehandle);
		MessageBox(hWnd, L"Close Success", L"Close Success", MB_OK);
	}
	else
	{
		MessageBox(hWnd, L"vaild handle not exist", L"Close Error", MB_OK);
	}
}

//testing send message
void Button3Event(HANDLE devicehandle, HWND hWnd)
{
	int length = GetWindowTextLength(edit1);
	if (length > 0)
	{
		// 텍스트 가져오기
		TCHAR* input = (TCHAR*)malloc((length + 1) * sizeof(TCHAR));
		GetWindowText(edit1, input, length + 1);

		//char로 변환
		int charLength = WideCharToMultiByte(CP_ACP, 0, input, -1, NULL, 0, NULL, NULL);
		char* inputChar = (char*)malloc(charLength * sizeof(char));
		WideCharToMultiByte(CP_ACP, 0, input, -1, inputChar, charLength, NULL, NULL);

		ULONG returnLength = 0;
		char wr[4] = { 0 };

		if (devicehandle != INVALID_HANDLE_VALUE && devicehandle != NULL)
		{
			if (!DeviceIoControl(devicehandle, DEVICE_SEND, inputChar, (strlen(inputChar) + 1), NULL, 0, &returnLength, 0))
			{
				MessageBox(hWnd, L"DeviceIoControl error", L"send", MB_OK);
			}
			else
			{
				_itoa_s(returnLength, wr, 4, 10);
				MessageBox(hWnd, L"send success", L"send success", MB_OK);
			}
		}
		else
			MessageBox(hWnd, L"error", L"Send Error", MB_OK);

		free(input);
		free(inputChar);
	}
	else
	{
		MessageBox(hWnd, L"error : please input text.", L"send", MB_OK);
	}
}

//testing receive message
void Button4Event(HANDLE devicehandle, HWND hWnd)
{
	unsigned char message[128] = { 0 };
	ULONG returnLength = 0;
	AESKEY key[16] = { 0x26, 0x7f, 0xb8, 0x06, 0xa2, 0x7e, 0xb4, 0xa8, 0x47, 0x8a, 0x04, 0x68 , 0xfd, 0x11, 0xef, 0x05 };

	if (devicehandle != INVALID_HANDLE_VALUE && devicehandle != NULL)
	{
		if (!DeviceIoControl(devicehandle, DEVICE_REC, NULL, 0, message, (strlen(message) + 1), &returnLength, 0))
		{
			MessageBox(hWnd, L"DeviceIoControl error", L"Receive Error", MB_OK);
		}
		else
		{
			//받은 원형 hex값 출력하기
			WCHAR recvString[128] = { 0, };
			for (int i = 0; i < 16; i++) {
				_stprintf_s(&recvString[i * 2], 3, _T("%02x"), message[i]);  // UNICODE
			}
			MessageBox(hWnd, recvString, L"Receive Success", MB_OK);
			SetWindowText(edit2, recvString);

			//암호화된 문자열 복호화 및 출력하기
			BYTE byteResult[256] = { 0, };
			WCHAR test[128] = { 0, };
			int blockCount = returnLength / (BLOCKSIZE * 4);
			for (int i = 0; i < blockCount; i++)
			{
				AESDecrypt(&message[i * BLOCKSIZE * 4], &byteResult[i * BLOCKSIZE * 4], key);
			}
			int nLen = MultiByteToWideChar(CP_ACP, 0, byteResult, strlen(byteResult), NULL, NULL);
			MultiByteToWideChar(CP_ACP, 0, byteResult, strlen(byteResult), test, nLen);
			
			SetWindowText(edit3, test);
		}
	}
}