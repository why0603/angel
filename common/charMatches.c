/******************************************************************************************
* @file charMatches.cpp
* @anthor:Croky.Zheng
* @CreateTime:2010-05-07
* @descirption:�����ַ���ƥ�䣬���磺�����㷨��KMPƥ�䣬AC�Զ�������׺����trie���� ʵ��
* ����C�ļ��еı������������ں�����֮��ͳһ������ɣ�����ֻ��ʹ��CPP�ļ���ʽ
*****************************************************************************************/
#include "charMatches.h"
#include <stdlib.h>
//�����㷨
#define NAIVEOPERATION() \
	max = nSourceLen - nTargetLen;\
	for (i = 0; i <= max; i++) \
		{\
		if (pszSource[i] != first) \
			{\
			while (++i <= max && pszSource[i] != first);\
			}\
			if (i <= max) \
			{\
			j = i + 1;\
			end = j + nTargetLen - 1;\
			for (k = 1; j < end && pszSource[j] == pszTarget[k]; j++, k++);\
			if (j == end) \
				{\
				idx = i;\
				break;\
				}\
			}\
		}

//KMPģʽ��ƥ���㷨
//��һ��ò���е���࣬SO����Ϊ������㷨
//��ʵ�������Ϊ-1���ǶԵ�
#define KMPPATTERNOPERATION123(P,S,L) \
	P[0] = -1;\
	k = 0;\
	for(i=1; i<L; i++)\
	{\
	k = P[i-1];\
	while ((k>=0) && (S[k+1]!=S[i]))\
		{\
		k = P[k];\
		}\
		if (S[k+1] == S[i])\
		{\
		P[i] = k+1;\
		}\
		else \
		{\
		P[i] = 0;\
		}\
	}\

//KMPģʽ�����㷽��
#define KMPPATTERNOPERATION(P,T,TL) \
	P[0] = -1;\
	q = -1;\
	for(i=1; i<TL; i++)\
	{\
	while ((q>=0) && (T[q+1]!=T[i]))\
		{\
		q = P[q];\
		}\
		if (T[q+1] == T[i])\
		{\
		q = q+1;\
		}\
		P[i] = q;\
	}\

//KMP�ı������㷨
//S �ı�
//SL �ı�����
//T ģʽ��
//TL ģʽ������
//P ģʽ��״̬����
#define KMPOPERATION(S,SL,T,TL,P)	\
	q = P[0];\
	for (i=0; i<SL; i++)\
	{\
	while ((q >= 0) && (T[q+1] != S[i]))\
		{\
		q = P[q];\
		}\
		if (T[q+1] == S[i])\
		{\
		q++;\
		if ((q+1) == TL)\
			{\
			idx = (i - TL + 1);\
			break;\
			}\
			continue;\
		}\
		if (q >= 0) i--;\
	}\

typedef enum __enum_type {
	TM_INT8 = 1,	//char
	TM_INT16 = 2,	//wchar_t or short
	TM_INT32 = 4,	//int
	TM_INT64 = 8,	//long long
	TM_MAX = 256
} ETYPEMAP;

//�����㷨ʵ�ֺ���
static int NaiveIndexOf(
	void* pvSource,		//�ı���
	int nSourceLen,	//�ı�������ƥ��ĳ���
	void* pvTarget,		//ģʽ��
	int nTargetLen,	//ģʽ����Ҫƥ��ĳ���
	ETYPEMAP type	//Ĭ��Ϊchar����
	);

//KMP�㷨���ģʽ����״̬
static int* KmpGetPattern(
	void* pvTarget,
	int nTargetLen,
	ETYPEMAP type
	);

static int	KmpIndexOf(
	void* pvSource,			//�ı���
	//INT32 sourceOffset,	//�ı�����ʼ��ƫ�Ƶ�ַ
	int nSourceLen,	//�ı�������ƥ��ĳ���
	void* pvTarget,			//ģʽ��
	//INT32 targetOffset,	//ģʽ��ƫ��
	int nTargetLen,	//ģʽ����Ҫƥ��ĳ���
	int* pPattern,//ƥ�䴮
	ETYPEMAP type	//Ĭ��Ϊchar����
	);

