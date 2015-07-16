/******************************************************************************************
* @file hash.c
* @anthor:Croky.Zheng
* @CreateTime:2012/2/24
* @descirption:
*****************************************************************************************/

#include "hash.h"
#include <stdio.h>

unsigned long Hash1(char* pStr,unsigned long nLen)
{
	unsigned long i;
	unsigned long h = 0;
	if (NULL != pStr)
	{
		for(i=0; i<nLen; i++)
		{
			h = 31 * h + pStr[i];
		}
	}
	return h;
}

unsigned long Hash2(char* pStr,unsigned long nLen)
{
	unsigned long i;
	unsigned long g;
	unsigned long h = 0;
	if (NULL != pStr)
	{
		for(i=0; i<nLen; i++)
		{
			h = (h << 4) + pStr[i];
			if ((g = (h & 0xF0000000)) == 0)
			{
				h = h ^ (g >> 24);
				h = h ^ g;
			}
		}
	}
	return h;
}

unsigned long Hash3(char* pStr,unsigned long nLen)
{
	unsigned long i;
	unsigned long h = 0;
	if (NULL != pStr)
	{
		for(i=0; i<nLen; i++)
		{
			if ((i & 1) == 0)
			{
				h ^= ((h << 7) ^ (pStr[i]) ^ (h >> 3));
			}
			else
			{
				h ^= (~((h << 11) ^ (pStr[i]) ^ (h >> 5)));
			}
		}
	}
	return h;
}

unsigned long Hash4(char* pStr,unsigned long nLen)
{
	unsigned long i;
	unsigned long h2 = 4;
	unsigned long h = 1;
	if (NULL != pStr)
	{
		for(i=0; i<nLen; i++)
		{
			h ^= (((h & 63) + h2) * pStr[i]) + (h << 8);
			h2 += 3;
		}
	}
	return h;
}

unsigned long Hash5(char* pStr,unsigned long nLen)
{
	unsigned long i;
	unsigned long h = 1;
	if (NULL != pStr)
	{
		for(i=0; i<nLen; i++)
		{
			h *= 16777619;
			h ^= pStr[i];
		}
	}
	return h;
}

unsigned long Hash6(char* pStr,unsigned long nLen)  
{  
	unsigned long hash = nLen; 
	unsigned long i;
	for(i = 0; i < nLen; i++)  
	{  
		hash = ((hash << 5) ^ (hash >> 27)) ^ pStr[i];  
	}  
	return hash;  
}

unsigned long Hash7(char* pStr,unsigned long nLen)
{
	unsigned long i;
	unsigned long hash = 5381;
	for(i = 0; i < nLen; i++)  
	{  
		hash = ((hash << 5) + hash) + pStr[i];  
	}  
	return hash;
}