/******************************************************************************************
 * @file Md5.h
 * @anthor:Croky.Zheng
 * @CreateTime:2012-2-21
 * @descirption: MD5�㷨
 *****************************************************************************************/
#ifndef BELIAL_CRYPT_MD5_H
#define BELIAL_CRYPT_MD5_H
#include "typedef.h"
#include "define.h"

/* MD5 context. */
typedef struct {
  unsigned int state[4];/* state (ABCD) */
  unsigned int count[2];/* number of bits, modulo 2^64 (lsb first) */
  unsigned char buffer[64];/* input buffer �����µ��ַ�����64���ֽڵĲ��֣�����������*/
} MD5Context,*PMD5Context;

#ifdef __cplusplus
extern "C" {	//����C++
#endif
/********************************************
 * ��ʼ��MD5����,����״ֵ̬�����ȣ��������Ϣ
 * param pContext MD5Context�ṹ��
 *******************************************/
EXTERN void MD5Init(PMD5Context pContext);

/********************************************
 * MD5����
 * @param pContext ����״̬�Ľṹ��,ͬ��ʼ����ͬ
 * @param pInput �������������
 * @param nLen ��������ĳ���
 *******************************************/
EXTERN void MD5Update(
		PMD5Context pContext,
		unsigned char* pInput,
		unsigned int nLen);

/********************************************
 * MD5�������
 * ��һ����Ҫ�����������MD5���㣬������״̬
 * @param digest ��Ž��
 * @param pContext ״̬�ṹ�壬ͬ��ʼ����ͬ
 *******************************************/
EXTERN void MD5Final(
		PMD5Context pContext,
		unsigned char digest[16]);
#ifdef __cplusplus
}//����C++
#endif
#endif