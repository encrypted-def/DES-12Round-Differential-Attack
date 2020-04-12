#include <stdio.h>
#include <windows.h>
#include <time.h>
#include <random>
#include <stdlib.h>
#include <algorithm>
using namespace std;
unsigned int key_int[32];
const int SBOX[8][64] = {
  { 14, 0, 4, 15, 13, 7, 1, 4, 2, 14, 15, 2, 11, 13, 8, 1, 3, 10, 10, 6, 6, 12, 12, 11, 5, 9, 9, 5, 0, 3, 7, 8, 4, 15, 1, 12, 14, 8, 8, 2, 13, 4, 6, 9, 2, 1, 11, 7, 15, 5, 12, 11, 9, 3, 7, 14, 3, 10, 10, 0, 5, 6, 0, 13, },
  { 15, 3, 1, 13, 8, 4, 14, 7, 6, 15, 11, 2, 3, 8, 4, 14, 9, 12, 7, 0, 2, 1, 13, 10, 12, 6, 0, 9, 5, 11, 10, 5, 0, 13, 14, 8, 7, 10, 11, 1, 10, 3, 4, 15, 13, 4, 1, 2, 5, 11, 8, 6, 12, 7, 6, 12, 9, 0, 3, 5, 2, 14, 15, 9, },
  { 10, 13, 0, 7, 9, 0, 14, 9, 6, 3, 3, 4, 15, 6, 5, 10, 1, 2, 13, 8, 12, 5, 7, 14, 11, 12, 4, 11, 2, 15, 8, 1, 13, 1, 6, 10, 4, 13, 9, 0, 8, 6, 15, 9, 3, 8, 0, 7, 11, 4, 1, 15, 2, 14, 12, 3, 5, 11, 10, 5, 14, 2, 7, 12, },
  { 7, 13, 13, 8, 14, 11, 3, 5, 0, 6, 6, 15, 9, 0, 10, 3, 1, 4, 2, 7, 8, 2, 5, 12, 11, 1, 12, 10, 4, 14, 15, 9, 10, 3, 6, 15, 9, 0, 0, 6, 12, 10, 11, 1, 7, 13, 13, 8, 15, 9, 1, 4, 3, 5, 14, 11, 5, 12, 2, 7, 8, 2, 4, 14, },
  { 2, 14, 12, 11, 4, 2, 1, 12, 7, 4, 10, 7, 11, 13, 6, 1, 8, 5, 5, 0, 3, 15, 15, 10, 13, 3, 0, 9, 14, 8, 9, 6, 4, 11, 2, 8, 1, 12, 11, 7, 10, 1, 13, 14, 7, 2, 8, 13, 15, 6, 9, 15, 12, 0, 5, 9, 6, 10, 3, 4, 0, 5, 14, 3, },
  { 12, 10, 1, 15, 10, 4, 15, 2, 9, 7, 2, 12, 6, 9, 8, 5, 0, 6, 13, 1, 3, 13, 4, 14, 14, 0, 7, 11, 5, 3, 11, 8, 9, 4, 14, 3, 15, 2, 5, 12, 2, 9, 8, 5, 12, 15, 3, 10, 7, 11, 0, 14, 4, 1, 10, 7, 1, 6, 13, 0, 11, 8, 6, 13, },
  { 4, 13, 11, 0, 2, 11, 14, 7, 15, 4, 0, 9, 8, 1, 13, 10, 3, 14, 12, 3, 9, 5, 7, 12, 5, 2, 10, 15, 6, 8, 1, 6, 1, 6, 4, 11, 11, 13, 13, 8, 12, 1, 3, 4, 7, 10, 14, 7, 10, 9, 15, 5, 6, 0, 8, 15, 0, 14, 5, 2, 9, 3, 2, 12, },
  { 13, 1, 2, 15, 8, 13, 4, 8, 6, 10, 15, 3, 11, 7, 1, 4, 10, 12, 9, 5, 3, 6, 14, 11, 5, 0, 0, 14, 12, 9, 7, 2, 7, 2, 11, 1, 4, 14, 1, 7, 9, 4, 12, 10, 14, 8, 2, 13, 0, 15, 6, 12, 10, 9, 13, 0, 15, 3, 3, 5, 5, 6, 8, 11, }
};
int v1 = 0;
int v2 = 0;
int v3 = 0;
int diff_table[8][64][16];
// diff table을 갱신함
void setDiffTable(void) {
  for (int idx = 0; idx < 8; idx++)
    for (int i = 0; i < 64; i++)
      for (int j = 0; j < 64; j++)
        diff_table[idx][i^j][SBOX[idx][i] ^ SBOX[idx][j]]++;
}
int getSboxOutputBit(byte Intermediate[8], int idx, int dir) {
  byte O[4] = { Intermediate[0 + 4 * dir], Intermediate[1 + 4 * dir], Intermediate[2 + 4 * dir], Intermediate[3 + 4 * dir] };
  switch (idx) {
  case 1: return ((O[1] & 0x80) >> 4) | ((O[2] & 0x80) >> 5) | (O[2] & 0x02) | ((O[3] & 0x02) >> 1);
  case 2: return (O[1] & 0x08) | ((O[3] & 0x10) >> 2) | ((O[0] & 0x40) >> 5) | ((O[2] & 0x40) >> 6);
  case 3: return ((O[2] & 0x01) << 3) | ((O[1] & 0x01) << 2) | ((O[3] & 0x04) >> 1) | ((O[0] & 0x04) >> 2);
  case 4: return ((O[3] & 0x40) >> 3) | ((O[2] & 0x10) >> 2) | ((O[1] & 0x40) >> 5) | ((O[0] & 0x80) >> 7);
  case 5: return ((O[0] & 0x01) << 3) | (O[1] & 0x04) | ((O[3] & 0x80) >> 6) | ((O[0] & 0x20) >> 5);
  case 6: return ((O[0] & 0x10) >> 1) | ((O[3] & 0x08) >> 1) | ((O[1] & 0x20) >> 4) | ((O[2] & 0x20) >> 5);
  case 7: return ((O[3] & 0x01) << 3) | ((O[1] & 0x10) >> 2) | ((O[2] & 0x04) >> 1) | ((O[0] & 0x02) >> 1);
  case 8: return (O[0] & 0x08) | ((O[3] & 0x20) >> 3) | (O[1] & 0x02) | ((O[2] & 0x08) >> 3);
  default: return -1; // unreachable
  }
  return -1; // unreachable
}