int	NaiveIndexOf(
	void* pvSource,	//�ı���
	int nSourceLen,	//�ı�������ƥ��ĳ���
	void* pvTarget,	//ģʽ��
	int nTargetLen,	//ģʽ����Ҫƥ��ĳ���
	ETYPEMAP type	//Ĭ��Ϊchar����
	)
{
	int idx = -1;
	int max,i,j,end,k;
	if ((nTargetLen < nSourceLen) && (nTargetLen > 0))
	{
		switch(type)
		{
		case TM_INT8:
			{
				PCHAR pszSource = (PCHAR)pvSource;
				PCHAR pszTarget = (PCHAR)pvTarget;
				CHAR first = *pszTarget;
				NAIVEOPERATION();
			}
			break;
		case TM_INT16:
			{
				PWCHAR pszSource = (PWCHAR)pvSource;
				PWCHAR pszTarget = (PWCHAR)pvTarget;
				WCHAR first = *pszTarget;
				NAIVEOPERATION();
			}
			break;
		case TM_INT32:
			{
				PINT32 pszSource = (PINT32)pvSource;
				PINT32 pszTarget = (PINT32)pvTarget;
				INT32 first = *pszTarget;
				NAIVEOPERATION();
			}
			break;
		case TM_INT64:
			{
				PINT64 pszSource = (PINT64)pvSource;
				PINT64 pszTarget = (PINT64)pvTarget;
				INT64 first = *pszTarget;
				NAIVEOPERATION();
			}
			break;
		default:
			{
				PCHAR pszSource = (PCHAR)pvSource;
				PCHAR pszTarget = (PCHAR)pvTarget;
				CHAR first = *pszTarget;
				NAIVEOPERATION();
			}
			break;
		}
		//char first  = *pszTarget;

	}
	return idx;
}

int* KmpGetPattern(
	void* pvTarget,
	int nTargetLen,
	ETYPEMAP type
	)
{
	int i,q;
	if (nTargetLen > 0)
	{
		//int* pPattern = new int[nTargetLen];c++����
		int* pPattern = (int*)malloc(nTargetLen*sizeof(int));

		switch(type)
		{
		case TM_INT8:
			{
				PCHAR pszTarget = (PCHAR)pvTarget;
				KMPPATTERNOPERATION(pPattern,pszTarget,nTargetLen);
			}
			break;
		case TM_INT16:
			{
				PWCHAR pszTarget = (PWCHAR)pvTarget;
				KMPPATTERNOPERATION(pPattern,pszTarget,nTargetLen);
			}
			break;
		case TM_INT32:
			{
				PINT32 pszTarget = (PINT32)pvTarget;
				KMPPATTERNOPERATION(pPattern,pszTarget,nTargetLen);
			}
			break;
		case TM_INT64:
			{
				PINT64 pszTarget = (PINT64)pvTarget;
				KMPPATTERNOPERATION(pPattern,pszTarget,nTargetLen);
			}
			break;
		default:
			{
				PCHAR pszTarget = (PCHAR)pvTarget;
				KMPPATTERNOPERATION(pPattern,pszTarget,nTargetLen);
			}
			break;
		}

		return pPattern;
	}
	return NULL;
}

int	KmpIndexOf(
	void* pvSource,			//�ı���
	//INT32 sourceOffset,	//�ı�����ʼ��ƫ�Ƶ�ַ
	int nSourceLen,	//�ı�������ƥ��ĳ���
	void* pvTarget,			//ģʽ��
	//INT32 targetOffset,	//ģʽ��ƫ��
	int nTargetLen,	//ģʽ����Ҫƥ��ĳ���
	int* pPattern,//ƥ�䴮
	ETYPEMAP type	//Ĭ��Ϊchar����								
	)
{
	int idx = -1;
	int i,q;
	if ((nTargetLen < nSourceLen) && (nTargetLen > 0))
	{
		int end = nSourceLen - nTargetLen + 1;//���һ�����Բ���Ƚϵ��ַ�
		switch(type)
		{
		case TM_INT8:
			{
				PCHAR pszSource = (PCHAR)pvSource;
				PCHAR pszTarget = (PCHAR)pvTarget;
				KMPOPERATION(pszSource,nSourceLen,pszTarget,nTargetLen,pPattern);
				//int q = pPattern[0];
				//for (int i=0; i<nSourceLen; i++)
				//{
				//	while ((q>=0) && (pszSource[i]!=pszTarget[q+1]))
				//	{
				//		q = pPattern[q];
				//	}
				//	if (pszTarget[q+1] == pszSource[i])
				//	{
				//		q++;
				//		if ((q+1) == nTargetLen)
				//		{
				//			idx = (i-nTargetLen+1);
				//			break;
				//		}
				//		continue;
				//	}
				//	if (q>=0) 
				//	{
				//		//q = pPattern[q];
				//		i--;
				//	}
				//}
			}
			break;
		case TM_INT16:
			{
				PWCHAR pszSource = (PWCHAR)pvSource;
				PWCHAR pszTarget = (PWCHAR)pvTarget;
				KMPOPERATION(pszSource,nSourceLen,pszTarget,nTargetLen,pPattern);
			}
			break;
		case TM_INT32:
			{
				PINT32 pszSource = (PINT32)pvSource;
				PINT32 pszTarget = (PINT32)pvTarget;
				KMPOPERATION(pszSource,nSourceLen,pszTarget,nTargetLen,pPattern);
			}
			break;
		case TM_INT64:
			{
				PINT64 pszSource = (PINT64)pvSource;
				PINT64 pszTarget = (PINT64)pvTarget;
				KMPOPERATION(pszSource,nSourceLen,pszTarget,nTargetLen,pPattern);
			}
			break;
		default:
			{
				PCHAR pszSource = (PCHAR)pvSource;
				PCHAR pszTarget = (PCHAR)pvTarget;
				KMPOPERATION(pszSource,nSourceLen,pszTarget,nTargetLen,pPattern);
			}
			break;
		}
		//char first  = *pszTarget;

	}
	return idx;
}
int	CNaiveIndexOf(
	PCHAR pszSource,	//�ı���
	int nSourceLen,	//�ı�������ƥ��ĳ���
	PCHAR pszTarget,	//ģʽ��
	int nTargetLen	//ģʽ����Ҫƥ��ĳ���
	)
{
	return NaiveIndexOf(pszSource,nSourceLen,pszTarget,nTargetLen,TM_INT8);
}

