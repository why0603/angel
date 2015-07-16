/******************************************************************************************
* @file computer.c
* @anthor:Croky.Zheng
* @CreateTime:2012/2/25
* @descirption:һЩ���õĻ�������
*****************************************************************************************/

#include "computer.h"
#if !defined(LINUX)
#include <Windows.h>
#include <intrin.h>
#endif
/**
* ��˷���������0x01234567
* ��Ӧ����������Ϊ��0x01   0x23   0x45   0x67
* �ڴ��ַ��С����
* С�˷����෴���ڴ��ַ�Ӵ�С
* �����ֽ���Ϊ��˷���JAVA���ֽ��������С�˷�
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

//��ȡCPU���ֳ�
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
 * GetTickCount���ص��Ǳ���Windows����������ms��
 * ����WIN API
 * 64λ��֧��������� http://msdn.microsoft.com/en-us/library/hh977022.aspx
 */
unsigned __int64 GetCpuTimeStampCounter()
{
	//RDTSC-Read Time-Stamp Counter
	//�Կ�������CPU������ʱ��������
/*
#if defined(WIN32) || defined(WINDOWS)
	//windows ֱ��ʹ��eax��Ϊ��������ֵ��������ʵ�����͹���
	__asm
	{
		_emit 0x0F			;RDTSCָ�������
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
//windows 32λƽ̨
#if defined(ASM) && !defined(LINUX)
	if (cpuwordsize == CpuWordSize_64)
	{
		//64λƽ̨��֧���������
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
			_emit 0x0F			;RTDSCָ�������
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
		counter = GetTickCount();//��������������ڵ�ʱ��
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
�Ĵ�������		16		32		64		
	000			ax		eax		rax			//Ϊ�������ô��ݲ��� 
	001			cx		ecx		rcx			//�����÷�����
	010			dx		edx		rdx			//Ϊ�������ô��ݲ���
	011			bx		ebx		rbx			//Ϊ�������ô��ݲ���
	100			sp		esp		rsp			//ջ����/ָ֡��
	101			bp		ebp		rbp			//ջ����/ָ֡��
	110			si		esi		rsi			//Ϊ�������ô��ݲ���
	111			di		edi		rdi			//Ϊ�������ô��ݲ���
	1000		r8w		r8d		r8			//Ϊ�������ô��ݲ���
	1001		r9w		r9d		r9			//Ϊ�������ô��ݲ��� 
	1010		r10w	r10d	r10			//��ʱ
	1011		r11w	r11d	r11			//��ʱ
	1100		r12w	r12d	r12			//�����÷�����
	1101		r13w	r13d	r13			//�����÷�����
	1110		r14w	r14d	r14			//�����÷�����
	1111		r15w	r15d	r15			//�����÷�����	
	�����Ĵ���
	rflags			��־�Ĵ��� 
	rip				ָ��ָ��
	mmx0/st0		64 λý��͸���Ĵ���
	mmx1/st1		64 λý��͸���Ĵ���
	mmx2/st2		64 λý��͸���Ĵ���
	mmx3/st3		64 λý��͸���Ĵ���
	mmx4/st4		64 λý��͸���Ĵ���
	mmx5/st5		64 λý��͸���Ĵ���
	mmx6/st6		64 λý��͸���Ĵ���
	mmx7/st7		64 λý��͸���Ĵ���
	xmm0		128 λý��Ĵ���
	xmm1		128 λý��Ĵ���
	xmm2		128 λý��Ĵ���
	xmm3		128 λý��Ĵ���
	xmm4		128 λý��Ĵ���
	xmm5		128 λý��Ĵ���
	xmm6		128 λý��Ĵ���
	xmm7		128 λý��Ĵ���
	xmm8		128 λý��Ĵ���
	xmm9		128 λý��Ĵ���
	xmm10		128 λý��Ĵ���
	xmm11		128 λý��Ĵ���
	xmm12		128 λý��Ĵ���
	xmm13		128 λý��Ĵ���
	xmm14		128 λý��Ĵ���
	xmm15		128 λý��Ĵ���
	cs			�μĴ���
	os			�μĴ���
	es			�μĴ���
	fs			�μĴ���
	gs			�μĴ���
	ss			�μĴ���
	fcw			fxsave �� fxstor �ڴ�ӳ�������
	fsw			fxsave �� fxstor �ڴ�ӳ��״̬��
	ftw			fxsave �� fxstor �ڴ�ӳ������
	fop			fxsave �� fxstor �ڴ�ӳ�����һ�� x87 op ����
	frip		fxsave �� fxstor �ڴ�ӳ�� 64 λ�����ƫ��
	frdp		fxsave �� fxstor �ڴ�ӳ�� 64 λ���ڶ�ƫ��
	mxcsr		fxsave �� fxstor �ڴ�ӳ�� 128 λý��ָ����ƺ�״̬�Ĵ���
	mxcsr_mask	mxcsr_mask �е�����λָʾ mxcsr ��֧�ֵ�����λ
*/

#if defined(WIN32) || defined(WINDOWS)
//ָ���̶���CPU
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
	//1������ִ�е�CPU����λ��ʾ
	//����������1,3��CPU�����У�����0x4=0101 ��1,2,3�����У�����0xe = 1110
	SetThreadAffinityMask(hThread,mask);
	//��ʵ������
	//��õİ취����Ҫ�������߳�ȫ�����˵�����CPU
}

/**
	CONTEXT_CONTROL
	��ȡEBP��EIP��CS��EFLAGS��ESP��SS�Ĵ�����ֵ��
	CONTEXT_INTEGER
	��ȡEAX��EBX��ECX��EDX��ESI��EDI�Ĵ�����ֵ��
	CONTEXT_SEGMENTS
	��ȡDS��ES��FS��GS�Ĵ�����ֵ��
	CONTEXT_FLOATING_POINT
	��ȡ�йظ������Ĵ�����ֵ��
	CONTEXT_DEBUG_REGISTERS
	��ȡDR0��DR1��DR2��DR3��DR6��DR7�Ĵ�����ֵ��
	CONTEXT_FULL
	����CONTEXT_CONTROL | CONTEXT_INTEGER | CONTEXT_SEG
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

//��ȡ��־�Ĵ�����ֵ
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
			push eax;//����EAX
			pushfd;//��eflags��־ѹ���ջ,popfd ��ջ��д��eflags
			pop eax;//��ջ��д��eax(ջ����EFLAGS)
			mov fls,eax;//д�����
			pop eax;//�ָ�EAX��ֵ
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
			lea eax,__line__;//����ᱣ��EIP��ֵ
			mov fls,eax;
			pop eax;
		}
		*/