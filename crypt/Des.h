/******************************************************************************************
 * @file Des.h
 * @anthor:Croky.Zheng
 * @CreateTime:2011-8-5
 * @descirption: des加密算法
 *****************************************************************************************/
#ifndef BELIAL_CRYPT_DES_H
#define BELIAL_CRYPT_DES_H
#include "define.h"
#include "typedef.h"


typedef enum __enum_Des_MODE
{
	Des_Mode_MIN = -1,
	Des_Mode_ECB = 0,//普通加密方式
	Des_Mode_BCB = 1,//ECB方式，//以上个密文组异或之后再加密
	Des_Mode_MAX
} DesMode;

typedef struct
{
	char key[8];
	INT64 subkey[16];
	DesMode mode;
}DesContext,*DesContextPtr;

#ifdef __cplusplus
extern "C" {	//兼容C++
#endif

/**************************************************************
 * 加密算法初始化，需要使用加密密钥
 * @param pKey 密钥
 * @param nSize 密钥长度
 *************************************************************/
EXTERN void DesInit(DesContextPtr contextPtr);

/*********************************************************
 * 对数据块做DES加密
 * DES使用56位密钥加密64位明文，输出64位密文
 * @param pSrc 明文块地址
 * @param srcLen 明文数据块长度
 * @param pDes 密文块地址
 * @param desLen 存储密文块的长度
 * @param pKey 密钥地址
 * @param keyLen 密钥长度
 * @return 密文的长度
 *********************************************************/
EXTERN void DesEncrypt(DesContextPtr contextPtr,
	void* pInput,
	unsigned int srcLen,
	void* pOutput);


/*********************************************************
 * 对数据块做DES解密,被解密的数据块一定要是8的整数倍
 * DES使用56位密钥加密64位明文，输出64位密文
 * @param pSrc 密文块地址
 * @param srcLen 密文数据块长度
 * @param pDes 明文块地址
 * @param desLen 存储明文块的长度
 * @param pKey 密钥地址
 * @param keyLen 密钥长度
 * @return 明文的长度
 *********************************************************/
EXTERN void DesDecrypt(DesContextPtr contextPtr,
	void* pInput,
	unsigned int srcLen,
	void* pOutput);



#ifdef __cplusplus
}//兼容C++
#endif
#endif