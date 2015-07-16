/******************************************************************************************
 * @file _coder.h
 * @anthor:Croky.Zheng
 * @CreateTime:
 * @descirption:
 *****************************************************************************************/
#ifndef BELIAL_COMMON_CODER_H
#define BELIAL_COMMON_CODER_H
#include "typedef.h"
#include "define.h"
#include "computer.h"

#define UTF16_1(c)		(0xd800|((((c)-0x10000)>>10)&0x3ff))
#define UTF16_2(c)		(0xdc00|((((c)-0x10000))&0x3ff))
#define UTF16(c)		(((c)>=0&&(c)<=0xffff)?(c) : ((((0xd800|((((c)-0x10000)>>10)&0x3ff)))<<16) | (0xdc00|(((c)-0x10000)&0x3ff))))
#define UTF16_LEN(c)	(((c)>=0&&(c)<=0xffff) ? 2 : 4)
#define UTF8_4(c)		(((0xf0|(((c)>>18)&0x3f))<<24) | ((0x80|(((c)>>12)&0x3f))<<16) | ((0x80|(((c)>>6)&0x3f))<<8) | (0x80|(((c)&0x3f))))
#define UTF8_3(c)		(((0xe0|(((c)>>12)&0x3f))<<16) | ((0x80|(((c)>>6)&0x3f))<<8) | (0x80|(((c)&0x3f))))
#define UTF8_2(c)		(((0xc0|(((c)>>6)&0x3f))<<8) | (0x80|(((c)&0x3f))))
#define UTF8_1(c)		(c)
#define UTF8(c)			(((c)>=0&&(c)<=0x7f)?UTF8_1(c):((c)>=0x80&&(c)<=0x7ff)?UTF8_2((c)):((c)>=0x800&&(c)<=0xffff)?UTF8_3((c)):UTF8_4((c)))
#define UTF8_LEN(c)		(((c)>=0&&(c)<=0x7f)?(1):((c)>=0x80&&(c)<=0x7ff)?2:((c)>=0x800&&(c)<=0xffff)?3:4)

#define UNICODE_UTF8_4(c1,c2,c3,c4)		((((c1)&0x7)<<18) | (((c2)&0x3f)<<12) | (((c3)&0x3f)<<6) | ((c4)&0x3f))
#define UNICODE_UTF8_3(c1,c2,c3)		((((c1)&0xf)<<12) | (((c2)&0x3f)<<6) | ((c3)&0x3f))
#define UNICODE_UTF8_2(c1,c2)			((((c1)&0x1f)<<6) | ((c2)&0x3f))
#define UNICODE_UTF8_1(c1)	(c1)

#define GBNOWORD	0xa3bf		//GBK的 ?
#define UNICODENOWORD	0x1fff  //UNICODE ?
#define BIGNOWORD	0x53a1		//big5 ?

#define ISZHWORD_UNICODE(c)		((((c)>=19968)&&((c)<=40869)) ? TRUE:FALSE)//是否是中文字,Unicode
#define ERROR_NO	0
#define ERROR_NULL 1
#define ERROR_PARAM 2

//UNICODE分段数据特征，用于GBK和big5
typedef struct
{
	unsigned short startIndex;		//此段最小编码对应的映射编码的起始索引
	unsigned short terminalIndex;		//此段最大编码对应的映射编码的起始索引,此段明显浪费
	unsigned short minCode;			//段中最小编码
	unsigned short maxCode;			//段中最大编码
} CoderSeg,*CoderSegPtr;

//UNICODE32分段数据特征,应用于UNICODE32和GB18030
typedef struct
{
	long startIndex;		//此段最小编码对应的映射编码的起始索引
	long terminalIndex;		//此段最大编码对应的映射编码的起始索引,此段明显浪费
	long minCode;			//段中最小编码
	long maxCode;			//段中最大编码
} CoderSegEx,*CoderSegExPtr;


/*
1字节 0xxxxxxx (0000 0000 --- 0000 007f)	//0x0
2字节 110xxxxx 10xxxxxx (0000 0080 --- 0000 07FF)	//0xc0 
3字节 1110xxxx 10xxxxxx 10xxxxxx (0000 0800 --- 0000 FFFF)0xe0 
4字节 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx (0001 0000 --- 001F FFFF)//0xf0 
5字节 111110xx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx (0020 0000 --- 03FF FFFF)//0xf8 
6字节 1111110x 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx (0400 0000 --- 07FFF FFFF)//0xfc 
*/
//支持的编码类型
typedef	enum
{
	ST_NULL,
	ST_ASCII,
	ST_UTF8,
	ST_UTF16,
	ST_UNICODE,
	ST_BIG5,
	ST_GBK,
	ST_GB18030,
	ST_MAX
} StrCoder;

