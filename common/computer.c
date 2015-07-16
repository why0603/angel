/******************************************************************************************
* @file computer.c
* @anthor:Croky.Zheng
* @CreateTime:2012/2/25
* @descirption:一些有用的基础函数
*****************************************************************************************/

#include "computer.h"
#if !defined(LINUX)
#include <Windows.h>
#include <intrin.h>
#endif
/**
* 大端法保存数据0x01234567
* 对应的数据依次为：0x01   0x23   0x45   0x67
* 内存地址从小到大
* 小端法则相反，内存地址从大到小
* 网络字节序为大端法，JAVA的字节序好像是小端法
*/
Endian GetCpuEndian()
{
	union
	{
		unsigned int iValue;
		unsigned char cByte;
	} ByteEndian;
	ByteEndian.iValue = 0x1;
	return (Endian)ByteEndian.cByte;
}

//获取CPU的字长
CpuWordSize GetCpuWordSize(void *pDummy1,void *pDummy2) 
{
	long offset = (long)&pDummy2 - (long)&pDummy1;
	int ret = 0; 
	offset = (offset < 0) ? -offset : offset;
	if (8 == offset)
	{
		ret = 64;
	}
	else if (4 == offset)
	{
		ret = 32;
	}
	else if (2 == offset)
	{
		ret = 16;
	}
	else if (1 == offset)
	{
		ret = 8;
	}
	else
	{
		ret = 0;
	}
	return (CpuWordSize)ret;
}

/*
 * QueryPerformanceCounter 
 * QueryPerformanceFrequency
 * GetTickCount返回的是本次Windows启动以来的ms数
 * 两个WIN API
 * 64位不支持内联汇编 http://msdn.microsoft.com/en-us/library/hh977022.aspx
 */
unsigned __int64 GetCpuTimeStampCounter()
{
	//RDTSC-Read Time-Stamp Counter
	//自开机以来CPU经历的时钟周期数
/*
#if defined(WIN32) || defined(WINDOWS)
	//windows 直接使用eax作为函数返回值，所以其实这样就够了
	__asm
	{
		_emit 0x0F			;RDTSC指令操作码
		_emit 0x31			;
	}
#endif
*/
#if defined(ASM)	
	unsigned long high;
	unsigned long low;
#endif
	unsigned long long counter = 0;
	unsigned long cpuwordsize = GetCpuWordSize(NULL,NULL);
#if defined(LINUX) && defined(ASM)
/*
	typedef union
	{
		unsigned long long counter;
		struct
		{
			unsigned long high;
			unsigned long low;
		} CounterStruct;
	} CpuCounter;
	CpuCounter cpuCounter;
*/
	if (cpuwordsize == CpuWordSize_64)
	{
		__asm__ volatile ("rdtsc" : "=A" (counter));
	}
	else if (cpuwordsize == CpuWordSize_32)
	{
		__asm__ __volatile__ ("rdtsc" : "=a"(low), "=d"(high));
		counter = low | (high << 32);
	}
//#endif
//#if defined(WIN32) || defined(WINDOWS)
#endif
//windows 32位平台
#if defined(ASM) && !defined(LINUX)
	if (cpuwordsize == CpuWordSize_64)
	{
		//64位平台不支持内联汇编
		//_asm mov counter,rax;
		counter = __rdtsc();
	}
	else if (cpuwordsize == CpuWordSize_32)
	{
		_asm push eax;
		_asm push edx;
		/*
		__asm
		{
			_emit 0x0F			;RTDSC指令操作码
			_emit 0x31			;
		}
		*/
		_asm rtdsc;
		_asm mov low,eax;
		_asm mov high,edx;
		_asm pop edx;
		_asm pop eax;
		counter = low | (high << 32);
	}
#endif
#if !defined(ASM) && !defined(LINUX)
	 LARGE_INTEGER time;
	 //LARGE_INTEGER freq;
	if (cpuwordsize == CpuWordSize_64)
	{
		//counter = GetTickCount64();
		counter = __rdtsc();
	}
	else if (cpuwordsize == CpuWordSize_32)
	{
		counter = GetTickCount();//计算机启动到现在的时间
	}
	else
	{
		QueryPerformanceCounter(&time);
		//QueryPerformanceFrequency(&freq);
		//counter = (unsigned __int64)(time.QuadPart / freq.QuadPart);
		counter = time.QuadPart;
	}
#endif
	return counter;
}


