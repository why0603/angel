/******************************************************************************************
 * @file typedef.h
 * @anthor:Croky.Zheng
 * @CreateTime:
 * @descirption:
 *****************************************************************************************/
#ifndef BELIAL_COMMON_TYPEDEF_H
#define BELIAL_COMMON_TYPEDEF_H

#ifndef FAR
#ifdef USING_C166	//c166����
#define FAR _huge
#else
#define FAR
#endif
#endif

//��ֵ���Ͷ���
typedef	unsigned	char	UINT8;
typedef	unsigned	short	UINT16;
typedef unsigned	int		UINT;
typedef unsigned	long	UINT32;

typedef UINT8		BYTE;
typedef BYTE *	PBYTE;

typedef	char	INT8;
typedef	short	INT16;
typedef	int		INT;
typedef	long	INT32;


#if defined(WINDOWS) || defined(WIN32)
typedef __int64		INT64;
typedef unsigned __int64	UINT64;
#else
typedef long long INT64;
typedef unsigned long long	UINT64;
#endif

#if defined(MAC) || defined(LINUX)
typedef long long __int64;
#endif
//��ֵ����ָ�붨��
typedef	UINT8*	PUINT8;
typedef	UINT16*	PUINT16;
typedef	UINT*	PUINT;
typedef	UINT32*	PUINT32;
typedef INT64*	PINT64;
typedef	void	VOID;
typedef	VOID*	PVOID;
typedef	INT*	PINT;
typedef	INT8*	PINT8;
typedef	INT16*	PINT16;
typedef	INT32*	PINT32;

//������C166����ֵ����ָ�붨��
typedef	UINT8		FAR *	LPUINT8;
typedef	UINT16		FAR *	LPUINT16;
typedef	UINT		FAR *	LPUINT;
typedef	UINT32		FAR *	LPUINT32;

typedef	INT			FAR *	LPINT;
typedef	INT8		FAR *	LPINT8;
typedef	INT16		FAR *	LPINT16;
typedef	INT32		FAR *	LPINT32;


#ifdef WINDOWS
typedef wchar_t WCHAR;    // wc,   16-bit UNICODE character
#else
// some Macintosh compilers don't define wchar_t in a convenient location, or define it as a char
typedef unsigned short WCHAR;    // wc,   16-bit UNICODE character
#endif

//�ַ����͵Ķ���
//typedef unsigned	char	ACHAR;//��ָ��ACSII����
typedef char				CHAR;//һЩ���ֽ��ַ�������Ϊ����

//�ַ�ָ�����͵Ķ���
//typedef ACHAR *	CHAR;
typedef CHAR *	PCHAR;
typedef WCHAR *	PWCHAR;


typedef const PCHAR		CPCHAR;
typedef const PWCHAR	CPWCHAR;
typedef UINT	BOOL;

//��������
typedef enum
{
	//δ֪����
	Unknow_Type = 0,

	//�ֽ�����
	Char_Type = 1,

	//������
	Short_Type = 2,
	
	//����,4�ֽ�
	Long_Type = 4,

	//����,8�ֽ�
	LongLong_Type = 8,
	
	//ָ������
	Pointer_Type,

	//��\0��β�ı䳤�ַ���
	Chars_Type,
	
	//��\0\0��β��unicode�ַ���
	Unicode_Type,
	//�䳤�ֽ�����,���ַ�ʽ��ô�����
	//�����ϲ���÷��Լ�Э��
	VarChar_Type,
	//All_Type = Char_Type | Short_Type | Long_Type | LongLong_Type | Chars_Type
} DataType;


#define ISPOINTERTYPE(x) (((x) == Unknow_Type) || \
	((x) == Pointer_Type) || \
	((x) == Chars_Type))

typedef enum
{
	False = 0,
	True = 1
} Boolean;

typedef enum
{
	//С��
	Compare_Less = -1,

	//����
	Compare_Equal = 0,

	//����
	Compare_Great = 1,
} CompareResult;
#endif