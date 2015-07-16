/******************************************************************************************
 * @file common.h
 * @anthor:Croky.Zheng
 * @CreateTime:2013/6/6
 * @descirption: һЩ��������
 *****************************************************************************************/
#ifndef BELIAL_COMMON_COMMON_H
#define BELIAL_COMMON_COMMON_H
#include "typedef.h"
#include "define.h"



#ifdef __cplusplus
extern "C" {	//����C++
#endif


/**
 * ��ȡ���ݵ��ֽ���
 */
unsigned long GetByteSizeOfData(DataType dataType,void* data);

/**
 * �Ƚ��������ݵĴ�С,������len�ĳ�����ǿ������ת��(��ԭʼ�����д����ֽ����е��������)
 * 1���ֽڽ�����char���ͱȽ�
 * 2���ֽڽ�����short���ͱȽ�
 * 4���ֽڽ�����long���ͱȽ�
 * 8���ֽڽ�����__int64���ͱȽ�
 * 0���ֽڽ�������\0�������ַ�����ʽ�Ƚ�strcmp
 * �����ֽ�����ֱ��ʹ���ڴ�Ƚ�memcmp
 * @param data1 ��һ���Ƚ���
 * @param data2 �ڶ����Ƚ���
 * @param len �Ƚ�˫���ĳ��ȣ������\0��β���ַ�,��ʹ��0
 * @param isSigned True �з���  False �޷���
 * @return data1>data2=1 data1=data2=0 data1<data2=-1
 */
EXTERN CompareResult Compare(void* data1,void* data2,DataType dataType,unsigned long len,Boolean isSigned);

/**
 * ��src�����ݿ�����dest��
 * @param dest Ŀ���ڴ��ַ
 * @param src ��Դ�ڴ��ַ
 * @len ���ݳ���
 * ���������͹���ͬ��
 */
EXTERN void CopyData(void* dest,void* src,DataType dataType,unsigned long len);
#ifdef __cplusplus
}
#endif
#endif