/******************************************************************************************
 * @file keccak.h
 * @anthor:Croky.Zheng
 * @CreateTime:2013-5-15
 * @descirption: һ������MD5��SHA�㷨
 * ��ʵΪSHA-3�㷨,��224��256,384,512�����������
 *****************************************************************************************/
#ifndef BELIAL_CRYPT_KECCARK_H
#define BELIAL_CRYPT_KECCARK_H
#include "typedef.h"
#include "define.h"


/* MD5 context. */
typedef struct {
  unsigned int state[4];/* state (ABCD) */
  unsigned int count[2];/* number of bits, modulo 2^64 (lsb first) */
  unsigned char buffer[64];/* input buffer �����µ��ַ�����64���ֽڵĲ��֣�����������*/
} KeccakContext,*KeccakContextPtr;

#ifdef __cplusplus
extern "C" {	//����C++
#endif


#ifdef __cplusplus
}//����C++
#endif
#endif