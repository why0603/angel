/******************************************************************************************
 * @file hash.h
 * @anthor:Croky.Zheng
 * @CreateTime:
 * @descirption:�ַ��������һЩ����
 *****************************************************************************************/
#ifndef BELIAL_COMMON_HASH_H
#define BELIAL_COMMON_HASH_H
#include "typedef.h"
#include "define.h"


#ifdef __cplusplus
extern "C" {	//����C++
#endif

EXTERN unsigned long Hash1(char* pStr,unsigned long nLen);

EXTERN unsigned long Hash2(char* pStr,unsigned long nLen);

EXTERN unsigned long Hash3(char* pStr,unsigned long nLen);

EXTERN unsigned long Hash4(char* pStr,unsigned long nLen);

EXTERN unsigned long Hash5(char* pStr,unsigned long nLen);

EXTERN unsigned long Hash6(char* pStr,unsigned long nLen);

EXTERN unsigned long Hash7(char* pStr,unsigned long nLen);
#ifdef __cplusplus
}
#endif
#endif