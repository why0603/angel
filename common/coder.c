/******************************************************************************************
 * @file _coder.c
 * @anthor:Croky.Zheng
 * @CreateTime:
 * @descirption:
 *****************************************************************************************/
#include "coder.h"
#include "charutils.h"
#include "big5.c"
#include "gbk.c"
#include "gb18030.c"
#include <stdio.h>

#ifdef __cplusplus
extern "C" {	//����C++
#endif
Endian	utf16_orderbyte = Endian_Small;
#ifdef __cplusplus
}	//����C++
#endif
NATIVE	long	SearchSegs(CoderSeg segs[],
							   unsigned short segslen,
							   unsigned short coder);

NATIVE	long	SearchSegsEx(CoderSegEx segs[],
								 unsigned short segslen,
								 long coder);

NATIVE	long	AppendUnicode(long unicode,//unicode�ַ�
								char* desc,//Ŀ�걣���ַ���ָ��
								unsigned long index);//��ʼ����

NATIVE	long	AppendASCII(long unicode,//unicode�ַ�
								char* desc,//Ŀ�걣���ַ���ָ��
								unsigned long index);//��ʼ����
NATIVE	long	AppendUTF16(long unicode,//unicode�ַ�
								char* desc,//Ŀ�걣���ַ���ָ��
								unsigned long index);//��ʼ����
NATIVE	long	AppendUTF8(long unicode,//unicode�ַ�
								char* desc,//Ŀ�걣���ַ���ָ��
								unsigned long index);//��ʼ����
NATIVE	long	AppendBig5(long unicode,//unicode�ַ�
								char* desc,//Ŀ�걣���ַ���ָ��
								unsigned long index);//��ʼ����
NATIVE	long	AppendGbk(long unicode,//unicode�ַ�
								char* desc,//Ŀ�걣���ַ���ָ��
								unsigned long index);//��ʼ����
NATIVE	long	AppendGb18030(long unicode,//unicode�ַ�
								char* desc,//Ŀ�걣���ַ���ָ��
								unsigned long index);//��ʼ����


Boolean	DecoderUNICODE(char*	uni32,//UTF8�ַ���
						char*	desc,//Ŀ���ַ���
						unsigned long srclen,//Դ�ַ����ֽڳ���
						StrCoder desccoder,
						StrSizePtr size)
{
	unsigned long i;
	long unicode = 0;
	size->b_length = 0;
	size->length = 0;
	if (NULL == uni32) return False;
	srclen >>= 2;//�õ�ʵ�ʳ���
	for (i=0; i<srclen; i++)
	{
		unicode = *(((PINT32)uni32)+i);
		size->length++;//�ַ�����һ��
		switch(desccoder)
		{
		case ST_ASCII:
			size->b_length += AppendASCII(unicode,desc,size->b_length);
			break;
		case ST_UTF8:
			size->b_length += AppendUTF8(unicode,desc,size->b_length);
			break;
		case ST_UTF16://1,2�ֽ�
			size->b_length += AppendUTF16(unicode,desc,size->b_length);
			break;
		case ST_BIG5://1,2�ֽ�
			size->b_length += AppendBig5(unicode,desc,size->b_length);
			break;
		case ST_GBK://1,2,4�ֽ�
			size->b_length += AppendGbk(unicode,desc,size->b_length);
			break;
		case ST_GB18030:
			size->b_length += AppendGb18030(unicode,desc,size->b_length);
			break;
		default://��û��֧�ֵı���ʱ��ֱ�ӷ������UTF8���봮����ʾ��UNICODE�ַ�����
			return False;
		}
	}
	return True;

}
Boolean	DecoderASCII(char*	ascii,//UTF8�ַ���
						char*	desc,//Ŀ���ַ���
						unsigned long srclen,//Դ�ַ����ֽڳ���
						StrCoder desccoder,
						StrSizePtr size)
{
	unsigned long i;
	long unicode = 0;
	size->b_length = 0;
	size->length = 0;
	if (NULL == ascii) return False;
	for (i=0; i<srclen; i++)
	{
		if ((ascii[i]&0xff) > 0x7f)
		{
			continue;
		}
		unicode = ascii[i]&0xff;
		size->length++;//�ַ�����һ��
		switch(desccoder)
		{
		case ST_UNICODE:
			size->b_length += AppendUnicode(unicode,desc,size->b_length);
			break;
		case ST_UTF8:
			size->b_length += AppendUTF8(unicode,desc,size->b_length);
			break;
		case ST_UTF16://1,2�ֽ�
			size->b_length += AppendUTF16(unicode,desc,size->b_length);
			break;
		case ST_BIG5://1,2�ֽ�
			size->b_length += AppendBig5(unicode,desc,size->b_length);
			break;
		case ST_GBK://1,2,4�ֽ�
			size->b_length += AppendGbk(unicode,desc,size->b_length);
			break;
		case ST_GB18030:
			size->b_length += AppendGb18030(unicode,desc,size->b_length);
			break;
		default://��û��֧�ֵı���ʱ��ֱ�ӷ������UTF8���봮����ʾ��UNICODE�ַ�����
			return False;
		}
	}
	return True;
}