const unsigned int SB1[64] = { 0x00808200,0x00000000,0x00008000,0x00808202,0x00808002,0x00008202,0x00000002,0x00008000,
0x00000200,0x00808200,0x00808202,0x00000200,0x00800202,0x00808002,0x00800000,0x00000002,
0x00000202,0x00800200,0x00800200,0x00008200,0x00008200,0x00808000,0x00808000,0x00800202,
0x00008002,0x00800002,0x00800002,0x00008002,0x00000000,0x00000202,0x00008202,0x00800000,
0x00008000,0x00808202,0x00000002,0x00808000,0x00808200,0x00800000,0x00800000,0x00000200,
0x00808002,0x00008000,0x00008200,0x00800002,0x00000200,0x00000002,0x00800202,0x00008202,
0x00808202,0x00008002,0x00808000,0x00800202,0x00800002,0x00000202,0x00008202,0x00808200,
0x00000202,0x00800200,0x00800200,0x00000000,0x00008002,0x00008200,0x00000000,0x00808002 };
const unsigned int SB2[64] = { 0x40084010,0x40004000,0x00004000,0x00084010,0x00080000,0x00000010,0x40080010,0x40004010,
0x40000010,0x40084010,0x40084000,0x40000000,0x40004000,0x00080000,0x00000010,0x40080010,
0x00084000,0x00080010,0x40004010,0x00000000,0x40000000,0x00004000,0x00084010,0x40080000,
0x00080010,0x40000010,0x00000000,0x00084000,0x00004010,0x40084000,0x40080000,0x00004010,
0x00000000,0x00084010,0x40080010,0x00080000,0x40004010,0x40080000,0x40084000,0x00004000,
0x40080000,0x40004000,0x00000010,0x40084010,0x00084010,0x00000010,0x00004000,0x40000000,
0x00004010,0x40084000,0x00080000,0x40000010,0x00080010,0x40004010,0x40000010,0x00080010,
0x00084000,0x00000000,0x40004000,0x00004010,0x40000000,0x40080010,0x40084010,0x00084000 };
const unsigned int SB3[64] = { 0x00000104,0x04010100,0x00000000,0x04010004,0x04000100,0x00000000,0x00010104,0x04000100,
0x00010004,0x04000004,0x04000004,0x00010000,0x04010104,0x00010004,0x04010000,0x00000104,
0x04000000,0x00000004,0x04010100,0x00000100,0x00010100,0x04010000,0x04010004,0x00010104,
0x04000104,0x00010100,0x00010000,0x04000104,0x00000004,0x04010104,0x00000100,0x04000000,
0x04010100,0x04000000,0x00010004,0x00000104,0x00010000,0x04010100,0x04000100,0x00000000,
0x00000100,0x00010004,0x04010104,0x04000100,0x04000004,0x00000100,0x00000000,0x04010004,
0x04000104,0x00010000,0x04000000,0x04010104,0x00000004,0x00010104,0x00010100,0x04000004,
0x04010000,0x04000104,0x00000104,0x04010000,0x00010104,0x00000004,0x04010004,0x00010100 };
const unsigned int SB4[64] = { 0x80401000,0x80001040,0x80001040,0x00000040,0x00401040,0x80400040,0x80400000,0x80001000,
0x00000000,0x00401000,0x00401000,0x80401040,0x80000040,0x00000000,0x00400040,0x80400000,
0x80000000,0x00001000,0x00400000,0x80401000,0x00000040,0x00400000,0x80001000,0x00001040,
0x80400040,0x80000000,0x00001040,0x00400040,0x00001000,0x00401040,0x80401040,0x80000040,
0x00400040,0x80400000,0x00401000,0x80401040,0x80000040,0x00000000,0x00000000,0x00401000,
0x00001040,0x00400040,0x80400040,0x80000000,0x80401000,0x80001040,0x80001040,0x00000040,
0x80401040,0x80000040,0x80000000,0x00001000,0x80400000,0x80001000,0x00401040,0x80400040,
0x80001000,0x00001040,0x00400000,0x80401000,0x00000040,0x00400000,0x00001000,0x00401040 };
const unsigned int SB5[64] = { 0x00000080,0x01040080,0x01040000,0x21000080,0x00040000,0x00000080,0x20000000,0x01040000,
0x20040080,0x00040000,0x01000080,0x20040080,0x21000080,0x21040000,0x00040080,0x20000000,
0x01000000,0x20040000,0x20040000,0x00000000,0x20000080,0x21040080,0x21040080,0x01000080,
0x21040000,0x20000080,0x00000000,0x21000000,0x01040080,0x01000000,0x21000000,0x00040080,
0x00040000,0x21000080,0x00000080,0x01000000,0x20000000,0x01040000,0x21000080,0x20040080,
0x01000080,0x20000000,0x21040000,0x01040080,0x20040080,0x00000080,0x01000000,0x21040000,
0x21040080,0x00040080,0x21000000,0x21040080,0x01040000,0x00000000,0x20040000,0x21000000,
0x00040080,0x01000080,0x20000080,0x00040000,0x00000000,0x20040000,0x01040080,0x20000080 };
const unsigned int SB6[64] = { 0x10000008,0x10200000,0x00002000,0x10202008,0x10200000,0x00000008,0x10202008,0x00200000,
0x10002000,0x00202008,0x00200000,0x10000008,0x00200008,0x10002000,0x10000000,0x00002008,
0x00000000,0x00200008,0x10002008,0x00002000,0x00202000,0x10002008,0x00000008,0x10200008,
0x10200008,0x00000000,0x00202008,0x10202000,0x00002008,0x00202000,0x10202000,0x10000000,
0x10002000,0x00000008,0x10200008,0x00202000,0x10202008,0x00200000,0x00002008,0x10000008,
0x00200000,0x10002000,0x10000000,0x00002008,0x10000008,0x10202008,0x00202000,0x10200000,
0x00202008,0x10202000,0x00000000,0x10200008,0x00000008,0x00002000,0x10200000,0x00202008,
0x00002000,0x00200008,0x10002008,0x00000000,0x10202000,0x10000000,0x00200008,0x10002008 };
const unsigned int SB7[64] = { 0x00100000,0x02100001,0x02000401,0x00000000,0x00000400,0x02000401,0x00100401,0x02100400,
0x02100401,0x00100000,0x00000000,0x02000001,0x00000001,0x02000000,0x02100001,0x00000401,
0x02000400,0x00100401,0x00100001,0x02000400,0x02000001,0x02100000,0x02100400,0x00100001,
0x02100000,0x00000400,0x00000401,0x02100401,0x00100400,0x00000001,0x02000000,0x00100400,
0x02000000,0x00100400,0x00100000,0x02000401,0x02000401,0x02100001,0x02100001,0x00000001,
0x00100001,0x02000000,0x02000400,0x00100000,0x02100400,0x00000401,0x00100401,0x02100400,
0x00000401,0x02000001,0x02100401,0x02100000,0x00100400,0x00000000,0x00000001,0x02100401,
0x00000000,0x00100401,0x02100000,0x00000400,0x02000001,0x02000400,0x00000400,0x00100001 };
const unsigned int SB8[64] = { 0x08000820,0x00000800,0x00020000,0x08020820,0x08000000,0x08000820,0x00000020,0x08000000,
0x00020020,0x08020000,0x08020820,0x00020800,0x08020800,0x00020820,0x00000800,0x00000020,
0x08020000,0x08000020,0x08000800,0x00000820,0x00020800,0x00020020,0x08020020,0x08020800,
0x00000820,0x00000000,0x00000000,0x08020020,0x08000020,0x08000800,0x00020820,0x00020000,
0x00020820,0x00020000,0x08020800,0x00000800,0x00000020,0x08020020,0x00000800,0x00020820,
0x08000800,0x00000020,0x08000020,0x08020000,0x08020020,0x08000000,0x00020000,0x08000820,
0x00000000,0x08020820,0x00020020,0x08000020,0x08020000,0x08000800,0x08000820,0x00000000,
0x08020820,0x00020800,0x00020800,0x00000820,0x00000820,0x00020020,0x08000000,0x08020800 };

