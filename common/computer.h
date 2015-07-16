/******************************************************************************************
 * @file computer.h
 * @anthor:Croky.Zheng
 * @CreateTime:2012/2/25
 * @descirption:一些有用的基础函数
 *****************************************************************************************/

#ifndef BELIAL_COMMON_COMPUTER_H
#define BELIAL_COMMON_COMPUTER_H
#include "typedef.h"
#include "define.h"

//判断是否溢出，溢出>0
#define ISOF(x) (x)&(0x400)

typedef enum enum_cpu_endian {
	Endian_Big = 0,//大端法
	Endian_Small = 1,//小端法
	Endian_Net = 0,//网络字节序
} Endian;

typedef enum enum_cpu_word_size {
	CpuWordSize_Nothing = 0,//计算错误
	CpuWordSize_8 = 8,//16位 
	CpuWordSize_16 = 16,//16位 
	CpuWordSize_32 = 32,//32位
	CpuWordSize_64 = 64,//64位
} CpuWordSize;

#ifdef __cplusplus
extern "C" {	//兼容C++
#endif

/**
 * 大端法保存数据0x01234567
 * 对应的数据依次为：0x01   0x23   0x45   0x67
 * 内存地址从小到大
 * 小端法则相反，内存地址从大到小
 * 网络字节序为大端法，JAVA的字节序好像是小端法
 * @return CPU字节序
 */
EXTERN Endian GetCpuEndian();

/**
 * 计算CPU的字长，调用方法
 * CYGetCpuWordSize(NULL,NULL);即可
 * 用于计算两个long指针的距离
 * 在编译阶段则可以通过sizeof(long)来获取
 * @return 获取CPU的字长
 */
EXTERN CpuWordSize GetCpuWordSize(void *pDummy1,void *pDummy2);

/**
 * 获取CPU当前的运行时钟
 */
EXTERN unsigned __int64 GetCpuTimeStampCounter();

/**
 * 获取标志寄存器的值
 ------------------------------------------------------------------------------
 |31     |22|21|20|19|18|17|16|15|14|13|12|11|10|9 |8 |7 |6 |5 |4 |3 |2 |1 |0 |
 |-----------------------------------------------------------------------------
 |          |ID|VP|VF|AC|VM|RF|0 |NT|PL|IO|OF|DF|IF|TF|SF|ZF|0 |AF|0 |PF|1 |CF|
 ------------------------------------------------------------------------------
	IF(Interruptenable)——位9,中断许可标志。控制处理器对可屏蔽硬件中断请求的响应。置1则开启可屏蔽硬件中断响应，置0则关闭可屏蔽硬件中断响应。IF标志不影响异常和不可屏蔽中断（NMI）的产生。CPL、IOPL和控制寄存器CR4中的VME标志决定着IF标志是否可由CLI、STI、POPF、POPFD和IRET指令修改。
	IOPL(I/OPrivilegeLevelField)——位12&位13,I/O特权域。标示当前进程或任务的I/O特权级别。当前进程或任务的CPL必须小于或等于IOPL才可以访问I/O地址空间。只有CPL为0的进程才可用POPF和IRET指令修改这个域。IOPL是控制IF标志修改的机制之一，同时也是当虚拟模式扩展生效时（控制寄存器CR4中的VME置位），控制虚拟8086模式下中断处理的机制之一。
	NT(NestedTask)——位14，嵌套任务。控制被中断和被调用的任务的嵌套执行链。处理器调用一个由CALL指令、中断或者异常触发的任务时设置该位，调用IRET指令返回时检测并修改该位。该标志可以由POPF/POPFD指令直接置位或置0，然而在应用程序中修改该标志的状态会产生不可预料的异常。
	RF(Resume)——位16,恢复。控制处理器对指令断点的响应。置1则暂时禁用指令断点产生调试异常(debugexceptions,＃DE)，但是其它异常情况仍可以产生异常。置0则指令断点产生调试异常。RF标志的主要功能是许可从调试异常(指令断点引发的)后面的那个指令开始继续执行。调试软件必须在用IRETD指令返回到被中断程序之前，将栈中的EFLAGES映象中的该位置为1，以阻止指令断点产生另外的调试异常。在返回并成功执行断点指令之后，处理器会自动清零该位，从而许可继续产生指令断点故障。
	VM(Virtual-8086mode)——位17,虚拟8086模式。置1则进入虚拟8086模式，置0则返回保护模式。
	AC(Alignmentcheck)——位18,对齐检查。置位该标志和控制寄存器CR0的AM标志则启用对内存引用的对齐检查，清除这两个标志则禁用对齐检查。当引用一个没有对齐的操作数时，将会产生一个对齐检查异常，比如在奇地址引用一个字地址或在不是4的倍数的地址引用一个双字地址。对齐检查异常只在用户态（3级特权）下产生。默认特权为0的内存引用，比如段描述符表的装载，并不产生这个异常，尽管同样的操作在用户态会产生异常。对齐检查异常用于检查数据的对齐，当处理器之间交换数据时这很有用，交换数据需要所有的数据对齐。对齐检查异常也可供解释程序使用。让某些指针不对齐就好比做上特殊标记，这样就无需对每个指针都进行检查，只在用到的时候，对这些特殊指针进行处理就可以了。
	VIF(VirtualInterrupt)——位19,虚拟中断。是IF标志的一个虚拟映象。这个标志是和VIP标志一起使用的。当控制寄存器CR4中的VME或者PVI标志置为1且IOPL小于3时，处理器只识别VIF标志（VME标志用来启用虚拟8086模式扩展，PVI标志启用保护模式下的虚拟中断）。
	VIP(Virtualinterruptpending)——位20,虚拟中断等待。置1表明有一个正在等待处理的中断，置0表明没有等待处理的中断。该标志和VIF一起使用。处理器读取该标志但从来不修改它。当VME标志或者控制寄存器CR4中的PVI标志置1且IOPL小于3时，处理器只识别VIP标志。（VME标志启用虚拟8086模式扩展，PVI标志启用保护模式虚拟中断）。
	ID(Identification)——识别（第21位）。置1或0表明是否支持CPUID指令。
	其它的通用标志：
	CF——进位标志
	PF——恢复标志
	AF——辅助标志
	ZF——零标志
	SF——负号标志
	DF——方向标志
	OF——溢出标志

	综合上面所述，当需要检测某次运算之后是否有溢出，比如（0xefffffff * 3在32位运算中，即已经溢出)
	则需要检测OF标志位是否为1
 */
EXTERN unsigned __int64 GetRegFlags();

#if defined(WIN32) || defined(WINDOWS)
/**
 * 指定线程在固定的CPU上运行
 */
EXTERN void FixedCpuRun(void* hThread);//typedef void* HANDLE;

/**
 * 获取线程的标志位
 */
EXTERN unsigned long GetThreadRegFlags(HANDLE hThread);
#endif
#ifdef __cplusplus
}
#endif
#endif