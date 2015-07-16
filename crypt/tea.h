/******************************************************************************************
 * @file Tea.h
 * @anthor:Croky.Zheng
 * @CreateTime:2011/8/3
 * @descirption:ʹ��Tea+�㷨���ܣ���������
 *****************************************************************************************/
#ifndef BELIAL_CRYPT_TEA_H
#define BELIAL_CRYPT_TEA_H
#include "typedef.h"
#include "define.h"

typedef enum __enum_Tea_MODE
{
	Tea_Mode = 0,//tea
	XTea_Mode = 1,//xtea
	XXTea_Mode = 2,//xxtea
} TeaMode;

typedef struct
{
	unsigned int key[4];
	TeaMode mode;
}TeaContext,*TeaContextPtr;

#ifdef __cplusplus
extern "C" {	//����C++
#endif

/**************************************************************
 * �����㷨��ʼ������Ҫʹ�ü�����Կ
 * @param pKey ��Կ
 * @param nSize ��Կ����
 *************************************************************/
EXTERN void TeaInit(TeaContextPtr contextPtr);

/**
 * �������룬�16���ֽ�
 */
EXTERN void TeaSetKey(TeaContextPtr context,char* pKey,unsigned int nLen);

/***********************************************************
 * ���ַ���ʹ��TEA�㷨����
 * @param pInput  ����
 * @param pOutput ���ܺ������
 * @param nSize	���ĵĳ���
 * @param mode ���ܵ�ģʽ
 * @return 
 ***********************************************************/
EXTERN	void TeaDecrypt(TeaContextPtr contextPtr,
	void* pInput,
	unsigned int nSize,
	void* pOutput);


/***********************************************************
 * ���ַ���ʹ��TEA�㷨����
 * @param pInput  ��Ҫ���ܵ�����
 * @param pOutput	���ܺ�����ı����ָ��
 * @param nSize	���ĵĳ���,���Ȳ���8�ı���ʱ���������ֽڣ�ֵΪ0
 * @param mode ���ܵ�ģʽ
 * @return 
 ***********************************************************/
EXTERN	void TeaEncrypt(TeaContextPtr contextPtr,
	void* pInput,
	unsigned int nSize,
	void* pOutput);
#ifdef __cplusplus
}//����C++
#endif
#endif