/******************************************************************************************
* @file Md5.c
* @anthor:Croky.Zheng
* @CreateTime:2012/2/21
* @descirption: MD5信息摘要算法
* MD5算法描述:
* 数据填充
* 首先需要对信息进行填充，使其位长对512求余的结果等于448
* 因此，信息的位长（Bits Length）将被扩展至N*512+448，即N*64+56个字节（Bytes），N为一个正整数
* 填充的方法如下，在信息的后面填充一个1和无数个0，直到满足上面的条件时才停止用0对信息的填充
* 然后，在在这个结果后面附加一个以64位二进制表示的填充前信息长度(多少位，而不是字节)
* 经过这两步的处理，现在的信息的位长=N*512+448+64=(N+1)*512，即长度恰好是512的整数倍
* 四轮运算：
* FF(a, b, c, d, Mj, s, ti)表示 a = b + ((a + F(b, c, d) + Mj + ti) << s)
* GG(a, b, c, d, Mj, s, ti)表示 a = b + ((a + G(b, c, d) + Mj + ti) << s)
* HH(a, b, c, d, Mj, s, ti)表示 a = b + ((a + H(b, c, d) + Mj + ti) << s)
* II(a, b, c, d, Mj, s, ti)表示 a = b + ((a + I(b, c, d) + Mj + ti) << s)
* 假设Mj表示消息的第j个子分组（从0到15）512位，16个32位整数
* 在第i步中，ti是4294967296*abs(sin(i))的整数部分，i的单位是弧度。(4294967296等于2的32次方)
* 这个算法的实现，居然默认是在大端机上运行
* 搞得几个数字不一样
* MD5 使用LITTLE-ENDIAN的方式
* MD5("abc")=900150983CD24FB0D6963F7D28E17F72
*****************************************************************************************/
#include "Md5.h"
#include <string.h>

//0x01234567
#define A 0x67452301
//0x89abcdef
#define B 0xefcdab89
//0xfedcba98
#define C 0x98badcfe
//0x76543210
#define D 0x10325476

#define MAX_VALUE 4294967296     //2的32次方的无符号整型
#define S11 7
#define S12 12
#define S13 17
#define S14 22
#define S21 5
#define S22 9
#define S23 14
#define S24 20
#define S31 4
#define S32 11
#define S33 16
#define S34 23
#define S41 6
#define S42 10
#define S43 15
#define S44 21

//这种移位方式有问题吧，如果x是小于0的
//那么可能会补1,所以要求x必须为无符号整型
//这样就同java的 >>> 会补0
//循环左移(n) 雷同汇编rol指令
#define ROTATE_LEFT(x, n) (((x) << (n)) | ((x) >> (32-(n))))

/*
#define F(X,Y,Z) ((X)&(Y))|((~(X))&(Z))
#define G(X,Y,Z) (X&Z)|(Y&(~Z))
#define H(X,Y,Z) ((X)^(Y)^(Z))
#define I(X,Y,Z) ((Y)^((X)|(~(Z))))
*/
#define F(x, y, z) (((x) & (y)) | ((~x) & (z)))
#define G(x, y, z) (((x) & (z)) | ((y) & (~z)))
#define H(x, y, z) ((x) ^ (y) ^ (z))
#define I(x, y, z) ((y) ^ ((x) | (~z)))

//四轮运算
#define FF(a,b,c,d,x,s,ac) {\
 (a) += F((b),(c),(d)) + (x) + (unsigned int)(ac);\
 (a) = ROTATE_LEFT((a),(s));\
 (a) += (b);\
  }

#define GG(a,b,c,d,x,s,ac){ \
 (a) += G((b),(c),(d)) + (x) + (unsigned int)(ac);\
 (a) = ROTATE_LEFT((a),(s));\
 (a) += (b);\
  }

#define HH(a,b,c,d,x,s,ac){\
 (a) += H((b),(c),(d)) + (x) + (unsigned int)(ac);\
 (a) = ROTATE_LEFT((a), (s)); \
 (a) += (b);\
  }

#define II(a,b,c,d,x,s,ac){\
 (a) += I((b),(c),(d)) + (x) + (unsigned int)(ac);\
 (a) = ROTATE_LEFT((a),(s));\
 (a) += (b);\
  }

//将512位64个字节的字节流转为16个32位整型数据
static void Decode(unsigned int* pOutput, 
	unsigned char* pInput, 
	unsigned int nLen);

//将16个32位整型数据转化为字节流
static void Encode(unsigned char* pOutput,
	unsigned int* pInput,
	unsigned int nLen);

//将字节流做四轮加密
static void MD5Transform(unsigned int state[4] , 
	unsigned char block[64]);