//unsigned __int64 __readeflags(void);
/*
寄存器编码		16		32		64		
	000			ax		eax		rax			//为函数调用传递参数 
	001			cx		ecx		rcx			//被调用方保存
	010			dx		edx		rdx			//为函数调用传递参数
	011			bx		ebx		rbx			//为函数调用传递参数
	100			sp		esp		rsp			//栈管理/帧指针
	101			bp		ebp		rbp			//栈管理/帧指针
	110			si		esi		rsi			//为函数调用传递参数
	111			di		edi		rdi			//为函数调用传递参数
	1000		r8w		r8d		r8			//为函数调用传递参数
	1001		r9w		r9d		r9			//为函数调用传递参数 
	1010		r10w	r10d	r10			//临时
	1011		r11w	r11d	r11			//临时
	1100		r12w	r12d	r12			//被调用方保存
	1101		r13w	r13d	r13			//被调用方保存
	1110		r14w	r14d	r14			//被调用方保存
	1111		r15w	r15d	r15			//被调用方保存	
	其他寄存器
	rflags			标志寄存器 
	rip				指令指针
	mmx0/st0		64 位媒体和浮点寄存器
	mmx1/st1		64 位媒体和浮点寄存器
	mmx2/st2		64 位媒体和浮点寄存器
	mmx3/st3		64 位媒体和浮点寄存器
	mmx4/st4		64 位媒体和浮点寄存器
	mmx5/st5		64 位媒体和浮点寄存器
	mmx6/st6		64 位媒体和浮点寄存器
	mmx7/st7		64 位媒体和浮点寄存器
	xmm0		128 位媒体寄存器
	xmm1		128 位媒体寄存器
	xmm2		128 位媒体寄存器
	xmm3		128 位媒体寄存器
	xmm4		128 位媒体寄存器
	xmm5		128 位媒体寄存器
	xmm6		128 位媒体寄存器
	xmm7		128 位媒体寄存器
	xmm8		128 位媒体寄存器
	xmm9		128 位媒体寄存器
	xmm10		128 位媒体寄存器
	xmm11		128 位媒体寄存器
	xmm12		128 位媒体寄存器
	xmm13		128 位媒体寄存器
	xmm14		128 位媒体寄存器
	xmm15		128 位媒体寄存器
	cs			段寄存器
	os			段寄存器
	es			段寄存器
	fs			段寄存器
	gs			段寄存器
	ss			段寄存器
	fcw			fxsave 和 fxstor 内存映像控制字
	fsw			fxsave 和 fxstor 内存映像状态字
	ftw			fxsave 和 fxstor 内存映像标记字
	fop			fxsave 和 fxstor 内存映像最后一个 x87 op 代码
	frip		fxsave 和 fxstor 内存映像 64 位代码段偏移
	frdp		fxsave 和 fxstor 内存映像 64 位日期段偏移
	mxcsr		fxsave 和 fxstor 内存映像 128 位媒体指令控制和状态寄存器
	mxcsr_mask	mxcsr_mask 中的设置位指示 mxcsr 中支持的特征位
*/

#if defined(WIN32) || defined(WINDOWS)
//指定固定的CPU
void FixedCpuRun(HANDLE hThread)
{
	DWORD_PTR mask;
	DWORD_PTR sysmask;
	if (NULL == hThread)
	{
		hThread = GetCurrentThread();
	}
	mask = GetProcessAffinityMask(GetCurrentProcess(),&mask,&sysmask);
	//SetProcessAffinityMask(GetCurrentProcess(),mask);
	//1，允许执行的CPU，用位表示
	//比如允许在1,3个CPU上运行，则是0x4=0101 在1,2,3上运行，则是0xe = 1110
	SetThreadAffinityMask(hThread,mask);
	//其实还不够
	//最好的办法还需要将其他线程全部搬运到其他CPU
}

/**
	CONTEXT_CONTROL
	获取EBP，EIP，CS，EFLAGS，ESP和SS寄存器的值。
	CONTEXT_INTEGER
	获取EAX，EBX，ECX，EDX，ESI和EDI寄存器的值。
	CONTEXT_SEGMENTS
	获取DS，ES，FS和GS寄存器的值。
	CONTEXT_FLOATING_POINT
	获取有关浮点数寄存器的值。
	CONTEXT_DEBUG_REGISTERS
	获取DR0，DR1，DR2，DR3，DR6，DR7寄存器的值。
	CONTEXT_FULL
	等于CONTEXT_CONTROL | CONTEXT_INTEGER | CONTEXT_SEG
*/
unsigned long GetThreadRegFlags(HANDLE hThread)
{
	CONTEXT cotext;
	if (NULL == hThread)
	{
		hThread = GetCurrentThread();
	}
	cotext.ContextFlags = CONTEXT_FULL;
	GetThreadContext(hThread,&cotext);
	return cotext.EFlags;
}

#endif

//获取标志寄存器的值
unsigned __int64 GetRegFlags()
{
	unsigned __int64 fls = 0;
	unsigned long cpuwordsize = GetCpuWordSize(NULL,NULL);
#if !defined(LINUX)
	if (cpuwordsize == CpuWordSize_64)
	{
		//counter = GetTickCount64();
		fls = __readeflags();
	}
	else if (cpuwordsize == CpuWordSize_32)
	{
#if defined(ASM)
		__asm
		{
			push eax;//保存EAX
			pushfd;//将eflags标志压入堆栈,popfd 将栈顶写入eflags
			pop eax;//将栈顶写入eax(栈顶即EFLAGS)
			mov fls,eax;//写入变量
			pop eax;//恢复EAX的值
		}
#endif
	}
#else
	__asm__ volatile ("pushf/n/t"
              "popl %%eax"
              : "=a"(fls)
              :
              : "memory" 
              );
#endif
	return fls;
}

		/*
		__asm
		{
			push eax;
__line__:
			lea eax,__line__;//这里会保存EIP的值
			mov fls,eax;
			pop eax;
		}
		*/