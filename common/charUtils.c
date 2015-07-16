/******************************************************************************************
 * @file charutils.c
 * @anthor:Croky.Zheng
 * @CreateTime:
 * @descirption:
 *****************************************************************************************/
#include "charUtils.h"
#include <stdio.h>
static const INT8 digits[] = {
	'0','1','2','3','4','5','6','7',
	'8','9','A','B','C','D','E','F',
	'G','H','I','J','K','L','M','N',
	'O','P','Q','R','S','T','U','V',
	'X','Y','Z'};

static const INT32 sizetable[] = { 9, 99, 999, 9999, 99999, 999999, 9999999,
                                      99999999, 999999999, MAXINTEGER };

static const CHAR digitunits[] = { //个位数
	'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
	'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
	'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
	'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
	'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
	'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
	'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
	'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
	'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
	'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
	} ;

static const CHAR digittens[] = {//十位数
	'0', '0', '0', '0', '0', '0', '0', '0', '0', '0',
	'1', '1', '1', '1', '1', '1', '1', '1', '1', '1',
	'2', '2', '2', '2', '2', '2', '2', '2', '2', '2',
	'3', '3', '3', '3', '3', '3', '3', '3', '3', '3',
	'4', '4', '4', '4', '4', '4', '4', '4', '4', '4',
	'5', '5', '5', '5', '5', '5', '5', '5', '5', '5',
	'6', '6', '6', '6', '6', '6', '6', '6', '6', '6',
	'7', '7', '7', '7', '7', '7', '7', '7', '7', '7',
	'8', '8', '8', '8', '8', '8', '8', '8', '8', '8',
	'9', '9', '9', '9', '9', '9', '9', '9', '9', '9',
	} ;


//将字节转为16进制方式
UINT Byte2Hex(PBYTE pInput,
	PCHAR pOutput,
	UINT nLen,
	CHAR cDelimiter)
{
	unsigned int i,len,k = 2;
	len = nLen * 2;
	if (0 != cDelimiter)
	{
		k = 3;
		len += nLen - 1;
	}
	if (pOutput != 0)
	{
		for (i=0; i<nLen; i++)
		{
			pOutput[i * k] = digits[(pInput[i] >> 4) & 0xf];
			pOutput[i * k + 1] = digits[pInput[i] & 0xf];
			if ((0 != cDelimiter) && (i < (nLen - 1)))
			{
				pOutput[i * k + 2] = cDelimiter;
			}
		}
	}
	return len;
}
//shift为计算的位数，比如2进制就每次需要计算1位，8进制3位，16进制4位
UINT8	Int2Str(UINT64 number,
	INT32 shift,
	PCHAR pStr)//按照指定的进制将数字转换为字符
{
	//保证右移补0
	//UINT32 n = (UINT32)number;
	CHAR buf[32] = {0};
	INT32 pos = 0;
	INT32 radix = 1 << shift;
	//位数下的所有位都为1
	INT32 mask = radix - 1;
	if (NULL == pStr) return (UINT8)0;
	do 
	{	
		buf[pos++] = digits[number & mask];
		number >>= shift;
	} while (number != 0);
	mask = pos;//做为字符数量
	while(pos-- > 0)
	{
		*pStr++ = buf[pos];
	}
	*pStr = 0;
	return (UINT8)mask;
}

//将指定进制的
INT32 Str2Int(PCHAR pStr,INT32 shift)
{
	INT32 n = 0;
	INT32 radix = 32;
	if (NULL == pStr) return 0;
	while ((0!=*pStr) &&( 0!=radix))
	{
		n <<= shift;
		if (ISDIGIT(*pStr))
			n |= (*pStr-'0');
		else if (ISUPPER(*pStr))
			n |= ((*pStr - 'A')+10);
		else if (ISLOWER(*pStr))
			n |= ((*pStr - 'a')+10);
		radix -= shift;
		pStr++;
	}
	return n;
}

INT32 HexStr2Int(PCHAR pStr)
{
	return Str2Int(pStr,4);
}

INT32 OctStr2Int(PCHAR pStr)
{
	return Str2Int(pStr,3);
}

INT32 BinStr2Int(PCHAR pStr)
{
	return Str2Int(pStr,1);
}

UINT8 Int2HexStr(UINT64 number,PCHAR pStr)//将数字转换为16进制字符格式
{
	return Int2Str(number,4,pStr);
}

