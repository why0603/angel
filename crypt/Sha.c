/******************************************************************************************
* @file Sha.c
* @anthor:Croky.Zheng
* @CreateTime:2012/2/25
* @descirption: SHA1��ϢժҪ�㷨
* SHA1�㷨����:
* ��λ��ʽ��MD5һ������ȫȱʧ���ַ����448������һ��Ϊ1������Ϊ0
* ����64λ��ʾ��ԭʼ�����ֽڳ���
* ��5����ʼ������ǰ4����MD5һ���������һ��,��С��Ҳ�б仯
* SHA1�㷨���ֽ������������ش�˷�����X86�Դ���ָ��ת����ʵ���෴��
* ����(int*)(abc)��õ�������Md5��ȡ����ʵ���෴��
* SHA1Ϊ0x61626300��MD5Ӧ����0x00636261(���ҿ����õ�INTEL CPU��ͬ)
* SHA ʹ��BIG-ENDIAN�ķ�ʽ
* SHA1("abc") = a9993e364706816aba3e25717850c26c9cd0d89d
*****************************************************************************************/
#include "Sha.h"
#include <string.h>

#define MAX_VALUE 4294967296     //2��32�η����޷�������

#define A 0x67452301

#define B 0xefcdab89

#define C 0x98badcfe

#define D 0x10325476

#define E 0xC3D2E1F0

// (0 <= t <= 19)
#define KT20 0x5A827999

// (20 <= t <= 39)
#define KT40 0x6ED9EBA1

// (40 <= t <= 59)
#define KT60 0x8F1BBCDC

// (60 <= t <= 79)
#define KT80 0xCA62C1D6

// (0 <= t <= 19)
#define FT20(b,c,d) (((b) & (c)) | ((~(b)) & (d)))

// (20 <= t <= 39)
#define FT40(b,c,d) ((b) ^ (c) ^ (d))

// (40 <= t <= 59)
#define FT60(b,c,d) (((b) & (c)) | ((b) & (d)) | ((c) & (d)))

// (60 <= t <= 79)
#define FT80(b,c,d) ((b) ^ (c) ^ (d))

//��16λ���
#define LSW(x,y) (((x) & 0xffff) + ((y) & 0xffff))

//��16λ����ټӵ�16λ��ӵĽ�λ
#define MSW(x,y) (((x) >> 16) + ((y) >> 16) + (LSW(x,y) >> 16))

//����(x+y) Mod 2��32�η�    (����2��32�η�����MSW���н�λ���������øɵ��˽�λ�����������)
#define SHA1_ADD(x,y) ((MSW(x,y) << 16) | (LSW(x,y) & 0xffff))

//�Ե�λ��
#define ROTATE_LEFT(x, n) (((x) << (n)) | ((x) >> (32-(n))))

//KT��������
#define SHA1_KT(x) (((x) < 20) ? KT20 : ((x) < 40) ? KT40 : ((x) < 60) ? KT60 : KT80)

//FT��������
#define SHA1_FT(x,b,c,d) (((x) < 20) ? FT20((b),(c),(d)) : \
	((x) < 40) ? FT40((b),(c),(d)) : \
	((x) < 60) ? FT60((b),(c),(d)) : \
	FT80((b),(c),(d)))

//ͬ��ADDMOD32   
static unsigned int Sha_Add(unsigned int x,
	unsigned int y);


static unsigned int Sha_Kt(unsigned int x);

static unsigned int Sha_Ft(unsigned int x,
	unsigned int b,
	unsigned int c,
	unsigned int d);
//���ֽ��������ּ���
static void ShaTransform(unsigned int state[5] , 
	unsigned char block[64]);

//��512λ64���ֽڵ��ֽ���תΪ16��32λ��������
static void Decode(unsigned int* pOutput, 
	unsigned char* pInput, 
	unsigned int nLen);

//��16��32λ��������ת��Ϊ�ֽ���
static void Encode(unsigned char* pOutput,
	unsigned int* pInput,
	unsigned int nLen);

