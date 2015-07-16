/******************************************************************************************
 * @file Sha.h
 * @anthor:Croky.Zheng
 * @CreateTime:2012-2-25
 * @descirption: SHA1�㷨
 *****************************************************************************************/
#ifndef BELIAL_CRYPT_SHA_H
#define BELIAL_CRYPT_SHA_H
#include "typedef.h"
#include "define.h"
/* SHA context. */
typedef struct {
  unsigned int state[5];/* state (ABCDE) */
  unsigned int count[2];/* number of bits, modulo 2^64 (lsb first) */
  unsigned char buffer[64];/* input buffer �����µ��ַ�����64���ֽڵĲ��֣�����������*/
} ShaContext,*PShaContext;

#ifdef __cplusplus
extern "C" {	//����C++
#endif
/********************************************
 * ��ʼ��MD5����,����״ֵ̬�����ȣ��������Ϣ
 * param pContext MD5_Context�ṹ��
 *******************************************/
EXTERN void ShaInit(PShaContext pContext);

/********************************************
 * MD5����
 * @param pContext ����״̬�Ľṹ��,ͬ��ʼ����ͬ
 * @param pInput �������������
 * @param nLen ��������ĳ���
 *******************************************/
EXTERN void ShaUpdate(
		PShaContext pContext,
		unsigned char* pInput,
		unsigned int nLen);

/********************************************
 * MD5�������
 * ��һ����Ҫ�����������MD5���㣬������״̬
 * @param digest ��Ž��
 * @param pContext ״̬�ṹ�壬ͬ��ʼ����ͬ
 *******************************************/
EXTERN void ShaFinal(
		PShaContext pContext,
		unsigned char digest[16]);

#ifdef __cplusplus
}//����C++
#endif
#endif