Boolean	DecoderGB18030(char*	gb18030,//UTF8�ַ���
						char*	desc,//Ŀ���ַ���
						unsigned long srclen,//Դ�ַ����ֽڳ���
						StrCoder desccoder,
						StrSizePtr size)
{
	unsigned long i;
	long unicode = 0;
	long low = 0;
	size->b_length = 0;
	size->length = 0;
	if (NULL == gb18030) return False;
	for (i=0; i<srclen; i++)
	{
		if ((gb18030[i]&0xff) <= 0x7f)//ascii
		{
			unicode = gb18030[i]&0xff;
		}
		else if (((gb18030[i]&0xff)>=0x81) && ((gb18030[i]&0xff)<=0xfe))//0x81-0xFE
		{
			if ((i+1)<srclen)
			{
				low = gb18030[i+1]&0xff;
				if (((low>=0x40)&&(low<=0x7e)) || ((low>=0xa1)&&(low<=0xfe)))//0x40-0x7E  0x80-0xFE
				{
					unicode = GetUnicodeByGb18030(((gb18030[i]&0xff)<<8) | (gb18030[i+1]&0xff));//�޷�ʹ��++i��ת
					i += 1;
				}
				else if ((low>=0x30) && (low<=0x39))//4�ֽ�
				{
					if ((i+3)<srclen)//���ٻ���Ҫ3���ֽ�
					{
						if (((gb18030[i+2]&0xff)<0x81) || ((gb18030[i+2]&0xff)>0xfe)) continue;//0x81-0xFE
						if (((gb18030[i+3]&0xff)<0x30) || ((gb18030[i+3]&0xff)>0x39)) continue;//0x30-0x39
						unicode = GetUnicodeByGb18030(((gb18030[i]&0xff)<<24) 
							| ((gb18030[i+1]&0xff)<<16) 
							| ((gb18030[i+2]&0xff)<<8) 
							| (gb18030[i+3]&0xff));//�޷�ʹ��++i��ת
						i += 3;
					}
				}
			}
		}
		size->length++;//�ַ�����һ��
		switch(desccoder)
		{
		case ST_UNICODE:
			size->b_length += AppendUnicode(unicode,desc,size->b_length);
			break;
		case ST_ASCII:
			size->b_length += AppendASCII(unicode,desc,size->b_length);
			break;
		case ST_UTF8:
			size->b_length += AppendUTF8(unicode,desc,size->b_length);
			break;
		case ST_UTF16://1,2�ֽ�
			size->b_length += AppendUTF16(unicode,desc,size->b_length);
			break;
		case ST_BIG5://1,2�ֽ�
			size->b_length += AppendBig5(unicode,desc,size->b_length);
			break;
		case ST_GBK://1,2,4�ֽ�
			size->b_length += AppendGbk(unicode,desc,size->b_length);
			break;
		default://��û��֧�ֵı���ʱ��ֱ�ӷ������UTF8���봮����ʾ��UNICODE�ַ�����
			return False;
		}
	}
	return True;
}

