#include "header.h"

static const uint32_t K[64] = {
0xd76aa478,0xe8c7b756,0x242070db,0xc1bdceee,0xf57c0faf,0x4787c62a,0xa8304613,0xfd469501,
0x698098d8,0x8b44f7af,0xffff5bb1,0x895cd7be,0x6b901122,0xfd987193,0xa679438e,0x49b40821,
0xf61e2562,0xc040b340,0x265e5a51,0xe9b6c7aa,0xd62f105d,0x02441453,0xd8a1e681,0xe7d3fbc8,
0x21e1cde6,0xc33707d6,0xf4d50d87,0x455a14ed,0xa9e3e905,0xfcefa3f8,0x676f02d9,0x8d2a4c8a,
0xfffa3942,0x8771f681,0x6d9d6122,0xfde5380c,0xa4beea44,0x4bdecfa9,0xf6bb4b60,0xbebfbc70,
0x289b7ec6,0xeaa127fa,0xd4ef3085,0x04881d05,0xd9d4d039,0xe6db99e5,0x1fa27cf8,0xc4ac5665,
0xf4292244,0x432aff97,0xab9423a7,0xfc93a039,0x655b59c3,0x8f0ccc92,0xffeff47d,0x85845dd1,
0x6fa87e4f,0xfe2ce6e0,0xa3014314,0x4e0811a1,0xf7537e82,0xbd3af235,0x2ad7d2bb,0xeb86d391};

static const int S[64] = {
7,12,17,22,7,12,17,22,7,12,17,22,7,12,17,22,
5,9,14,20,5,9,14,20,5,9,14,20,5,9,14,20,
4,11,16,23,4,11,16,23,4,11,16,23,4,11,16,23,
6,10,15,21,6,10,15,21,6,10,15,21,6,10,15,21};

#define ROTL(x, c) (((x) << (c)) | ((x) >> (32 - (c))))

/* processes one 64-byte block */
static void md5_block(uint32_t st[4], const uint8_t blk[64])
{
	uint32_t M[16], a = st[0], b = st[1], c = st[2], d = st[3];

	for (int i = 0; i < 16; i++)
		M[i] = blk[i*4] | (blk[i*4+1] << 8) | (blk[i*4+2] << 16) | ((uint32_t)blk[i*4+3] << 24);
	for (int i = 0; i < 64; i++)
	{
		uint32_t f;
		int g;
		if (i < 16)      { f = (b & c) | (~b & d); g = i; }
		else if (i < 32) { f = (d & b) | (~d & c); g = (5*i + 1) % 16; }
		else if (i < 48) { f = b ^ c ^ d;          g = (3*i + 5) % 16; }
		else             { f = c ^ (b | ~d);       g = (7*i) % 16; }
		f = f + a + K[i] + M[g];
		a = d; d = c; c = b;
		b = b + ROTL(f, S[i]);
	}
	st[0] += a; st[1] += b; st[2] += c; st[3] += d;
}

/* MD5 of a string -> 32 lowercase hex chars + '\0' */
void md5_string(const char *input, char output_hex[33])
{
	uint32_t st[4] = {0x67452301, 0xefcdab89, 0x98badcfe, 0x10325476};
	size_t len = strlen(input), i, rem;
	uint8_t blk[64];
	uint64_t bits;

	for (i = 0; i + 64 <= len; i += 64)
		md5_block(st, (const uint8_t *)input + i);
	rem = len - i;
	memset(blk, 0, 64);
	memcpy(blk, input + i, rem);
	blk[rem] = 0x80;
	if (rem >= 56)
	{
		md5_block(st, blk);
		memset(blk, 0, 64);
	}
	bits = (uint64_t)len * 8;
	for (int k = 0; k < 8; k++)
		blk[56 + k] = (uint8_t)(bits >> (8 * k));
	md5_block(st, blk);
	for (int w = 0; w < 4; w++)
		for (int k = 0; k < 4; k++)
			sprintf(output_hex + (w*4 + k) * 2, "%02x", (st[w] >> (8*k)) & 0xff);
	output_hex[32] = '\0';
}
