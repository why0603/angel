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
		#define EXTERN  __declspec(dllexport)	//需要导出DLL函数表
	#elif defined(IMPORTDLL)
		#define EXTERN  __declspec(dllimport)	//需要导入的DLL函数
	#else
		#define EXTERN	extern	//可静态调用
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
	#define	EQUAL	0 //等于
	#define GREAT	1 //大于
	#define LESS	-1//小于

//成功
#define SUCCESS		1
//#endif
//获取结构体成员的偏移量
#define OFFSETOF(s,m)   (size_t)&(((s *)0)->m)

//获取数组的长度
#define ARRAYLEN(s)	((sizeof(s))/(sizeof(s[0])))

//补充数v到4的倍数
#define CHECKDIV2(v) (((v)&0x1)?((v)+2-((v)&0x1)):(v))

//补充数v到4的倍数
#define CHECKDIV4(v) (((v)&0x3)?((v)+4-((v)&0x3)):(v))
//Des ,Tea ,Blowfish三个加密算法都是扩充到8个字节的明文
//因此可以试用这个共同的定义来确定密文的长度
//补充数v到8的倍数
#define CHECKDIV8(v) (((v)&0x7)?((v)+8-((v)&0x7)):(v))

//补充数v到16的倍数
#define CHECKDIV16(v) (((v)&0xf)?((v)+16-((v)&0xf)):(v))

//补充数v到32的倍数
#define CHECKDIV32(v) (((v)&0x1f)?((v)+32-((v)&0x1f)):(v))

//补充数v到64的倍数
#define CHECKDIV64(v) (((v)&0x3f)?((v)+64-((v)&0x1f)):(v))

#define HIGH32(n)	(((n)>>32)&0xffffffff)	//高32位
#define LOW32(n)	((n)&0xffffffff)		//低32位
#define HIGH16(n)	(((n)>>16)&0xffff)		//高16位
#define LOW16(n)	((n)&0xffff)			//低16位
#define HIGH8(n)	(((n)>>8)&0xff)			//高8位
#define LOW8(n)		((n)&0xff)				//低8位


//定义逻辑移位操作运算
//循环移位,最多循环移位31位
//将x循环左移n位
//w表示总位数
#define ROTL(x,n,w) (((x)<<(n&(w-1))) | ((x)>>(w-(n&(w-1)))))

//将x循环右移n位
#define ROTR(x,n,w) (((x)>>(n&(w-1))) | ((x)<<(w-(n&(w-1)))))
//这里加入要忽略的警告
//#pragma warning(disable:9305)//wp64检测
//#pragma warning(disable:4616)//wp64检测



//这里加入要导入的LIB
//#pragma comment(lib, "WSock32.lib")
#endif