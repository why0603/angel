/******************************************************************************************
 * @file common.h
 * @anthor:Croky.Zheng
 * @CreateTime:2013/6/6
 * @descirption: 一些公共函数
 *****************************************************************************************/
#include "common.h"
#include <stdlib.h>
#include <string.h>

unsigned long GetByteSizeOfData(DataType dataType,void* data)
{
	unsigned long byte_size = (unsigned long)dataType;
	if ((dataType == Unknow_Type) || (dataType == Pointer_Type))
	{
		byte_size = 8;
	}
	else if (dataType == Chars_Type)
	{
		byte_size = (unsigned long)strlen((const char*)data) + 1;
	}
	else if (dataType == Unicode_Type)
	{
		byte_size = (unsigned long)wcslen((const wchar_t*)data) * 2 + 2;
	}
	else
	{
		byte_size = 0;
	}
	return byte_size;
}

CompareResult Compare(void* data1,void* data2,DataType dataType,unsigned long len,Boolean isSigned)
{
	CompareResult ret = Compare_Equal;
	if (data1 != data2)
	{
		//是4的倍数
		switch (dataType)
		{
		case Chars_Type:
			ret = (CompareResult)strcmp((const char*)data1,(const char*)data2);
			break;

		case Unicode_Type:
			ret = (CompareResult)wcscmp((const wchar_t*)data1,(const wchar_t*)data2);
			break;

		case Char_Type:
			if (isSigned)
			{
				if (*((char*)data1) > *((char*)data2))
				{
					ret = Compare_Great;
				} 
				else if (*((char*)data1) < *((char*)data2))
				{
					ret  = Compare_Less;
				}
			}
			else
			{
				if (*((unsigned char*)data1) > *((unsigned char*)data2))
				{
					ret = Compare_Great;
				} 
				else if (*((unsigned char*)data1) < *((unsigned char*)data2))
				{
					ret  = Compare_Less;
				}
			}
			break;
		case Short_Type:
			if (isSigned)
			{
				if (*((short*)data1) > *((short*)data2))
				{
					ret = Compare_Great;
				} 
				else if (*((short*)data1) < *((short*)data2))
				{
					ret  = Compare_Less;
				}
			}
			else
			{
				if (*((unsigned short*)data1) > *((unsigned short*)data2))
				{
					ret = Compare_Great;
				} 
				else if (*((unsigned short*)data1) < *((unsigned short*)data2))
				{
					ret  = Compare_Less;
				}
			}
			break;
		case Long_Type:
			if (isSigned)
			{
				if (*((long*)data1) > *((long*)data2))
				{
					ret = Compare_Great;
				} 
				else if (*((long*)data1) < *((long*)data2))
				{
					ret  = Compare_Less;
				}
			}
			else
			{
				if (*((unsigned long*)data1) > *((unsigned long*)data2))
				{
					ret = Compare_Great;
				} 
				else if (*((unsigned long*)data1) < *((unsigned long*)data2))
				{
					ret  = Compare_Less;
				}
			}
			break;
		case LongLong_Type:
			if (isSigned)
			{
				if (*((__int64*)data1) > *((__int64*)data2))
				{
					ret = Compare_Great;
				} 
				else if (*((__int64*)data1) < *((__int64*)data2))
				{
					ret  = Compare_Less;
				}
			}
			else
			{
				if (*((unsigned __int64*)data1) > *((unsigned __int64*)data2))
				{
					ret = Compare_Great;
				} 
				else if (*((unsigned __int64*)data1) < *((unsigned __int64*)data2))
				{
					ret  = Compare_Less;
				}
			}
			break;
		default:
			ret = (CompareResult)memcmp(data1,data2,len);
			break;
		}
	}
	return ret;
}

void CopyData(void* dest,void* src,DataType dataType,unsigned long len)
{
	if ((NULL != dest) && (NULL != src) && (len > 0))
	{
		switch(dataType)
		{
			case Chars_Type:
				//strcpy((char*)dest,(const char*)src);
				//((char*)dest)[len-1] = (char)0;
				strncpy((char*)dest,(const char*)src,len);
				break;

			case Unicode_Type:
				//wcscpy((wchar_t*)dest,(const wchar_t*)src);
				//((wchar_t*)dest)[(len/2)-1] = (wchar_t)0;
				wcsncpy((wchar_t*)dest,(const wchar_t*)src,len);
				break;

			case Char_Type:
				*(char*)dest = *(char*)src;
				break;

			case Short_Type:
				*(short*)dest = *(short*)src;
				break;

			case Long_Type:
				*(long*)dest = *(long*)src;
				break;

			case LongLong_Type:
				*(__int64*)dest = *(__int64*)src;
				break;

			default:
				memcpy(dest,src,len);
				break;
		}
	}
}