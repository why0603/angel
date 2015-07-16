/******************************************************************************************
 * @file list.h
 * @anthor:Croky.Zheng
 * @CreateTime:2013-5-20
 * @descirption:����ÿ����һ��Ԫ�أ�����Ҫ���¿���һ��Ԫ�ؿռ�
 * �����ʱ����Ҫmallocһ���ڴ棬ʵ�����Ѿ�ƫ��
 * ����Ϊû��ʹ��ʮ��������ʵɾ�����ٶ��Ǹ��ӵ���
 * ��Ϊÿ�ζ���Ҫ�ӵ�һ���ڵ㿪ʼ�����ߣ����ÿ�ζ���ɾ�����һ���ڵ㣬���൱������������
 * ���ĺô��������ݵ�ʱ����Ҫȥ���Ʊ�����,��INSERT��DELETE,SEARCH�����൱����
 * �����Ҫ�����ȡ�������ݣ�������ҪƵ�����������ɾ�����ݣ��ǻ�����array 
 * ֻ�Ƕ�ɾ��������Ҫ���������΢�ĸ�Ϊ˫��ʮ�֣�����
 * ��Ϊstack,queue���������㹻��
 * ʵ��֤�������������Ч�ʲ���һ��ĵͣ��ر��Ǵ�����malloc��Ļ��գ���ֱ�����ڳ���
 * ���ԣ��������õĺ�
 *****************************************************************************************/
#ifndef BELIAL_COLLECT_LIST_H
#define BELIAL_COLLECT_LIST_H
#include "typedef.h"
#include "define.h"

typedef struct __list_node
{
	//��һ���ڵ��ָ��
	struct __list_node* next;
	//���������ݣ��������鲢��ռ�ýṹ��Ŀռ�,0 bytes
	//���Ҫ�����ֻ���ַ�����ָ�룬����Ҫ��insert�����ݲ�����Ϊָ���ָ��
	char data[];
}ListNode,*ListNodePtr;

//
typedef struct
{
	//��ǰ��Ԫ�ظ���
	unsigned long size;

	//����Ԫ�ص��ֽ���
	unsigned long elementByteSize;

	//�����Ƿ��з���,Ĭ��Ϊ�з�����ֵ
	Boolean isSigned;

	//��������
	DataType dataType;

	//ָ�������һ��Ԫ�صĽڵ�
	ListNodePtr head;

	//ָ���������һ��Ԫ�صĽڵ�
	ListNodePtr tail;
}List,*ListPtr;

//���������������ȡ���ݵķ�ʽЧ��ʮ�ֵ���
//�����ṩһ���ص�����������ÿ��ö�����е�����
//���÷����Ա�дһ���ص�������������ÿ���ڵ�
typedef void (*ListCallBack)(ListPtr list,ListNodePtr node);


