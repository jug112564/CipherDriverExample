#pragma once
#include "types.h"

#define HIHEX(x) (x>>4)
#define LOWHEX(x) (x&0x0F)
#define BTOW(b0,b1,b2,b3) (((AESWORD)b0 <<24)|((AESWORD)b1 << 16)|((AESWORD)b2<<8)|((AESWORD)b3))

BYTE GFMultiple(BYTE b, BYTE n);
AESWORD RotateWord(AESWORD wordValue);
AESWORD SubWord(AESWORD wordValue);