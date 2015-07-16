/******************************************************************************************
 * @file Base64.h
 * @anthor:Croky.Zheng
 * @CreateTime:2012-2-27
 * @descirption: BASE64�����㷨,base64ֻ�Ǽ򵥵ı���ת���������ڵļ����㷨���Ǻ�����
 *****************************************************************************************/
#ifndef BELIAL_CRYPT_BASE64_H
#define BELIAL_CRYPT_BASE64_H
#include "typedef.h"
#include "define.h"

//����BASE64�����ĳ���
#define ENBASE64SIZE(x) (((x)/3)*4 + ((x)%3==0?0:4))

//����BASE64������Ҫ�ĳ���
#define DEBASE64SIZE(x) ((x)/4)*3)

typedef enum
{
	Base64_Mode_MIN = -1,
	Base64_Mode_PEM = 0,//ԭʼ
	Base64_Mode_URL = 1,//URL����
	Base64_Mode_REG = 2,//������ʽ����
	Base64_Mode_MAX
} Base64Mode;


#ifdef __cplusplus
extern "C" {	//����C++
#endif


/***********************************************************
 * ���ֽ�������BASE64����
 * @param pInput  Ҫ��������ݵ�ַ
 * @param pOutput	BASE64����֮��ı����ַ
 * @param nSize	�ֽ������ֽڸ���
 * @param mode �����ģʽ
 ***********************************************************/
EXTERN unsigned int Base64Encrypt(unsigned char* pInput,
	unsigned int nSize,//������ֽ���
	unsigned char* pOutput,
	Base64Mode mode);

/***********************************************************
 * ���ֽ�������BASE64����
 * @param pInput  Ҫ����ĵ�ַ
 * @param pOutput	BASE64����֮��ı����ַ
 * @param nSize	�ֽ������ֽڸ���
 * @param mode �����ģʽ
 ***********************************************************/
EXTERN unsigned int Base64Decrypt(unsigned char* pInput,
	unsigned int nSize,//������ֽ���
	unsigned char* pOutput,
	Base64Mode mode);

#ifdef __cplusplus
}//����C++
#endif
#endif