//最大填充数据,最后需要再补所有参与计算的长度
static unsigned char PADDING[64] = {
  0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

//static void MD5_memcpy((POINTER, POINTER, unsigned int));
//static void MD5_memset((POINTER, int, unsigned int));

//初始化context
void MD5Init(PMD5Context pContext)
{
	pContext->count[0] = 0;
	pContext->count[1] = 0;
	pContext->state[0] = A;
	pContext->state[1] = B;
	pContext->state[2] = C;
	pContext->state[3] = D;
}

void MD5Update(
	PMD5Context pContext,	/* context */
	unsigned char* pInput,	/* input block */
	unsigned int nLen/* length of input block */)
{
	unsigned int i, index, partLen;

	//长度除于8,其实是每次运算需要512位，64*8
	/* Compute number of bytes mod 64 */
	index = (unsigned int)((pContext->count[0] >> 3) & 0x3f);

	//需要运算的字节量*8
	pContext->count[0] += ((unsigned int)nLen << 3);

	//这是不是表示溢出了？
	/* Update number of bits */
	if ((pContext->count[0]) < ((unsigned int)nLen << 3))
	{
		pContext->count[1]++;
	}

	pContext->count[1] += ((unsigned int)nLen >> 29);

	//需要从新的数据中使用可以用的字符数
	partLen = 64 - index;

	/* Transform as many times as possible.
	*/
	if (nLen >= partLen) {
		// MD5_memcpy((unsigned char *)&pContext->buffer[index], (unsigned char *)pInput, partLen);
		memcpy(&pContext->buffer[index],pInput,partLen);
		MD5Transform(pContext->state, pContext->buffer);

		for (i = partLen; (i + 63) < nLen; i += 64)
		{
			MD5Transform(pContext->state, &pInput[i]);
		}

		index = 0;
	}
	else
	{
		i = 0;
	}

	/* Buffer remaining input */
	//MD5_memcpy((unsigned char *)&pContext->buffer[index], (unsigned char *)&pInput[i],nLen-i);
	memcpy(&pContext->buffer[index],&pInput[i],nLen-i);
}

void MD5Final(
	PMD5Context pContext,/* context */
	unsigned char digest[16]/* message digest */)
{
	unsigned int index, padLen;
	unsigned char bits[8]= {0};

	/* Save number of bits */
	Encode(bits, pContext->count, 8);

	/* Pad out to 56 mod 64.
	*/
	index = (unsigned int)((pContext->count[0] >> 3) & 0x3f);
	padLen = (index < 56) ? (56 - index) : (120 - index);
	//对填充数据做MD5运算
	MD5Update(pContext, PADDING, padLen);

	//填充之前的长度
	MD5Update(pContext, bits, 8);
	/* Store state in digest */
	Encode(digest, pContext->state, 16);

	/* Zeroize sensitive information.*/
	//MD5_memset ((POINTER)context, 0, sizeof (*context));
	memset(pContext,0,sizeof(*pContext));//可以注释，除非打算重用
}

void Decode(unsigned int* pOutput, 
	unsigned char* pInput, 
	unsigned int nLen)
{
	unsigned int i, j;
	for (i = 0, j = 0; j < nLen; i++, j += 4)
	{
		pOutput[i] = ((unsigned int)pInput[j]) | 
			(((unsigned int)pInput[j+1]) << 8) |
			(((unsigned int)pInput[j+2]) << 16) | 
			(((unsigned int)pInput[j+3]) << 24);
	}
}

void Encode(unsigned char* pOutput,
	unsigned int* pInput,
	unsigned int nLen)
{
	unsigned int i, j;
	for (i = 0, j = 0; j < nLen; i++, j += 4) 
	{
		pOutput[j] = (unsigned char)(pInput[i] & 0xff);
		pOutput[j+1] = (unsigned char)((pInput[i] >> 8) & 0xff);
		pOutput[j+2] = (unsigned char)((pInput[i] >> 16) & 0xff);
		pOutput[j+3] = (unsigned char)((pInput[i] >> 24) & 0xff);
	}
}

void MD5Transform(unsigned int state[4] , 
	unsigned char block[64])
{
	unsigned int a = state[0], b = state[1], c = state[2], d = state[3], x[16];
	//整好512位64个字节
	//将64字节的数据块转为16个32位整数
	//MD5的这一步特别浪费速度，但是对于不同字节序确实是个问题
	//而且这里根本没有判断字节序，在非X86上可能就会失效
	Decode (x, block, 64);
	/* Round 1 */
	FF (a, b, c, d, x[ 0], S11, 0xd76aa478); /* 1 */
	FF (d, a, b, c, x[ 1], S12, 0xe8c7b756); /* 2 */
	FF (c, d, a, b, x[ 2], S13, 0x242070db); /* 3 */
	FF (b, c, d, a, x[ 3], S14, 0xc1bdceee); /* 4 */

	FF (a, b, c, d, x[ 4], S11, 0xf57c0faf); /* 5 */
	FF (d, a, b, c, x[ 5], S12, 0x4787c62a); /* 6 */
	FF (c, d, a, b, x[ 6], S13, 0xa8304613); /* 7 */
	FF (b, c, d, a, x[ 7], S14, 0xfd469501); /* 8 */

	FF (a, b, c, d, x[ 8], S11, 0x698098d8); /* 9 */
	FF (d, a, b, c, x[ 9], S12, 0x8b44f7af); /* 10 */
	FF (c, d, a, b, x[10], S13, 0xffff5bb1); /* 11 */
	FF (b, c, d, a, x[11], S14, 0x895cd7be); /* 12 */

	FF (a, b, c, d, x[12], S11, 0x6b901122); /* 13 */
	FF (d, a, b, c, x[13], S12, 0xfd987193); /* 14 */
	FF (c, d, a, b, x[14], S13, 0xa679438e); /* 15 */
	FF (b, c, d, a, x[15], S14, 0x49b40821); /* 16 */

	/* Round 2 */
	GG (a, b, c, d, x[ 1], S21, 0xf61e2562); /* 17 */
	GG (d, a, b, c, x[ 6], S22, 0xc040b340); /* 18 */
	GG (c, d, a, b, x[11], S23, 0x265e5a51); /* 19 */
	GG (b, c, d, a, x[ 0], S24, 0xe9b6c7aa); /* 20 */

	GG (a, b, c, d, x[ 5], S21, 0xd62f105d); /* 21 */
	GG (d, a, b, c, x[10], S22,  0x2441453); /* 22 */
	GG (c, d, a, b, x[15], S23, 0xd8a1e681); /* 23 */
	GG (b, c, d, a, x[ 4], S24, 0xe7d3fbc8); /* 24 */

	GG (a, b, c, d, x[ 9], S21, 0x21e1cde6); /* 25 */
	GG (d, a, b, c, x[14], S22, 0xc33707d6); /* 26 */
	GG (c, d, a, b, x[ 3], S23, 0xf4d50d87); /* 27 */
	GG (b, c, d, a, x[ 8], S24, 0x455a14ed); /* 28 */

	GG (a, b, c, d, x[13], S21, 0xa9e3e905); /* 29 */
	GG (d, a, b, c, x[ 2], S22, 0xfcefa3f8); /* 30 */
	GG (c, d, a, b, x[ 7], S23, 0x676f02d9); /* 31 */
	GG (b, c, d, a, x[12], S24, 0x8d2a4c8a); /* 32 */

	/* Round 3 */
	HH (a, b, c, d, x[ 5], S31, 0xfffa3942); /* 33 */
	HH (d, a, b, c, x[ 8], S32, 0x8771f681); /* 34 */
	HH (c, d, a, b, x[11], S33, 0x6d9d6122); /* 35 */
	HH (b, c, d, a, x[14], S34, 0xfde5380c); /* 36 */

	HH (a, b, c, d, x[ 1], S31, 0xa4beea44); /* 37 */
	HH (d, a, b, c, x[ 4], S32, 0x4bdecfa9); /* 38 */
	HH (c, d, a, b, x[ 7], S33, 0xf6bb4b60); /* 39 */
	HH (b, c, d, a, x[10], S34, 0xbebfbc70); /* 40 */

	HH (a, b, c, d, x[13], S31, 0x289b7ec6); /* 41 */
	HH (d, a, b, c, x[ 0], S32, 0xeaa127fa); /* 42 */
	HH (c, d, a, b, x[ 3], S33, 0xd4ef3085); /* 43 */
	HH (b, c, d, a, x[ 6], S34,  0x4881d05); /* 44 */

	HH (a, b, c, d, x[ 9], S31, 0xd9d4d039); /* 45 */
	HH (d, a, b, c, x[12], S32, 0xe6db99e5); /* 46 */
	HH (c, d, a, b, x[15], S33, 0x1fa27cf8); /* 47 */
	HH (b, c, d, a, x[ 2], S34, 0xc4ac5665); /* 48 */

	/* Round 4 */
	II (a, b, c, d, x[ 0], S41, 0xf4292244); /* 49 */
	II (d, a, b, c, x[ 7], S42, 0x432aff97); /* 50 */
	II (c, d, a, b, x[14], S43, 0xab9423a7); /* 51 */
	II (b, c, d, a, x[ 5], S44, 0xfc93a039); /* 52 */

	II (a, b, c, d, x[12], S41, 0x655b59c3); /* 53 */
	II (d, a, b, c, x[ 3], S42, 0x8f0ccc92); /* 54 */
	II (c, d, a, b, x[10], S43, 0xffeff47d); /* 55 */
	II (b, c, d, a, x[ 1], S44, 0x85845dd1); /* 56 */

	II (a, b, c, d, x[ 8], S41, 0x6fa87e4f); /* 57 */
	II (d, a, b, c, x[15], S42, 0xfe2ce6e0); /* 58 */
	II (c, d, a, b, x[ 6], S43, 0xa3014314); /* 59 */
	II (b, c, d, a, x[13], S44, 0x4e0811a1); /* 60 */

	II (a, b, c, d, x[ 4], S41, 0xf7537e82); /* 61 */
	II (d, a, b, c, x[11], S42, 0xbd3af235); /* 62 */
	II (c, d, a, b, x[ 2], S43, 0x2ad7d2bb); /* 63 */
	II (b, c, d, a, x[ 9], S44, 0xeb86d391); /* 64 */

	state[0] += a;
	state[1] += b;
	state[2] += c;
	state[3] += d;
	//将临时变量清0,有必要吗?
	//MD5_memset ((POINTER)x, 0, sizeof (x));
	memset(x,0,sizeof(x));//Croky 可以注释
}