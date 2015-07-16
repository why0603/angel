/******************************************************************************************
 * @file hashtable.h
 * @anthor:Croky.Zheng
 * @CreateTime:2013-5-24
 * @descirption:��ϣ��
 *****************************************************************************************/
#ifndef BELIAL_COLLECT_HASHTABLE_H
#define BELIAL_COLLECT_HASHTABLE_H
#include "typedef.h"
#include "define.h"

//��ϣ�㷨����ָ��
typedef unsigned long (*HashCode)(char*,unsigned long);

//KEY�ȽϺ���ָ��
typedef Boolean (*KeyEqual)(void*,void*);

//���ݴ洢��ʽ
typedef enum
{
	//���洢
	NotSaveMode,
	
	//ֱ�Ӹ�ֵ
	DirectMode,

	//������ֵ
	CopyMode,
}DataSaveMode;

typedef struct __HashNode__
{

	//�����HASHֵ
	unsigned long hashValue;

	//��һ���ڵ�
	struct __HashNode__* next;
	
	//����valueΪ���͵�Ӧ����Ҫ����һЩ
	//�������˼������е��������ͣ���������64λϵͳ�г䵱ָ�������
	//����������С������ʱ���˷�һ���ռ�
	//__int64 value;

	//һֱ�������Ի��ڱ���KEYֵ
	//���ھ��������������ͬHASH�㷨������ֵӦ�û�������
	//�������������������ң���Ȼ��������������
	//����������ȱ��ģ� ��Ϊ�޷�ö��KEY
	//__int64 key;
	//valueָ��,value��ʹ�ÿ�������ʽ����������
	//void* value;
	//void* key;
	//
	//key-value  ,KEY��VALUE��һ��
	char keyValue[];
} HashNode,*HashNodePtr;

typedef struct
{
	//��ǰ��ϣ���е�Ԫ������
	unsigned long size;

	//Ͱ����
	unsigned long bucketCount;

	//Ͱʹ����
	unsigned long bucketUseCount;

	//�ٽ�ֵ->��չ����,���Ͱʹ�����������������������
	float raiseRate;

	//�Ƿ�ʹ������ķ�ʽ,ʹ������ʽ���������һЩ�ռ��ϵĸ���
	//Ͱ��ʹ����Ҳ����,�⽫Ҫ��Ͱ�ķֲ����Ӿ��ȣ���Ҫ���õĹ�ϣ�㷨
	unsigned long useList;

	//key���ֽڳ���,���ֵΪ0��ʾΪ�������ַ���
	unsigned long keyByteSize;

	DataType keyType;

	//value���ֽڳ���,���ֵ����0����ڹ�ϣ���д����ڴ棬����VALUE
	unsigned long valueByteSize;

	DataType valueType;

	//��ϣֵ�ĵ��ã������ָ������ʹ��Ĭ�Ϲ�ϣֵ�㷨
	HashCode hashCode;

	//key�Ƚϵ��㷨,������ṩ�˺�������ʵ���������ַ�����ֵ
	//���������ӽṹ��ıȽϣ����޷���֤
	KeyEqual keyEqual;
	
	//�������ʼ��ַ
	void* address;
} HashTable,*HashTablePtr;

typedef void (*HashTableCallBack)(HashTablePtr table,char* keyValue);
#ifdef __cplusplus
extern "C" {	//����C++
#endif

/**
 * ������ϣ��
 * @param capacity ��ϣ��ĳ�ʼ����
 * @param keyByteSize key���ֽ���
 * @param valueByteSize value���ֽ���
 */
EXTERN HashTablePtr CreateHashTable(unsigned long capacity,unsigned long keyByteSize,unsigned long valueByteSize);

/**
 * ����Ĭ�Ϲ�ϣ��
 * ����Ϊ64��Ͱ��key,value���ֽ�����Ϊ�����λ��
 */
EXTERN HashTablePtr CreateDefaultHashTable();

/**
 * ��ϣ�����Ƿ����key
 * @param table Ҫ��ѯ�ı�
 * @param key Ҫ��ѯ��KEY
 */
EXTERN Boolean ContairHashTable(HashTablePtr table,void* key);


/**
 * �ڹ�ϣ���в�ѯֵ,��ֵ��Ϊָ�뷵��
 * @param table Ҫ��ѯ�ı�
 * @param key Ҫ��ѯ��KEY
 * @return ����ֵ���ڵ��ڴ��ַ
 */
EXTERN void* GetHashTable(HashTablePtr table,void* key);

/** 
 * �ڹ�ϣ���в�ѯֵ ֵ������VALUE��
 * @param table Ҫ��ѯ�ı�
 * @param key Ҫ��ѯ��KEY
 * @param value ����ֵ���ڴ��ַ
 */
EXTERN void GetFromHashTable(HashTablePtr table,void* key,void* value);

/**
 * ��������ӵ���ϣ����
 * @param table Ҫ��ѯ�ı�
 * @param key Ҫ��ѯ��KEY
 * @param value ���ֵ���ڴ��ַ
 */
EXTERN void PutHashTable(HashTablePtr table,void* key,void* value);


/**
 * ��ĳ���ؼ��ʴӹ�ϣ����ɾ��
 * @param table Ҫ��ѯ�ı�
 * @param key Ҫɾ����KEY
 */
EXTERN void RemoveHashTable(HashTablePtr table,void* key);

/**
 * �����ϣ����ɾ�����е�Ͱ
 * @param table Ҫ��ѯ�ı�
 */
EXTERN void CleanHashTable(HashTablePtr table);

/**
 * ���ٹ�ϣ���ͷ������ڴ�
 * @param table Ҫ��ѯ�ı�
 */
EXTERN void DestroyHashTable(HashTablePtr table);

/**
 * ö�ٹ�ϣ�����ûص������������е�����
 */
EXTERN void EnumHashTable(HashTablePtr table,HashTableCallBack fnHashTable);
#ifdef __cplusplus
}//����C++
#endif
#endif