Boolean	DecoderGBK(char*	gbk,//UTF8�ַ���
						char*	desc,//Ŀ���ַ���
						unsigned long srclen,//Դ�ַ����ֽڳ���
						StrCoder desccoder,
						StrSizePtr size)
{
	unsigned long i;
	long unicode = 0;
	long low = 0;
	size->b_length = 0;
	size->length = 0;
	if (NULL == gbk) return False;
	for (i=0; i<srclen; i++)
	{
		if ((gbk[i]&0xff) <= 0x7f)//ascii
		{
			unicode = gbk[i]&0xff;
		}
		else if (((gbk[i]&0xff)>=0x81) && ((gbk[i]&0xff)<=0xfe))//0x81-0xFE
		{
			if ((i+1)<srclen)
			{
				low = gbk[i+1]&0xff;
				if (((low>=0x40)&&(low<=0x7e)) || ((low>=0xa1)&&(low<=0xfe)))//0x40-0x7E  0x80-0xFE
				{
					unicode = GetUnicodeByGbk(((gbk[i]&0xff)<<8) | (gbk[i+1]&0xff));//i++,++i��ʽ��Ȼ������
					i += 1;
				}
			}
		}
		size->length++;//�ַ�����һ��
		switch(desccoder)
		{
		case ST_UNICODE:
			size->b_length += AppendUnicode(unicode,desc,size->b_length);
			break;
		case ST_ASCII:
			size->b_length += AppendASCII(unicode,desc,size->b_length);
			break;
		case ST_UTF8:
			size->b_length += AppendUTF8(unicode,desc,size->b_length);
			break;
		case ST_UTF16://1,2�ֽ�
			size->b_length += AppendUTF16(unicode,desc,size->b_length);
			break;
		case ST_BIG5://1,2�ֽ�
			size->b_length += AppendBig5(unicode,desc,size->b_length);
			break;
		case ST_GB18030://1,2,4�ֽ�
			size->b_length += AppendGb18030(unicode,desc,size->b_length);
			break;
		default://��û��֧�ֵı���ʱ��ֱ�ӷ������UTF8���봮����ʾ��UNICODE�ַ�����
			return False;
		}
	}
	return True;
}

