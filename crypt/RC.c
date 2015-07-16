/******************************************************************************************
* @file RC.c
* @anthor:Croky.Zheng
* @CreateTime:2012/2/27
* @descirption: RC5��RC6�����㷨
* RC5�㷨��һ�������ֵĽṹ��RC5-w/r/b
* ����w���ֳ���ֵ������16��32��64���ڲ�ͬ���ֳ����ĺ����Ŀ�ķ��鳤��Ϊ2 * wλ��r�Ǽ���������b����Կ�ֽڳ���
* w�Ա��ر�ʾ���ֵĳߴ�16,32,64
* r�������� 0��255
* b��Կ���ֽڳ��� 0��255
*****************************************************************************************/
#include "Rc.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

/* Magic constants for key setup */
//����������������������Կ����
//Odd((e-2)mod 2^w)       Odd(x)С��x����С������
#define P16 0xb7e1
#define P32 0xb7e15163
#define P64 0xb7e151638aed2a6b
//#define P64 0xb7151628aed2a6b
//Odd((phi-1) mod 2^w
#define Q16 0x9e37
#define Q32 0x9e3779b9
#define Q64 0x9e3779b97f4a7c15
//����Ϊ16,32,64λʱPE�ı�ʾ��ʽ
#define PHI 0.618033988749 //1.618033988749
#define E 2.718281828459

//Ĭ�ϼ�������
#define ROUND	20

//��������Ϊ0-25
#define MAXROUND 25

//��Կ�ֽڳ���
#define KEYSIZE	128


//���ݼ�����ֳ�����PW��QW��ֵ
static void SetPwQw(unsigned int nWordSize,
	unsigned long* pPw,
	unsigned long* pQw);

//��ȡ
static int GetCpuWordSize(void *pNull1, 
	void *pNull2);

//����һ���ֳ�
static void Encrypt5(PRCContext pContext,
	unsigned long* pA,
	unsigned long* pB);

//����һ���ֳ�
static void Decrypt5(PRCContext pContext,
	unsigned long* pA,
	unsigned long* pB);

//����һ���ֳ�
static void Encrypt6(PRCContext pContext,
	unsigned long* pA,
	unsigned long* pB,
	unsigned long* pC,
	unsigned long* pD);

//����һ���ֳ�
static void Decrypt6(PRCContext pContext,
	unsigned long* pA,
	unsigned long* pB,
	unsigned long* pC,
	unsigned long* pD);

//����Կ����
static void RCSetKey(PRCContext pContext);

static void F(unsigned long* pA,
	unsigned long* pB,
	unsigned long* pC,
	unsigned long* pD);

void RCInit(PRCContext pContext)
{
	unsigned long a = 9990;
	unsigned long b = 8888;
	unsigned long c = 7770;
	unsigned long d = 6666;

	if ((pContext->mode <= Rc_Mode_Min) || 
		(pContext->mode >= Rc_Mode_Max))
	{
		pContext->mode = RC_Mode_RC5;
	}

	//����int���ͱ�ʾ�ֳ�,������64λ��,long����64λ
	//w�ֳ�  �ֽڱ�ʾ
	pContext->nWordSizeByte = GetCpuWordSize(NULL,NULL);//sizeof(long);
	//CPU���ڵ���64λʱ����32λ��
	//��֧��64λ
	if (pContext->nWordSizeByte >= 8)
	{
		pContext->nWordSizeByte = 4;
	}
	//w�ֳ�  λ��ʾ
	pContext->nWordSizeBit = pContext->nWordSizeByte * 8;
	//r ��������
	if ((pContext->nRound <= 0))
	{
		pContext->nRound = ROUND;
	}
	else
	{
		pContext->nRound = pContext->nRound % MAXROUND;
	}
	//log2(w)
	pContext->nShiftCount = (unsigned int)(log((long double)pContext->nWordSizeBit) / log((long double)2));

	//pContext->nShiftCount = (unsigned int)(log((long double)pContext->nWordSizeByte) / log((long double)2));

	if (pContext->mode == RC_Mode_RC5)
	{
		//�������2*r + 2  ����rΪ��������,���������СΪ�����ֳ�
		pContext->nSubKeyLen = 2 * pContext->nRound + 2;
	}
	else
	{
		//�������2*r + 2  ����rΪ��������,���������СΪ�����ֳ�
		pContext->nSubKeyLen = 2 * pContext->nRound + 3;
	}
		//���ķ���  �������ķ���Ϊ2*w(�ֳ�),ʹ��2*(r + 1)��,rΪ��������
	pContext->pSubKey = (unsigned long*)malloc(pContext->nSubKeyLen * pContext->nWordSizeByte);
	memset(pContext->pSubKey,0,pContext->nSubKeyLen * pContext->nWordSizeByte);
	RCSetKey(pContext);
}

