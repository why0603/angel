/******************************************************************************************
 * @file keccak.h
 * @anthor:Croky.Zheng
 * @CreateTime:2013-5-15
 * @descirption: 一个类似MD5，SHA算法
 * 其实为SHA-3算法,有224，256,384,512四种输出长度
 *****************************************************************************************/
#ifndef BELIAL_CRYPT_KECCARK_H
#define BELIAL_CRYPT_KECCARK_H
#include "typedef.h"
#include "define.h"


/* MD5 context. */
typedef struct {
  unsigned int state[4];/* state (ABCD) */
  unsigned int count[2];/* number of bits, modulo 2^64 (lsb first) */
  unsigned char buffer[64];/* input buffer 将更新的字符不足64个字节的部分，保存在这里*/
} KeccakContext,*KeccakContextPtr;

#ifdef __cplusplus
extern "C" {	//兼容C++
#endif


#ifdef __cplusplus
}//兼容C++
#endif
#endif