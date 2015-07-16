/******************************************************************************************
 * @file charMatches.h
 * @anthor:Croky.Zheng
 * @CreateTime:2010-05-05
 * @descirption:�����ַ���ƥ�䣬���磺�����㷨��KMPƥ�䣬AC�Զ�������׺����trie���� ����
 *****************************************************************************************/
#ifndef BELIAL_COMMON_CHARMATCHES_H
#define BELIAL_COMMON_CHARMATCHES_H
#include "typedef.h"
#include "define.h"


#ifdef __cplusplus
extern "C" {	//����C++
#endif


/**
 * ʹ�������㷨�����ַ������Ӵ�
 * �ַ���ΪASCII��ʽ
 * �����㷨������ΪO((n-m+1)*n)��
 */
EXTERN	int	CNaiveIndexOf(
									PCHAR pszSource,			//�ı���
									//INT32 sourceOffset,	//�ı�����ʼ��ƫ�Ƶ�ַ
									int nSourceLen,	//�ı�������ƥ��ĳ���
									PCHAR pszTarget,			//ģʽ��
									//INT32 targetOffset,	//ģʽ��ƫ��
									int nTargetLen	//ģʽ����Ҫƥ��ĳ���
								);

/**
 * ʹ�������㷨�����ַ������Ӵ�
 * �ַ���Ϊ UNICODE ��ʽ
 */
EXTERN	int	WNaiveIndexOf(
									PWCHAR pwszSource,			//�ı���
									//INT32 sourceOffset,	//�ı�����ʼ��ƫ�Ƶ�ַ
									int nSourceLen,	//�ı�������ƥ��ĳ���
									PWCHAR pwszTarget,			//ģʽ��
									//INT32 targetOffset,	//ģʽ��ƫ��
									int nTargetLen	//ģʽ����Ҫƥ��ĳ���
								);

/**
 * ʹ�������㷨�����ַ������Ӵ�
 * �ַ���Ϊ ����32λ ��ʽ
 */
EXTERN	int	NNaiveIndexOf(
									PINT32 pnSource,			//�ı���
									//INT32 sourceOffset,	//�ı�����ʼ��ƫ�Ƶ�ַ
									int nSourceLen,		//�ı�������ƥ��ĳ���
									PINT32 pnTarget,			//ģʽ��
									//INT32 targetOffset,	//ģʽ��ƫ��
									int nTargetLen		//ģʽ����Ҫƥ��ĳ���
								);

/**
 * ʹ�������㷨�����ַ������Ӵ�
 * �ַ���Ϊ ����64λ ��ʽ
 */
EXTERN	int	DNNaiveIndexOf(
									PINT64 pdnSource,			//�ı���
									//INT32 sourceOffset,		//�ı�����ʼ��ƫ�Ƶ�ַ
									int nSourceLen,			//�ı�������ƥ��ĳ���
									PINT64 pdnTarget,			//ģʽ��
									//INT32 targetOffset,		//ģʽ��ƫ��
									int nTargetLen			//ģʽ����Ҫƥ��ĳ���
								);

/**
 * ����KMPƥ���㷨�е�ģʽ��
 * ����ģʽ��Ϊ ababcde
 * �����ÿһ�ε�����״̬
 * pszTarget ģʽ��
 */
EXTERN int* CGetKmpPattern(
							   PCHAR pszTarget,
							   int nTargetLen
							  );

/**
 * ����KMPƥ���㷨�е�ģʽ��
 * ����ģʽ��Ϊ ababcde
 * �����ÿһ�ε�����״̬
 * pwszTarget Unicode�����ģʽ��
 */
EXTERN int* WGetKmpPattern(
							   PWCHAR pwszTarget,
							   int nTargetLen
							  );

/**
 * ����KMPƥ���㷨�е�ģʽ��
 * ����ģʽ��Ϊ ababcde
 * �����ÿһ�ε�����״̬
 * pnTarget ģʽ��Ϊ32λ��������
 */
EXTERN int* NGetKmpPattern(
							   PINT32 pnTarget,
							   int nTargetLen
							  );

/**
 * ����KMPƥ���㷨�е�ģʽ��
 * ����ģʽ��Ϊ ababcde
 * �����ÿһ�ε�����״̬
 * pdnTarget ģʽ��Ϊ64λ��������
 */
EXTERN int* DNGetKmpPattern(
							   PINT64 pdnTarget,
							   int nTargetLen
							  );

/**
 * �ͷ�ģʽ������
 */
EXTERN void FreeKmpPattern(
							   int* pnPattern
							  );
/**
 * ʹ��KMP�㷨�����ַ������Ӵ�
 * �ַ���ΪASCII��ʽ
 * �����㷨������ΪO((n-m+1)*n)��
 */
EXTERN	int	CKmpIndexOf(
									PCHAR pszSource,			//�ı���
									//INT32 sourceOffset,	//�ı�����ʼ��ƫ�Ƶ�ַ
									int nSourceLen,	//�ı�������ƥ��ĳ���
									PCHAR pszTarget,			//ģʽ��
									//INT32 targetOffset,	//ģʽ��ƫ��
									int nTargetLen,	//ģʽ����Ҫƥ��ĳ���
									int* pPattern
								);

/**
 * �㷨�����ַ������Ӵ�
 * �ַ���Ϊ UNICODE ��ʽ
 */
EXTERN	int	WKmpIndexOf(
									PWCHAR pwszSource,			//�ı���
									//INT32 sourceOffset,	//�ı�����ʼ��ƫ�Ƶ�ַ
									int nSourceLen,	//�ı�������ƥ��ĳ���
									PWCHAR pwszTarget,			//ģʽ��
									//INT32 targetOffset,	//ģʽ��ƫ��
									int nTargetLen,	//ģʽ����Ҫƥ��ĳ���
									int* pPattern
								);

/**
 * �㷨�����ַ������Ӵ�
 * �ַ���Ϊ ����32λ ��ʽ
 */
EXTERN	int	NKmpIndexOf(
									PINT32 pnSource,			//�ı���
									//INT32 sourceOffset,	//�ı�����ʼ��ƫ�Ƶ�ַ
									int nSourceLen,		//�ı�������ƥ��ĳ���
									PINT32 pnTarget,			//ģʽ��
									//INT32 targetOffset,	//ģʽ��ƫ��
									int nTargetLen,		//ģʽ����Ҫƥ��ĳ���
									int* pPattern
								);

/**
 * �㷨�����ַ������Ӵ�
 * �ַ���Ϊ ����64λ ��ʽ
 */
EXTERN	int	DNKmpIndexOf(
									PINT64 pdnSource,			//�ı���
									//INT32 sourceOffset,		//�ı�����ʼ��ƫ�Ƶ�ַ
									int nSourceLen,			//�ı�������ƥ��ĳ���
									PINT64 pdnTarget,			//ģʽ��
									//INT32 targetOffset,		//ģʽ��ƫ��
									int nTargetLen,			//ģʽ����Ҫƥ��ĳ���
									int* pPattern
							  );
#ifdef __cplusplus
}	//����C++
#endif

#endif