Boolean	DecoderBIG5(char*	big5,//UTF8�ַ���
						char*	desc,//Ŀ���ַ���
						unsigned long srclen,//Դ�ַ����ֽڳ���
						StrCoder desccoder,
						StrSizePtr size)
{
	unsigned long i;
	long unicode = 0;
	long low;
	size->b_length = 0;
	size->length = 0;
	if (NULL == big5) return False;
	for (i=0; i<srclen; i++)
	{
		if ((big5[i]&0xff) <= 0x7f)//ascii
		{
			unicode = big5[i]&0xff;
		}
		else if (((big5[i]&0xff)>=0xa1) && ((big5[i]&0xff)<=0xf9))//0xa1��0xf9
		{
			if ((i+1)<srclen)
			{
				low = big5[i+1]&0xff;
				if (((low>=0x40)&&(low<=0x7e)) || ((low>=0xa1)&&(low<=0xfe)))//0X40��0X7E����64-126����0XA1��0XFE
				{
					unicode = GetUnicodeByBig5(((big5[i]&0xff)<<8) | (big5[i+1]&0xff));
					i += 1;
				}
			}
		}
		size->length++;//�ַ�����һ��
		switch(desccoder)
		{
		case ST_UNICODE:
			size->b_length += AppendUnicode(unicode,desc,size->b_length);
			break;
		case ST_ASCII:
			size->b_length += AppendASCII(unicode,desc,size->b_length);
			break;
		case ST_UTF8:
			size->b_length += AppendUTF8(unicode,desc,size->b_length);
			break;
		case ST_UTF16://1,2�ֽ�
			size->b_length += AppendUTF16(unicode,desc,size->b_length);
			break;
		case ST_GBK://1,2�ֽ�
			size->b_length += AppendGbk(unicode,desc,size->b_length);
			break;
		case ST_GB18030://1,2,4�ֽ�
			size->b_length += AppendGb18030(unicode,desc,size->b_length);
			break;
		default://��û��֧�ֵı���ʱ��ֱ�ӷ������UTF8���봮����ʾ��UNICODE�ַ�����
			return False;
		}
	}
	return True;
}
//�ø�16λ��ʾ�ַ��������õ�16λ��ʾ�ֽڸ���
//UTF16���ڶ�UNICODE���б��� ����0-0xffff(65535)�򲻱�
//����0x10000-0x1ffff
//int unicode = 0x2A5FE-0x10000;//-0x40 
//int utf16 = (((0xd800|((unicode>>10)&0x3ff)))<<16) | (0xdc00|(unicode&0x3ff)); 
Boolean	DecoderUTF16(char*	utf16,//UTF8�ַ���
						char*	desc,//Ŀ���ַ���
						unsigned long  srclen,//Դ�ַ�������
						StrCoder desccoder,//Ŀ���ַ�������
						StrSizePtr size)//�ַ����ĳ���
{
	unsigned long i;
	long unicode = 0;
	long low = 0;
	size->b_length = 0;
	size->length = 0;
	if (NULL == utf16) return False;
	srclen = (srclen%2)==0?srclen:srclen-1;//UTF16�����ܵ����ֽ�
	for (i=0; i<srclen; i+=2)
	{
		if (utf16_orderbyte == Endian_Big)
		{
			unicode = ((utf16[i]&0xff)<<8) | (utf16[i+1]&0xff);
			if (unicode >= 0xd800 && unicode <= 0xdbff)
			{
				if ((i+2) < srclen)//��֤��һ���ֽ�
				{
					low = ((utf16[i+1]&0xff)<<8) | (utf16[i+2]&0xff);
					if (low >= 0xdc00 && low <= 0xdfff)
					{
						unicode = (((unicode-0xd800)&0x3ff)<<10)|((low-0xdc00)&0x3fff)+0x10000;
						i += 2;//��ʾ��4�ֽڵ�UTF16�ַ���UNICODE������0x10000-0x1ffff֮��
					}
				}
			}
		}
		else
		{
			unicode = (utf16[i]&0xff) | ((utf16[i+1]&0xff)<<8);
			if (unicode >= 0xd800 && unicode <= 0xdbff)
			{
				if ((i+2) < srclen)//��֤��һ���ֽ�
				{
					low = (utf16[i+1]&0xff) | ((utf16[i+2]&0xff)<<8);
					if (low >= 0xdc00 && low <= 0xdfff)
					{
						unicode = ((((unicode-0xd800)&0x3ff)<<10)|((low-0xdc00)&0x3ff))+0x10000;
						i += 2;//��ʾ��4�ֽڵ�UTF16�ַ���UNICODE������0x10000-0x1ffff֮��
					}
				}
			}
		}
		size->length++;//�ַ�����һ��
		switch(desccoder)
		{
		case ST_UNICODE:
			size->b_length += AppendUnicode(unicode,desc,size->b_length);
			break;
		case ST_ASCII:
			size->b_length += AppendASCII(unicode,desc,size->b_length);
			break;
		case ST_UTF8:
			size->b_length += AppendUTF8(unicode,desc,size->b_length);
			break;
		case ST_BIG5://1,2�ֽ�
			size->b_length += AppendBig5(unicode,desc,size->b_length);
			break;
		case ST_GBK://1,2�ֽ�
			size->b_length += AppendGbk(unicode,desc,size->b_length);
			break;
		case ST_GB18030://1,2,4�ֽ�
			size->b_length += AppendGb18030(unicode,desc,size->b_length);
			break;
		default://��û��֧�ֵı���ʱ��ֱ�ӷ������UTF8���봮����ʾ��UNICODE�ַ�����
			return False;
		}
	}
	return True;
}

