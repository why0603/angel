/******************************************************************************************
 * @file charUtils.h
 * @anthor:Croky.Zheng
 * @CreateTime:
 * @descirption:字符串处理的一些函数
 *****************************************************************************************/
#ifndef BELIAL_COMMON_CHARUTILS_H
#define BELIAL_COMMON_CHARUTILS_H
#include "typedef.h"
#include "define.h"

#define ISDIGIT(c) ((((c)>='0') && ((c)<='9')) ? TRUE:FALSE)
#define ISUPPER(c) ((((c)>='A') && ((c)<='Z')) ? TRUE:FALSE)
#define ISLOWER(c) ((((c)>='a') && ((c)<='z')) ? TRUE:FALSE)
#define ISLETTER(c) (ISUPPER(c) | ISLOWER(c))
#define ISLETTERORDIGHT(c) (ISLETTER(c) | ISDIGIT(c))

#define ISOCTLETTER(c) ((((c)>='0') && ((c)<='7')) ? TRUE:FALSE)
#define ISBINLETTER(c) ((((c)=='1') || ((c)=='0')) ? TRUE:FALSE)
#ifdef __cplusplus
extern "C" {	//兼容C++
#endif
//将数字转换为16进制字符格式
EXTERN	UINT8 Int2HexStr(UINT64 number,PCHAR pStr);

//将数字转换为2进制字符格式
EXTERN	UINT8	Int2BinStr(UINT64 number,PCHAR pStr);

//将数字转换为8进制字符格式
EXTERN	UINT8	Int2OctStr(UINT64 number,PCHAR pStr);

//将数字转换为10进制字符格式
EXTERN	UINT8	Int2DecStr(INT32 number,PCHAR pStr);

//按照指定的进制将数字转换为字符
EXTERN	UINT8	Int2Str(UINT64 number,INT32 shift,PCHAR pStr);

/********************************************************
 * 将字节流转化为16进制的字符串
 * @param pInput 需要转化的字节流
 * @param pOutput 转化后的结果输出
 * @param nLen pInput的长度
 * @param cDelimiter 字节分隔符
 * @return 转化后的长度
 *******************************************************/
EXTERN UINT Byte2Hex(PBYTE pInput,
	PCHAR pOutput,
	UINT nLen,
	CHAR cDelimiter);
/*
EXTERN BOOL IsDigit(CHAR c);//字符是否属于数字
EXTERN BOOL IsUpper(CHAR c);//是否大写字母
EXTERN BOOL IsLower(CHAR c);//是否小写字母
EXTERN BOOL IsLetter(CHAR c); //是否是字母
EXTERN BOOL IsLetterOrDigit(CHAR c); //是否是字母或者数字
*/
//返回字符串的长度，以0为结束
EXTERN	UINT32 StrLen(PCHAR pStr);

//从pStr 拷贝 字符串到 pDest
EXTERN	PCHAR StrCopy(PCHAR pSrc,PCHAR pDest,UINT32 srcLen);

//比较两个字符串是否相等
EXTERN	BOOL StrEqual(PCHAR pSrc,PCHAR pDest);

//比较两个字符串是否相等
EXTERN	INT32 StrComp(PCHAR pSrc,PCHAR pDest);

//获得一个字符串的HASH值
EXTERN	UINT32 Hash(PCHAR key);//,UINT32 length) ;

/**
 * 将16进制的字符串转为整型数据
 * @param pStr 要转换的字符串
 * @return 转换后的数据
 */
EXTERN	INT32 HexStr2Int(PCHAR pStr);
/**
 * 将8进制的字符串转为整型数据
 * @param pStr 要转换的字符串
 * @return 转换后的数据
 */
EXTERN	INT32 OctStr2Int(PCHAR pStr);
/**
 * 将2进制的字符串转为整型数据
 * @param pStr 要转换的字符串
 * @return 转换后的数据
 */
EXTERN	INT32 BinStr2Int(PCHAR pStr);

/**
 * 一个整型数字，的字符长度
 * 比如：124334 的长度为6
 * 7809879098 的长度为10
 * @param n 数字
 * @return 数字n的字符长度
 */
EXTERN	UINT8	StrSizeOfInt(INT32 n);

/**
 * 将字符串转换按照指定进制转为整型数据
 * @param pStr 要转换的字符串
 * @param shift 进制长度，16为4，8为3，2为1
 * @return 转换后的整型
 */
EXTERN	INT32 Str2Int(PCHAR pStr,INT32 shift);

/**
 * 将字符转为8个字节描述的2进制方式
 * @param ch 要转化的字符
 * @param bit[8] 8个字节存放0.1
 * @return 成功返回0，只会返回0
 */
EXTERN	INT ByteToBit(BYTE ch, BYTE bit[8]);

/**
 * 将字符串描述的0.1转为一个字符
 * @param pCh 转化后的结果
 * @param bit[8] 8个字节存放0.1
 * @return 成功返回0，只会返回0
 */
EXTERN	INT BitToByte(BYTE bit[8],PBYTE pCh);

EXTERN void CopyString(unsigned char dest[],char* src,unsigned int srcLen);
#ifdef __cplusplus
}
#endif
#endif