void RCDestroy(PRCContext pContext)
{
	//��������Կ�ռ�
	if ((NULL != pContext) && (NULL != pContext->pSubKey))
	{
		free(pContext->pSubKey);
	}
}
//������Կ
void RCSetKey(PRCContext pContext)
{
	int i;
	unsigned int j,k,t,w;
	unsigned long* pL;
	unsigned long a,b;
	unsigned long p,q;
	unsigned long max;

	//��Կ
	unsigned char* pKey = pContext->pKey;

	//��Կ����
	unsigned int nLen = pContext->nKeyLen;

	//����Կ����
	unsigned int nSubKeyLen = pContext->nSubKeyLen;
	//�ֳ�
	w = pContext->nWordSizeByte;

	//���ַ���ת��Ϊ�ֳ�������Ҫ�ĸ���
	k = (nLen % w == 0) ? (nLen / w) : (nLen / w) + 1;

	//���ֽ���
	t = k * w;

	//��ʱ���飬��Կת�����ֽڳ�����
	pL = (unsigned long*)malloc(t);
	memset(pL,0,t);

	//���ַ�������COPY���µ��ֳ�����(��˷�)
	for (i=(nLen-1); i>=0; i--)
	{
		pL[i/w] = (pL[i/w] << 8) + pKey[i];
	}
	
	//��ȡ���ֳ���ص�Pw��Qw��ֵ
	SetPwQw(w,&p,&q);

	//��ʼ��subkey
	pContext->pSubKey[0] = p;
	for (i=1; i<nLen; i++)
	{
		pContext->pSubKey[i] = pContext->pSubKey[i-1] + q;
	}

	//��Կ��չ
	a = 0;
	b = 0;
	i = 0;
	j = 0;
	//��Կת��֮��ĳ��� �� ����Կ�ĳ���
	//�ַ����� / �ֳ�       2 * (r + 1) rΪ��������
	max = (pContext->nSubKeyLen > k) ? pContext->nSubKeyLen : k;
	max = 3 * max;
	w = 1 << pContext->nShiftCount;//�����λ����
	//max times
	while (max-- > 0)
	{
		pContext->pSubKey[i] = pContext->pSubKey[i] + a + b;
		pContext->pSubKey[i] = ROTL(pContext->pSubKey[i],3,w);
		a = pContext->pSubKey[i];
		pL[j] = pL[j] + a + b;
		pL[j] = ROTL(pL[j],a+b,w);
		b = pL[j];
		i = (i+1) % nSubKeyLen;
		j = (j+1) % k;
	}
	free(pL);
}

//rc5����
void Encrypt5(PRCContext pContext,
	unsigned long* pA,
	unsigned long* pB)
{
	unsigned long i,w;
	register unsigned long a = *pA;
	register unsigned long b = *pB;
	w = pContext->nWordSizeBit;
	a += pContext->pSubKey[0];
	b += pContext->pSubKey[1];
	for (i=1; i<=pContext->nRound; i++)
	{
		a = ROTL((a ^ b),b,w);
		a += pContext->pSubKey[2 * i];
		b = ROTL((b ^ a),a,w);
		b += pContext->pSubKey[2 * i + 1];
	}
	*pA = a;
	*pB = b;
}

//rc5����
void Decrypt5(PRCContext pContext,
	unsigned long* pA,
	unsigned long* pB)
{
	unsigned long i,w;
	register unsigned long a = *pA;
	register unsigned long b = *pB;
	w = pContext->nWordSizeBit;
	for (i=pContext->nRound; i>=1; i--)
	{
		b -= pContext->pSubKey[2 * i + 1];
		b = ROTR(b,a,w) ^ a;
		a -= pContext->pSubKey[2 * i];
		a = ROTR(a,b,w) ^ b;
	}
	a -= pContext->pSubKey[0];
	b -= pContext->pSubKey[1];
	*pA = a;
	*pB = b;
}