//����������,�����Ҫ�ٲ����в������ĳ���
static unsigned char PADDING[64] = {
  0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

//��ʼ��context
void ShaInit(PShaContext pContext)
{
	pContext->count[0] = 0;
	pContext->count[1] = 0;
	pContext->state[0] = A;
	pContext->state[1] = B;
	pContext->state[2] = C;
	pContext->state[3] = D;
	pContext->state[4] = E;
}

void ShaUpdate(
	PShaContext pContext,	/* context */
	unsigned char* pInput,	/* input block */
	unsigned int nLen/* length of input block */)
{
	unsigned int i, index, partLen;

	//���ȳ���8,��ʵ��ÿ��������Ҫ512λ��64*8
	/* Compute number of bytes mod 64 */
	index = (unsigned int)((pContext->count[0] >> 3) & 0x3f);

	//��Ҫ������ֽ���*8
	pContext->count[0] += ((unsigned int)nLen << 3);

	//���ǲ��Ǳ�ʾ����ˣ�
	/* Update number of bits */
	if ((pContext->count[0]) < ((unsigned int)nLen << 3))
	{
		pContext->count[1]++;
	}

	pContext->count[1] += ((unsigned int)nLen >> 29);

	//��Ҫ���µ�������ʹ�ÿ����õ��ַ���
	partLen = 64 - index;

	/* Transform as many times as possible.
	*/
	if (nLen >= partLen) {
		memcpy(&pContext->buffer[index],pInput,partLen);
		ShaTransform(pContext->state, pContext->buffer);

		for (i = partLen; (i + 63) < nLen; i += 64)
		{
			ShaTransform(pContext->state, &pInput[i]);
		}

		index = 0;
	}
	else
	{
		i = 0;
	}

	/* Buffer remaining input */
	memcpy(&pContext->buffer[index],&pInput[i],nLen-i);
}

void ShaFinal(
	PShaContext pContext,/* context */
	unsigned char digest[20]/* message digest */)
{
	unsigned int index, padLen;
	unsigned char bits[8]= {0};

	/* Save number of bits */
	//����ʹ����С�˷������....
	Encode(bits, pContext->count+1, 4);
	Encode(&bits[4], pContext->count, 4);
	//bits[7] = 0x18;

	/* Pad out to 56 mod 64.
	*/
	index = (unsigned int)((pContext->count[0] >> 3) & 0x3f);
	padLen = (index < 56) ? (56 - index) : (120 - index);
	//�����������SHA��80������
	ShaUpdate(pContext, PADDING, padLen);

	//���֮ǰ�ĳ���
	ShaUpdate(pContext, bits, 8);
	/* Store state in digest */
	Encode(digest, pContext->state, 20);
	memset(pContext,0,sizeof(*pContext));//����ע�ͣ����Ǵ�������
}


void ShaTransform(unsigned int state[5] , 
	unsigned char block[64])
{
	unsigned int j,tmp;
	unsigned int a = state[0];
	unsigned int b = state[1];
	unsigned int c = state[2];
	unsigned int d = state[3];
	unsigned int e = state[4];
	unsigned int w[80] = {0};
	Decode(w,block,64);//תǰ��64���ֽ�Ϊ16����������
	//memcpy(w,block,64);
	for (j=0; j<80; j++)
	{
		if (j >= 16)
		{
			w[j] = ROTATE_LEFT(w[j-3] ^ w[j-8] ^ w[j-14] ^ w[j-16], 1);
		}
		tmp = SHA1_ADD(SHA1_ADD(ROTATE_LEFT(a, 5), SHA1_FT(j, b, c, d)),SHA1_ADD(SHA1_ADD(e, w[j]), SHA1_KT(j)));
		e = d;
		d = c;
		c = ROTATE_LEFT(b,30);
		b = a;
		a = tmp;
	}
	state[0] = SHA1_ADD(state[0], a);
	state[1] = SHA1_ADD(state[1], b);
	state[2] = SHA1_ADD(state[2], c);
	state[3] = SHA1_ADD(state[3], d);
	state[4] = SHA1_ADD(state[4], e);
}


void Decode(unsigned int* pOutput, 
	unsigned char* pInput, 
	unsigned int nLen)
{
	unsigned int i, j;
	for (i = 0, j = 0; j < nLen; i++, j += 4)
	{
		pOutput[i] = ((unsigned int)pInput[j+3]) | 
			(((unsigned int)pInput[j+2]) << 8) |
			(((unsigned int)pInput[j+1]) << 16) | 
			(((unsigned int)pInput[j]) << 24);
	}
}

void Encode(unsigned char* pOutput,
	unsigned int* pInput,
	unsigned int nLen)
{
	unsigned int i, j;
	for (i = 0, j = 0; j < nLen; i++, j += 4) 
	{
		pOutput[j+3] = (unsigned char)(pInput[i] & 0xff);
		pOutput[j+2] = (unsigned char)((pInput[i] >> 8) & 0xff);
		pOutput[j+1] = (unsigned char)((pInput[i] >> 16) & 0xff);
		pOutput[j] = (unsigned char)((pInput[i] >> 24) & 0xff);
	}
}

unsigned int Sha1_Add(unsigned int x,unsigned int y)
{
	/*
	unsigned int lsw = (x & 0xFFFF) + (y & 0xFFFF);
	unsigned int msw = (x >> 16) + (y >> 16) + (lsw >> 16);
	return (msw << 16) | (lsw & 0xFFFF);
	*/
	///*
#if defined(UINT64)
	undef UINT64
#endif
#if WIN32
		typedef unsigned __int64 UINT64;
#else
		typedef unsigned long long UINT64;
#endif
	UINT64 sum = 0;
	sum = x + y;
	return sum % MAX_VALUE;
	//*/
}

unsigned int Sha_Kt(unsigned int x)
{
	if(x < 20) return KT20;
	if(x < 40) return KT40;
	if(x < 60) return KT60;
	return KT80;
}

unsigned int Sha_Ft(unsigned int x,
	unsigned int b,
	unsigned int c,
	unsigned int d)
{
	if(x < 20) return (b & c) | ((~b) & d);
	if(x < 40) return b ^ c ^ d;
	if(x < 60) return (b & c) | (b & d) | (c & d);
	return b ^ c ^ d;
}