//�ø�16λ��ʾ�ַ��������õ�16λ��ʾ�ֽڸ���
/*
1�ֽ� 0xxxxxxx (0000 0000 --- 0000 007f)	//0x0
2�ֽ� 110xxxxx 10xxxxxx (0000 0080 --- 0000 07FF)	//0xc0 
3�ֽ� 1110xxxx 10xxxxxx 10xxxxxx (0000 0800 --- 0000 FFFF)0xe0 
4�ֽ� 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx (0001 0000 --- 001F FFFF)//0xf0 
5�ֽ� 111110xx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx (0020 0000 --- 03FF FFFF)//0xf8 
6�ֽ� 1111110x 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx (0400 0000 --- 07FFF FFFF)//0xfc 
*/
Boolean	DecoderUTF8(char*	utf8,//UTF8�ַ���
							char*	desc,//Ŀ���ַ���
							unsigned long  srclen,//Դ�ַ�������
							StrCoder desccoder,//Ŀ���ַ�������
							StrSizePtr size)//�ַ����ĳ���
{
	unsigned long i;
	long unicode = 0;
	size->b_length = 0;
	size->length = 0;
	if (NULL == utf8) return False;
	for (i=0; i<srclen; i++)
	{
		//��1-6�ֽ�6�����������5��6�ֽ���ʱ��û���ã�����ֻ��1-4�ֽ����ж�
		if ((utf8[i]&0xff)<=0x7f)//1�ֽ� 0x0-0x7f
		{
			unicode = UNICODE_UTF8_1(utf8[i]);
		}
		else if ((utf8[i]&0xff)<=0xdf)//2�ֽ� 0xc0-0xdf
		{
			if ((i+1)<srclen)
			{
				if (((utf8[i+1]&0xff)<0x80) || ((utf8[i+1]&0xff)>0xbf)) continue;
				unicode = UNICODE_UTF8_2(utf8[i],utf8[i+1]);
				i += 1;
			}
		}
		else if ((utf8[i]&0xff)<=0xef)//3�ֽ� 0xe0-0xef
		{
			if ((i+2)<srclen)
			{
				if ((((utf8[i+1]|utf8[i+2])&0xff)<0x80) || (((utf8[i+1]|utf8[i+2])&0xff)>0xbf)) continue;
				unicode = UNICODE_UTF8_3(utf8[i],utf8[i+1],utf8[i+2]);
				i += 2;
			}
		}
		else if ((utf8[i]&0xff)<=0xf7)//4�ֽ� 0xf0-0xf7
		{
			if ((i+3)<srclen)
			{
				if ((((utf8[i+1]|utf8[i+2]|utf8[i+3])&0xff)<0x80) || (((utf8[i+1]|utf8[i+2]|utf8[i+3])&0xff)>0xbf)) continue;
				unicode = UNICODE_UTF8_4(utf8[i],utf8[i+1],utf8[i+2],utf8[i+3]);
				i += 3;
			}
		}//����5��6�ֽ����������ʱ��û�У���������
		else if ((utf8[i]&0xff)<=0xf8)//5�ֽ� 0xf8-0xfb
		{
			continue;
		}
		else if ((utf8[i]&0xff)<=0xf8)//6�ֽ� 0xfc-0xfe
		{
			continue;
		}
		else
		{
			continue;
		}
		size->length++;//�ַ�����һ��
		switch(desccoder)
		{
		case ST_UNICODE:
			size->b_length += AppendUnicode(unicode,desc,size->b_length);
			break;
		case ST_ASCII:
			size->b_length += AppendASCII(unicode,desc,size->b_length);
			break;
		case ST_UTF16:
			size->b_length += AppendUTF16(unicode,desc,size->b_length);
			break;
		case ST_BIG5://1,2�ֽ�
			size->b_length += AppendBig5(unicode,desc,size->b_length);
			break;
		case ST_GBK://1,2�ֽ�
			size->b_length += AppendGbk(unicode,desc,size->b_length);
			break;
		case ST_GB18030://1,2,4�ֽ�
			size->b_length += AppendGb18030(unicode,desc,size->b_length);
			break;
		default://��û��֧�ֵı���ʱ��ֱ�ӷ������UTF8���봮����ʾ��UNICODE�ַ�����
			return False;
		}
	}
	return True;
}

long	AppendUnicode(long unicode,//unicode�ַ�
					char* desc,
					unsigned long index)
{
	if (NULL != desc)
	{
		*((PINT32)(desc+index)) = unicode;
	}
	return 4;
}

long	AppendASCII(long unicode,
					char* desc,
					unsigned long index)
{
	if (NULL != desc)
	{
		*(desc+index) = (CHAR)(unicode&0x7f);
	}
	return 1;
}

