#pragma once

#include "types.h"
#include "formula.h"

#define BLOCKSIZE 4
#define KEYSIZE 4
#define ROUNDNUMBER 10

void SubsitudeBytes(BYTE state[][4], BYTE box[16][16]);
void ShiftRows(BYTE state[][4], AESBOOL isInv);
void MixColumns(BYTE state[][4], BYTE mixMatrix[4][4]);
void AddRoundKey(BYTE state[][4], AESWORD* rKey);
void KeyExpansion(BYTE* key, AESWORD* W);