/*
* DES electronic codebook encryption of one block
*/
void DES(unsigned int* key, byte* text, byte* C)
{
  unsigned int right, left, v0, v1;
  int i, keystep;
  left = text[3] | ((unsigned int)text[2] << 8) | ((unsigned int)text[1] << 16) | ((unsigned int)text[0] << 24);
  right = text[7] | ((unsigned int)text[6] << 8) | ((unsigned int)text[5] << 16) | ((unsigned int)text[4] << 24);
  v0 = key[0];
  v0 ^= (right >> 1) | (right << 31);
  left ^= SB1[(v0 >> 26) & 0x3f]
    ^ SB3[(v0 >> 18) & 0x3f]
    ^ SB5[(v0 >> 10) & 0x3f]
    ^ SB7[(v0 >> 2) & 0x3f];
  v1 = key[1];
  v1 ^= (right << 3) | (right >> 29);
  left ^= SB2[(v1 >> 26) & 0x3f]
    ^ SB4[(v1 >> 18) & 0x3f]
    ^ SB6[(v1 >> 10) & 0x3f]
    ^ SB8[(v1 >> 2) & 0x3f];
  v0 = key[2];
  v0 ^= (left >> 1) | (left << 31);
  right ^= SB1[(v0 >> 26) & 0x3f]
    ^ SB3[(v0 >> 18) & 0x3f]
    ^ SB5[(v0 >> 10) & 0x3f]
    ^ SB7[(v0 >> 2) & 0x3f];
  v1 = key[3];
  v1 ^= (left << 3) | (left >> 29);
  right ^= SB2[(v1 >> 26) & 0x3f]
    ^ SB4[(v1 >> 18) & 0x3f]
    ^ SB6[(v1 >> 10) & 0x3f]
    ^ SB8[(v1 >> 2) & 0x3f];
  v0 = key[4];
  v0 ^= (right >> 1) | (right << 31);
  left ^= SB1[(v0 >> 26) & 0x3f]
    ^ SB3[(v0 >> 18) & 0x3f]
    ^ SB5[(v0 >> 10) & 0x3f]
    ^ SB7[(v0 >> 2) & 0x3f];
  v1 = key[5];
  v1 ^= (right << 3) | (right >> 29);
  left ^= SB2[(v1 >> 26) & 0x3f]
    ^ SB4[(v1 >> 18) & 0x3f]
    ^ SB6[(v1 >> 10) & 0x3f]
    ^ SB8[(v1 >> 2) & 0x3f];
  v0 = key[6];
  v0 ^= (left >> 1) | (left << 31);
  right ^= SB1[(v0 >> 26) & 0x3f]
    ^ SB3[(v0 >> 18) & 0x3f]
    ^ SB5[(v0 >> 10) & 0x3f]
    ^ SB7[(v0 >> 2) & 0x3f];
  v1 = key[7];
  v1 ^= (left << 3) | (left >> 29);
  right ^= SB2[(v1 >> 26) & 0x3f]
    ^ SB4[(v1 >> 18) & 0x3f]
    ^ SB6[(v1 >> 10) & 0x3f]
    ^ SB8[(v1 >> 2) & 0x3f];
  v0 = key[8];
  v0 ^= (right >> 1) | (right << 31);
  left ^= SB1[(v0 >> 26) & 0x3f]
    ^ SB3[(v0 >> 18) & 0x3f]
    ^ SB5[(v0 >> 10) & 0x3f]
    ^ SB7[(v0 >> 2) & 0x3f];
  v1 = key[9];
  v1 ^= (right << 3) | (right >> 29);
  left ^= SB2[(v1 >> 26) & 0x3f]
    ^ SB4[(v1 >> 18) & 0x3f]
    ^ SB6[(v1 >> 10) & 0x3f]
    ^ SB8[(v1 >> 2) & 0x3f];
  v0 = key[10];
  v0 ^= (left >> 1) | (left << 31);
  right ^= SB1[(v0 >> 26) & 0x3f]
    ^ SB3[(v0 >> 18) & 0x3f]
    ^ SB5[(v0 >> 10) & 0x3f]
    ^ SB7[(v0 >> 2) & 0x3f];
  v1 = key[11];
  v1 ^= (left << 3) | (left >> 29);
  right ^= SB2[(v1 >> 26) & 0x3f]
    ^ SB4[(v1 >> 18) & 0x3f]
    ^ SB6[(v1 >> 10) & 0x3f]
    ^ SB8[(v1 >> 2) & 0x3f];
  v0 = key[12];
  v0 ^= (right >> 1) | (right << 31);
  left ^= SB1[(v0 >> 26) & 0x3f]
    ^ SB3[(v0 >> 18) & 0x3f]
    ^ SB5[(v0 >> 10) & 0x3f]
    ^ SB7[(v0 >> 2) & 0x3f];
  v1 = key[13];
  v1 ^= (right << 3) | (right >> 29);
  left ^= SB2[(v1 >> 26) & 0x3f]
    ^ SB4[(v1 >> 18) & 0x3f]
    ^ SB6[(v1 >> 10) & 0x3f]
    ^ SB8[(v1 >> 2) & 0x3f];
  v0 = key[14];
  v0 ^= (left >> 1) | (left << 31);
  right ^= SB1[(v0 >> 26) & 0x3f]
    ^ SB3[(v0 >> 18) & 0x3f]
    ^ SB5[(v0 >> 10) & 0x3f]
    ^ SB7[(v0 >> 2) & 0x3f];
  v1 = key[15];
  v1 ^= (left << 3) | (left >> 29);
  right ^= SB2[(v1 >> 26) & 0x3f]
    ^ SB4[(v1 >> 18) & 0x3f]
    ^ SB6[(v1 >> 10) & 0x3f]
    ^ SB8[(v1 >> 2) & 0x3f];
  v0 = key[16];
  v0 ^= (right >> 1) | (right << 31);
  left ^= SB1[(v0 >> 26) & 0x3f]
    ^ SB3[(v0 >> 18) & 0x3f]
    ^ SB5[(v0 >> 10) & 0x3f]
    ^ SB7[(v0 >> 2) & 0x3f];
  v1 = key[17];
  v1 ^= (right << 3) | (right >> 29);
  left ^= SB2[(v1 >> 26) & 0x3f]
    ^ SB4[(v1 >> 18) & 0x3f]
    ^ SB6[(v1 >> 10) & 0x3f]
    ^ SB8[(v1 >> 2) & 0x3f];
  v0 = key[18];
  v0 ^= (left >> 1) | (left << 31);
  right ^= SB1[(v0 >> 26) & 0x3f]
    ^ SB3[(v0 >> 18) & 0x3f]
    ^ SB5[(v0 >> 10) & 0x3f]
    ^ SB7[(v0 >> 2) & 0x3f];
  v1 = key[19];
  v1 ^= (left << 3) | (left >> 29);
  right ^= SB2[(v1 >> 26) & 0x3f]
    ^ SB4[(v1 >> 18) & 0x3f]
    ^ SB6[(v1 >> 10) & 0x3f]
    ^ SB8[(v1 >> 2) & 0x3f];
  v0 = key[20];
  v0 ^= (right >> 1) | (right << 31);
  left ^= SB1[(v0 >> 26) & 0x3f]
    ^ SB3[(v0 >> 18) & 0x3f]
    ^ SB5[(v0 >> 10) & 0x3f]
    ^ SB7[(v0 >> 2) & 0x3f];
  v1 = key[21];
  v1 ^= (right << 3) | (right >> 29);
  left ^= SB2[(v1 >> 26) & 0x3f]
    ^ SB4[(v1 >> 18) & 0x3f]
    ^ SB6[(v1 >> 10) & 0x3f]
    ^ SB8[(v1 >> 2) & 0x3f];
  v0 = key[22];
  v0 ^= (left >> 1) | (left << 31);
  right ^= SB1[(v0 >> 26) & 0x3f]
    ^ SB3[(v0 >> 18) & 0x3f]
    ^ SB5[(v0 >> 10) & 0x3f]
    ^ SB7[(v0 >> 2) & 0x3f];
  v1 = key[23];
  v1 ^= (left << 3) | (left >> 29);
  right ^= SB2[(v1 >> 26) & 0x3f]
    ^ SB4[(v1 >> 18) & 0x3f]
    ^ SB6[(v1 >> 10) & 0x3f]
    ^ SB8[(v1 >> 2) & 0x3f];
  v0 = left;
  v1 = right;
  C[7] = v0;
  C[6] = v0 >> 8;
  C[5] = v0 >> 16;
  C[4] = v0 >> 24;
  C[3] = v1;
  C[2] = v1 >> 8;
  C[1] = v1 >> 16;
  C[0] = v1 >> 24;
}
void print_8byte(byte* P) {
  for(int i = 0; i < 8; i++)
    printf("%02X ", P[i]);
  printf("\n");
}
// text와 text_new의 diff는 v / 19 60 00 00
int DES_isRightPath(unsigned int* key, byte* text, byte* text_new)
{
  unsigned int right, left, v0, v1;
  unsigned int right_new, left_new, v0_new, v1_new;
  left = text[3] | ((unsigned int)text[2] << 8) | ((unsigned int)text[1] << 16) | ((unsigned int)text[0] << 24);
  right = text[7] | ((unsigned int)text[6] << 8) | ((unsigned int)text[5] << 16) | ((unsigned int)text[4] << 24);
  left_new = text_new[3] | ((unsigned int)text_new[2] << 8) | ((unsigned int)text_new[1] << 16) | ((unsigned int)text_new[0] << 24);
  right_new = text_new[7] | ((unsigned int)text_new[6] << 8) | ((unsigned int)text_new[5] << 16) | ((unsigned int)text_new[4] << 24);
  v0 = key[0];
  v0_new = key[0];
  v0 ^= (right >> 1) | (right << 31);
  v0_new ^= (right_new >> 1) | (right_new << 31);
  left ^= SB1[(v0 >> 26) & 0x3f]
    ^ SB3[(v0 >> 18) & 0x3f]
    ^ SB5[(v0 >> 10) & 0x3f]
    ^ SB7[(v0 >> 2) & 0x3f];
  left_new ^= SB1[(v0_new >> 26) & 0x3f]
    ^ SB3[(v0_new >> 18) & 0x3f]
    ^ SB5[(v0_new >> 10) & 0x3f]
    ^ SB7[(v0_new >> 2) & 0x3f];
  v1 = key[1];
  v1_new = key[1];
  v1 ^= (right << 3) | (right >> 29);
  v1_new ^= (right_new << 3) | (right_new >> 29);
  left ^= SB2[(v1 >> 26) & 0x3f]
    ^ SB4[(v1 >> 18) & 0x3f]
    ^ SB6[(v1 >> 10) & 0x3f]
    ^ SB8[(v1 >> 2) & 0x3f];
  left_new ^= SB2[(v1_new >> 26) & 0x3f]
    ^ SB4[(v1_new >> 18) & 0x3f]
    ^ SB6[(v1_new >> 10) & 0x3f]
    ^ SB8[(v1_new >> 2) & 0x3f];
  // 1R left diff check
  if (left != left_new)
    return false;
  v0 = key[2];
  v0 ^= (left >> 1) | (left << 31);
  right ^= SB1[(v0 >> 26) & 0x3f]
    ^ SB3[(v0 >> 18) & 0x3f]
    ^ SB5[(v0 >> 10) & 0x3f]
    ^ SB7[(v0 >> 2) & 0x3f];
  v1 = key[3];
  v1 ^= (left << 3) | (left >> 29);
  right ^= SB2[(v1 >> 26) & 0x3f]
    ^ SB4[(v1 >> 18) & 0x3f]
    ^ SB6[(v1 >> 10) & 0x3f]
    ^ SB8[(v1 >> 2) & 0x3f];
  v0_new = key[2];
  v0_new ^= (left_new >> 1) | (left_new << 31);
  right_new ^= SB1[(v0_new >> 26) & 0x3f]
    ^ SB3[(v0_new >> 18) & 0x3f]
    ^ SB5[(v0_new >> 10) & 0x3f]
    ^ SB7[(v0_new >> 2) & 0x3f];
  v1_new = key[3];
  v1_new ^= (left_new << 3) | (left_new >> 29);
  right_new ^= SB2[(v1_new >> 26) & 0x3f]
    ^ SB4[(v1_new >> 18) & 0x3f]
    ^ SB6[(v1_new >> 10) & 0x3f]
    ^ SB8[(v1_new >> 2) & 0x3f];

  v0 = key[4];
  v0_new = key[4];
  v0 ^= (right >> 1) | (right << 31);
  v0_new ^= (right_new >> 1) | (right_new << 31);
  left ^= SB1[(v0 >> 26) & 0x3f]
    ^ SB3[(v0 >> 18) & 0x3f]
    ^ SB5[(v0 >> 10) & 0x3f]
    ^ SB7[(v0 >> 2) & 0x3f];
  left_new ^= SB1[(v0_new >> 26) & 0x3f]
    ^ SB3[(v0_new >> 18) & 0x3f]
    ^ SB5[(v0_new >> 10) & 0x3f]
    ^ SB7[(v0_new >> 2) & 0x3f];
  v1 = key[5];
  v1_new = key[5];
  v1 ^= (right << 3) | (right >> 29);
  v1_new ^= (right_new << 3) | (right_new >> 29);
  left ^= SB2[(v1 >> 26) & 0x3f]
    ^ SB4[(v1 >> 18) & 0x3f]
    ^ SB6[(v1 >> 10) & 0x3f]
    ^ SB8[(v1 >> 2) & 0x3f];
  left_new ^= SB2[(v1_new >> 26) & 0x3f]
    ^ SB4[(v1_new >> 18) & 0x3f]
    ^ SB6[(v1_new >> 10) & 0x3f]
    ^ SB8[(v1_new >> 2) & 0x3f];
  // 3R 19 60 check
  if (left != left_new)
    return 4;
  v0 = key[6];
  v0 ^= (left >> 1) | (left << 31);
  right ^= SB1[(v0 >> 26) & 0x3f]
    ^ SB3[(v0 >> 18) & 0x3f]
    ^ SB5[(v0 >> 10) & 0x3f]
    ^ SB7[(v0 >> 2) & 0x3f];
  v1 = key[7];
  v1 ^= (left << 3) | (left >> 29);
  right ^= SB2[(v1 >> 26) & 0x3f]
    ^ SB4[(v1 >> 18) & 0x3f]
    ^ SB6[(v1 >> 10) & 0x3f]
    ^ SB8[(v1 >> 2) & 0x3f];
  v0_new = key[6];
  v0_new ^= (left_new >> 1) | (left_new << 31);
  right_new ^= SB1[(v0_new >> 26) & 0x3f]
    ^ SB3[(v0_new >> 18) & 0x3f]
    ^ SB5[(v0_new >> 10) & 0x3f]
    ^ SB7[(v0_new >> 2) & 0x3f];
  v1_new = key[7];
  v1_new ^= (left_new << 3) | (left_new >> 29);
  right_new ^= SB2[(v1_new >> 26) & 0x3f]
    ^ SB4[(v1_new >> 18) & 0x3f]
    ^ SB6[(v1_new >> 10) & 0x3f]
    ^ SB8[(v1_new >> 2) & 0x3f];

  v0 = key[8];
  v0_new = key[8];
  v0 ^= (right >> 1) | (right << 31);
  v0_new ^= (right_new >> 1) | (right_new << 31);
  left ^= SB1[(v0 >> 26) & 0x3f]
    ^ SB3[(v0 >> 18) & 0x3f]
    ^ SB5[(v0 >> 10) & 0x3f]
    ^ SB7[(v0 >> 2) & 0x3f];
  left_new ^= SB1[(v0_new >> 26) & 0x3f]
    ^ SB3[(v0_new >> 18) & 0x3f]
    ^ SB5[(v0_new >> 10) & 0x3f]
    ^ SB7[(v0_new >> 2) & 0x3f];
  v1 = key[9];
  v1_new = key[9];
  v1 ^= (right << 3) | (right >> 29);
  v1_new ^= (right_new << 3) | (right_new >> 29);
  left ^= SB2[(v1 >> 26) & 0x3f]
    ^ SB4[(v1 >> 18) & 0x3f]
    ^ SB6[(v1 >> 10) & 0x3f]
    ^ SB8[(v1 >> 2) & 0x3f];
  left_new ^= SB2[(v1_new >> 26) & 0x3f]
    ^ SB4[(v1_new >> 18) & 0x3f]
    ^ SB6[(v1_new >> 10) & 0x3f]
    ^ SB8[(v1_new >> 2) & 0x3f];
  // 5R 19 60 check
  if (left != left_new)
    return 6;
  v0 = key[10];
  v0 ^= (left >> 1) | (left << 31);
  right ^= SB1[(v0 >> 26) & 0x3f]
    ^ SB3[(v0 >> 18) & 0x3f]
    ^ SB5[(v0 >> 10) & 0x3f]
    ^ SB7[(v0 >> 2) & 0x3f];
  v1 = key[11];
  v1 ^= (left << 3) | (left >> 29);
  right ^= SB2[(v1 >> 26) & 0x3f]
    ^ SB4[(v1 >> 18) & 0x3f]
    ^ SB6[(v1 >> 10) & 0x3f]
    ^ SB8[(v1 >> 2) & 0x3f];
  v0_new = key[10];
  v0_new ^= (left_new >> 1) | (left_new << 31);
  right_new ^= SB1[(v0_new >> 26) & 0x3f]
    ^ SB3[(v0_new >> 18) & 0x3f]
    ^ SB5[(v0_new >> 10) & 0x3f]
    ^ SB7[(v0_new >> 2) & 0x3f];
  v1_new = key[11];
  v1_new ^= (left_new << 3) | (left_new >> 29);
  right_new ^= SB2[(v1_new >> 26) & 0x3f]
    ^ SB4[(v1_new >> 18) & 0x3f]
    ^ SB6[(v1_new >> 10) & 0x3f]
    ^ SB8[(v1_new >> 2) & 0x3f];

  v0 = key[12];
  v0_new = key[12];
  v0 ^= (right >> 1) | (right << 31);
  v0_new ^= (right_new >> 1) | (right_new << 31);
  left ^= SB1[(v0 >> 26) & 0x3f]
    ^ SB3[(v0 >> 18) & 0x3f]
    ^ SB5[(v0 >> 10) & 0x3f]
    ^ SB7[(v0 >> 2) & 0x3f];
  left_new ^= SB1[(v0_new >> 26) & 0x3f]
    ^ SB3[(v0_new >> 18) & 0x3f]
    ^ SB5[(v0_new >> 10) & 0x3f]
    ^ SB7[(v0_new >> 2) & 0x3f];
  v1 = key[13];
  v1_new = key[13];
  v1 ^= (right << 3) | (right >> 29);
  v1_new ^= (right_new << 3) | (right_new >> 29);
  left ^= SB2[(v1 >> 26) & 0x3f]
    ^ SB4[(v1 >> 18) & 0x3f]
    ^ SB6[(v1 >> 10) & 0x3f]
    ^ SB8[(v1 >> 2) & 0x3f];
  left_new ^= SB2[(v1_new >> 26) & 0x3f]
    ^ SB4[(v1_new >> 18) & 0x3f]
    ^ SB6[(v1_new >> 10) & 0x3f]
    ^ SB8[(v1_new >> 2) & 0x3f];
  // 7R 19 60 check
  if (left != left_new)
    return 8; // 8Round 가능
  v0 = key[14];
  v0 ^= (left >> 1) | (left << 31);
  right ^= SB1[(v0 >> 26) & 0x3f]
    ^ SB3[(v0 >> 18) & 0x3f]
    ^ SB5[(v0 >> 10) & 0x3f]
    ^ SB7[(v0 >> 2) & 0x3f];
  v1 = key[15];
  v1 ^= (left << 3) | (left >> 29);
  right ^= SB2[(v1 >> 26) & 0x3f]
    ^ SB4[(v1 >> 18) & 0x3f]
    ^ SB6[(v1 >> 10) & 0x3f]
    ^ SB8[(v1 >> 2) & 0x3f];
  v0_new = key[14];
  v0_new ^= (left_new >> 1) | (left_new << 31);
  right_new ^= SB1[(v0_new >> 26) & 0x3f]
    ^ SB3[(v0_new >> 18) & 0x3f]
    ^ SB5[(v0_new >> 10) & 0x3f]
    ^ SB7[(v0_new >> 2) & 0x3f];
  v1_new = key[15];
  v1_new ^= (left_new << 3) | (left_new >> 29);
  right_new ^= SB2[(v1_new >> 26) & 0x3f]
    ^ SB4[(v1_new >> 18) & 0x3f]
    ^ SB6[(v1_new >> 10) & 0x3f]
    ^ SB8[(v1_new >> 2) & 0x3f];

  v0 = key[16];
  v0_new = key[16];
  v0 ^= (right >> 1) | (right << 31);
  v0_new ^= (right_new >> 1) | (right_new << 31);
  left ^= SB1[(v0 >> 26) & 0x3f]
    ^ SB3[(v0 >> 18) & 0x3f]
    ^ SB5[(v0 >> 10) & 0x3f]
    ^ SB7[(v0 >> 2) & 0x3f];
  left_new ^= SB1[(v0_new >> 26) & 0x3f]
    ^ SB3[(v0_new >> 18) & 0x3f]
    ^ SB5[(v0_new >> 10) & 0x3f]
    ^ SB7[(v0_new >> 2) & 0x3f];
  v1 = key[17];
  v1_new = key[17];
  v1 ^= (right << 3) | (right >> 29);
  v1_new ^= (right_new << 3) | (right_new >> 29);
  left ^= SB2[(v1 >> 26) & 0x3f]
    ^ SB4[(v1 >> 18) & 0x3f]
    ^ SB6[(v1 >> 10) & 0x3f]
    ^ SB8[(v1 >> 2) & 0x3f];
  left_new ^= SB2[(v1_new >> 26) & 0x3f]
    ^ SB4[(v1_new >> 18) & 0x3f]
    ^ SB6[(v1_new >> 10) & 0x3f]
    ^ SB8[(v1_new >> 2) & 0x3f];
  // 9R 19 60 check
  if (left != left_new) {
    printf("P1 : ");
    print_8byte(text);
    printf("P2 : ");
    print_8byte(text_new);
    printf("C1 : %08X %08X\nC2 : %08X %08X\n", right, left, right_new, left_new);
    return 10; // 10Round 가능
  }
  v0 = key[18];
  v0 ^= (left >> 1) | (left << 31);
  right ^= SB1[(v0 >> 26) & 0x3f]
    ^ SB3[(v0 >> 18) & 0x3f]
    ^ SB5[(v0 >> 10) & 0x3f]
    ^ SB7[(v0 >> 2) & 0x3f];
  v1 = key[19];
  v1 ^= (left << 3) | (left >> 29);
  right ^= SB2[(v1 >> 26) & 0x3f]
    ^ SB4[(v1 >> 18) & 0x3f]
    ^ SB6[(v1 >> 10) & 0x3f]
    ^ SB8[(v1 >> 2) & 0x3f];
  v0_new = key[18];
  v0_new ^= (left_new >> 1) | (left_new << 31);
  right_new ^= SB1[(v0_new >> 26) & 0x3f]
    ^ SB3[(v0_new >> 18) & 0x3f]
    ^ SB5[(v0_new >> 10) & 0x3f]
    ^ SB7[(v0_new >> 2) & 0x3f];
  v1_new = key[19];
  v1_new ^= (left_new << 3) | (left_new >> 29);
  right_new ^= SB2[(v1_new >> 26) & 0x3f]
    ^ SB4[(v1_new >> 18) & 0x3f]
    ^ SB6[(v1_new >> 10) & 0x3f]
    ^ SB8[(v1_new >> 2) & 0x3f];
  v0 = key[20];
  v0_new = key[20];
  v0 ^= (right >> 1) | (right << 31);
  v0_new ^= (right_new >> 1) | (right_new << 31);
  left ^= SB1[(v0 >> 26) & 0x3f]
    ^ SB3[(v0 >> 18) & 0x3f]
    ^ SB5[(v0 >> 10) & 0x3f]
    ^ SB7[(v0 >> 2) & 0x3f];
  left_new ^= SB1[(v0_new >> 26) & 0x3f]
    ^ SB3[(v0_new >> 18) & 0x3f]
    ^ SB5[(v0_new >> 10) & 0x3f]
    ^ SB7[(v0_new >> 2) & 0x3f];
  v1 = key[21];
  v1_new = key[21];
  v1 ^= (right << 3) | (right >> 29);
  v1_new ^= (right_new << 3) | (right_new >> 29);
  left ^= SB2[(v1 >> 26) & 0x3f]
    ^ SB4[(v1 >> 18) & 0x3f]
    ^ SB6[(v1 >> 10) & 0x3f]
    ^ SB8[(v1 >> 2) & 0x3f];
  left_new ^= SB2[(v1_new >> 26) & 0x3f]
    ^ SB4[(v1_new >> 18) & 0x3f]
    ^ SB6[(v1_new >> 10) & 0x3f]
    ^ SB8[(v1_new >> 2) & 0x3f];
  //11R r', 19 60 check
//  if ((right ^ right_new) != 0x19600000)
//    return false;
  v0 = key[22];
  v0 ^= (left >> 1) | (left << 31);
  right ^= SB1[(v0 >> 26) & 0x3f]
    ^ SB3[(v0 >> 18) & 0x3f]
    ^ SB5[(v0 >> 10) & 0x3f]
    ^ SB7[(v0 >> 2) & 0x3f];
  v1 = key[23];
  v1 ^= (left << 3) | (left >> 29);
  right ^= SB2[(v1 >> 26) & 0x3f]
    ^ SB4[(v1 >> 18) & 0x3f]
    ^ SB6[(v1 >> 10) & 0x3f]
    ^ SB8[(v1 >> 2) & 0x3f];
  v0_new = key[22];
  v0_new ^= (left_new >> 1) | (left_new << 31);
  right_new ^= SB1[(v0_new >> 26) & 0x3f]
    ^ SB3[(v0_new >> 18) & 0x3f]
    ^ SB5[(v0_new >> 10) & 0x3f]
    ^ SB7[(v0_new >> 2) & 0x3f];
  v1_new = key[23];
  v1_new ^= (left_new << 3) | (left_new >> 29);
  right_new ^= SB2[(v1_new >> 26) & 0x3f]
    ^ SB4[(v1_new >> 18) & 0x3f]
    ^ SB6[(v1_new >> 10) & 0x3f]
    ^ SB8[(v1_new >> 2) & 0x3f];
  printf("P1 : ");
  print_8byte(text);
  printf("P2 : ");
  print_8byte(text_new);
  printf("C1 : %08X %08X\nC2 : %08X %08X\n", right, left, right_new, left_new);
  v0 = left;
  v1 = right;
  return 12;
}