long	AppendUTF16(long unicode,
					char* desc,
					unsigned long index)
{
	unsigned long gbcoder;
	long pos = index;
	if (unicode >= 0 && unicode <= 0xffff)
	{
		if (NULL == desc) return 2;
		if (utf16_orderbyte == Endian_Big)
		{
			desc[pos++] = (CHAR)(unicode>>8);
			desc[pos++] = (CHAR)(unicode);
		}
		else
		{
			desc[pos++] = (CHAR)(unicode);
			desc[pos++] = (CHAR)(unicode>>8);
		}
	}
	else
	{
		if (NULL == desc) return 4;
		if (utf16_orderbyte == Endian_Big)
		{
			gbcoder = UTF16_1(unicode);
			desc[pos++] = (CHAR)(gbcoder>>8);
			desc[pos++] = (CHAR)(gbcoder);
			gbcoder = UTF16_2(unicode);
			desc[pos++] = (CHAR)(gbcoder>>8);
			desc[pos++] = (CHAR)(gbcoder);
		}
		else
		{
			gbcoder = UTF16_1(unicode);
			desc[pos++] = (CHAR)(gbcoder);
			desc[pos++] = (CHAR)(gbcoder>>8);
			gbcoder = UTF16_2(unicode);
			desc[pos++] = (CHAR)(gbcoder);
			desc[pos++] = (CHAR)(gbcoder>>8);
		}
	}
	return pos-index;
}

long	AppendUTF8(long unicode,
				   char* desc,
					unsigned long index)
{
	long pos = index;
	//(((c)>=0&&(c)<=0x7f)?UTF8_1(c):((c)>=0x80&&(c)<=0x7ff)?UTF8_2((c)):((c)>=0x800&&(c)<=0xffff)?UTF8_3((c)):UTF8_4((c)))
	if (unicode>=0 && unicode <= 0x7f)//1�ֽ�
	{
		if (NULL == desc) return 1;
		desc[pos++] = (CHAR)unicode;
	}
	else if (unicode>=0x80 && unicode<=0x7ff)//2�ֽ�
	{
		//(((0xc0|(((c)>>6)&0x3f))<<8) | (0x80|(((c)&0x3f))))
		if (NULL == desc) return 2;
		desc[pos++] = (CHAR)(0xc0|((unicode>>6)&0x3f));
		desc[pos++] = (CHAR)(0x80|(unicode&0x3f));
	}
	else if (unicode>=0x800 && unicode<=0xffff)//3�ֽ�
	{
		//#define UTF8_3(c)		(((0xe0|(((c)>>12)&0x3f))<<16) | ((0x80|(((c)>>6)&0x3f))<<8) | (0x80|(((c)&0x3f))))
		if (NULL == desc) return 3;
		desc[pos++] = (CHAR)(0xe0|((unicode>>12)&0x3f));
		desc[pos++] = (CHAR)(0x80|((unicode>>6)&0x3f));
		desc[pos++] = (CHAR)(0x80|(unicode&0x3f));
	}
	else if (unicode>=0x10000 && unicode<=0x1fffff)//4�ֽ�
	{
		//(((0xf0|(((c)>>18)&0x3f))<<24) | ((0x80|(((c)>>12)&0x3f))<<16) | ((0x80|(((c)>>6)&0x3f))<<8) | (0x80|(((c)&0x3f))))
		if (NULL == desc) return 4;
		desc[pos++] = (CHAR)(0xf0|((unicode>>18)&0x3f));
		desc[pos++] = (CHAR)(0x80|((unicode>>12)&0x3f));
		desc[pos++] = (CHAR)(0x80|((unicode>>6)&0x3f));
		desc[pos++] = (CHAR)(0x80|(unicode&0x3f));
	}//����5��6�ֽڵ������Ϊû�оͲ�����
	else if (unicode>=0x200000 && unicode<=0x3ffffff)//5�ֽ�
	{
	}
	else if (unicode>=0x4000000 && unicode<=0x7fffffff)//6�ֽ�
	{
	}
	else//4�ֽ�
	{
		//���ﲻ���غ��п��ܵ�����ѭ��
	}
	return pos-index;
}

