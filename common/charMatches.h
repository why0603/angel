/******************************************************************************************
 * @file charMatches.h
 * @anthor:Croky.Zheng
 * @CreateTime:2010-05-05
 * @descirption:用于字符串匹配，比如：朴素算法，KMP匹配，AC自动机，后缀树，trie树等 声明
 *****************************************************************************************/
#ifndef BELIAL_COMMON_CHARMATCHES_H
#define BELIAL_COMMON_CHARMATCHES_H
#include "typedef.h"
#include "define.h"


#ifdef __cplusplus
extern "C" {	//兼容C++
#endif


/**
 * 使用朴素算法查找字符串的子串
 * 字符串为ASCII形式
 * 朴素算法的最坏情况为O((n-m+1)*n)次
 */
EXTERN	int	CNaiveIndexOf(
									PCHAR pszSource,			//文本串
									//INT32 sourceOffset,	//文本串起始的偏移地址
									int nSourceLen,	//文本串参与匹配的长度
									PCHAR pszTarget,			//模式串
									//INT32 targetOffset,	//模式串偏移
									int nTargetLen	//模式串需要匹配的长度
								);

/**
 * 使用朴素算法查找字符串的子串
 * 字符串为 UNICODE 形式
 */
EXTERN	int	WNaiveIndexOf(
									PWCHAR pwszSource,			//文本串
									//INT32 sourceOffset,	//文本串起始的偏移地址
									int nSourceLen,	//文本串参与匹配的长度
									PWCHAR pwszTarget,			//模式串
									//INT32 targetOffset,	//模式串偏移
									int nTargetLen	//模式串需要匹配的长度
								);

/**
 * 使用朴素算法查找字符串的子串
 * 字符串为 整型32位 形式
 */
EXTERN	int	NNaiveIndexOf(
									PINT32 pnSource,			//文本串
									//INT32 sourceOffset,	//文本串起始的偏移地址
									int nSourceLen,		//文本串参与匹配的长度
									PINT32 pnTarget,			//模式串
									//INT32 targetOffset,	//模式串偏移
									int nTargetLen		//模式串需要匹配的长度
								);

/**
 * 使用朴素算法查找字符串的子串
 * 字符串为 整型64位 形式
 */
EXTERN	int	DNNaiveIndexOf(
									PINT64 pdnSource,			//文本串
									//INT32 sourceOffset,		//文本串起始的偏移地址
									int nSourceLen,			//文本串参与匹配的长度
									PINT64 pdnTarget,			//模式串
									//INT32 targetOffset,		//模式串偏移
									int nTargetLen			//模式串需要匹配的长度
								);

/**
 * 计算KMP匹配算法中的模式项
 * 比如模式串为 ababcde
 * 计算出每一次的有限状态
 * pszTarget 模式串
 */
EXTERN int* CGetKmpPattern(
							   PCHAR pszTarget,
							   int nTargetLen
							  );

/**
 * 计算KMP匹配算法中的模式项
 * 比如模式串为 ababcde
 * 计算出每一次的有限状态
 * pwszTarget Unicode编码的模式串
 */
EXTERN int* WGetKmpPattern(
							   PWCHAR pwszTarget,
							   int nTargetLen
							  );

/**
 * 计算KMP匹配算法中的模式项
 * 比如模式串为 ababcde
 * 计算出每一次的有限状态
 * pnTarget 模式串为32位整型数据
 */
EXTERN int* NGetKmpPattern(
							   PINT32 pnTarget,
							   int nTargetLen
							  );

/**
 * 计算KMP匹配算法中的模式项
 * 比如模式串为 ababcde
 * 计算出每一次的有限状态
 * pdnTarget 模式串为64位整型数据
 */
EXTERN int* DNGetKmpPattern(
							   PINT64 pdnTarget,
							   int nTargetLen
							  );

/**
 * 释放模式计算结果
 */
EXTERN void FreeKmpPattern(
							   int* pnPattern
							  );
/**
 * 使用KMP算法查找字符串的子串
 * 字符串为ASCII形式
 * 朴素算法的最坏情况为O((n-m+1)*n)次
 */
EXTERN	int	CKmpIndexOf(
									PCHAR pszSource,			//文本串
									//INT32 sourceOffset,	//文本串起始的偏移地址
									int nSourceLen,	//文本串参与匹配的长度
									PCHAR pszTarget,			//模式串
									//INT32 targetOffset,	//模式串偏移
									int nTargetLen,	//模式串需要匹配的长度
									int* pPattern
								);

/**
 * 算法查找字符串的子串
 * 字符串为 UNICODE 形式
 */
EXTERN	int	WKmpIndexOf(
									PWCHAR pwszSource,			//文本串
									//INT32 sourceOffset,	//文本串起始的偏移地址
									int nSourceLen,	//文本串参与匹配的长度
									PWCHAR pwszTarget,			//模式串
									//INT32 targetOffset,	//模式串偏移
									int nTargetLen,	//模式串需要匹配的长度
									int* pPattern
								);

/**
 * 算法查找字符串的子串
 * 字符串为 整型32位 形式
 */
EXTERN	int	NKmpIndexOf(
									PINT32 pnSource,			//文本串
									//INT32 sourceOffset,	//文本串起始的偏移地址
									int nSourceLen,		//文本串参与匹配的长度
									PINT32 pnTarget,			//模式串
									//INT32 targetOffset,	//模式串偏移
									int nTargetLen,		//模式串需要匹配的长度
									int* pPattern
								);

/**
 * 算法查找字符串的子串
 * 字符串为 整型64位 形式
 */
EXTERN	int	DNKmpIndexOf(
									PINT64 pdnSource,			//文本串
									//INT32 sourceOffset,		//文本串起始的偏移地址
									int nSourceLen,			//文本串参与匹配的长度
									PINT64 pdnTarget,			//模式串
									//INT32 targetOffset,		//模式串偏移
									int nTargetLen,			//模式串需要匹配的长度
									int* pPattern
							  );
#ifdef __cplusplus
}	//兼容C++
#endif

#endif