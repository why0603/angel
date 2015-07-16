/******************************************************************************************
 * @file Tea.c
 * @anthor:Croky.Zheng
 * @CreateTime:2011/5/9
 * @descirption: tea++�����㷨
 * Tea�㷨ʹ��128λ��Կ,ʹ��4��32λ��ʾ
 * �㷨�ٶȿ죬����Ч��Ҳ����
 * Tea�㷨Ҫ�����ı�����8�ı���
 * ��XXTea�㷨��ֻ��Ҫ��4�ı�������
 *****************************************************************************************/
#include "tea.h"
#include <string.h>


#define TEADELTA	0x9E3779B9            //((����5)-1)*2^31;
#define TEASUM		0xC6EF3720            //ʵ����Ӧ����0x13C6EF3720���������delta*32��ֵ
#define MAXKEYLEN	16                   //�����Կ����(�ֽ�)
/*
//���뱣֤�����ܺͽ��ܵ��ַ�������Ϊ8�ı���
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
 * ÿ�μ���8���ֽڣ�һ����������ַ������Ȳ���8���ֽ�ʱ�����0
 * @param v Ҫ�����ܵ�8���ֽ�
 * @param k ��Կ(16���ֽ�)
 * @return v ���ܺ������Ҳ������v�����У�����v��һ��ָ�����
 */
static void TeaEncode(TeaContextPtr contextPtr,int v[2]);


/***********************************************************
 * ÿ�ν���8���ֽ�
 * @param v �����ܣ���Ҫ���ܵ�����
 * @param k ����ʱ�õ���Կ�����ڽ���
 * @return
 * ���ܺ�����������ĵȳ������޸�����ָ�������
 ***********************************************************/
static void TeaDecode(TeaContextPtr contextPtr,int v[2]);

//XXTea�㷨����
static void XXTeaEncode(TeaContextPtr contextPtr,int* v,unsigned int nSize);


//XXTea�㷨����
static void XXTeaDecode(TeaContextPtr contextPtr,int* v,unsigned int nSize);

//static long k[4] = {0};
//static unsigned int k[4] = {0};


//TEA�ļ����㷨�൱�򵥣�����������Ҫ��ʼ��
void TeaInit(TeaContextPtr context)
{
	//memset(k,0,MAXKEYLEN);//16���ֽڳ���
	//memcpy(k,pKey,(nLen > MAXKEYLEN) ? MAXKEYLEN : nLen);
}

void TeaSetKey(TeaContextPtr context,char* pKey,unsigned int nLen)
{
	memset(context->key,0,MAXKEYLEN);//16���ֽڳ���
	memcpy(context->key,pKey,(nLen > MAXKEYLEN) ? MAXKEYLEN : nLen);
}

void TeaEncode(TeaContextPtr context,int v[2])
{
	int v0 = v[0];
	int v1 = v[1];
	unsigned int sum=0;
	int i;
	//32���û�
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
	unsigned int y = v[0];//��һ����
	unsigned int sum = 0;
	unsigned int len = nSize - 1; 
	unsigned int z = v[len];//���һ����

	if (nSize > 1) {
		q = 6 + 52 / nSize;
		while (q-- > 0) {//q�ּ���,����������Խ�̼�������Խ��
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
	unsigned int y = v[0];//��һ����
	unsigned int sum = 0;
	unsigned int len = nSize - 1; 
	unsigned int z = v[len];//���һ����
	
	if (nSize > 1) {
		q = 6 + 52 / nSize;
		sum = q * TEADELTA;
		while (sum != 0) {//q�ּ���,����������Խ�̼�������Խ��
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
	loop = CHECKDIV8(nSize) / 8;//��Ҫ����Ĵ���
	memset(pOutput,0,CHECKDIV8(nSize));//���ֲ������ŵ�������
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
	//����8�ı����Ͳ�����������
	if ((nSize % 8) != 0)
	{
		return;
	}
	loop = nSize / 8;//��Ҫ����Ĵ���
	memset(pOutput,0,nSize);//���ֲ������ŵ�������
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