#include <stdio.h>
#include <windows.h>
#include <time.h>
#include <random>
#include <stdlib.h>
#include <algorithm>
void DES(unsigned int* key, byte* text, byte* C);
void des_key_setup(byte* key, unsigned int* ek);