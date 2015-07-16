/******************************************************************************************
 * @file Tea.c
 * @anthor:Croky.Zheng
 * @CreateTime:2011/5/9
 * @descirption: tea++加密算法
 * Tea算法使用128位密钥,使用4个32位表示
 * 算法速度快，加密效果也不错
 * Tea算法要求明文必须是8的倍数
 * 而XXTea算法则只需要是4的倍数即可
 *****************************************************************************************/
#include "tea.h"
#include <string.h>


#define TEADELTA	0x9E3779B9            //((开根5)-1)*2^31;
#define TEASUM		0xC6EF3720            //实际上应该是0x13C6EF3720这个数才是delta*32的值
#define MAXKEYLEN	16                   //最大密钥长度(字节)
/*
//必须保证被加密和解密的字符串长度为8的倍数
static	int teaEnSize(int len)
{
	return ((len%8)==0) ? len:((len/8)+1)*8;
}

static	int teaDeSize(int len)
{
	return ((len%8)==0) ? len:((len/8)-1)*8;
}
*/

/*
 * 每次加密8个字节，一般性情况当字符串长度不够8个字节时，向后补0
 * @param v 要被加密的8个字节
 * @param k 密钥(16个字节)
 * @return v 加密后的数据也保存在v变量中，所以v是一个指针变量
 */
static void TeaEncode(TeaContextPtr contextPtr,int v[2]);


/***********************************************************
 * 每次解码8个字节
 * @param v 被加密，需要解密的数据
 * @param k 加密时用的密钥，用于解密
 * @return
 * 解密后的明文与密文等长，并修改密文指向的数据
 ***********************************************************/
static void TeaDecode(TeaContextPtr contextPtr,int v[2]);

//XXTea算法加密
static void XXTeaEncode(TeaContextPtr contextPtr,int* v,unsigned int nSize);


//XXTea算法解密
static void XXTeaDecode(TeaContextPtr contextPtr,int* v,unsigned int nSize);

//static long k[4] = {0};
//static unsigned int k[4] = {0};


//TEA的加密算法相当简单，根本都不需要初始化
void TeaInit(TeaContextPtr context)
{
	//memset(k,0,MAXKEYLEN);//16个字节长度
	//memcpy(k,pKey,(nLen > MAXKEYLEN) ? MAXKEYLEN : nLen);
}

void TeaSetKey(TeaContextPtr context,char* pKey,unsigned int nLen)
{
	memset(context->key,0,MAXKEYLEN);//16个字节长度
	memcpy(context->key,pKey,(nLen > MAXKEYLEN) ? MAXKEYLEN : nLen);
}

void TeaEncode(TeaContextPtr context,int v[2])
{
	int v0 = v[0];
	int v1 = v[1];
	unsigned int sum=0;
	int i;
	//32次置换
	for (i=0; i<32; i++)
	{
		sum += TEADELTA;
		v0 += ((v1<<4) + context->key[0]) ^ (v1 + sum) ^ ((v1>>5) + context->key[1]);
		v1 += ((v0<<4) + context->key[2]) ^ (v0 + sum) ^ ((v0>>5) + context->key[3]);
	}
	v[0] = v0;
	v[1] = v1;
}

void TeaDecode(TeaContextPtr context,int v[2])
{
	int v0 = v[0];
	int v1 = v[1];
	unsigned int sum=0;
	int i;
	sum = TEASUM;
	for (i=0; i<32; i++) 
	{
		v1 -= ((v0<<4) + context->key[2]) ^ (v0 + sum) ^ ((v0>>5) + context->key[3]);
		v0 -= ((v1<<4) + context->key[0]) ^ (v1 + sum) ^ ((v1>>5) + context->key[1]);
		sum -= TEADELTA;
	}
	v[0] = v0;
	v[1] = v1;
}

#define MOVOP (z>>5^y<<2) + (y>>3^z<<4)^(sum^y) + (context->key[p&3^e]^z)

void XXTeaEncode(TeaContextPtr context,int* v,unsigned int nSize)
{
    unsigned int p, q ;
	unsigned int e; 
	unsigned int y = v[0];//第一个数
	unsigned int sum = 0;
	unsigned int len = nSize - 1; 
	unsigned int z = v[len];//最后一个数

	if (nSize > 1) {
		q = 6 + 52 / nSize;
		while (q-- > 0) {//q轮加密,这样看起来越短加密轮数越多
			sum += TEADELTA;
			e = (sum >> 2) & 3;
			for (p=0; p < len; p++)
			{
				y = v[p+1],z = v[p] += MOVOP;//
			}
			y = v[0];
			z = v[len] += MOVOP;
		}
	}
}


void XXTeaDecode(TeaContextPtr context,int* v,unsigned int nSize)
{
    unsigned int p, q ;
	unsigned int e; 
	unsigned int y = v[0];//第一个数
	unsigned int sum = 0;
	unsigned int len = nSize - 1; 
	unsigned int z = v[len];//最后一个数
	
	if (nSize > 1) {
		q = 6 + 52 / nSize;
		sum = q * TEADELTA;
		while (sum != 0) {//q轮加密,这样看起来越短加密轮数越多
			e = (sum >> 2) & 3;
			for (p=len; p>0; p--)
			{
				z = v[p-1],y = v[p] -= MOVOP;
			}
			z = v[len];
			y = v[0] -= MOVOP;
			sum -= TEADELTA;
		}
	}
}

void TeaEncrypt(TeaContextPtr context,void* pInput,unsigned int nSize,void* pOutput)
{
	int loop,i;
	int* pV;
	loop = CHECKDIV8(nSize) / 8;//共要计算的次数
	memset(pOutput,0,CHECKDIV8(nSize));//这种操作最后放到函数外
	memcpy(pOutput,pInput,nSize);
	pV = (int*)pOutput;
	if (context->mode == Tea_Mode)
	{
		for (i=0; i<loop;i++)
		{
			TeaEncode(context,pV + (i * 2));
		}
	}
	else if (context->mode == XTea_Mode)
	{
		XXTeaEncode(context,pV,loop *  2);
	}
	else if (context->mode == XXTea_Mode)
	{
		XXTeaEncode(context,pV,loop *  2);
	}
	return;
}

void TeaDecrypt(TeaContextPtr context,void* pInput,unsigned int nSize,void* pOutput)
{
	int loop,i;
	int* pV;
	//不是8的倍数就不用来解密了
	if ((nSize % 8) != 0)
	{
		return;
	}
	loop = nSize / 8;//共要计算的次数
	memset(pOutput,0,nSize);//这种操作最后放到函数外
	memcpy(pOutput,pInput,nSize);
	pV = (int*)pOutput;
	if (context->mode == Tea_Mode)
	{
		for (i=0; i<loop;i++)
		{
			TeaDecode(context,pV + (i * 2));
		}
	}
	else if (context->mode == XTea_Mode)
	{
		XXTeaDecode(context,pV,loop *  2);
	}
	else if (context->mode == XXTea_Mode)
	{
		XXTeaDecode(context,pV,loop *  2);
	}
	return;
}