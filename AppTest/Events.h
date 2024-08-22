
#pragma once
#include <Windows.h>
#include <tchar.h>
#define DEVICE_SEND CTL_CODE(FILE_DEVICE_UNKNOWN,0x801,METHOD_BUFFERED,FILE_WRITE_DATA)
#define DEVICE_REC CTL_CODE(FILE_DEVICE_UNKNOWN,0x802,METHOD_BUFFERED,FILE_READ_DATA)

#include ".\..\AES128\AES128.h"

void CreateTestDriverWindows(HWND hWnd);
void PaintEvent(HWND hWnd, HDC hdc, PAINTSTRUCT* ps);
HANDLE Button1Event(HWND hWnd);
void Button2Event(HANDLE devicehandle, HWND hWnd);
void Button3Event(HANDLE devicehandle, HWND hWnd);
void Button4Event(HANDLE devicehandle, HWND hWnd);