//����
void Encrypt6(PRCContext pContext,
	unsigned long* pA,
	unsigned long* pB,
	unsigned long* pC,
	unsigned long* pD)
{
	unsigned long i,w,t,u,t1;
	register unsigned long a = *pA;
	register unsigned long b = *pB;
	register unsigned long c = *pC;
	register unsigned long d = *pD;
	w = pContext->nWordSizeBit;
	b += pContext->pSubKey[0];
	d += pContext->pSubKey[1];
	for (i=1; i<=pContext->nRound; i++)
	{
		//����b����u��ֵ
		t = b * (2 * b + 1);
		t = ROTL(t,pContext->nShiftCount,w);
		//����d����u��ֵ
		u = d * (2 * d + 1);
		u = ROTL(u,pContext->nShiftCount,w);
		
		a = ROTL((a ^ t),u,w);
		a = a + pContext->pSubKey[2 * i];
		c = ROTL((c ^ u),t,w);
		c = c + pContext->pSubKey[2 * i + 1];
		//�û�
		//printf("i=%d,%u,%u,%u,%u,%u,%u\r\n",i,a,b,c,d,t,u);
		///*
		t1 = a;
		a = b;
		b = c;
		c = d;
		d = t1;
		//*/
	}
	//printf("-------------------------------------\r\n");
	//����rc5�㷨�ֲ�����Կ,2*r+2�Ѿ���������ĩβ
	//��ʾrc6����Ҫ��rc5�ٶ����һ����Կ
	a += pContext->pSubKey[2 * pContext->nRound + 2];
	c += pContext->pSubKey[2 * pContext->nRound + 3];
	*pA = a;
	*pB = b;
	*pC = c;
	*pD = d;
}

//����
void Decrypt6(PRCContext pContext,
	unsigned long* pA,
	unsigned long* pB,
	unsigned long* pC,
	unsigned long* pD)
{
	unsigned long i,w,t,u,t1;
	register unsigned long a = *pA;
	register unsigned long b = *pB;
	register unsigned long c = *pC;
	register unsigned long d = *pD;
	w = pContext->nWordSizeBit;
	c -= pContext->pSubKey[2 * pContext->nRound + 3];
	a -= pContext->pSubKey[2 * pContext->nRound + 2];
	for (i=pContext->nRound; i>=1; i--)
	{
		//printf("i=%d,%u,%u,%u,%u\r\n",i,a,b,c,d);
		//���û�
		//FR(&a,&b,&c,&d);
		///*
		t1 = d;
		d = c;
		c = b;
		b = a;
		a = t1;
		//*/
		//����u��ֵ,��λ��;����ʱʹ��d��ΪΨһ����������
		u = d * (2 * d + 1);
		u = ROTL(u,pContext->nShiftCount,w);

		//����b����t��ֵ
		t = b * (2 * b + 1);
		t = ROTL(t,pContext->nShiftCount,w);
		
		//printf("i=%d,%u,%u,%u,%u,%u,%u\r\n",i,a,b,c,d,t,u);
		c -= pContext->pSubKey[2 * i + 1];
		c = ROTR((c),t,w);
		c ^= u;
		a -= pContext->pSubKey[2 * i];
		a = ROTR((a),u,w);
		a ^= t;
	}
	//printf("=============================================\r\n");
	//����rc5�㷨�ֲ�����Կ,2*r+2�Ѿ���������ĩβ
	//��ʾrc6����Ҫ��rc5�ٶ����һ����Կ
	d -= pContext->pSubKey[1];
	b -= pContext->pSubKey[0];
	*pA = a;
	*pB = b;
	*pC = c;
	*pD = d;
}

//(a,b,c,d) ---> (b,c,d,a)
void F(unsigned long* pA,
	unsigned long* pB,
	unsigned long* pC,
	unsigned long* pD)
{
	unsigned long tmp = *pA;
	*pA = *pB;
	*pB = *pC;
	*pC = *pD;
	*pD = tmp;
}
//b,c,d,a ---> (a,b,c,d)
void FR(unsigned long* pA,
	unsigned long* pB,
	unsigned long* pC,
	unsigned long* pD)
{
	unsigned long tmp = *pD;
	*pD = *pC;
	*pC = *pB;
	*pB = *pA;
	*pA = tmp;
}

