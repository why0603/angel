/******************************************************************************************
* @file RC.c
* @anthor:Croky.Zheng
* @CreateTime:2012/2/27
* @descirption: RC5，RC6加密算法
* RC5算法有一个面向字的结构：RC5-w/r/b
* 这里w是字长其值可以是16、32或64对于不同的字长明文和密文块的分组长度为2 * w位，r是加密轮数，b是密钥字节长度
* w以比特表示的字的尺寸16,32,64
* r加密轮数 0～255
* b密钥的字节长度 0～255
*****************************************************************************************/
#include "Rc.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

/* Magic constants for key setup */
//定义两个常量，用于子密钥生成
//Odd((e-2)mod 2^w)       Odd(x)小于x的最小奇整数
#define P16 0xb7e1
#define P32 0xb7e15163
#define P64 0xb7e151638aed2a6b
//#define P64 0xb7151628aed2a6b
//Odd((phi-1) mod 2^w
#define Q16 0x9e37
#define Q32 0x9e3779b9
#define Q64 0x9e3779b97f4a7c15
//以上为16,32,64位时PE的表示方式
#define PHI 0.618033988749 //1.618033988749
#define E 2.718281828459

//默认加密轮数
#define ROUND	20

//加密轮数为0-25
#define MAXROUND 25

//密钥字节长度
#define KEYSIZE	128


//根据计算机字长设置PW和QW的值
static void SetPwQw(unsigned int nWordSize,
	unsigned long* pPw,
	unsigned long* pQw);

//获取
static int GetCpuWordSize(void *pNull1, 
	void *pNull2);

//加密一个字长
static void Encrypt5(PRCContext pContext,
	unsigned long* pA,
	unsigned long* pB);

//解密一个字长
static void Decrypt5(PRCContext pContext,
	unsigned long* pA,
	unsigned long* pB);

//加密一个字长
static void Encrypt6(PRCContext pContext,
	unsigned long* pA,
	unsigned long* pB,
	unsigned long* pC,
	unsigned long* pD);

//解密一个字长
static void Decrypt6(PRCContext pContext,
	unsigned long* pA,
	unsigned long* pB,
	unsigned long* pC,
	unsigned long* pD);

//子密钥计算
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

	//本来int类型表示字长,但是在64位下,long才是64位
	//w字长  字节表示
	pContext->nWordSizeByte = GetCpuWordSize(NULL,NULL);//sizeof(long);
	//CPU大于等于64位时，则按32位算
	//不支持64位
	if (pContext->nWordSizeByte >= 8)
	{
		pContext->nWordSizeByte = 4;
	}
	//w字长  位表示
	pContext->nWordSizeBit = pContext->nWordSizeByte * 8;
	//r 加密轮数
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
		//分组个数2*r + 2  其中r为加密轮数,单个分组大小为两个字长
		pContext->nSubKeyLen = 2 * pContext->nRound + 2;
	}
	else
	{
		//分组个数2*r + 2  其中r为加密轮数,单个分组大小为两个字长
		pContext->nSubKeyLen = 2 * pContext->nRound + 3;
	}
		//密文分组  单个密文分组为2*w(字长),使用2*(r + 1)个,r为加密轮数
	pContext->pSubKey = (unsigned long*)malloc(pContext->nSubKeyLen * pContext->nWordSizeByte);
	memset(pContext->pSubKey,0,pContext->nSubKeyLen * pContext->nWordSizeByte);
	RCSetKey(pContext);
}

