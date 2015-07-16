/******************************************************************************************
 * @file define.h
 * @anthor:Croky.Zheng
 * @CreateTime:
 * @descirption:
 *****************************************************************************************/
#ifndef BELIAL_COMMON_DEFINE_H
#define BELIAL_COMMON_DEFINE_H
#ifndef EXTERN
	#if defined(EXPORTDLL) || defined(_WINDLL)
		#define EXTERN  __declspec(dllexport)	//��Ҫ����DLL������
	#elif defined(IMPORTDLL)
		#define EXTERN  __declspec(dllimport)	//��Ҫ�����DLL����
	#else
		#define EXTERN	extern	//�ɾ�̬����
	#endif
#endif

//#if defined(CONSTDEFINED)
	#define CONST		const
	#define NATIVE	static
	#define FALSE  0
	#define TRUE   1
	#define ERROR	-1
	#define	MAXINTEGER	0x7fffffff
	#define MININTEGER	0x80000000
	#define MININTEGERSTRING	"-2147483648"
	#define POINTNUMBER	52429
	#define	EQUAL	0 //����
	#define GREAT	1 //����
	#define LESS	-1//С��

//�ɹ�
#define SUCCESS		1
//#endif
//��ȡ�ṹ���Ա��ƫ����
#define OFFSETOF(s,m)   (size_t)&(((s *)0)->m)

//��ȡ����ĳ���
#define ARRAYLEN(s)	((sizeof(s))/(sizeof(s[0])))

//������v��4�ı���
#define CHECKDIV2(v) (((v)&0x1)?((v)+2-((v)&0x1)):(v))

//������v��4�ı���
#define CHECKDIV4(v) (((v)&0x3)?((v)+4-((v)&0x3)):(v))
//Des ,Tea ,Blowfish���������㷨�������䵽8���ֽڵ�����
//��˿������������ͬ�Ķ�����ȷ�����ĵĳ���
//������v��8�ı���
#define CHECKDIV8(v) (((v)&0x7)?((v)+8-((v)&0x7)):(v))

//������v��16�ı���
#define CHECKDIV16(v) (((v)&0xf)?((v)+16-((v)&0xf)):(v))

//������v��32�ı���
#define CHECKDIV32(v) (((v)&0x1f)?((v)+32-((v)&0x1f)):(v))

//������v��64�ı���
#define CHECKDIV64(v) (((v)&0x3f)?((v)+64-((v)&0x1f)):(v))

#define HIGH32(n)	(((n)>>32)&0xffffffff)	//��32λ
#define LOW32(n)	((n)&0xffffffff)		//��32λ
#define HIGH16(n)	(((n)>>16)&0xffff)		//��16λ
#define LOW16(n)	((n)&0xffff)			//��16λ
#define HIGH8(n)	(((n)>>8)&0xff)			//��8λ
#define LOW8(n)		((n)&0xff)				//��8λ


//�����߼���λ��������
//ѭ����λ,���ѭ����λ31λ
//��xѭ������nλ
//w��ʾ��λ��
#define ROTL(x,n,w) (((x)<<(n&(w-1))) | ((x)>>(w-(n&(w-1)))))

//��xѭ������nλ
#define ROTR(x,n,w) (((x)>>(n&(w-1))) | ((x)<<(w-(n&(w-1)))))
//�������Ҫ���Եľ���
//#pragma warning(disable:9305)//wp64���
//#pragma warning(disable:4616)//wp64���



//�������Ҫ�����LIB
//#pragma comment(lib, "WSock32.lib")
#endif