#ifdef __cplusplus
extern "C" {	//����C++
#endif

/**
 * ����һ��Ĭ�����������洢CPU�ֳ�������
 * @param capacity ����ĳ�ʼ������
 * @return ��������ָ��
 */
EXTERN	ListPtr CreateDefaultList();

/**
 * ����һ��ָ�����ݳ�������
 * @param elementByteSize ����洢�����ݳ���
 * @return ��������ָ��
 */
EXTERN	ListPtr CreateList(DataType dataType);
/**
 * �������ݵ�����ͷ��
 * @param list ����Ҫ���������
 * @data ����
 */
EXTERN	void InsertListHead(ListPtr list,void* data);

/**
 * �������ݵ�����β��
 * @param list ����Ҫ���������
 * @data ����
 */
EXTERN	void InsertList(ListPtr list,void* data);

//�������,��С���������������
EXTERN void InsertListSort(ListPtr list,void* data);
/**
 * �����������������ݵ�����ͷ��
 * @param list ����Ҫ���������
 * @data ����
 * @count ���ݵ�����
 */
EXTERN	void InsertMoreListHead(ListPtr list,void* data,unsigned long count);


/**
 * �����������������ݵ�����β��
 * @param list ����Ҫ���������
 * @data ����
 * @count ���ݵ�����
 */
EXTERN	void InsertMoreList(ListPtr list,void* data,unsigned long count);

/**
 * ������������������
 * @param dest Ŀ������
 * @src ����ʧЧ���������ܶԴ��������κ��ڴ��ͷŵĲ����������Ӱ��dest
 */
EXTERN	void InsertListToList(ListPtr dest,ListPtr src);

/**
 * ������������������,src������dest�����ǰ��
 * @param dest Ŀ������
 * @src ����ʧЧ���������ܶԴ��������κ��ڴ��ͷŵĲ����������Ӱ��dest
 */
EXTERN	void InsertListToListHead(ListPtr dest,ListPtr src);

/**
 * ��ȡ�����������е�λ��,���ֲ���֮����ʵ��û�ж�����壬
 * ������ò�Ҫ�������صĲ���
 * @param list ����
 * @param data Ҫ���ҵ�����
 * @return ���ش����ݵ���ţ���������ڣ��򷵻�-1 
 */
EXTERN	int IndexOfList(ListPtr list,void* data);

/**
 * ��ȡָ����ŵ����ݣ����ַ�����Ч�������൱�ĵ�
 * �����ȡ���ݵķ�ʽ����û��Ǵ�list->head��ʼ���²���next = NULL�Ľڵ�
 * @param list ����
 * @param data ���ݴ��λ��
 * @param index ȡָ�����
 */
EXTERN	void GetDataFromList(ListPtr list,void* data,unsigned long index);

/** 
 * ɾ��ͷ�ڵ㣬����������
 * @param list ����
 * @param data ���ݴ��λ��
 */
EXTERN	void RemoveListHead(ListPtr list,void* data);

/** 
 * ɾ��β�ڵ㣬���������ݣ���Ҫ���¶�λβ���ڵ�
 * @param list ����
 * @param data ���ݴ��λ��
 */
EXTERN	void RemoveListTail(ListPtr list,void* data);


/** 
 * ɾ������ָ�����ݵĽڵ�
 * @param list ����
 * @param data Ҫ��ɾ��������
 */
EXTERN	void RemoveDataList(ListPtr list,void* data);

/**
 * ����������ݺͽڵ�,���ͷŽڵ�ռ�õ��ڴ�
 * @param list ����
 */
EXTERN	void CleanList(ListPtr list);

/**
 * ���������ͷ������ڴ�
 */
EXTERN	void DestroyList(ListPtr list);

/**
 * ö���������нڵ㲢ִ�лص������������ڵ�
 */
EXTERN void EnumListNode(ListPtr list,ListCallBack fnList);

//--------------------------------------------------------
/**
 * ������ѹ��ջ
 * @param list ����Ҫѹ�������
 * @data ����
 */
EXTERN	void PushStack(ListPtr list,void* data);


/**
 * ������ѹ��ջ
 * @param list ����Ҫѹ�������
 * @data ����
 * @count ���ݵ�����
 */
EXTERN	void PushAllStack(ListPtr list,void* data,unsigned long count);

/** 
 * ���ݳ�ջ
 * @param list ����
 * @data ���ݴ洢��λ��
 */
EXTERN	void PopStack(ListPtr list,void* data);


/** 
 * �������ݳ�ջ
 * @param list ����
 */
EXTERN	void PopAllStack(ListPtr list);

//---------------------------------------------------ջ����
//---------------------------------------------------���в���
/**
 * �������ƽ�����
 * @param list ����
 * @data ����
 */
EXTERN	void OfferQueue(ListPtr list,void* data);

/**
 * ��ȡ���Ƴ�����
 * @param list ����
 * @data ���ݴ��λ��
 */
EXTERN	void PollQueue(ListPtr list,void* data);

/**
 * ��ȡ���ǲ��Ƴ�����
 * @param list ����
 * @data ���ݴ��λ��
 */
EXTERN	void PeekQueue(ListPtr list,void* data);

//-----------------------------------------------���в�������




#ifdef __cplusplus
}//����C++
#endif
#endif