UINT8 Int2BinStr(UINT64 number,PCHAR pStr)//将数字转换为2进制字符格式
{
	return Int2Str(number,1,pStr);
}

UINT8 Int2OctStr(UINT64 number,PCHAR pStr)//将数字转换为8进制字符格式
{
	return Int2Str(number,3,pStr);
}

UINT8 Int2DecStr(INT32 number,PCHAR pStr)//将数字转换为10进制字符格式
{
	UINT8	numberLen;
	UINT32 n;
	INT32 r,q;
	//CHAR	sign = 0;
	if (number == MININTEGER) //这个数会比较特别,因为计算机的原因，它会等于-1
	{
        StrCopy(MININTEGERSTRING,pStr,11);
		pStr[12] = 0;
		return (UINT8)StrLen(pStr);
    }
	numberLen = 0;
	if (number < 0)
	{
		numberLen += 1;
		number = -number;
		*pStr = '-';//符号
	}
	numberLen += StrSizeOfInt(number);
	*(pStr+numberLen) = 0;
	while (number >= 65536) 
	{
		q = number / 100;
        r = number - q * 100;//((q << 6) + (q << 5) + (q << 2));//=q*100=q*64+q*32+q*4;求余
        number = q;
        pStr [--numberLen] = digitunits[r];
        pStr [--numberLen] = digittens[r];
    }
	n = (UINT32)number; //保证无符号右移
	do //while(TRUE)
	{
		q = ((n * POINTNUMBER)) >> (19);//16+3
        r = n - q * 10;//((q << 3) + (q << 1));  // r = i-(q*10) ...
        pStr [--numberLen] = digits [r];
        n = q;
        //if (n == 0) break;
    } while(n > 0);
	return numberLen;
}

PCHAR StrCopy(PCHAR pSrc,PCHAR pDest,UINT32 srcLen)//从pStr 拷贝 字符串到 pDest
{
	UINT32 pos = 0;
	while((pos<srcLen) && *pSrc)
	{
		pDest[pos] = pSrc[pos];
		pos++;
	}
	return pDest;
}

UINT32 StrLen(PCHAR pStr)//返回字符串的长度，以0为结束
{
	PINT8 pStart = pStr;
	while(*pStr)
		pStr++;
	return (UINT32)(pStr - pStart);
}

UINT8	StrSizeOfInt(INT32 n)
{
	INT32 i;
	for(i=0;;i++)
	{
		if (n <= sizetable[i])
			return (UINT8)(i+1);
	}
	return (UINT8)0;
}

BOOL StrEqual(PCHAR pSrc,PCHAR pDest)//比较两个字符串是否相等
{
	while((*pSrc!=0 && pDest!=0))
	{
		if (*pSrc++ != *pDest++) return FALSE;
	}

	if (*pSrc != *pDest) return FALSE;//是否都是为0时退出
	return TRUE;
}

INT32 StrComp(PCHAR pSrc,PCHAR pDest)//比较两个字符串是否相等
{
	while((*pSrc!=0 && pDest!=0))
	{
		if (*pSrc > *pDest) return GREAT;
		else if (*pSrc < *pDest) return LESS;
		pSrc++;
		pDest++;
	}
	//到这最少有一个为0
	if (*pSrc == *pDest) return EQUAL;//
	else if (*pSrc==0) return LESS;
	else	return GREAT;
}

UINT32 Hash(PCHAR key) 
{ 
	register UINT32 n1=1, n2=4; 
	while (*key) 
	{ 
		n1 ^= (((n1&63)+n2)*((UINT32)(UINT8)*key++)) + (n1<<8); 
		n2 += 3; 
	} 
	return((UINT32) n1); 
} 

//字节转换成二进制
INT ByteToBit(BYTE ch, BYTE bit[8]){
	int cnt;
	for(cnt = 0;cnt < 8; cnt++){
		*(bit+cnt) = (ch>>cnt)&1;
	}
	return 0;
}

//二进制转换成字节
INT BitToByte(BYTE bit[8],PBYTE pCh){
	int cnt;
	for(cnt = 0;cnt < 8; cnt++){
		*pCh |= (*(bit + cnt)<<cnt);
	}
	return 0;
}

void CopyString(unsigned char dest[],char* src,unsigned int srcLen)
{
	if ((NULL != dest) && (NULL != src))
	{
		if (srcLen < 0)
		{
			srcLen = StrLen(src);
		}
		while (srcLen-- >0)
		{
			dest[srcLen] = src[srcLen];
		}
	}
}

