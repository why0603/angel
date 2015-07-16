/******************************************************************************************
 * @file Rijndael.h
 * @anthor:Croky.Zheng
 * @CreateTime:2013-5-4
 * @descirption: Rijindael算法
 * 高级加密算法，就是AES
 *****************************************************************************************/
#ifndef BELIAL_CRYPT_RIJNDAEL_H
#define BELIAL_CRYPT_RIJNDAEL_H
#include "typedef.h"
#include "define.h"

//算法的密钥最长字节数
#define RIJNDAELKEYMAXLEN	32

//使用rijndael加密算法，输入的字符数需要的解密串长度
//这里取最大的值，实际情况可能并不需要这么多
#define ENRIJNDAELSIZE(x) CHECKDIV16(x)

#define DERIJNDAELSIZE(x) (x)

//AES算法中的密钥和明文长度,只能使128,192,256 3种
typedef enum
{
	KeySize_128 = 16,
	KeySize_192 = 24,//tea
	KeySize_256 = 32
} KeySize;

typedef struct
{
	//密钥,使用最长的方式
	unsigned char cryptKey[32];
	//密钥长度,只有128,192,256位三种
	KeySize keySize;
	//每次计算的明文长度,一样只有128,192,256三种
	KeySize plainSize;
	//产生的轮密钥,nr最大为14，nb最大为8,则需要的矩阵列为nb*(nr+1)=8*15=120
	unsigned long roundKey[120];
	//明文的最大长度,字长（以32位记)
	unsigned long nb;
	//轮数
	unsigned long nr;
	//密文的字长
	unsigned long nk;
} RijndaelContext,*RijndaelContextPtr;

#ifdef __cplusplus
extern "C" {	//兼容C++
#endif

/********************************************
 * 初始化RijnDael算法
 * param pContext MD5Context结构体
 *******************************************/
EXTERN void RijndaelInit(RijndaelContextPtr contextPtr);


/********************************************
 * RijnDael加密
 * param contextPtr RijnDaelContext结构体
 * param pInput 输入,需要加密的数据
 * param nSize 输入长度
 * param pOutput 输出
 *******************************************/
EXTERN void RijndaelEncrypt(RijndaelContextPtr contextPtr,void* pInput,unsigned int nSize,void* pOutput);


/********************************************
 * RijnDael解密
 * param contextPtr RijnDaelContext结构体
 * param pInput 输入
 * param nSize 输入长度
 * param pOutput 输出
 *******************************************/
EXTERN void RijndaelDecrypt(RijndaelContextPtr contextPtr,void* pInput,unsigned int nSize,void* pOutput);
//EXTERN void test();
#ifdef __cplusplus
}
#endif
#endif