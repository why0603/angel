/******************************************************************************************
 * @file Md5.h
 * @anthor:Croky.Zheng
 * @CreateTime:2012-2-21
 * @descirption: MD5算法
 *****************************************************************************************/
#ifndef BELIAL_CRYPT_MD5_H
#define BELIAL_CRYPT_MD5_H
#include "typedef.h"
#include "define.h"

/* MD5 context. */
typedef struct {
  unsigned int state[4];/* state (ABCD) */
  unsigned int count[2];/* number of bits, modulo 2^64 (lsb first) */
  unsigned char buffer[64];/* input buffer 将更新的字符不足64个字节的部分，保存在这里*/
} MD5Context,*PMD5Context;

#ifdef __cplusplus
extern "C" {	//兼容C++
#endif
/********************************************
 * 初始化MD5运算,设置状态值，长度，缓存等信息
 * param pContext MD5Context结构体
 *******************************************/
EXTERN void MD5Init(PMD5Context pContext);

/********************************************
 * MD5运算
 * @param pContext 保存状态的结构体,同初始化相同
 * @param pInput 参与运算的数据
 * @param nLen 参与运算的长度
 *******************************************/
EXTERN void MD5Update(
		PMD5Context pContext,
		unsigned char* pInput,
		unsigned int nLen);

/********************************************
 * MD5运算结束
 * 这一步主要对填充数据做MD5运算，并重置状态
 * @param digest 存放结果
 * @param pContext 状态结构体，同初始化相同
 *******************************************/
EXTERN void MD5Final(
		PMD5Context pContext,
		unsigned char digest[16]);
#ifdef __cplusplus
}//兼容C++
#endif
#endif