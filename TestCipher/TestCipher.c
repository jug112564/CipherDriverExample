#include "TestCipher.h"

#ifdef ALLOC_PRAGMA
#pragma alloc_text (INIT, DriverEntry)
#pragma alloc_text (PAGE, TestUnload)
#pragma alloc_text (PAGE, DispatchDevCTL)
#endif

UNICODE_STRING DeviceName = RTL_CONSTANT_STRING(L"\\Device\\SampleCiperDevice1");
UNICODE_STRING SymLinkName = RTL_CONSTANT_STRING(L"\\DosDevices\\SampleCiperDevicelink1");

PDEVICE_OBJECT DeviceObject = NULL;

BYTE* encryptedValue;
ULONG encryptedLength;

int EncryptDataWithAES(BYTE* origin, BYTE *result)
{
	int originLen = strlen(origin);
	AESKEY key[16] = { 0x26, 0x7f, 0xb8, 0x06, 0xa2, 0x7e, 0xb4, 0xa8, 0x47, 0x8a, 0x04, 0x68 , 0xfd, 0x11, 0xef, 0x05 };

	// 블록 수 계산 및 패딩을 위한 길이 조정
	int blockCount = (originLen % (BLOCKSIZE * 4)) ? (originLen / (BLOCKSIZE * 4) + 1) : (originLen / (BLOCKSIZE * 4));
	int paddedLen = blockCount * BLOCKSIZE * 4;

	BYTE padded_origin[128] = { 0, };  // 패딩된 원본 데이터
	RtlCopyMemory(padded_origin, origin, originLen);  // 원본 데이터를 복사하여 패딩된 배열에 저장
	
	// AES 암호화
	for (int i = 0; i < blockCount; i++)
	{
		AESEncrypt(&padded_origin[i * BLOCKSIZE * 4], &result[i * BLOCKSIZE * 4], key);
	}
	return paddedLen;
}

VOID TestUnload(PDRIVER_OBJECT DriverObject)
{
	IoDeleteSymbolicLink(&SymLinkName);
	IoDeleteDevice(DeviceObject);

	ExFreePoolWithTag(encryptedValue, 'eval');
	DbgPrint("driver unload\n");
}

NTSTATUS DispatchDevCTL(PDEVICE_OBJECT DeviceOjbect, PIRP Irp)
{
	PIO_STACK_LOCATION irpsp = IoGetCurrentIrpStackLocation(Irp);
	NTSTATUS status = STATUS_SUCCESS;
	ULONG returnLength = 0;
	BYTE* buffer = Irp->AssociatedIrp.SystemBuffer;
	ULONG inLength = irpsp->Parameters.DeviceIoControl.InputBufferLength;
	ULONG outLength = irpsp->Parameters.DeviceIoControl.OutputBufferLength;
	WCHAR* demo = L"test data";

	switch (irpsp->Parameters.DeviceIoControl.IoControlCode)
	{
	case DEVICE_SEND:
	{
		DbgPrint("send data is %ws \n", buffer);

		//buffer을 이용한 암호화
		returnLength = EncryptDataWithAES(buffer, encryptedValue);
		encryptedLength = returnLength;
		break;
	}
	case DEVICE_REC:
	{
		RtlCopyMemory(buffer, encryptedValue, 128);
		returnLength = encryptedLength;
		break;
	}
	default:
		status = STATUS_INVALID_PARAMETER;
	}

	Irp->IoStatus.Status = status;
	Irp->IoStatus.Information = returnLength;
	IoCompleteRequest(Irp, IO_NO_INCREMENT);
	return status;
}

NTSTATUS DispatchPassThru(PDEVICE_OBJECT DeviceOjbect, PIRP Irp)
{
	PIO_STACK_LOCATION irpsp = IoGetCurrentIrpStackLocation(Irp);
	NTSTATUS status = STATUS_SUCCESS;

	switch (irpsp->MajorFunction)
	{
	case IRP_MJ_CREATE:
		DbgPrint("create request\n");
		break;
	case IRP_MJ_CLOSE:
		DbgPrint("close request\n");
		break;
	default:
		status = STATUS_INVALID_PARAMETER;
		break;
	}

	Irp->IoStatus.Information = 0;
	Irp->IoStatus.Status = status;
	IoCompleteRequest(Irp, IO_NO_INCREMENT);
	return status;
}

NTSTATUS DriverEntry(PDRIVER_OBJECT DriverObject, PUNICODE_STRING RegistryPath)
{
	NTSTATUS status;
	int i;

	DriverObject->DriverUnload = TestUnload;

	//create device
	status = IoCreateDevice(
		DriverObject,
		0,
		&DeviceName,
		FILE_DEVICE_UNKNOWN,
		FILE_DEVICE_SECURE_OPEN,
		FALSE,
		&DeviceObject
	);
	if (!NT_SUCCESS(status))
	{
		DbgPrint("create device failed\n");
		return status;
	}

	status = IoCreateSymbolicLink(&SymLinkName, &DeviceName);
	if (!NT_SUCCESS(status))
	{
		DbgPrint("create symbolic link failed\n");
		IoDeleteDevice(DeviceObject);
		return status;
	}

	for (i = 0; i < IRP_MJ_MAXIMUM_FUNCTION; i++) {
		DriverObject->MajorFunction[i] = DispatchPassThru;
	}

	//offer send and recv
	DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = DispatchDevCTL;

	encryptedValue = (BYTE*)ExAllocatePoolWithTag(NonPagedPool, 128, 'eval');
	if (encryptedValue == NULL)
	{
		DbgPrint("Memory allocation for encryptedValue failed\n");
		IoDeleteSymbolicLink(&SymLinkName);
		IoDeleteDevice(DeviceObject);
		return STATUS_INSUFFICIENT_RESOURCES;
	}

	DbgPrint("driver load\n");

	return status;
}