int	WNaiveIndexOf(
	PWCHAR pwszSource,	//�ı���
	int nSourceLen,	//�ı�������ƥ��ĳ���
	PWCHAR pwszTarget,	//ģʽ��
	int nTargetLen	//ģʽ����Ҫƥ��ĳ���
	)
{
	return NaiveIndexOf(pwszSource,nSourceLen,pwszTarget,nTargetLen,TM_INT16);
}

int	NNaiveIndexOf(
	PINT32 pnSource,	//�ı���
	int nSourceLen,	//�ı�������ƥ��ĳ���
	PINT32 pnTarget,	//ģʽ��
	int nTargetLen	//ģʽ����Ҫƥ��ĳ���
	)
{
	return NaiveIndexOf(pnSource,nSourceLen,pnTarget,nTargetLen,TM_INT32);
}

int	DNNaiveIndexOf(
	PINT64 pdnSource,	//�ı���
	int nSourceLen,	//�ı�������ƥ��ĳ���
	PINT64 pdnTarget,	//ģʽ��
	int nTargetLen	//ģʽ����Ҫƥ��ĳ���
	)
{
	return NaiveIndexOf(pdnSource,nSourceLen,pdnTarget,nTargetLen,TM_INT64);
}

int* CGetKmpPattern(
	PCHAR pszTarget,
	int nTargetLen
	)
{
	return KmpGetPattern(pszTarget,nTargetLen,TM_INT8);
}

int* WGetKmpPattern(
	PWCHAR pwszTarget,
	int nTargetLen
	)
{
	return KmpGetPattern(pwszTarget,nTargetLen,TM_INT16);
}

int* NGetKmpPattern(
	PINT32 pnTarget,
	int nTargetLen
	)
{
	return KmpGetPattern(pnTarget,nTargetLen,TM_INT32);
}


int* DNGetKmpPattern(
	PINT64 pdnTarget,
	int nTargetLen
	)
{
	return KmpGetPattern(pdnTarget,nTargetLen,TM_INT64);
}

void FreeKmpPattern(int* pnPattern)
{
	//delete [] pnPattern;
	free(pnPattern);
}

int	CKmpIndexOf(
	PCHAR pszSource,			//�ı���
	int nSourceLen,	//�ı�������ƥ��ĳ���
	PCHAR pszTarget,			//ģʽ��
	int nTargetLen,	//ģʽ����Ҫƥ��ĳ���
	int* pPattern
	)
{
	return KmpIndexOf(pszSource,nSourceLen,pszTarget,nTargetLen,pPattern,TM_INT8);
}
int	WKmpIndexOf(
	PWCHAR pwszSource,			//�ı���
	//INT32 sourceOffset,	//�ı�����ʼ��ƫ�Ƶ�ַ
	int nSourceLen,	//�ı�������ƥ��ĳ���
	PWCHAR pwszTarget,			//ģʽ��
	//INT32 targetOffset,	//ģʽ��ƫ��
	int nTargetLen,	//ģʽ����Ҫƥ��ĳ���
	int* pPattern
	)
{
	return KmpIndexOf(pwszSource,nSourceLen,pwszTarget,nTargetLen,pPattern,TM_INT16);
}

int	NKmpIndexOf(
	PINT32 pnSource,			//�ı���
	//INT32 sourceOffset,	//�ı�����ʼ��ƫ�Ƶ�ַ
	int nSourceLen,		//�ı�������ƥ��ĳ���
	PINT32 pnTarget,			//ģʽ��
	//INT32 targetOffset,	//ģʽ��ƫ��
	int nTargetLen,		//ģʽ����Ҫƥ��ĳ���
	int* pPattern
	)
{
	return KmpIndexOf(pnSource,nSourceLen,pnTarget,nTargetLen,pPattern,TM_INT32);
}

int	DNKmpIndexOf(
	PINT64 pdnSource,			//�ı���
	//INT32 sourceOffset,		//�ı�����ʼ��ƫ�Ƶ�ַ
	int nSourceLen,			//�ı�������ƥ��ĳ���
	PINT64 pdnTarget,			//ģʽ��
	//INT32 targetOffset,		//ģʽ��ƫ��
	int nTargetLen,			//ģʽ����Ҫƥ��ĳ���
	int* pPattern
	)
{
	return KmpIndexOf(pdnSource,nSourceLen,pdnTarget,nTargetLen,pPattern,TM_INT64);
}