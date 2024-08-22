#pragma once 
#ifdef _KERNEL_MODE
#include <wdm.h>
#define malloc(size) ExAllocatePoolWithTag(NonPagedPool, size, 'KMEM')
#define free(ptr) ExFreePool(ptr)
#else
#include <stdlib.h>
#endif

#include "AESOperation.h"

void AESEncrypt(BYTE* in, BYTE* out, BYTE* key);
void AESDecrypt(BYTE* in, BYTE* out, BYTE* key);