/*
* key compression permutation, 4 bits at a time
*/
static unsigned int comptab[] = {

  0x000000,0x010000,0x000008,0x010008,0x000080,0x010080,0x000088,0x010088,
  0x000000,0x010000,0x000008,0x010008,0x000080,0x010080,0x000088,0x010088,

  0x000000,0x100000,0x000800,0x100800,0x000000,0x100000,0x000800,0x100800,
  0x002000,0x102000,0x002800,0x102800,0x002000,0x102000,0x002800,0x102800,

  0x000000,0x000004,0x000400,0x000404,0x000000,0x000004,0x000400,0x000404,
  0x400000,0x400004,0x400400,0x400404,0x400000,0x400004,0x400400,0x400404,

  0x000000,0x000020,0x008000,0x008020,0x800000,0x800020,0x808000,0x808020,
  0x000002,0x000022,0x008002,0x008022,0x800002,0x800022,0x808002,0x808022,

  0x000000,0x000200,0x200000,0x200200,0x001000,0x001200,0x201000,0x201200,
  0x000000,0x000200,0x200000,0x200200,0x001000,0x001200,0x201000,0x201200,

  0x000000,0x000040,0x000010,0x000050,0x004000,0x004040,0x004010,0x004050,
  0x040000,0x040040,0x040010,0x040050,0x044000,0x044040,0x044010,0x044050,

  0x000000,0x000100,0x020000,0x020100,0x000001,0x000101,0x020001,0x020101,
  0x080000,0x080100,0x0a0000,0x0a0100,0x080001,0x080101,0x0a0001,0x0a0101,

  0x000000,0x000100,0x040000,0x040100,0x000000,0x000100,0x040000,0x040100,
  0x000040,0x000140,0x040040,0x040140,0x000040,0x000140,0x040040,0x040140,

  0x000000,0x400000,0x008000,0x408000,0x000008,0x400008,0x008008,0x408008,
  0x000400,0x400400,0x008400,0x408400,0x000408,0x400408,0x008408,0x408408,

  0x000000,0x001000,0x080000,0x081000,0x000020,0x001020,0x080020,0x081020,
  0x004000,0x005000,0x084000,0x085000,0x004020,0x005020,0x084020,0x085020,

  0x000000,0x000800,0x000000,0x000800,0x000010,0x000810,0x000010,0x000810,
  0x800000,0x800800,0x800000,0x800800,0x800010,0x800810,0x800010,0x800810,

  0x000000,0x010000,0x000200,0x010200,0x000000,0x010000,0x000200,0x010200,
  0x100000,0x110000,0x100200,0x110200,0x100000,0x110000,0x100200,0x110200,

  0x000000,0x000004,0x000000,0x000004,0x000080,0x000084,0x000080,0x000084,
  0x002000,0x002004,0x002000,0x002004,0x002080,0x002084,0x002080,0x002084,

  0x000000,0x000001,0x200000,0x200001,0x020000,0x020001,0x220000,0x220001,
  0x000002,0x000003,0x200002,0x200003,0x020002,0x020003,0x220002,0x220003,
};