long	AppendBig5(long unicode,
				   char* desc,
					unsigned long index)
{
	unsigned long gbcoder;
	long pos = index;
	if (unicode >= 0 && unicode <= 0x7f)
	{
		if (NULL == desc) return 1;
		desc[pos++] = (CHAR)unicode;
	}
	else
	{
		if (NULL == desc) return 2;
		gbcoder = GetBig5ByUnicode((unsigned short)unicode);
		desc[pos++] = (CHAR)(gbcoder>>8);
		desc[pos++] = (CHAR)gbcoder;
	}
	return pos;
}

long	AppendGbk(long unicode,
				  char* desc,
					unsigned long index)
{
	unsigned long gbcoder;
	long pos = index;
	if (unicode >= 0 && unicode <= 0x7f)
	{
		if (NULL == desc) return 1;
		desc[pos++] = (CHAR)unicode;
	}
	else
	{
		if (NULL == desc) return 2;
		gbcoder = GetGbkByUnicode((unsigned short)unicode);
		desc[pos++] = (CHAR)(gbcoder>>8);
		desc[pos++] = (CHAR)gbcoder;
	}
	return pos-index;
}

long	AppendGb18030(long unicode,
					  char* desc,
					unsigned long index)
{
	long pos = index;
	unsigned long gbcoder = GetGb18030ByUnicode(unicode);//gb18030����0-0x7f��ASCII����
	if (gbcoder >= 0 && gbcoder <= 0xff)		//1�ֽ�GB18030
	{
		if (NULL == desc) return 1;
		desc[pos++] = (CHAR)gbcoder;
	}
	else if (gbcoder >= 0x0100 && gbcoder <= 0xffff) //2�ֽ�GB18030
	{
		if (NULL == desc) return 2;
		desc[pos++] = (CHAR)(gbcoder>>8);
		desc[pos++] = (CHAR)gbcoder;
	}
	else//4�ֽ�GB18030
	{
		if (NULL == desc) return 4;
		desc[pos++] = (CHAR)(gbcoder>>24);
		desc[pos++] = (CHAR)(gbcoder>>16);
		desc[pos++] = (CHAR)(gbcoder>>8);
		desc[pos++] = (CHAR)gbcoder;
	}
	return pos-index;
}


unsigned short	GetBig5ByUnicode(unsigned short	unicode)
{
	long index = SearchSegs(unicode_big5_segs,unicode_big5_segs_length,unicode);
	if (-1 != index)
		return unicode_big5_list[index];
	else
		return BIGNOWORD;
}

unsigned short	GetUnicodeByBig5(unsigned short big5)
{
	long index = SearchSegs(big5_unicode_segs,big5_unicode_segs_length,big5);
	if (-1 != index)
		return big5_unicode_list[index];
	else
		return UNICODENOWORD;
}

unsigned short	GetGbkByUnicode(unsigned short	unicode)
{
	long index = SearchSegs(unicode_gbk_segs,unicode_gbk_segs_length,unicode);
	if (-1 != index)
		return unicode_gbk_list[index];
	else
		return GBNOWORD;
}

unsigned short	GetUnicodeByGbk(unsigned short	gbk)
{
	long index = SearchSegs(gbk_unicode_segs,gbk_unicode_segs_length,gbk);
	if (-1 != index)
		return gbk_unicode_list[index];
	else
		return UNICODENOWORD;
}

long	GetGb18030ByUnicode(long unicode)
{
	long index = SearchSegsEx(unicode_gb18030_segs,unicode_gb18030_segs_length,unicode);
	if (-1 != index)
		return unicode_gb18030_list[index];
	else
		return GBNOWORD;
}

long	GetUnicodeByGb18030(long gb18030)
{
	long index = SearchSegsEx(gb18030_unicode_segs,gb18030_unicode_segs_length,gb18030);
	if (-1 != index)
		return gb18030_unicode_list[index];
	else
		return UNICODENOWORD;
}


Boolean	IsGbk(unsigned short gbk)//�������Ƿ���GBK���룬��Χ����
{
	UINT8 c1 = (UINT8)((gbk>>8)&0xff);
	UINT8 c2 = (UINT8)(gbk&0xff);
	if ((c1>=0x81 && c1<=0xfe) && (c2>=0x40 && c2<=0xfe) && (c2 != 0x7f))
	{
		return True;
	}
	return False;
}