void RCDestroy(PRCContext pContext)
{
	//销毁子密钥空间
	if ((NULL != pContext) && (NULL != pContext->pSubKey))
	{
		free(pContext->pSubKey);
	}
}
//设置密钥
void RCSetKey(PRCContext pContext)
{
	int i;
	unsigned int j,k,t,w;
	unsigned long* pL;
	unsigned long a,b;
	unsigned long p,q;
	unsigned long max;

	//密钥
	unsigned char* pKey = pContext->pKey;

	//密钥长度
	unsigned int nLen = pContext->nKeyLen;

	//子密钥组数
	unsigned int nSubKeyLen = pContext->nSubKeyLen;
	//字长
	w = pContext->nWordSizeByte;

	//将字符串转化为字长整型需要的个数
	k = (nLen % w == 0) ? (nLen / w) : (nLen / w) + 1;

	//总字节数
	t = k * w;

	//临时数组，密钥转换成字节长整型
	pL = (unsigned long*)malloc(t);
	memset(pL,0,t);

	//将字符串数组COPY到新的字长数组(大端法)
	for (i=(nLen-1); i>=0; i--)
	{
		pL[i/w] = (pL[i/w] << 8) + pKey[i];
	}
	
	//获取与字长相关的Pw和Qw的值
	SetPwQw(w,&p,&q);

	//初始化subkey
	pContext->pSubKey[0] = p;
	for (i=1; i<nLen; i++)
	{
		pContext->pSubKey[i] = pContext->pSubKey[i-1] + q;
	}

	//密钥扩展
	a = 0;
	b = 0;
	i = 0;
	j = 0;
	//密钥转换之后的长度 和 子密钥的长度
	//字符串长 / 字长       2 * (r + 1) r为加密轮数
	max = (pContext->nSubKeyLen > k) ? pContext->nSubKeyLen : k;
	max = 3 * max;
	w = 1 << pContext->nShiftCount;//最大移位次数
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

//rc5加密
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

//rc5解密
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

//加密
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
		//根据b计算u的值
		t = b * (2 * b + 1);
		t = ROTL(t,pContext->nShiftCount,w);
		//根据d计算u的值
		u = d * (2 * d + 1);
		u = ROTL(u,pContext->nShiftCount,w);
		
		a = ROTL((a ^ t),u,w);
		a = a + pContext->pSubKey[2 * i];
		c = ROTL((c ^ u),t,w);
		c = c + pContext->pSubKey[2 * i + 1];
		//置换
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
	//按照rc5算法分布的密钥,2*r+2已经到达数组末尾
	//表示rc6最少要比rc5再多产生一组密钥
	a += pContext->pSubKey[2 * pContext->nRound + 2];
	c += pContext->pSubKey[2 * pContext->nRound + 3];
	*pA = a;
	*pB = b;
	*pC = c;
	*pD = d;
}

//加密
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
		//逆置换
		//FR(&a,&b,&c,&d);
		///*
		t1 = d;
		d = c;
		c = b;
		b = a;
		a = t1;
		//*/
		//计算u的值,移位数;加密时使用d做为唯一参与计算的数
		u = d * (2 * d + 1);
		u = ROTL(u,pContext->nShiftCount,w);

		//根据b计算t的值
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
	//按照rc5算法分布的密钥,2*r+2已经到达数组末尾
	//表示rc6最少要比rc5再多产生一组密钥
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
		/* 暂时就不要支持64位的吧
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
	//只支持32位
	unsigned int w = 4;//GetCpuWordSize(NULL,NULL);
	//保证总字节是字长的倍数
	//这里最大字长是4
	//nLen = nSize + (w - (nSize % w));
	//nLen = nSize;
	//rc5一次计算2个字长
	if (pContext->mode == RC_Mode_RC5)
	{
		//RC5一次计算两个字长,8个字节
		nLen = CHECKDIV8(nSize);
		//转成字长后的个数
	}
	else
	{
		//RC6一次计算4个字长k,16个字节
		nLen = CHECKDIV16(nSize);
	}
	//补齐到可运算字节数
	//nLen += (nLen % q) == 0 ? 0 : q - (q % 16);
	//字长数
	nOutputLen = nLen / w;
	//清0
	memset(pOutWord,0,nLen);
	//拷贝字节
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
	//只支持32位
	unsigned int w = 4;//GetCpuWordSize(NULL,NULL);
	//保证总字节是字长的倍数
	//这里最大字长是4
	//nLen = nSize + (w - (nSize % w));
	//nLen = nSize;
	//rc5一次计算2个字长
	if (pContext->mode == RC_Mode_RC5)
	{
		//RC5一次计算两个字长,8个字节
		nLen = CHECKDIV8(nSize);
		//转成字长后的个数
	}
	else
	{
		//RC6一次计算4个字长k,16个字节
		nLen = CHECKDIV16(nSize);
	}
	//补齐到可运算字节数
	//nLen += (nLen % q) == 0 ? 0 : q - (q % 16);
	//字长数
	nOutputLen = nLen / w;
	//清0
	memset(pOutWord,0,nLen);
	//拷贝字节
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