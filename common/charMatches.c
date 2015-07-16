/******************************************************************************************
* @file charMatches.cpp
* @anthor:Croky.Zheng
* @CreateTime:2010-05-07
* @descirption:用于字符串匹配，比如：朴素算法，KMP匹配，AC自动机，后缀树，trie树等 实现
* 由于C文件中的变量声明必须在函数名之后统一声明完成，所以只能使用CPP文件方式
*****************************************************************************************/
#include "charMatches.h"
#include <stdlib.h>
//朴素算法
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

//KMP模式串匹配算法
//这一步貌似有点多余，SO，改为下面的算法
//其实这里如果为-1就是对的
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

//KMP模式串计算方法
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

//KMP文本搜索算法
//S 文本
//SL 文本长度
//T 模式串
//TL 模式串长度
//P 模式串状态数组
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

//朴素算法实现函数
static int NaiveIndexOf(
	void* pvSource,		//文本串
	int nSourceLen,	//文本串参与匹配的长度
	void* pvTarget,		//模式串
	int nTargetLen,	//模式串需要匹配的长度
	ETYPEMAP type	//默认为char类型
	);

//KMP算法获得模式串的状态
static int* KmpGetPattern(
	void* pvTarget,
	int nTargetLen,
	ETYPEMAP type
	);

static int	KmpIndexOf(
	void* pvSource,			//文本串
	//INT32 sourceOffset,	//文本串起始的偏移地址
	int nSourceLen,	//文本串参与匹配的长度
	void* pvTarget,			//模式串
	//INT32 targetOffset,	//模式串偏移
	int nTargetLen,	//模式串需要匹配的长度
	int* pPattern,//匹配串
	ETYPEMAP type	//默认为char类型
	);

int	NaiveIndexOf(
	void* pvSource,	//文本串
	int nSourceLen,	//文本串参与匹配的长度
	void* pvTarget,	//模式串
	int nTargetLen,	//模式串需要匹配的长度
	ETYPEMAP type	//默认为char类型
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
		//int* pPattern = new int[nTargetLen];c++数组
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
	void* pvSource,			//文本串
	//INT32 sourceOffset,	//文本串起始的偏移地址
	int nSourceLen,	//文本串参与匹配的长度
	void* pvTarget,			//模式串
	//INT32 targetOffset,	//模式串偏移
	int nTargetLen,	//模式串需要匹配的长度
	int* pPattern,//匹配串
	ETYPEMAP type	//默认为char类型								
	)
{
	int idx = -1;
	int i,q;
	if ((nTargetLen < nSourceLen) && (nTargetLen > 0))
	{
		int end = nSourceLen - nTargetLen + 1;//最后一个可以参与比较的字符
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
	PCHAR pszSource,	//文本串
	int nSourceLen,	//文本串参与匹配的长度
	PCHAR pszTarget,	//模式串
	int nTargetLen	//模式串需要匹配的长度
	)
{
	return NaiveIndexOf(pszSource,nSourceLen,pszTarget,nTargetLen,TM_INT8);
}

int	WNaiveIndexOf(
	PWCHAR pwszSource,	//文本串
	int nSourceLen,	//文本串参与匹配的长度
	PWCHAR pwszTarget,	//模式串
	int nTargetLen	//模式串需要匹配的长度
	)
{
	return NaiveIndexOf(pwszSource,nSourceLen,pwszTarget,nTargetLen,TM_INT16);
}

int	NNaiveIndexOf(
	PINT32 pnSource,	//文本串
	int nSourceLen,	//文本串参与匹配的长度
	PINT32 pnTarget,	//模式串
	int nTargetLen	//模式串需要匹配的长度
	)
{
	return NaiveIndexOf(pnSource,nSourceLen,pnTarget,nTargetLen,TM_INT32);
}

int	DNNaiveIndexOf(
	PINT64 pdnSource,	//文本串
	int nSourceLen,	//文本串参与匹配的长度
	PINT64 pdnTarget,	//模式串
	int nTargetLen	//模式串需要匹配的长度
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
	PCHAR pszSource,			//文本串
	int nSourceLen,	//文本串参与匹配的长度
	PCHAR pszTarget,			//模式串
	int nTargetLen,	//模式串需要匹配的长度
	int* pPattern
	)
{
	return KmpIndexOf(pszSource,nSourceLen,pszTarget,nTargetLen,pPattern,TM_INT8);
}
int	WKmpIndexOf(
	PWCHAR pwszSource,			//文本串
	//INT32 sourceOffset,	//文本串起始的偏移地址
	int nSourceLen,	//文本串参与匹配的长度
	PWCHAR pwszTarget,			//模式串
	//INT32 targetOffset,	//模式串偏移
	int nTargetLen,	//模式串需要匹配的长度
	int* pPattern
	)
{
	return KmpIndexOf(pwszSource,nSourceLen,pwszTarget,nTargetLen,pPattern,TM_INT16);
}

int	NKmpIndexOf(
	PINT32 pnSource,			//文本串
	//INT32 sourceOffset,	//文本串起始的偏移地址
	int nSourceLen,		//文本串参与匹配的长度
	PINT32 pnTarget,			//模式串
	//INT32 targetOffset,	//模式串偏移
	int nTargetLen,		//模式串需要匹配的长度
	int* pPattern
	)
{
	return KmpIndexOf(pnSource,nSourceLen,pnTarget,nTargetLen,pPattern,TM_INT32);
}

int	DNKmpIndexOf(
	PINT64 pdnSource,			//文本串
	//INT32 sourceOffset,		//文本串起始的偏移地址
	int nSourceLen,			//文本串参与匹配的长度
	PINT64 pdnTarget,			//模式串
	//INT32 targetOffset,		//模式串偏移
	int nTargetLen,			//模式串需要匹配的长度
	int* pPattern
	)
{
	return KmpIndexOf(pdnSource,nSourceLen,pdnTarget,nTargetLen,pPattern,TM_INT64);
}