static int keysh[] =
{
  1, 1, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 1,
};

static void
keycompperm(unsigned int left, unsigned int right, unsigned int *ek)
{
  unsigned int v0, v1;
  int i;

  for (i = 0; i < 16; i++) {
    left = (left << keysh[i]) | (left >> (28 - keysh[i]));
    left &= 0xfffffff0;
    right = (right << keysh[i]) | (right >> (28 - keysh[i]));
    right &= 0xfffffff0;
    v0 = comptab[6 * (1 << 4) + ((left >> (32 - 4)) & 0xf)]
      | comptab[5 * (1 << 4) + ((left >> (32 - 8)) & 0xf)]
      | comptab[4 * (1 << 4) + ((left >> (32 - 12)) & 0xf)]
      | comptab[3 * (1 << 4) + ((left >> (32 - 16)) & 0xf)]
      | comptab[2 * (1 << 4) + ((left >> (32 - 20)) & 0xf)]
      | comptab[1 * (1 << 4) + ((left >> (32 - 24)) & 0xf)]
      | comptab[0 * (1 << 4) + ((left >> (32 - 28)) & 0xf)];
    v1 = comptab[13 * (1 << 4) + ((right >> (32 - 4)) & 0xf)]
      | comptab[12 * (1 << 4) + ((right >> (32 - 8)) & 0xf)]
      | comptab[11 * (1 << 4) + ((right >> (32 - 12)) & 0xf)]
      | comptab[10 * (1 << 4) + ((right >> (32 - 16)) & 0xf)]
      | comptab[9 * (1 << 4) + ((right >> (32 - 20)) & 0xf)]
      | comptab[8 * (1 << 4) + ((right >> (32 - 24)) & 0xf)]
      | comptab[7 * (1 << 4) + ((right >> (32 - 28)) & 0xf)];
    ek[0] = (((v0 >> (24 - 6)) & 0x3f) << 26)
      | (((v0 >> (24 - 18)) & 0x3f) << 18)
      | (((v1 >> (24 - 6)) & 0x3f) << 10)
      | (((v1 >> (24 - 18)) & 0x3f) << 2);
    ek[1] = (((v0 >> (24 - 12)) & 0x3f) << 26)
      | (((v0 >> (24 - 24)) & 0x3f) << 18)
      | (((v1 >> (24 - 12)) & 0x3f) << 10)
      | (((v1 >> (24 - 24)) & 0x3f) << 2);
    ek += 2;
  }
}

