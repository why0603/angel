/******************************************************************************************
 * @file Rijndael.h
 * @anthor:Croky.Zheng
 * @CreateTime:2013-5-4
 * @descirption: Rijindael�㷨
 * �߼������㷨������AES
 *****************************************************************************************/
#ifndef BELIAL_CRYPT_RIJNDAEL_H
#define BELIAL_CRYPT_RIJNDAEL_H
#include "typedef.h"
#include "define.h"

//�㷨����Կ��ֽ���
#define RIJNDAELKEYMAXLEN	32

//ʹ��rijndael�����㷨��������ַ�����Ҫ�Ľ��ܴ�����
//����ȡ����ֵ��ʵ��������ܲ�����Ҫ��ô��
#define ENRIJNDAELSIZE(x) CHECKDIV16(x)

#define DERIJNDAELSIZE(x) (x)

//AES�㷨�е���Կ�����ĳ���,ֻ��ʹ128,192,256 3��
typedef enum
{
	KeySize_128 = 16,
	KeySize_192 = 24,//tea
	KeySize_256 = 32
} KeySize;

typedef struct
{
	//��Կ,ʹ����ķ�ʽ
	unsigned char cryptKey[32];
	//��Կ����,ֻ��128,192,256λ����
	KeySize keySize;
	//ÿ�μ�������ĳ���,һ��ֻ��128,192,256����
	KeySize plainSize;
	//����������Կ,nr���Ϊ14��nb���Ϊ8,����Ҫ�ľ�����Ϊnb*(nr+1)=8*15=120
	unsigned long roundKey[120];
	//���ĵ���󳤶�,�ֳ�����32λ��)
	unsigned long nb;
	//����
	unsigned long nr;
	//���ĵ��ֳ�
	unsigned long nk;
} RijndaelContext,*RijndaelContextPtr;

#ifdef __cplusplus
extern "C" {	//����C++
#endif

/********************************************
 * ��ʼ��RijnDael�㷨
 * param pContext MD5Context�ṹ��
 *******************************************/
EXTERN void RijndaelInit(RijndaelContextPtr contextPtr);


/********************************************
 * RijnDael����
 * param contextPtr RijnDaelContext�ṹ��
 * param pInput ����,��Ҫ���ܵ�����
 * param nSize ���볤��
 * param pOutput ���
 *******************************************/
EXTERN void RijndaelEncrypt(RijndaelContextPtr contextPtr,void* pInput,unsigned int nSize,void* pOutput);


/********************************************
 * RijnDael����
 * param contextPtr RijnDaelContext�ṹ��
 * param pInput ����
 * param nSize ���볤��
 * param pOutput ���
 *******************************************/
EXTERN void RijndaelDecrypt(RijndaelContextPtr contextPtr,void* pInput,unsigned int nSize,void* pOutput);
//EXTERN void test();
#ifdef __cplusplus
}
#endif
#endif