/******************************************************************************************
 * @file Des.h
 * @anthor:Croky.Zheng
 * @CreateTime:2011-8-5
 * @descirption: des�����㷨
 *****************************************************************************************/
#ifndef BELIAL_CRYPT_DES_H
#define BELIAL_CRYPT_DES_H
#include "define.h"
#include "typedef.h"


typedef enum __enum_Des_MODE
{
	Des_Mode_MIN = -1,
	Des_Mode_ECB = 0,//��ͨ���ܷ�ʽ
	Des_Mode_BCB = 1,//ECB��ʽ��//���ϸ����������֮���ټ���
	Des_Mode_MAX
} DesMode;

typedef struct
{
	char key[8];
	INT64 subkey[16];
	DesMode mode;
}DesContext,*DesContextPtr;

#ifdef __cplusplus
extern "C" {	//����C++
#endif

/**************************************************************
 * �����㷨��ʼ������Ҫʹ�ü�����Կ
 * @param pKey ��Կ
 * @param nSize ��Կ����
 *************************************************************/
EXTERN void DesInit(DesContextPtr contextPtr);

/*********************************************************
 * �����ݿ���DES����
 * DESʹ��56λ��Կ����64λ���ģ����64λ����
 * @param pSrc ���Ŀ��ַ
 * @param srcLen �������ݿ鳤��
 * @param pDes ���Ŀ��ַ
 * @param desLen �洢���Ŀ�ĳ���
 * @param pKey ��Կ��ַ
 * @param keyLen ��Կ����
 * @return ���ĵĳ���
 *********************************************************/
EXTERN void DesEncrypt(DesContextPtr contextPtr,
	void* pInput,
	unsigned int srcLen,
	void* pOutput);


/*********************************************************
 * �����ݿ���DES����,�����ܵ����ݿ�һ��Ҫ��8��������
 * DESʹ��56λ��Կ����64λ���ģ����64λ����
 * @param pSrc ���Ŀ��ַ
 * @param srcLen �������ݿ鳤��
 * @param pDes ���Ŀ��ַ
 * @param desLen �洢���Ŀ�ĳ���
 * @param pKey ��Կ��ַ
 * @param keyLen ��Կ����
 * @return ���ĵĳ���
 *********************************************************/
EXTERN void DesDecrypt(DesContextPtr contextPtr,
	void* pInput,
	unsigned int srcLen,
	void* pOutput);



#ifdef __cplusplus
}//����C++
#endif
#endif