//字符串长度
typedef struct __string_size {
	unsigned long	length;		//字符个数
	unsigned long	b_length;	//字节长度
} StrSize,*StrSizePtr;

/*
typedef STRSIZE StrSize;
typedef StrSize PStrSize;
typedef StrCoder StrCoder;
*/

#ifdef __cplusplus
extern "C" {	//兼容C++
#endif

//将UTF8编码的字符串编码为srccoder指定的字符串
//@return 不支持的编码返回FALSE，反之为TRUE
EXTERN	Boolean	DecoderUTF8(char*	utf8,//UTF8字符串
									  char*	desc,//目标字符串
									  unsigned long srclen,//源字符串字节长度
									  StrCoder desccoder,
									  StrSizePtr size);//目标字符串长度

//将UTF16编码的字符串编码为descoder指定的字符串
//@return 不支持的编码返回FALSE，反之为TRUE
EXTERN	Boolean	DecoderUTF16(char*	utf16,//unicode字符串
									  char*	desc,//目标字符串
									  unsigned long srclen,//源字符串字节长度
									  StrCoder desccoder,
									  StrSizePtr size);//目标字符串长度

//将big5编码的字符串编码为descoder指定的字符串
//@return 不支持的编码返回FALSE，反之为TRUE
EXTERN	Boolean	DecoderBIG5(char*	big5,//big5字符串
									  char*	desc,//目标字符串
									  unsigned long srclen,//源字符串字节长度
									  StrCoder desccoder,
									  StrSizePtr size);//目标字符串长度

//将gbk编码的字符串编码为descoder指定的字符串
//@return 不支持的编码返回FALSE，反之为TRUE
EXTERN	Boolean	DecoderGBK(char*	gbk,//gbk字符串
									  char*	desc,//目标字符串
									  unsigned long srclen,//源字符串字节长度
									  StrCoder desccoder,
									  StrSizePtr size);//目标字符串长度

//将gb18030编码的字符串编码为descoder指定的字符串
//@return 不支持的编码返回FALSE，反之为TRUE
EXTERN	Boolean	DecoderGB18030(char*	gb18030,//gb18030字符串
									  char*	desc,//目标字符串
									  unsigned long srclen,//源字符串字节长度
									  StrCoder desccoder,
									  StrSizePtr size);//目标字符串长度

EXTERN	Boolean	DecoderASCII(char*	ascii,//ascii字符串
						char*	desc,//目标字符串
						unsigned long srclen,//源字符串字节长度
						StrCoder desccoder,
						StrSizePtr size);//目标字符串长度
//将UNICODE 32编码的字符串编码为descoder指定的字符串
EXTERN	Boolean	DecoderUNICODE(char*	uni32,//unicode32字符串
						char*	desc,//目标字符串
						unsigned long srclen,//源字符串字节长度
						StrCoder desccoder,
						StrSizePtr size);//目标字符串长度



//返回BIG5字符对应的UNICODE编码，没有返回0x3f (?)
EXTERN	unsigned short	GetBig5ByUnicode(unsigned short unicode);

//返回GBK字符对应的UNICODE编码，没有返回0x3f (?)
EXTERN	unsigned short	GetGbkByUnicode(unsigned short unicode);

//返回Gb18030字符对应的UNICODE编码，没有返回0x3f (?)
EXTERN	long		GetGb18030ByUnicode(long unicode);

//返回UNICODE字符对应的BIG5编码
EXTERN	unsigned short	GetUnicodeByBig5(unsigned short big5);

//返回UNICODE字符对应的gbk编码
EXTERN	unsigned short	GetUnicodeByGbk(unsigned short gbk);

//返回UNICODE字符对应的gb18030编码
EXTERN	long		GetUnicodeByGb18030(long gb18030);

//此数据是否是GBK编码，范围查找
EXTERN Boolean	IsGbk(unsigned short gbk);

//此数据是否是Gb2312编码，范围查找
EXTERN Boolean	IsGb2312(unsigned short gb2312);

//是否是GBK中文字
EXTERN Boolean IsGbkWord(unsigned short gbkcode);

//是否是GB2312中文字
EXTERN Boolean IsGb2312Word(unsigned short gb2312);

EXTERN void	SetUTF16ByteOrder(Endian endian);
EXTERN Endian	GetUTF16ByteOrder();
#ifdef __cplusplus
}
#endif
#endif