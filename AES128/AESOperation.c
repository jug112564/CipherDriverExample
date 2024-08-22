#include "AESOperation.h"

void SubsitudeBytes(BYTE state[][4], BYTE box[16][16])
{
	int i, j;

	for (i = 0; i < 4; i++)
	{
		for (j = 0; j < 4; j++)
		{
			state[i][j] = box[HIHEX(state[i][j])][LOWHEX(state[i][j])];
		}
	}
}

void ShiftRows(BYTE state[][4], AESBOOL isInv)
{
	int i, j;

	for (i = 0; i < 4; i++)
	{
		for (j = 0; j < 4; j++)
		{
			BYTE* row = state[i];
			if (isInv)
			{
				BYTE temp = row[0];
				row[0] = row[1];
				row[1] = row[2];
				row[2] = row[3];
				row[3] = temp;
			}
			else
			{
				BYTE temp = row[3];
				row[3] = row[2];
				row[2] = row[1];
				row[1] = row[0];
				row[0] = temp;
			}

		}
	}
}

void MixColumns(BYTE state[][4], BYTE mixMatrix[4][4])
{
	int i, j, k;

	BYTE mixResult[4][4] = { 0, };

	for (i = 0; i < 4; i++)
	{
		for (j = 0; j < 4; j++)
		{
			for (k = 0; k < 4; k++)
			{
				mixResult[i][j] ^= GFMultiple(mixMatrix[i][k], state[k][j]);
			}
		}
	}

	for (i = 0; i < 4; i++)
	{
		for (j = 0; j < 4; j++)
		{
			state[i][j] = mixResult[i][j];
		}
	}
}

void AddRoundKey(BYTE state[][4], AESWORD* rKey)
{
	int i, j;
	AESWORD mask, shift;

	for (i = 0; i < 4; i++)
	{
		shift = 24; 
		mask = 0xFF000000;
		for (j = 0; j < 4; j++)
		{
			state[j][i] = ((rKey[i] & mask) >> shift) ^ state[j][i];
			mask >>= 8;
			shift -= 8;
		}
	}
}

void KeyExpansion(BYTE* key, AESWORD* W)
{
	AESWORD temp;
	AESWORD Rcon[10] =
	{
		0x01000000,0x02000000,0x04000000,0x08000000,
		0x10000000,0x20000000,0x40000000,0x80000000,
		0x1b000000,0x36000000
	};
	int i = 0;
	while (i < KEYSIZE)
	{
		W[i] = BTOW(key[4 * i], key[4 * i + 1], key[4 * i + 2], key[4 * i + 3]);
		i = i + 1;
	}
	i = KEYSIZE;
	while (i < (BLOCKSIZE * (ROUNDNUMBER + 1)))
	{
		temp = W[i - 1];
		if (i % KEYSIZE == 0)
			temp = SubWord(RotateWord(temp)) ^ Rcon[i / KEYSIZE - 1];
		else if ((KEYSIZE > 6) && (i % KEYSIZE == 4))
			temp = SubWord(temp);
		W[i] = W[i - KEYSIZE] ^ temp;
		i += 1;
	}
}