#include "DES.h"
using namespace std;
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
int PC1[56] = { 56, 48, 40, 32, 24, 16, 8, 0, 57, 49, 41, 33, 25, 17, 9, 1, 58, 50, 42, 34, 26, 18, 10, 2, 59, 51, 43, 35, 62, 54, 46, 38, 30, 22, 14, 6, 61, 53, 45, 37, 29, 21, 13, 5, 60, 52, 44, 36, 28, 20, 12, 4, 27, 19, 11, 3 };
byte CipherOfNULLPlain[8]; // 내가 택한 key가 맞았는지 확인하기 위해 이 값을 참조할 예정
int diff_table[8][64][16];

// diff table을 갱신함
void setDiffTable(void) {
	for (int idx = 0; idx < 8; idx++)
		for (int i = 0; i < 64; i++)
			for (int j = 0; j < 64; j++)
				diff_table[idx][i^j][SBOX[idx][i] ^ SBOX[idx][j]]++;
}
void setRandomKey(byte* KEY) {
	random_device rd;
	mt19937 rng(rd());
	uniform_int_distribution<int> dist1(0, 0xFF);
	for (int i = 0; i < 8; i++)
	      KEY[i] = dist1(rng) & 0xFE; // 어차피 각 바이트의 마지막 비트는 parity이므로 0으로 만듬
}
void setRandomPlain(byte* P) {
	random_device rd;
	mt19937 rng(rd());
	uniform_int_distribution<int> dist1(0, 0xFF);
	for (int i = 0; i < 8; i++)
		P[i] = dist1(rng);
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
void setPlainBar(byte* P, byte* P_bar) { // 19 60 00 00을 P의 right에 XOR
	for (int i = 0; i < 8; i++)
		P_bar[i] = P[i];
	P_bar[4] = P[4] ^ 0x19;
	P_bar[5] = P[5] ^ 0x60;
}
void print_8byte(byte* P) {
	for (int i = 0; i < 8; i++)
		printf("%02X ", P[i]);
	printf("\n");
}

// F 함수의 출력 O에 대해 idx번째 S-box의 출력 결과를 반환 (idx=1 to 8)
// dir = 0 : left 4byte(0 to 3), dir = 1 : right 4byte(4 to 7)
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
// F 함수의 입력 I에 대해 idx번째 S-box의 출력 결과를 반환 (idx=1 to 8)
// dir = 0 : left 4byte(0 to 3), dir = 1 : right 4byte(4 to 7)
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
int ExtractBit() {
	return 0;
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


int DC_structure(unsigned int* key) {
	int cnt = 0;
	const int POOL_SIZE = 4096;
	byte P_arbitrary[8] = { 0x2C, 0x2F, 0x45, 0xE3, 0x26, 0xBC, 0xF9, 0xA1};
	//setRandomPlain(P_arbitrary); // 임의로 P_arbitrary를 택함
	byte P[POOL_SIZE][8]; // P_arbitrary에서 S1, S2, S3의 output bit를 0x000부터 0xFFF까지 바꾼 Plain이 들어감
	byte P_bar[POOL_SIZE][8]; // P[i]에 (00 00 00 00 19 60 00 00)을 XOR한 Plain
	for (int i = 0; i < POOL_SIZE; i++) {
		setPlainPool(P_arbitrary, P[i], i);
		setPlainBar(P[i], P_bar[i]);
	}
	byte T[POOL_SIZE][8];
	byte T_bar[POOL_SIZE][8];
	for (int i = 0; i < POOL_SIZE; i++) {
		DES(key, P[i], T[i]);
		DES(key, P_bar[i], T_bar[i]);
	}
	// S4-S8의 output bit의 차분이 0이어야 right key일 가능성이 있음
	const int MAX_PAIR = 10;

	int** survivePair = getSurvivingPair(P, P_bar, T, T_bar, POOL_SIZE, MAX_PAIR); // (idx0, idx1) pair가 반환됨
	for (int i = 0; i < MAX_PAIR; i++) {
		int idx0 = survivePair[i][0];
		int idx1 = survivePair[i][1];
		if (idx0 == -1)
			break;
		// R1의 input 차분은 00011001(19) 01100000(60) 00000000 00000000 
		// S1의 input 차분 : 000011
		// S2의 input 차분 : 110010
		// S3의 input 차분 : 101100
		// 해당하는 pair에 대응되는 key를 찾아 right key인지 확인할 것임
		bool keybit[56] = { 0, }; // PC1을 거친 상태의 키
								  // Round1 S3 부터 시작. (23 19 12 4 26 8)

		for (int r1s3 = 0; r1s3 < 64; r1s3++) {

			keybit[23] = (r1s3 & 0x20) >> 5;
			keybit[19] = (r1s3 & 0x10) >> 4;
			keybit[12] = (r1s3 & 0x08) >> 3;
			keybit[4] = (r1s3 & 0x04) >> 2;
			keybit[26] = (r1s3 & 0x02) >> 1;
			keybit[8] = r1s3 & 0x01;
			int R1S3_key = r1s3;
			int R1S3_input1 = getSboxInputBit(P[idx0], 3, 1);
			int R1S3_input2 = getSboxInputBit(P_bar[idx1], 3, 1);
			int R1S3_output1 = SBOX[2][R1S3_input1 ^ R1S3_key];
			int R1S3_output2 = SBOX[2][R1S3_input2 ^ R1S3_key];
			int R1S3_output_XOR = R1S3_output1 ^ R1S3_output2;
			int expected_R1S3_output_XOR = getSboxOutputBit(P[idx0], 3, 0) ^ getSboxOutputBit(P_bar[idx1], 3, 0);
			if (R1S3_output_XOR != expected_R1S3_output_XOR)
				continue;
			// output XOR이 일치한다면 이제 Round12 S4로 넘어감.(8 27 19 12 5 22) 27 5 22을 정해야함
			for (int r12s4 = 0; r12s4 < 8; r12s4++) {
				keybit[27] = (r12s4 & 0x04) >> 2;
				keybit[5] = (r12s4 & 0x02) >> 1;
				keybit[22] = r12s4 & 0x01;
				int R12S4_key = (int)keybit[8] << 5 | (int)keybit[27] << 4 | (int)keybit[19] << 3 | (int)keybit[12] << 2 | (int)keybit[5] << 1 | (int)keybit[22];
				int R12S4_input1 = getSboxInputBit(T[idx0], 4, 1);
				int R12S4_input2 = getSboxInputBit(T_bar[idx1], 4, 1);
				int R12S4_output1 = SBOX[3][R12S4_input1 ^ R12S4_key];
				int R12S4_output2 = SBOX[3][R12S4_input2 ^ R12S4_key];
				int R12S4_output_XOR = SBOX[3][R12S4_input1 ^ R12S4_key] ^ SBOX[3][R12S4_input2 ^ R12S4_key];
				int expected_R12S4_output_XOR = getSboxOutputBit(T[idx0], 4, 0) ^ getSboxOutputBit(T_bar[idx1], 4, 0) ^ 0x2; // (19 60 00 00이 XOR되어서 출력 diff의 9번째 MSB = S4의 2번째 MSB가 달라지게 됨
				if (R12S4_output_XOR != expected_R12S4_output_XOR)			
					continue;				
				//ROUND12 S2(23 20 7 26 13 2) 20 7 13 2를 정해야함
				for (int r12s2 = 0; r12s2 < 16; r12s2++) {
					keybit[20] = (r12s2 & 0x08) >> 3;
					keybit[7] = (r12s2 & 0x04) >> 2;
					keybit[13] = (r12s2 & 0x02) >> 1;
					keybit[2] = r12s2 & 0x01;
					int R12S2_key = (int)keybit[23] << 5 | (int)keybit[20] << 4 | (int)keybit[7] << 3 | (int)keybit[26] << 2 | (int)keybit[13] << 1 | (int)keybit[2];
					int R12S2_input1 = getSboxInputBit(T[idx0], 2, 1);
					int R12S2_input2 = getSboxInputBit(T_bar[idx1], 2, 1);
					int R12S2_output1 = SBOX[1][R12S2_input1 ^ R12S2_key];
					int R12S2_output2 = SBOX[1][R12S2_input2 ^ R12S2_key];
					int R12S2_output_XOR = R12S2_output1 ^ R12S2_output2;
					int expected_R12S2_output_XOR = getSboxOutputBit(T[idx0], 2, 0) ^ getSboxOutputBit(T_bar[idx1], 2, 0);
					if (R12S2_output_XOR != expected_R12S2_output_XOR)
						continue;

					//ROUND1 S2(3 0 15 6 21 10). 모든 비트를 다 정해야됨
					for (int r1s2 = 0; r1s2 < 64; r1s2++) {
						keybit[3] = (r1s2 & 0x20) >> 5;
						keybit[0] = (r1s2 & 0x10) >> 4;
						keybit[15] = (r1s2 & 0x08) >> 3;
						keybit[6] = (r1s2 & 0x04) >> 2;
						keybit[21] = (r1s2 & 0x02) >> 1;
						keybit[10] = r1s2 & 0x01;
						int R1S2_key = r1s2;
						int R1S2_input1 = getSboxInputBit(P[idx0], 2, 1);
						int R1S2_input2 = getSboxInputBit(P_bar[idx1], 2, 1);
						int R1S2_output1 = SBOX[1][R1S2_input1 ^ R1S2_key];
						int R1S2_output2 = SBOX[1][R1S2_input2 ^ R1S2_key];
						int R1S2_output_XOR = R1S2_output1 ^ R1S2_output2;
						int expected_R1S2_output_XOR = getSboxOutputBit(P[idx0], 2, 0) ^ getSboxOutputBit(P_bar[idx1], 2, 0);
						if (R1S2_output_XOR != expected_R1S2_output_XOR)
							continue;
						for (int r12s1 = 0; r12s1 < 8; r12s1++) {
							keybit[9] = (r12s1 & 0x04) >> 2;
							keybit[16] = (r12s1 & 0x02) >> 1;
							keybit[25] = r12s1 & 0x01;
							int R12S1_key = (int)keybit[6] << 5 | (int)keybit[9] << 4 | (int)keybit[3] << 3 | (int)keybit[16] << 2 | (int)keybit[21] << 1 | (int)keybit[25];
							int R12S1_input1 = getSboxInputBit(T[idx0], 1, 1);
							int R12S1_input2 = getSboxInputBit(T_bar[idx1], 1, 1);
							int R12S1_output1 = SBOX[0][R12S1_input1 ^ R12S1_key];
							int R12S1_output2 = SBOX[0][R12S1_input2 ^ R12S1_key];
							int R12S1_output_XOR = R12S1_output1 ^ R12S1_output2;
							int expected_R12S1_output_XOR = getSboxOutputBit(T[idx0], 1, 0) ^ getSboxOutputBit(T_bar[idx1], 1, 0);
							if (R12S1_output_XOR != expected_R12S1_output_XOR)
								continue;
							//ROUND12 S3(15 11 4 24 18 0), 11 24 18을 정해야됨
							for (int r12s3 = 0; r12s3 < 8; r12s3++) {
								keybit[11] = (r12s3 & 0x04) >> 2;
								keybit[24] = (r12s3 & 0x02) >> 1;
								keybit[18] = r12s3 & 0x01;
								int R12S3_key = (int)keybit[15] << 5 | (int)keybit[11] << 4 | (int)keybit[4] << 3 | (int)keybit[24] << 2 | (int)keybit[18] << 1 | (int)keybit[0];
								int R12S3_input1 = getSboxInputBit(T[idx0], 3, 1);
								int R12S3_input2 = getSboxInputBit(T_bar[idx1], 3, 1);
								int R12S3_output1 = SBOX[2][R12S3_input1 ^ R12S3_key];
								int R12S3_output2 = SBOX[2][R12S3_input2 ^ R12S3_key];
								int R12S3_output_XOR = R12S3_output1 ^ R12S3_output2;
								int expected_R12S3_output_XOR = getSboxOutputBit(T[idx0], 3, 0) ^ getSboxOutputBit(T_bar[idx1], 3, 0);
								if (R12S3_output_XOR != expected_R12S3_output_XOR)
									continue;

								//ROUND1 S1(14 17 11 24 1 5), 14 17 1을 정해야됨
								for (int r1s1 = 0; r1s1 < 8; r1s1++) {
									keybit[14] = (r1s1 & 0x04) >> 2;
									keybit[17] = (r1s1 & 0x02) >> 1;
									keybit[1] = r1s1 & 0x01;
									int R1S1_key = (int)keybit[14] << 5 | (int)keybit[17] << 4 | (int)keybit[11] << 3 | (int)keybit[24] << 2 | (int)keybit[1] << 1 | (int)keybit[5];
									int R1S1_input1 = getSboxInputBit(P[idx0], 1, 1);
									int R1S1_input2 = getSboxInputBit(P_bar[idx1], 1, 1);
									int R1S1_output1 = SBOX[0][R1S1_input1 ^ R1S1_key];
									int R1S1_output2 = SBOX[0][R1S1_input2 ^ R1S1_key];
									int R1S1_output_XOR = R1S1_output1 ^ R1S1_output2;
									int expected_R1S1_output_XOR = getSboxOutputBit(P[idx0], 1, 0) ^ getSboxOutputBit(P_bar[idx1], 1, 0);
									if (R1S1_output_XOR != expected_R1S1_output_XOR)
										continue;

									// 여기까지 통과했으면 일단 expected XOR은 다 제대로 나옴. left key는 다 복원한 상태.
									// 이제 right key를 찾아낼 것임
									// R11S1_input1 ^ R11S1_input2 = 0x03
									// 내가 P[idx0]에 대한 input value를 정하면 P_bar[idx1]에 대한 input value또한 정해지고, S1-S4에 대한 key는 아니까 output value가 기대한 XOR값이 나오는지 확인 가능
									// Permutation S1 : 24 15 6 19 20 28, 24 19 20 28 값을 임의로 정해야 함
									bool I12_input1[32] = { 0, };
									bool I12_input2[32] = { 0, };
									for (int r11s1 = 0; r11s1 < 16; r11s1++) {
										int R11S1_key = (int)keybit[4] << 5 | (int)keybit[7] << 4 | (int)keybit[1] << 3 | (int)keybit[14] << 2 | (int)keybit[19] << 1 | (int)keybit[23];
										I12_input1[24] = (r11s1 & 0x08) >> 3;
										int i2 = R12S4_output1 & 0x01;
										int i3 = (R12S2_output1 & 0x02) >> 1;
										I12_input1[19] = (r11s1 & 0x04) >> 2;
										I12_input1[20] = (r11s1 & 0x02) >> 1;
										I12_input1[28] = r11s1 & 0x01;
										int R11S1_input1 = ((int)I12_input1[24] << 5 | i2 << 4 | i3 << 3 | (int)I12_input1[19] << 2 | (int)I12_input1[20] << 1 | (int)I12_input1[28]) ^ getSboxInputBit(T[idx0], 1, 0);
										int R11S1_input2 = R11S1_input1 ^ 0x03;
										int R11S1_output1 = SBOX[0][R11S1_input1 ^ R11S1_key];
										int R11S1_output2 = SBOX[0][R11S1_input2 ^ R11S1_key];
										int R11S1_output_XOR = R11S1_output1 ^ R11S1_output2;
										int expected_R11S1_output_XOR = getSboxOutputBit(T[idx0], 1, 1) ^ getSboxOutputBit(T_bar[idx1], 1, 1);
										if (R11S1_output_XOR != expected_R11S1_output_XOR)
											continue;
										int t = R11S1_input2 ^ getSboxInputBit(T_bar[idx1], 1, 0);
										I12_input2[24] = (t & 0x20) >> 5;
										I12_input2[19] = (t & 0x04) >> 2;
										I12_input2[20] = (t & 0x02) >> 1;
										I12_input2[28] = t & 0x01;
										// Round11 S2에서 input XOR은 0x32이다.
										// Permutation S2 : 20 28 11 27 16 0, 27 16 값을 임의로 정해야 함
										for (int r11s2 = 0; r11s2 < 4; r11s2++) {
											int R11S2_key = (int)keybit[21] << 5 | (int)keybit[18] << 4 | (int)keybit[5] << 3 | (int)keybit[24] << 2 | (int)keybit[11] << 1 | (int)keybit[0];
											int i3 = R12S3_output1 & 0x01;
											I12_input1[27] = (r11s2 & 0x02) >> 1;
											I12_input1[16] = r11s2 & 0x01;
											int i6 = (R12S1_output1 & 0x08) >> 3;
											int R11S2_input1 = ((int)I12_input1[20] << 5 | (int)I12_input1[28] << 4 | i3 << 3 | (int)I12_input1[27] << 2 | (int)I12_input1[16] << 1 | i6 ) ^ getSboxInputBit(T[idx0], 2, 0);
											int R11S2_input2 = R11S2_input1 ^ 0x32;
											int R11S2_output1 = SBOX[1][R11S2_input1 ^ R11S2_key];
											int R11S2_output2 = SBOX[1][R11S2_input2 ^ R11S2_key];
											int R11S2_output_XOR = R11S2_output1 ^ R11S2_output2;
											int expected_R11S2_output_XOR = getSboxOutputBit(T[idx0], 2, 1) ^ getSboxOutputBit(T_bar[idx1], 2, 1);
											if (R11S2_output_XOR != expected_R11S2_output_XOR)
												continue;
											int t = R11S2_input2 ^ getSboxInputBit(T_bar[idx1], 2, 0);
											I12_input2[27] = (t & 0x04) >> 2;
											I12_input2[16] = (t & 0x02) >> 1;
											// Round11 S3에서 input XOR은 0x2c이다.
											// Permutation S3 : 16 0 14 22 25 4, 22 25 값을 임의로 정해야 함
											for (int r11s3 = 0; r11s3 < 4; r11s3++) {
												int R11S3_key = (int)keybit[13] << 5 | (int)keybit[9] << 4 | (int)keybit[2] << 3 | (int)keybit[22] << 2 | (int)keybit[16] << 1 | (int)keybit[26];
												int i2 = (R12S1_output1 & 0x08) >> 3;
												int i3 = (R12S4_output1 & 0x02) >> 1;
												I12_input1[22] = (r11s3 & 0x02) >> 1;
												I12_input1[25] = r11s3 & 0x01;
												int i6 = (R12S2_output1 & 0x08) >> 3;
												int R11S3_input1 = ((int)I12_input1[16] << 5 | i2 << 4 | i3 << 3 | (int)I12_input1[22] << 2 | (int)I12_input1[25] << 1 | i6 ) ^ getSboxInputBit(T[idx0], 3, 0);
												int R11S3_input2 = R11S3_input1 ^ 0x2c;
												int R11S3_output1 = SBOX[2][R11S3_input1 ^ R11S3_key];
												int R11S3_output2 = SBOX[2][R11S3_input2 ^ R11S3_key];
												int R11S3_output_XOR = R11S3_output1 ^ R11S3_output2;
												int expected_R11S3_output_XOR = getSboxOutputBit(T[idx0], 3, 1) ^ getSboxOutputBit(T_bar[idx1], 3, 1);
												if (R11S3_output_XOR != expected_R11S3_output_XOR)
													continue;
												int t = R11S3_input2 ^ getSboxInputBit(T_bar[idx1], 3, 0);
												I12_input2[22] = (t & 0x04) >> 2;
												I12_input2[25] = (t & 0x02) >> 1;

												// Round12 S5에서 keybit(33 44 51 29 39 47)을 택하고 output value를 충족하는지 확인
												// output bit는 16 17 18 19이고 16 19는 값을 알고있음
												for (int r12s5 = 0; r12s5 < 4; r12s5++) {
													keybit[31] = (r12s5 & 0x20) >> 5;
													keybit[44] = (r12s5 & 0x10) >> 4;
													keybit[51] = (r12s5 & 0x08) >> 3;
													keybit[29] = (r12s5 & 0x04) >> 2;
													keybit[39] = (r12s5 & 0x02) >> 1;
													keybit[47] = r12s5 & 0x01;
													int R12S5_key = r12s5;
													int R12S5_input1 = getSboxInputBit(T[idx0], 5, 1);
													int R12S5_input2 = getSboxInputBit(T_bar[idx1], 5, 1);
													if ((SBOX[4][R12S5_input1 ^ R12S5_key] & 0x09) != ((int)I12_input1[16] << 3 | (int)I12_input1[19])) // 가운데 두 bit는 unknown이므로.
														continue;
													if ((SBOX[4][R12S5_input2 ^ R12S5_key] & 0x09) != ((int)I12_input2[16] << 3 | (int)I12_input2[19])) // 가운데 두 bit는 unknown이므로.
														continue;
													int t = SBOX[4][R12S5_input1 ^ R12S5_key];
													I12_input1[17] = (t & 0x04) >> 2;
													I12_input1[18] = (t & 0x02) >> 1;
													t = SBOX[4][R12S5_input2 ^ R12S5_key];
													I12_input2[17] = (t & 0x04) >> 2;
													I12_input2[18] = (t & 0x02) >> 1;

													// Round12 S6에서 keybit(50 32 43 37 53 40)을 택하고 output value를 충족하는지 확인
													// output bit는 20 21 22 23이고 20 22은 값을 알고있음 
													for (int r12s6 = 0; r12s6 < 64; r12s6++) {
														keybit[50] = (r12s6 & 0x20) >> 5;
														keybit[32] = (r12s6 & 0x10) >> 4;
														keybit[43] = (r12s6 & 0x08) >> 3;
														keybit[37] = (r12s6 & 0x04) >> 2;
														keybit[53] = (r12s6 & 0x02) >> 1;
														keybit[40] = r12s6 & 0x01;
														int R12S6_key = r12s6;
														int R12S6_input1 = getSboxInputBit(T[idx0], 6, 1);
														int R12S6_input2 = getSboxInputBit(T_bar[idx1], 6, 1);
														if ((SBOX[5][R12S6_input1 ^ R12S6_key] & 0x0A) != ((int)I12_input1[20] << 3 | (int)I12_input1[22] << 1)) // 가운데 두 bit는 unknown이므로.
															continue;
														if ((SBOX[5][R12S6_input2 ^ R12S6_key] & 0x0A) != ((int)I12_input2[20] << 3 | (int)I12_input2[22] << 1)) // 가운데 두 bit는 unknown이므로.
															continue;
														int t = SBOX[5][R12S6_input1 ^ R12S6_key];
														I12_input1[21] = (t & 0x04) >> 2;
														I12_input1[23] = t & 0x01;
														t = SBOX[5][R12S6_input2 ^ R12S6_key];
														I12_input2[21] = (t & 0x04) >> 2;
														I12_input2[23] = t & 0x01;
														// Round12 S7에서 keybit(36 41 31 48 54 45)을 택하고 output value를 충족하는지 확인
														// output bit는 24 25 26 27이고 24 25 27은 값을 알고있음 
														for (int r12s7 = 0; r12s7 < 64; r12s7++) {
															keybit[36] = (r12s7 & 0x20) >> 5;
															keybit[41] = (r12s7 & 0x10) >> 4;
															keybit[31] = (r12s7 & 0x08) >> 3;
															keybit[48] = (r12s7 & 0x04) >> 2;
															keybit[54] = (r12s7 & 0x02) >> 1;
															keybit[45] = r12s7 & 0x01;
															int R12S7_key = r12s7;
															int R12S7_input1 = getSboxInputBit(T[idx0], 7, 1);
															int R12S7_input2 = getSboxInputBit(T_bar[idx1], 7, 1);
															if ((SBOX[6][R12S7_input1 ^ R12S7_key] & 0x0D) != ((int)I12_input1[24] << 3 | (int)I12_input1[25] << 2 | (int)I12_input1[27])) // 가운데 한 bit는 unknown이므로.
																continue;
															if ((SBOX[6][R12S7_input2 ^ R12S7_key] & 0x0D) != ((int)I12_input2[24] << 3 | (int)I12_input2[25] << 2 | (int)I12_input2[27])) // 가운데 한 bit는 unknown이므로.
																continue;
															int t = SBOX[6][R12S7_input1 ^ R12S7_key];
															I12_input1[26] = (t & 0x02) >> 1;
															t = SBOX[6][R12S7_input2 ^ R12S7_key];
															I12_input2[26] = (t & 0x02) >> 1;
															// Round12 S8에서 keybit(38 34 42 28 49 52)을 택하고 output value를 충족하는지 확인
															// output bit는 28 29 30 31이고 28은 값을 알고있음 
															for (int r12s8 = 0; r12s8 < 64; r12s8++) {
																keybit[38] = (r12s8 & 0x20) >> 5;
																keybit[34] = (r12s8 & 0x10) >> 4;
																keybit[42] = (r12s8 & 0x08) >> 3;
																keybit[28] = (r12s8 & 0x04) >> 2;
																keybit[49] = (r12s8 & 0x02) >> 1;
																keybit[52] = r12s8 & 0x01;
																int R12S8_key = r12s8;
																int R12S8_input1 = getSboxInputBit(T[idx0], 8, 1);
																int R12S8_input2 = getSboxInputBit(T_bar[idx1], 8, 1);
																if ((SBOX[7][R12S8_input1 ^ R12S8_key] & 0x08) != ((int)I12_input1[28] << 3)) // 가운데 한 bit는 unknown이므로.
																	continue;
																if ((SBOX[7][R12S8_input2 ^ R12S8_key] & 0x08) != ((int)I12_input2[28] << 3)) // 가운데 한 bit는 unknown이므로.
																	continue;
																int t = SBOX[7][R12S8_input1 ^ R12S8_key];
																I12_input1[29] = (t & 0x04) >> 2;
																I12_input1[30] = (t & 0x02) >> 1;
																I12_input1[31] = t & 0x01;
																t = SBOX[7][R12S8_input2 ^ R12S8_key];
																I12_input2[29] = (t & 0x04) >> 2;
																I12_input2[30] = (t & 0x02) >> 1;
																I12_input2[31] = t & 0x01;
																int keylist[4] = { R12S1_key, R12S2_key, R12S3_key, R12S4_key };
																for (int i = 0; i < 4; i++) {
																	int t = SBOX[i][getSboxInputBit(T[idx0], i + 1, 1) ^ keylist[i]];
																	I12_input1[4 * i] = (t & 0x08) >> 3;
																	I12_input1[4 * i+1] = (t & 0x04) >> 2;
																	I12_input1[4 * i+2] = (t & 0x02) >> 1;
																	I12_input1[4 * i+3] = t & 0x01;
																}
																for (int i = 0; i < 4; i++) {
																	int t = SBOX[i][getSboxInputBit(T_bar[idx1], i + 1, 1) ^ keylist[i]];
																	I12_input2[4 * i] = (t & 0x08) >> 3;
																	I12_input2[4 * i + 1] = (t & 0x04) >> 2;
																	I12_input2[4 * i + 2] = (t & 0x02) >> 1;
																	I12_input2[4 * i + 3] = t & 0x01;
																}
																// 현재 R12를 통해 I12_input1, I12_input2를 전부 복원했고 key는 30 35 46 55 bit를 제외하고 전부 복원했다.
																// 이제 4bit는 전수조사를 통해 right key인지 알아낼 것이다.
																// right key인지 알아내는 방법은 CPA 환경이므로 그냥 실제로 암호화를 한 번 수행하기만 하면 된다.
																for (int key_brute = 0; key_brute < 16; key_brute++) {
																	keybit[30] = (key_brute & 0x08) >> 3;
																	keybit[35] = (key_brute & 0x04) >> 2;
																	keybit[46] = (key_brute & 0x02) >> 1;
																	keybit[55] = key_brute & 0x01;
																	byte candidateKey[8] = { 0, };
																	bool tempKeybit[64] = { 0, };
																	for(int i = 0; i < 56; i++)
																		tempKeybit[PC1[i]] = keybit[i];
																	for (int i = 0; i < 8; i++) 
																		for (int j = 0; j <= 6; j++) 
																			candidateKey[i] = (candidateKey[i] | tempKeybit[8 * i + j]) << 1;
																	unsigned int candkey[32];
																	des_key_setup(candidateKey, candkey);
																	byte NULLplain[8] = { 0, };
																	byte C[8] = { 0, };
																	DES(candkey, NULLplain, C);
																	bool isRightKey = true;
																	for (int i = 0; i < 8; i++) {
																		if (C[i] != CipherOfNULLPlain[i]) {
																			isRightKey = false;
																			break;
																		}
																	}
																	if (isRightKey) {
																		printf("Congratz!!! suggested key : ");
																		print_8byte(candidateKey);
																		printf("Plain1 : ");
																		print_8byte(P[idx0]);
																		printf("Plain2 : ");
																		print_8byte(P_bar[idx1]);
																		return true;
																	}
																}
															}
														}
													}
												}											
											} 
										}
									}
								}
							} 
						}
					}
				}
			}
		}
	}
	for (int i = 0; i < MAX_PAIR; i++)
		free(survivePair[i]);
	free(survivePair);
	return false;
}
void printKey(byte* KEY) {
	printf("KEY : ");
	for (int i = 0; i < 8; i++)
		printf("%02X ", KEY[i]);
	printf("\n");
}
int main(void)
{
	printf("Differential Cryptanalysis on DES\n");
	byte KEY[8] = { 0xA2, 0x16, 0x40, 0xC2, 0x4E, 0x18, 0x54, 0x06};
	unsigned int key[32];
	printf("Set random key.. ");
//	setRandomKey(KEY); // CPA 환경에서 이 KEY를 찾아내는 것이 목표임
	printKey(KEY);
	des_key_setup(KEY, key);
	setDiffTable();
	byte NullPlain[8] = { 0, };
	DES(key, NullPlain, CipherOfNULLPlain);
	printf("Cipher of NULL Plaintext : ");
	print_8byte(CipherOfNULLPlain);
	// 10000개의 structure에 400개 pair
	for (int T = 1; ; T++) {
		if(T % 100 == 0)
			printf("=============%d-th structure try==============\n", T);
		if (DC_structure(key)) {
			printf("\n\nfind key in %dth structure try\n", T);
			break;
		}
	}
}