int GetCpuWordSize(void *pNull1, void *pNull2)
{ 
	long offset = (long)&pNull2 - (long)&pNull1;
	return offset;  
}

void SetPwQw(unsigned int nWordSize,unsigned long* pPw,unsigned long* pQw)
{
	if (nWordSize == 2)
	{
		*pPw = P16;
		*pQw = Q16;
	}
	else if(nWordSize == 4)
	{
		*pPw = P32;
		*pQw = Q32;
	}
	else if(nWordSize == 8)
	{
		/* ��ʱ�Ͳ�Ҫ֧��64λ�İ�
			*pPw = P64;
			*pQw = Q64;
		*/
		*pPw = P32;
		*pQw = Q32;
	}
	else
	{
		*pPw = 0;
		*pQw = 0;
	}
}

void RCEncrypt(PRCContext pContext,
	void* pInput,
	unsigned int nSize,
	void* pOutput)
{
	unsigned int i;
	unsigned long* pOutWord = (unsigned long*)pOutput;
	unsigned int q = 0;
	unsigned int nLen;
	unsigned int nOutputLen;
	//ֻ֧��32λ
	unsigned int w = 4;//GetCpuWordSize(NULL,NULL);
	//��֤���ֽ����ֳ��ı���
	//��������ֳ���4
	//nLen = nSize + (w - (nSize % w));
	//nLen = nSize;
	//rc5һ�μ���2���ֳ�
	if (pContext->mode == RC_Mode_RC5)
	{
		//RC5һ�μ��������ֳ�,8���ֽ�
		nLen = CHECKDIV8(nSize);
		//ת���ֳ���ĸ���
	}
	else
	{
		//RC6һ�μ���4���ֳ�k,16���ֽ�
		nLen = CHECKDIV16(nSize);
	}
	//���뵽�������ֽ���
	//nLen += (nLen % q) == 0 ? 0 : q - (q % 16);
	//�ֳ���
	nOutputLen = nLen / w;
	//��0
	memset(pOutWord,0,nLen);
	//�����ֽ�
	memcpy(pOutWord,pInput,nSize);
	if (pContext->mode == RC_Mode_RC5)
	{
		for (i=0; i<nOutputLen; i+=2)
		{
			Encrypt5(pContext,&pOutWord[i],&pOutWord[i+1]);
		}
	}
	else
	{
		for (i=0; i<nOutputLen; i+=4)
		{
			Encrypt6(pContext,&pOutWord[i],&pOutWord[i+1],&pOutWord[i+2],&pOutWord[i+3]);
		}
	}
}

void RCDecrypt(PRCContext pContext,
	void* pInput,
	unsigned int nSize,
	void* pOutput)
{
	unsigned int i;
	unsigned long* pOutWord = (unsigned long*)pOutput;
	unsigned int q = 0;
	unsigned int nLen;
	unsigned int nOutputLen;
	//ֻ֧��32λ
	unsigned int w = 4;//GetCpuWordSize(NULL,NULL);
	//��֤���ֽ����ֳ��ı���
	//��������ֳ���4
	//nLen = nSize + (w - (nSize % w));
	//nLen = nSize;
	//rc5һ�μ���2���ֳ�
	if (pContext->mode == RC_Mode_RC5)
	{
		//RC5һ�μ��������ֳ�,8���ֽ�
		nLen = CHECKDIV8(nSize);
		//ת���ֳ���ĸ���
	}
	else
	{
		//RC6һ�μ���4���ֳ�k,16���ֽ�
		nLen = CHECKDIV16(nSize);
	}
	//���뵽�������ֽ���
	//nLen += (nLen % q) == 0 ? 0 : q - (q % 16);
	//�ֳ���
	nOutputLen = nLen / w;
	//��0
	memset(pOutWord,0,nLen);
	//�����ֽ�
	memcpy(pOutWord,pInput,nSize);
	if (pContext->mode == RC_Mode_RC5)
	{
		for (i=0; i<nOutputLen; i+=2)
		{
			Decrypt5(pContext,&pOutWord[i],&pOutWord[i+1]);
		}
	}
	else
	{
		for (i=0; i<nOutputLen; i+=4)
		{
			Decrypt6(pContext,&pOutWord[i],&pOutWord[i+1],&pOutWord[i+2],&pOutWord[i+3]);
		}
	}
}