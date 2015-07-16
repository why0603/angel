/******************************************************************************************
 * @file computer.h
 * @anthor:Croky.Zheng
 * @CreateTime:2012/2/25
 * @descirption:һЩ���õĻ�������
 *****************************************************************************************/

#ifndef BELIAL_COMMON_COMPUTER_H
#define BELIAL_COMMON_COMPUTER_H
#include "typedef.h"
#include "define.h"

//�ж��Ƿ���������>0
#define ISOF(x) (x)&(0x400)

typedef enum enum_cpu_endian {
	Endian_Big = 0,//��˷�
	Endian_Small = 1,//С�˷�
	Endian_Net = 0,//�����ֽ���
} Endian;

typedef enum enum_cpu_word_size {
	CpuWordSize_Nothing = 0,//�������
	CpuWordSize_8 = 8,//16λ 
	CpuWordSize_16 = 16,//16λ 
	CpuWordSize_32 = 32,//32λ
	CpuWordSize_64 = 64,//64λ
} CpuWordSize;

#ifdef __cplusplus
extern "C" {	//����C++
#endif

/**
 * ��˷���������0x01234567
 * ��Ӧ����������Ϊ��0x01   0x23   0x45   0x67
 * �ڴ��ַ��С����
 * С�˷����෴���ڴ��ַ�Ӵ�С
 * �����ֽ���Ϊ��˷���JAVA���ֽ��������С�˷�
 * @return CPU�ֽ���
 */
EXTERN Endian GetCpuEndian();

/**
 * ����CPU���ֳ������÷���
 * CYGetCpuWordSize(NULL,NULL);����
 * ���ڼ�������longָ��ľ���
 * �ڱ���׶������ͨ��sizeof(long)����ȡ
 * @return ��ȡCPU���ֳ�
 */
EXTERN CpuWordSize GetCpuWordSize(void *pDummy1,void *pDummy2);

/**
 * ��ȡCPU��ǰ������ʱ��
 */
EXTERN unsigned __int64 GetCpuTimeStampCounter();

/**
 * ��ȡ��־�Ĵ�����ֵ
 ------------------------------------------------------------------------------
 |31     |22|21|20|19|18|17|16|15|14|13|12|11|10|9 |8 |7 |6 |5 |4 |3 |2 |1 |0 |
 |-----------------------------------------------------------------------------
 |          |ID|VP|VF|AC|VM|RF|0 |NT|PL|IO|OF|DF|IF|TF|SF|ZF|0 |AF|0 |PF|1 |CF|
 ------------------------------------------------------------------------------
	IF(Interruptenable)����λ9,�ж����ɱ�־�����ƴ������Կ�����Ӳ���ж��������Ӧ����1����������Ӳ���ж���Ӧ����0��رտ�����Ӳ���ж���Ӧ��IF��־��Ӱ���쳣�Ͳ��������жϣ�NMI���Ĳ�����CPL��IOPL�Ϳ��ƼĴ���CR4�е�VME��־������IF��־�Ƿ����CLI��STI��POPF��POPFD��IRETָ���޸ġ�
	IOPL(I/OPrivilegeLevelField)����λ12&λ13,I/O��Ȩ�򡣱�ʾ��ǰ���̻������I/O��Ȩ���𡣵�ǰ���̻������CPL����С�ڻ����IOPL�ſ��Է���I/O��ַ�ռ䡣ֻ��CPLΪ0�Ľ��̲ſ���POPF��IRETָ���޸������IOPL�ǿ���IF��־�޸ĵĻ���֮һ��ͬʱҲ�ǵ�����ģʽ��չ��Чʱ�����ƼĴ���CR4�е�VME��λ������������8086ģʽ���жϴ����Ļ���֮һ��
	NT(NestedTask)����λ14��Ƕ�����񡣿��Ʊ��жϺͱ����õ������Ƕ��ִ����������������һ����CALLָ��жϻ����쳣����������ʱ���ø�λ������IRETָ���ʱ��Ⲣ�޸ĸ�λ���ñ�־������POPF/POPFDָ��ֱ����λ����0��Ȼ����Ӧ�ó������޸ĸñ�־��״̬���������Ԥ�ϵ��쳣��
	RF(Resume)����λ16,�ָ������ƴ�������ָ��ϵ����Ӧ����1����ʱ����ָ��ϵ���������쳣(debugexceptions,��DE)�����������쳣����Կ��Բ����쳣����0��ָ��ϵ���������쳣��RF��־����Ҫ���������ɴӵ����쳣(ָ��ϵ�������)������Ǹ�ָ�ʼ����ִ�С�����������������IRETDָ��ص����жϳ���֮ǰ����ջ�е�EFLAGESӳ���еĸ�λ��Ϊ1������ָֹ��ϵ��������ĵ����쳣���ڷ��ز��ɹ�ִ�жϵ�ָ��֮�󣬴��������Զ������λ���Ӷ����ɼ�������ָ��ϵ���ϡ�
	VM(Virtual-8086mode)����λ17,����8086ģʽ����1���������8086ģʽ����0�򷵻ر���ģʽ��
	AC(Alignmentcheck)����λ18,�����顣��λ�ñ�־�Ϳ��ƼĴ���CR0��AM��־�����ö��ڴ����õĶ����飬�����������־����ö����顣������һ��û�ж���Ĳ�����ʱ���������һ���������쳣�����������ַ����һ���ֵ�ַ���ڲ���4�ı����ĵ�ַ����һ��˫�ֵ�ַ���������쳣ֻ���û�̬��3����Ȩ���²�����Ĭ����ȨΪ0���ڴ����ã����������������װ�أ�������������쳣������ͬ���Ĳ������û�̬������쳣���������쳣���ڼ�����ݵĶ��룬��������֮�佻������ʱ������ã�����������Ҫ���е����ݶ��롣�������쳣Ҳ�ɹ����ͳ���ʹ�á���ĳЩָ�벻����ͺñ����������ǣ������������ÿ��ָ�붼���м�飬ֻ���õ���ʱ�򣬶���Щ����ָ����д����Ϳ����ˡ�
	VIF(VirtualInterrupt)����λ19,�����жϡ���IF��־��һ������ӳ�������־�Ǻ�VIP��־һ��ʹ�õġ������ƼĴ���CR4�е�VME����PVI��־��Ϊ1��IOPLС��3ʱ��������ֻʶ��VIF��־��VME��־������������8086ģʽ��չ��PVI��־���ñ���ģʽ�µ������жϣ���
	VIP(Virtualinterruptpending)����λ20,�����жϵȴ�����1������һ�����ڵȴ��������жϣ���0����û�еȴ��������жϡ��ñ�־��VIFһ��ʹ�á���������ȡ�ñ�־���������޸�������VME��־���߿��ƼĴ���CR4�е�PVI��־��1��IOPLС��3ʱ��������ֻʶ��VIP��־����VME��־��������8086ģʽ��չ��PVI��־���ñ���ģʽ�����жϣ���
	ID(Identification)����ʶ�𣨵�21λ������1��0�����Ƿ�֧��CPUIDָ�
	������ͨ�ñ�־��
	CF������λ��־
	PF�����ָ���־
	AF����������־
	ZF�������־
	SF�������ű�־
	DF���������־
	OF���������־

	�ۺ���������������Ҫ���ĳ������֮���Ƿ�����������磨0xefffffff * 3��32λ�����У����Ѿ����)
	����Ҫ���OF��־λ�Ƿ�Ϊ1
 */
EXTERN unsigned __int64 GetRegFlags();

#if defined(WIN32) || defined(WINDOWS)
/**
 * ָ���߳��ڹ̶���CPU������
 */
EXTERN void FixedCpuRun(void* hThread);//typedef void* HANDLE;

/**
 * ��ȡ�̵߳ı�־λ
 */
EXTERN unsigned long GetThreadRegFlags(HANDLE hThread);
#endif
#ifdef __cplusplus
}
#endif
#endif