Boolean	IsGb2312(unsigned short gb2312)//�������Ƿ���Gb2312���룬��Χ����
{
	UINT8 c1 = (UINT8)((gb2312>>8)&0xff);
	UINT8 c2 = (UINT8)(gb2312&0xff);
	if ((c1>=0xa1&&c1<=0xfe) && (c2>=0xa1&&c2<=0xfe))
	{
		return True;
	}
	return False;
}

Boolean IsGbkWord(unsigned short gbkcode)//�Ƿ���GBK������
{
	//OXBOA1-F7FE
	//OX8140-AOFE
	//OXAA40-FEAO
	UINT8 c1 = 0;
	UINT8 c2 = 0;
	if ((c1>=0x81 && c1<=0x40) && (c2>=0xa0 && c2<=0xfe))
	{
		return True;
	}
	c1 = (UINT8)((gbkcode>>8)&0xff);
	c2 = (UINT8)(gbkcode&0xff);
	if ((c1>=0xa1 && c1<=0x40) && (c2>=0xfe && c2<=0xa0))
	{
		return True;
	}
	return IsGb2312Word(gbkcode);
}

Boolean IsGb2312Word(unsigned short gb2312)//�Ƿ���GB2312������
{
		//OXBOA1-F7FE
	UINT8 c1 = (UINT8)((gb2312>>8)&0xff);
	UINT8 c2 = (UINT8)(gb2312 & 0xff);
	if ((c1>=0xb0&&c1<=0xf7) && (c2>=0xa1&&c2<=0xfe))
	{
		return True;
	}
	return False;
}

VOID	SetUTF16ByteOrder(Endian endian)
{
	utf16_orderbyte = endian;
}

Endian	GetUTF16ByteOrder()
{
	return utf16_orderbyte;
}


//unicode_big5_segs
long SearchSegs(CoderSeg segs[],
							   unsigned short segslen,
							   unsigned short coder)
{
	int start = 0;
	int mark = 0;		//��ʼ�������ͱ�Ƕ�
	int terminal = segslen-1;	//��һ��
	long index = -1;
	int temp;
	while(True)
	{
		if (start > terminal)
		{
			temp = start;
			start = terminal;
			terminal = temp;
		}
		if ((terminal - start) <= 7)
		{
			for (;start<=terminal;start++)
			{
				if (segs[start].minCode <= coder && segs[start].maxCode >= coder)
				{
					index = segs[start].startIndex + (coder - segs[start].minCode);
					break;
				}
			}
			break;
		}
		mark = (start + terminal) >> 1;
		if (segs[mark].minCode <= coder && segs[mark].maxCode >= coder)
		{
			index = segs[mark].startIndex + (coder - segs[mark].minCode);
			break;
		}
		
		if (segs[mark].minCode > coder)
		{
			terminal = mark;
		}
		else
		{
			start = mark;
		}
	}
	return index;
}

//unicode_big5_segs
long SearchSegsEx(CoderSegEx segs[],
							   unsigned short segslen,
							   long coder)
{
	int start = 0;
	int mark = 0;		//��ʼ�������ͱ�Ƕ�
	int terminal = segslen-1;	//��һ��
	long index = -1;
	int temp;
	while(True)
	{
		if (start > terminal)
		{
			temp = start;
			start = terminal;
			terminal = temp;
		}
		if ((terminal - start) <= 7)
		{
			for (;start<=terminal;start++)
			{
				if (segs[start].minCode <= coder && segs[start].maxCode >= coder)
				{
					index = segs[start].startIndex + (coder - segs[start].minCode);
					break;
				}
			}
			break;
		}
		mark = (start + terminal) >> 1;
		if (segs[mark].minCode <= coder && segs[mark].maxCode >= coder)
		{
			index = segs[mark].startIndex + (coder - segs[mark].minCode);
			break;
		}
		
		if (segs[mark].minCode > coder)
		{
			terminal = mark;
		}
		else
		{
			start = mark;
		}
	}
	return index;
}
