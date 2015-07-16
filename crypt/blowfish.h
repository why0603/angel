/*********************************************************************
 * @file Blowfish.h
 * @anthor:Croky.Zheng
 * @CreateTime:2011/8/3
 * @descirption:使用BLOWFISH算法加密，解密数据
 **********************************************************************/
#ifndef BELIAL_CRYPT_BLOWFISH_H
#define BELIAL_CRYPT_BLOWFISH_H
#include "define.h"
#include "typedef.h"

typedef enum __enum_BF_mode
{
	BF_Mode_Min = -1,
	BF_Mode_ECB = 0,//直接加密
	BF_Mode_BCB = 1,//以上个密文组异或之后再加密
	BF_Mode_Max
} BlowFishMode;

typedef struct {
	char* pKey;
	unsigned int nLen;
	unsigned int pbox[18];
	unsigned int sbox[4][256];
	BlowFishMode mode;
} BlowFishContext,*PBlowFishContext;

#ifdef __cplusplus
extern "C" {	//兼容C++
#endif

/**************************************************************
 * 加密算法初始化，需要使用加密密钥
 * @param pKey 密钥
 * @param nSize 密钥长度
 *************************************************************/
/*
EXTERN void BlowFishInit(char* pKey,unsigned int nLen);
*/
/**************************************************************
 * 使用BLOWFISH加密数据
 * @param pInput 明文
 * @param nSize 明文长度
 * @param pOutput 密文
 * @param pSize 密文的长度 （返回）
 * @param mode 加密算法的模式 0,1
 *************************************************************/
/*
EXTERN void BlowFishEncrypt(void* pInput,
	unsigned int nSize,
	void* pOutput,
	BlowFishMode mode);
*/
/**************************************************************
 * 使用BLOWFISH解密数据
 * @param pInput 密文
 * @param nSize 密文长度
 * @param pOutput 明文
 * @param pSize 明文的长度 （返回）
 * @param mode 加密算法的模式 0,1
 *************************************************************/
/*
EXTERN void BlowFishDecrypt(void* pInput,
	unsigned int nSize,
	void* pOutput,
	BlowFishMode mode);
*/


/*
 * BlowFishInit的多线程版本
 */
EXTERN void BlowFishInit(PBlowFishContext pContext);

/*
 * BlowFishEncrypt的多线程版本
 */
EXTERN void BlowFishEncrypt(PBlowFishContext pContext,
	void* pInput,
	unsigned int nSize,
	void* pOutput);

/*
 * BlowFishDecrypt的多线程版本
 */
EXTERN void BlowFishDecrypt(PBlowFishContext pContext,
	void* pInput,
	unsigned int nSize,
	void* pOutput);
#ifdef __cplusplus
}//兼容C++
#endif
#endif