#pragma once
#include <ntddk.h>
#include <ntstrsafe.h>

#include ".\..\AES128\AES128.h"

#define DEVICE_SEND CTL_CODE(FILE_DEVICE_UNKNOWN,0x801,METHOD_BUFFERED,FILE_WRITE_DATA)
#define DEVICE_REC CTL_CODE(FILE_DEVICE_UNKNOWN,0x802,METHOD_BUFFERED,FILE_READ_DATA)

DRIVER_INITIALIZE DriverEntry;
DRIVER_UNLOAD TestUnload;
_Dispatch_type_(IRP_MJ_DEVICE_CONTROL) DRIVER_DISPATCH DispatchDevCTL;

int EncryptDataWithAES(BYTE* origin, BYTE* result);
NTSTATUS DispatchPassThru(PDEVICE_OBJECT DeviceOjbect, PIRP Irp);