void des_key_setup(byte key[8], unsigned int *ek)
{
  unsigned int left, right, v0, v1;

  v0 = key[0] | ((unsigned int)key[2] << 8) | ((unsigned int)key[4] << 16) | ((unsigned int)key[6] << 24);
  v1 = key[1] | ((unsigned int)key[3] << 8) | ((unsigned int)key[5] << 16) | ((unsigned int)key[7] << 24);
  left = ((v0 >> 1) & 0x40404040)
    | ((v0 >> 2) & 0x10101010)
    | ((v0 >> 3) & 0x04040404)
    | ((v0 >> 4) & 0x01010101)
    | ((v1 >> 0) & 0x80808080)
    | ((v1 >> 1) & 0x20202020)
    | ((v1 >> 2) & 0x08080808)
    | ((v1 >> 3) & 0x02020202);
  right = ((v0 >> 1) & 0x04040404)
    | ((v0 << 2) & 0x10101010)
    | ((v0 << 5) & 0x40404040)
    | ((v1 << 0) & 0x08080808)
    | ((v1 << 3) & 0x20202020)
    | ((v1 << 6) & 0x80808080);
  left = ((left << 6) & 0x33003300)
    | (left & 0xcc33cc33)
    | ((left >> 6) & 0x00cc00cc);
  v0 = ((left << 12) & 0x0f0f0000)
    | (left & 0xf0f00f0f)
    | ((left >> 12) & 0x0000f0f0);
  right = ((right << 6) & 0x33003300)
    | (right & 0xcc33cc33)
    | ((right >> 6) & 0x00cc00cc);
  v1 = ((right << 12) & 0x0f0f0000)
    | (right & 0xf0f00f0f)
    | ((right >> 12) & 0x0000f0f0);
  left = v0 & 0xfffffff0;
  right = (v1 & 0xffffff00) | ((v0 << 4) & 0xf0);

  keycompperm(left, right, ek);
}
void setRandomPlain(byte* P) {
  random_device rd;
  mt19937 rng(rd());
  uniform_int_distribution<int> dist1(0, 0xFF);
  for (int i = 0; i < 8; i++)
    P[i] = dist1(rng);
}
void setRandomKey(byte* K) {
  random_device rd;
  mt19937 rng(rd());
  uniform_int_distribution<int> dist1(0, 0xFF);
  for (int i = 0; i < 8; i++)
    K[i] = dist1(rng);
}

