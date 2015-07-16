/******************************************************************************************
 * @file Tea.h
 * @anthor:Croky.Zheng
 * @CreateTime:2011/8/3
 * @descirption:使用Tea+算法加密，解密数据
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
extern "C" {	//兼容C++
#endif

/**************************************************************
 * 加密算法初始化，需要使用加密密钥
 * @param pKey 密钥
 * @param nSize 密钥长度
 *************************************************************/
EXTERN void TeaInit(TeaContextPtr contextPtr);

/**
 * 设置密码，最长16个字节
 */
EXTERN void TeaSetKey(TeaContextPtr context,char* pKey,unsigned int nLen);

/***********************************************************
 * 将字符串使用TEA算法加密
 * @param pInput  密文
 * @param pOutput 解密后的明文
 * @param nSize	密文的长度
 * @param mode 加密的模式
 * @return 
 ***********************************************************/
EXTERN	void TeaDecrypt(TeaContextPtr contextPtr,
	void* pInput,
	unsigned int nSize,
	void* pOutput);


/***********************************************************
 * 将字符串使用TEA算法加密
 * @param pInput  需要加密的明文
 * @param pOutput	加密后的密文保存的指针
 * @param nSize	明文的长度,长度不是8的倍数时，将增加字节，值为0
 * @param mode 加密的模式
 * @return 
 ***********************************************************/
EXTERN	void TeaEncrypt(TeaContextPtr contextPtr,
	void* pInput,
	unsigned int nSize,
	void* pOutput);
#ifdef __cplusplus
}//兼容C++
#endif
#endif