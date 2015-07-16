/******************************************************************************************
 * @file RC.h
 * @anthor:Croky.Zheng
 * @CreateTime:2012-3-20
 * @descirption: RC5,6算法
 *****************************************************************************************/
#ifndef BELIAL_CRYPT_RC_H
#define BELIAL_CRYPT_RC_H
#include "typedef.h"
#include "define.h"

//rc5编码后的长度
#define ENRC5SIZE(x)	CHECKDIV8(x)
//rc6编码后的长度
#define ENRC6SIZE(x)	CHECKDIV16(x)

//RC5解码后的长度
#define DERC5SIZE(x)	(x)
//RC6解码后的长度
#define DERC6SIZE(x)	(x)


typedef enum __enum_Rc_MODE{
	Rc_Mode_Min = -1,
	RC_Mode_RC5 = 0,//tea
	RC_Mode_RC6 = 1,//xtea
	Rc_Mode_Max
} RcMode;

typedef struct {
	//字长 位
	unsigned int nWordSizeBit;

	//加密轮数
	unsigned int nRound;

	//字长 字节
	unsigned int nWordSizeByte;
	
	//密钥
	unsigned char *pKey;

	//密钥长度
	unsigned int nKeyLen;

	//移位量=log2(nWordSizeBit)
	unsigned int nShiftCount;

	//密钥分组
	unsigned long* pSubKey;

	//密钥分组长度
	unsigned int nSubKeyLen;

	//加密模式
	RcMode mode;
} RCContext,*PRCContext;


#ifdef __cplusplus
extern "C" {	//兼容C++
#endif
/********************************************
 * 初始化RC6算法
 * param pContext MD5Context结构体
 *******************************************/
EXTERN void RCInit(PRCContext pContext);

/***********************************************************
 * 将字符串使用rc5算法加密
 * @param pContext 算法初始化时候的上下文
 * @param pInput  需要加密的明文
 * @param pOutput	加密后的密文保存的指针
 * @param nSize	明文的长度,长度不是8的倍数时，将增加字节，值为0
 * @return 
 ***********************************************************/
EXTERN	void RCEncrypt(PRCContext pContext,
	void* pInput,
	unsigned int nSize,
	void* pOutput);

/***********************************************************
 * 将字符串使用TEA算法加密
 * @param pContext 算法初始化时候的上下文
 * @param pInput  密文
 * @param pOutput 解密后的明文
 * @param nSize	密文的长度
 * @return 
 ***********************************************************/
EXTERN	void RCDecrypt(PRCContext pContext,
	void* pInput,
	unsigned int nSize,
	void* pOutput);

EXTERN void RCDestroy(PRCContext pContext);

//CYEXTERN void RC6SetKey(PRC6Context pContext,char* pKey,unsigned int nLen);
#ifdef __cplusplus
}
#endif
#endif