void setPlainPool(byte* P_original, byte* P, int i) {
  // i의 31-20번째 bit가 P의 (1, 5), (8, 12, 15), (16, 17, 22, 23), (27, 29, 30) 번째 bit에 들어가면 됨.
  for (int i = 0; i < 8; i++)
    P[i] = P_original[i];
  P[0] ^= ((i & 1) << 6); i >>= 1; // i의 마지막 bit를 P의 1번째 bit에 XOR하고 i의 마지막 bit를 제거
  P[0] ^= ((i & 1) << 2); i >>= 1; // P의 5번째 bit  
  P[1] ^= ((i & 1) << 7); i >>= 1; // P의 8번째 bit
  P[1] ^= ((i & 1) << 3); i >>= 1; // P의 12번째 bit
  P[1] ^= (i & 1); i >>= 1; // P의 15번째 bit
  P[2] ^= ((i & 1) << 7); i >>= 1; // P의 16번째 bit
  P[2] ^= ((i & 1) << 6); i >>= 1; // P의 17번째 bit
  P[2] ^= ((i & 1) << 1); i >>= 1; // P의 22번째 bit
  P[2] ^= (i & 1); i >>= 1; // P의 23번째 bit
  P[3] ^= ((i & 1) << 4); i >>= 1; // P의 27번째 bit
  P[3] ^= ((i & 1) << 2); i >>= 1; // P의 29번째 bit
  P[3] ^= ((i & 1) << 1); // P의 30번째 bit
}
int getSboxInputBit(byte Intermediate[8], int idx, int dir) {
  byte I[4] = { Intermediate[0 + 4 * dir], Intermediate[1 + 4 * dir], Intermediate[2 + 4 * dir], Intermediate[3 + 4 * dir] };
  switch (idx) {
  case 1: return ((I[3] & 0x01) << 5) | ((I[0] & 0xf8) >> 3);
  case 2: return ((I[0] & 0x1f) << 1) | ((I[1] & 0x80) >> 7);
  case 3: return ((I[0] & 0x01) << 5) | ((I[1] & 0xf8) >> 3);
  case 4: return ((I[1] & 0x1f) << 1) | ((I[2] & 0x80) >> 7);
  case 5: return ((I[1] & 0x01) << 5) | ((I[2] & 0xf8) >> 3);
  case 6: return ((I[2] & 0x1f) << 1) | ((I[3] & 0x80) >> 7);
  case 7: return ((I[2] & 0x01) << 5) | ((I[3] & 0xf8) >> 3);
  case 8: return ((I[3] & 0x1f) << 1) | ((I[0] & 0x80) >> 7);
  default: return -1; // unreachable
  }
  return -1; // unreachable
}
int** getSurvivingPair(byte P[][8], byte P_bar[][8], byte T[][8], byte T_bar[][8], int POOL_SIZE, int MAX_PAIR) { // surviving pair의 index를 반환
  int** pairlist = (int**)malloc(sizeof(int*) * MAX_PAIR);
  for (int i = 0; i < MAX_PAIR; i++)
    pairlist[i] = (int*)malloc(sizeof(int) * 2);
  for (int i = 0; i < MAX_PAIR; i++) {
    pairlist[i][0] = -1;
    pairlist[i][1] = -1;
  }
  // S1, S2, S3의 output bit (8, 16, 22, 30), (12, 27, 1, 17), (23, 15, 29, 5)을 0으로 만든 32비트 값을 기준으로 정렬해 collision을 찾아낼 예정
  // T, T_bar의 right를 조작하면 됨.
  vector<pair<int, int>> V(POOL_SIZE * 2); // (32비트 값, index)
  for (int i = 0; i < POOL_SIZE; i++) {
    // P의 (1, 5), (8, 12, 15), (16, 17, 22, 23), (27, 29, 30) 번째 bit를 0으로 만들어야함
    // 10111011(BB) 01110110(76) 00111100(3C) 11101001(E9)
    V[i] = { (((T[i][4] << 24) | (T[i][5] << 16) | (T[i][6] << 8) | T[i][7])) & 0xBB763CE9, i };
    V[i + POOL_SIZE] = { (((T_bar[i][4] << 24) | (T_bar[i][5] << 16) | (T_bar[i][6] << 8) | T_bar[i][7])) & 0xBB763CE9, i + POOL_SIZE };
  }
  sort(V.begin(), V.end());
  int pairlist_idx = 0; // 다음번에 삽입할 pairlist의 index
  for (int i = 0; i < 2 * POOL_SIZE - 1; i++) { // V[i], V[i+1]의 first가 동일한지 확인할 예정. first가 동일한게 3개 이상이면 모든 경우를 포함하지 못할 수 있지만 너무나 빈약한 확률로 등장할 일이기 때문에 그냥 무시
    if (V[i].first == V[i + 1].first && V[i].second < POOL_SIZE && V[i + 1].second >= POOL_SIZE) { // V[i]는 T에 속해있고 V[i+1]은 T_bar에 속해있어야하므로 추가된 조건
                                                     // 이제 R1,11,12에서의 S-box의 차분을 살펴보아 실제로 가능한 차분인지 확인. 이 때 확률에 0.0745가 곱해지면서 1.19개의 pair만 살아남음
                                                     // 1. R1에서 S1,S2,S3의 차분이 실제로 가능한 차분인지 diff_table로 확인
                                                     // R1의 input 차분은 00011001(19) 01100000(60) 00000000 00000000 
                                                     // S1의 input 차분 : 000011
                                                     // S2의 input 차분 : 110010
                                                     // S3의 input 차분 : 101100
      int idx0 = V[i].second; // P에 대응되는 index
      int idx1 = V[i + 1].second - POOL_SIZE; // P_bar에 대응되는 index
      byte P_diff[8];
      byte C_diff[8];
      for (int i = 0; i < 8; i++) {
        P_diff[i] = P[idx0][i] ^ P_bar[idx1][i];
        C_diff[i] = T[idx0][i] ^ T_bar[idx1][i];
      }
      if (diff_table[0][0x03][getSboxOutputBit(P_diff, 1, 0)] == 0) // S1의 0x03에 대해 P_diff가 불가능한 차분일 경우
        continue;
      if (diff_table[1][0x32][getSboxOutputBit(P_diff, 2, 0)] == 0) // S2의 0x32에 대해 P_diff가 불가능한 차분일 경우
        continue;
      if (diff_table[2][0x2c][getSboxOutputBit(P_diff, 3, 0)] == 0) // S3의 0x2c에 대해 P_diff가 불가능한 차분일 경우
        continue;
      // 2. R11에서 S1, S2, S3의 차분이 실제로 가능한 차분인지 diff_table로 확인. R15에서 S1, S2, S3의 output 차분은 Cipher의 right
      if (diff_table[0][0x03][getSboxOutputBit(C_diff, 1, 1)] == 0) // S1의 0x03에 대해 P_diff가 불가능한 차분일 경우
        continue;
      if (diff_table[1][0x32][getSboxOutputBit(C_diff, 2, 1)] == 0) // S2의 0x32에 대해 P_diff가 불가능한 차분일 경우
        continue;
      if (diff_table[2][0x2c][getSboxOutputBit(C_diff, 3, 1)] == 0) // S3의 0x2c에 대해 P_diff가 불가능한 차분일 경우
        continue;
      // 3. R12에서 S1-S8의 차분이 실제로 가능한 차분인지 diff_table로 확인
      // C_diff[0~3]에 19(0001 1001) 60(0110 0000) 00 00을 XOR한 것이 F 함수의 출력이다.
      C_diff[0] ^= 0x19;
      C_diff[1] ^= 0x60;
      
      if (diff_table[0][getSboxInputBit(C_diff, 1, 1)][getSboxOutputBit(C_diff, 1, 0)] == 0) // S1의 input에 대해 P_diff가 불가능한 차분일 경우
        continue;
      if (diff_table[1][getSboxInputBit(C_diff, 2, 1)][getSboxOutputBit(C_diff, 2, 0)] == 0)
        continue;
      if (diff_table[2][getSboxInputBit(C_diff, 3, 1)][getSboxOutputBit(C_diff, 3, 0)] == 0)
        continue;
      if (diff_table[3][getSboxInputBit(C_diff, 4, 1)][getSboxOutputBit(C_diff, 4, 0)] == 0)
        continue;     
      if (diff_table[4][getSboxInputBit(C_diff, 5, 1)][getSboxOutputBit(C_diff, 5, 0)] == 0)
        continue;
      if (diff_table[5][getSboxInputBit(C_diff, 6, 1)][getSboxOutputBit(C_diff, 6, 0)] == 0)
        continue;
      if (diff_table[6][getSboxInputBit(C_diff, 7, 1)][getSboxOutputBit(C_diff, 7, 0)] == 0)
        continue;
      if (diff_table[7][getSboxInputBit(C_diff, 8, 1)][getSboxOutputBit(C_diff, 8, 0)] == 0)
        continue;
      // 이 모든 filtering을 통과했다면
      pairlist[pairlist_idx][0] = idx0;
      pairlist[pairlist_idx][1] = idx1;
      pairlist_idx++; // pairlist에 추가
      if (pairlist_idx == MAX_PAIR) // 만약 MAX_PAIR개에 도달했으면
        return pairlist; // 그냥 반환
    }
  }
  return pairlist;
}

