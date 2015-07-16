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

#define GBNOWORD	0xa3bf		//GBK�� ?
#define UNICODENOWORD	0x1fff  //UNICODE ?
#define BIGNOWORD	0x53a1		//big5 ?

#define ISZHWORD_UNICODE(c)		((((c)>=19968)&&((c)<=40869)) ? TRUE:FALSE)//�Ƿ���������,Unicode
#define ERROR_NO	0
#define ERROR_NULL 1
#define ERROR_PARAM 2

//UNICODE�ֶ���������������GBK��big5
typedef struct
{
	unsigned short startIndex;		//�˶���С�����Ӧ��ӳ��������ʼ����
	unsigned short terminalIndex;		//�˶��������Ӧ��ӳ��������ʼ����,�˶������˷�
	unsigned short minCode;			//������С����
	unsigned short maxCode;			//����������
} CoderSeg,*CoderSegPtr;

//UNICODE32�ֶ���������,Ӧ����UNICODE32��GB18030
typedef struct
{
	long startIndex;		//�˶���С�����Ӧ��ӳ��������ʼ����
	long terminalIndex;		//�˶��������Ӧ��ӳ��������ʼ����,�˶������˷�
	long minCode;			//������С����
	long maxCode;			//����������
} CoderSegEx,*CoderSegExPtr;


/*
1�ֽ� 0xxxxxxx (0000 0000 --- 0000 007f)	//0x0
2�ֽ� 110xxxxx 10xxxxxx (0000 0080 --- 0000 07FF)	//0xc0 
3�ֽ� 1110xxxx 10xxxxxx 10xxxxxx (0000 0800 --- 0000 FFFF)0xe0 
4�ֽ� 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx (0001 0000 --- 001F FFFF)//0xf0 
5�ֽ� 111110xx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx (0020 0000 --- 03FF FFFF)//0xf8 
6�ֽ� 1111110x 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx (0400 0000 --- 07FFF FFFF)//0xfc 
*/
//֧�ֵı�������
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

//�ַ�������
typedef struct __string_size {
	unsigned long	length;		//�ַ�����
	unsigned long	b_length;	//�ֽڳ���
} StrSize,*StrSizePtr;

/*
typedef STRSIZE StrSize;
typedef StrSize PStrSize;
typedef StrCoder StrCoder;
*/

#ifdef __cplusplus
extern "C" {	//����C++
#endif

//��UTF8������ַ�������Ϊsrccoderָ�����ַ���
//@return ��֧�ֵı��뷵��FALSE����֮ΪTRUE
EXTERN	Boolean	DecoderUTF8(char*	utf8,//UTF8�ַ���
									  char*	desc,//Ŀ���ַ���
									  unsigned long srclen,//Դ�ַ����ֽڳ���
									  StrCoder desccoder,
									  StrSizePtr size);//Ŀ���ַ�������

//��UTF16������ַ�������Ϊdescoderָ�����ַ���
//@return ��֧�ֵı��뷵��FALSE����֮ΪTRUE
EXTERN	Boolean	DecoderUTF16(char*	utf16,//unicode�ַ���
									  char*	desc,//Ŀ���ַ���
									  unsigned long srclen,//Դ�ַ����ֽڳ���
									  StrCoder desccoder,
									  StrSizePtr size);//Ŀ���ַ�������

//��big5������ַ�������Ϊdescoderָ�����ַ���
//@return ��֧�ֵı��뷵��FALSE����֮ΪTRUE
EXTERN	Boolean	DecoderBIG5(char*	big5,//big5�ַ���
									  char*	desc,//Ŀ���ַ���
									  unsigned long srclen,//Դ�ַ����ֽڳ���
									  StrCoder desccoder,
									  StrSizePtr size);//Ŀ���ַ�������

//��gbk������ַ�������Ϊdescoderָ�����ַ���
//@return ��֧�ֵı��뷵��FALSE����֮ΪTRUE
EXTERN	Boolean	DecoderGBK(char*	gbk,//gbk�ַ���
									  char*	desc,//Ŀ���ַ���
									  unsigned long srclen,//Դ�ַ����ֽڳ���
									  StrCoder desccoder,
									  StrSizePtr size);//Ŀ���ַ�������

//��gb18030������ַ�������Ϊdescoderָ�����ַ���
//@return ��֧�ֵı��뷵��FALSE����֮ΪTRUE
EXTERN	Boolean	DecoderGB18030(char*	gb18030,//gb18030�ַ���
									  char*	desc,//Ŀ���ַ���
									  unsigned long srclen,//Դ�ַ����ֽڳ���
									  StrCoder desccoder,
									  StrSizePtr size);//Ŀ���ַ�������

EXTERN	Boolean	DecoderASCII(char*	ascii,//ascii�ַ���
						char*	desc,//Ŀ���ַ���
						unsigned long srclen,//Դ�ַ����ֽڳ���
						StrCoder desccoder,
						StrSizePtr size);//Ŀ���ַ�������
//��UNICODE 32������ַ�������Ϊdescoderָ�����ַ���
EXTERN	Boolean	DecoderUNICODE(char*	uni32,//unicode32�ַ���
						char*	desc,//Ŀ���ַ���
						unsigned long srclen,//Դ�ַ����ֽڳ���
						StrCoder desccoder,
						StrSizePtr size);//Ŀ���ַ�������



//����BIG5�ַ���Ӧ��UNICODE���룬û�з���0x3f (?)
EXTERN	unsigned short	GetBig5ByUnicode(unsigned short unicode);

//����GBK�ַ���Ӧ��UNICODE���룬û�з���0x3f (?)
EXTERN	unsigned short	GetGbkByUnicode(unsigned short unicode);

//����Gb18030�ַ���Ӧ��UNICODE���룬û�з���0x3f (?)
EXTERN	long		GetGb18030ByUnicode(long unicode);

//����UNICODE�ַ���Ӧ��BIG5����
EXTERN	unsigned short	GetUnicodeByBig5(unsigned short big5);

//����UNICODE�ַ���Ӧ��gbk����
EXTERN	unsigned short	GetUnicodeByGbk(unsigned short gbk);

//����UNICODE�ַ���Ӧ��gb18030����
EXTERN	long		GetUnicodeByGb18030(long gb18030);

//�������Ƿ���GBK���룬��Χ����
EXTERN Boolean	IsGbk(unsigned short gbk);

//�������Ƿ���Gb2312���룬��Χ����
EXTERN Boolean	IsGb2312(unsigned short gb2312);

//�Ƿ���GBK������
EXTERN Boolean IsGbkWord(unsigned short gbkcode);

//�Ƿ���GB2312������
EXTERN Boolean IsGb2312Word(unsigned short gb2312);

EXTERN void	SetUTF16ByteOrder(Endian endian);
EXTERN Endian	GetUTF16ByteOrder();
#ifdef __cplusplus
}
#endif
#endif