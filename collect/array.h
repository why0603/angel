/******************************************************************************************
 * @file array.h
 * @anthor:Croky.Zheng
 * @CreateTime:2013-5-15
 * @descirption:��̬����
 * �����Զ���ӳ��ȵ�����
 * Ԫ�س��ȹ̶�
 * ���Ը����������ʣ�ÿ����������֮����Ҫ�����µĿռ�
 * �µ��ڴ�ռ�������֮ǰ���ڴ�ռ䣬���Ҹ���ԭ�������������
 * ʹ���������飬��ó�ʼ����ʱ���֪�����ݵ����ֵ�����Ҷ���˳�����
 * �ܲ��������ݵ�ɾ��������кܶ�ɾ����������ᵼ��Ч��ʮ�ֵĵ�
 *****************************************************************************************/
#ifndef BELIAL_COLLECT_ARRAY_H
#define BELIAL_COLLECT_ARRAY_H
#include "typedef.h"
#include "define.h"

//
typedef struct
{
	//��ʼ����С,���������ָ����װ�ص�Ԫ�ظ���
	//unsigned long initCapacity;

	//����ĵ�ǰ��С����ʾ����װ�ص�Ԫ�ظ���
	unsigned long capacity;

	//��ǰ��Ԫ�ظ���
	unsigned long size;

	//����Ԫ�ص��ֽ���
	unsigned long elementByteSize;

	//���ݴ���
	unsigned long raiseCount;

	//�Ƿ�䳤����
	//�䳤������ֻ�ܱ�����ָ��
	DataType dataType;

	//�����Ƿ��з���,Ĭ��Ϊ�з�����ֵ
	Boolean isSigned;

	//�������ʼ��ַ
	void* address;
}Array,*ArrayPtr;


typedef enum
{
	//����������Χ
	Error_Index_Range = -1,
	//�ڴ����ʧ��
	Error_No_Memory = -2,
}ArrayError;

#ifdef __cplusplus
extern "C" {	//����C++
#endif

/**
 * ����һ����̬����
 * @param capacity ����ĳ�ʼ������
 * @param elementByteSize ÿ������Ԫ��ռ�õ��ֽ���
 * @param raiseRate ��������
 * @return ���ض�̬����ָ��
 */
EXTERN	ArrayPtr CreateArray(unsigned long capacity,unsigned long elementByteSize);

/**
 * ����һ�����飬ʹ��Ĭ�ϲ����������СΪ20����������Ϊ0.66
 * @param elementByteSize ÿ������Ԫ��ռ�õ��ֽ���
 * @return ���ض�̬����ָ��
 */
EXTERN	ArrayPtr CreateDefaultArray();

/**
 * ��������ӵ���̬����ĩβ
 * @param array ����ָ��
 * @param data ����ָ��
 */
EXTERN	void InsertArray(ArrayPtr array,void* data);

/**
 * ��������ӵ���̬���飬����ָ���������������������ǰ�����С���򲻻���ӳɹ�
 * @param array ����ָ��
 * @param data ����ָ��
 * @param index ָ������������
 * @return ��ӳɹ�����True,���򷵻�False
 */
EXTERN	Boolean InsertArrayIndex(ArrayPtr array,void* data,unsigned long index);

/**
 * ������������������ӵ���̬���飬����ָ���������������������ǰ�����С���򲻻���ӳɹ�
 * @param array ����ָ��
 * @param data ����ָ��
 * @param count ���ݸ���
 * @param index ָ������������
 * @return ��ӳɹ�����True,���򷵻�False
 */
EXTERN	Boolean InsertMoreArrayIndex(ArrayPtr array,void* data,unsigned long count,unsigned long index);

/**
 * ��һ����̬���������ȫ����ӵ���һ����̬���飬����ָ���������������������ǰ�����С���򲻻���ӳɹ�
 * @param dest Ŀ������
 * @param destIdx ��Ҫ�������ݵ�Ŀ������λ��
 * @param src ��Դ����
 * @param count ��Դ������Ҫ�����붮��Ŀ����������ݸ���
 * @param srcIdx ��Դ��������
 * @return ��ӳɹ�����True,���򷵻�False
 */
EXTERN	Boolean InsertArrayToArray(ArrayPtr dest,unsigned long destIdx,ArrayPtr src,unsigned long count,unsigned long srcIdx);

/**
 * ��ȡ��̬����ָ������������
 * @param array ��̬����
 * @param data ������ݵ�ָ��
 * @param index ָ������
 * @return �ɹ�����True,���򷵻�False
 */
EXTERN	Boolean GetDataFromArray(ArrayPtr array,void* data,unsigned long index);

/**
 * ��ȡ��̬����ָ������������
 * @param array ��̬����
 * @param index ָ������
 * @return ֱ�ӷ����������ڴ��еĵ�ַ
 */
EXTERN	void* GetDataArray(ArrayPtr array,unsigned long index);
/**
 * ��ȡ��̬����ָ�����ݵ�����������0������ʼ���ң�Ҫ��û�з������ݣ�����-1
 * @param array ��̬����
 * @param data Ҫ���ҵ�����
 * @return �ɹ��������ݵ�����,���򷵻�-1
 */
EXTERN	int IndexOfArray(ArrayPtr array,void* data);

/**
 * ɾ����̬����ָ�����������ݣ��������ݱ�����data�У�data���ΪNULL����������
 * @param array ��̬����
 * @param data ��ɾ��������
 * @param index ��ɾ�����ݵ�����
 * @return �ɹ��������ݵ�����,���򷵻�-1
 */
EXTERN	Boolean RemoveArrayIndex(ArrayPtr array,void* data,unsigned long index);


/**
 * ɾ����̬������ָ�������Ķ������ݣ�index = 3,ɾ��count=5����������Ϊ3,4,5,6,7��5�����ݽ���ɾ��
 * @param array ��̬����
 * @param data ��ɾ��������
 * @param count ��ɾ����������
 * @param index ��ɾ�����ݵ�����
 * @return �ɹ��������ݵ�����,���򷵻�-1
 */
EXTERN	Boolean RemoveArrayMoreIndex(ArrayPtr array,unsigned long count,unsigned long index);


/**
 * ɾ����̬������ָ��������,���û���ҵ����������ɾ��ʧ��
 * @param array ��̬����
 * @param data ��Ҫɾ��������
 * @return �ɹ��������ݵ�����,���򷵻�-1
 */
EXTERN	Boolean RemoveDataArray(ArrayPtr array,void* data);


/**
 * ɾ����̬������ָ���Ķ�������,���û���ҵ����������ɾ��ʧ��
 * @param array ��̬����
 * @param data ��Ҫɾ��������
 * @return �ɹ��������ݵ�����,���򷵻�-1
 */
EXTERN	void RemoveMoreDataArray(ArrayPtr array,void* data,unsigned long count);

/**
 * ��ȡ��̬�����������
 * @param array ��̬����
 * @param count Ҫ��ȡ����������������
 * @param index �������ڸ�������ָ��������
 * @return �����������ָ��
 */
EXTERN	ArrayPtr SubArray(ArrayPtr array,unsigned long count,unsigned long index);

/**
 * ����̬�������������
 * @param array �����������
 */
EXTERN	void CleanArray(ArrayPtr array);

/**
 * �������鲢�ͷ������ڴ�
 * ��������а����ж��Ϸ����ָ�룬�����ͷ�
 */
EXTERN	void DestroyArray(ArrayPtr array);

EXTERN	void SortArray(ArrayPtr array);

#ifdef __cplusplus
}//����C++
#endif
#endif