void setPlainBar(byte* P, byte* P_bar) { // 19 60 00 00을 P의 right에 XOR
  for (int i = 0; i < 8; i++)
    P_bar[i] = P[i];
  P_bar[4] = P[4] ^ 0x19;
  P_bar[5] = P[5] ^ 0x60;
}
int main(void)
{
  setDiffTable();
  byte KEY[8]; // 키를 지정
  setRandomKey(KEY);
  byte C[8];
  des_key_setup(KEY, key_int);
  printf("KEY : ");
  print_8byte(KEY);
  long long int cnt = 0;
  while (cnt >= 0) {
    cnt++;
    if (cnt % 1000000 == 0)
      printf("%lldK try\n", cnt / 1000000);
    const int POOL_SIZE = 4096;
    byte P_arbitrary[8];
    setRandomPlain(P_arbitrary); // 임의로 P_arbitrary를 택함
                   // S1, S2, S3의 output bit : (15,6,19,20), (28,11,27,16), (0,14,22,25)
                   // (0,6), (11,14,15), (16,19,20,22), (25,27,28)
    byte P[POOL_SIZE][8]; // P_arbitrary에서 S1, S2, S3의 output bit를 0x000부터 0xFFF까지 바꾼 Plain이 들어감
    byte P_bar[POOL_SIZE][8]; // P[i]에 (00 00 00 00 19 60 00 00)을 XOR한 Plain
    for (int i = 0; i < POOL_SIZE; i++) {
      setPlainPool(P_arbitrary, P[i], i);
      setPlainBar(P[i], P_bar[i]);
    }
    byte T[POOL_SIZE][8];
    byte T_bar[POOL_SIZE][8];
    for (int i = 0; i < POOL_SIZE; i++) {
      DES(key_int, P[i], T[i]);
      DES(key_int, P_bar[i], T_bar[i]);
    }
    const int MAX_PAIR = 50;
    int** survivePair = getSurvivingPair(P, P_bar, T, T_bar, POOL_SIZE, MAX_PAIR); // (idx0, idx1) pair가 반환됨
    for (int i = 0; i < MAX_PAIR; i++) {
      int idx0 = survivePair[i][0];
      int idx1 = survivePair[i][1];
      if (idx0 == -1)
        break;
      int roundnum = DES_isRightPath(key_int, P[idx0], P_bar[idx1]);
      if (roundnum >= 4)
        printf("%dR\n", roundnum);
      else if (roundnum == 12)
        printf("12R!!!\n");
    }
    for (int i = 0; i < MAX_PAIR; i++)
      free(survivePair[i]);
    free(survivePair);
  }
}
