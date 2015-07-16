/******************************************************************************************
 * @file Base64.h
 * @anthor:Croky.Zheng
 * @CreateTime:2012-2-27
 * @descirption: BASE64编码算法,base64只是简单的编码转换，和早期的加密算法倒是很相似
 *****************************************************************************************/
#ifndef BELIAL_CRYPT_BASE64_H
#define BELIAL_CRYPT_BASE64_H
#include "typedef.h"
#include "define.h"

//计算BASE64编码后的长度
#define ENBASE64SIZE(x) (((x)/3)*4 + ((x)%3==0?0:4))

//计算BASE64解码需要的长度
#define DEBASE64SIZE(x) ((x)/4)*3)

typedef enum
{
	Base64_Mode_MIN = -1,
	Base64_Mode_PEM = 0,//原始
	Base64_Mode_URL = 1,//URL变种
	Base64_Mode_REG = 2,//正则表达式变种
	Base64_Mode_MAX
} Base64Mode;


#ifdef __cplusplus
extern "C" {	//兼容C++
#endif


/***********************************************************
 * 将字节流按照BASE64编码
 * @param pInput  要编码的数据地址
 * @param pOutput	BASE64编码之后的保存地址
 * @param nSize	字节流的字节个数
 * @param mode 编码的模式
 ***********************************************************/
EXTERN unsigned int Base64Encrypt(unsigned char* pInput,
	unsigned int nSize,//输入的字节数
	unsigned char* pOutput,
	Base64Mode mode);

/***********************************************************
 * 将字节流按照BASE64解码
 * @param pInput  要解码的地址
 * @param pOutput	BASE64解码之后的保存地址
 * @param nSize	字节流的字节个数
 * @param mode 编码的模式
 ***********************************************************/
EXTERN unsigned int Base64Decrypt(unsigned char* pInput,
	unsigned int nSize,//输入的字节数
	unsigned char* pOutput,
	Base64Mode mode);

#ifdef __cplusplus
}//兼容C++
#endif
#endif