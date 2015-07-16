/******************************************************************************************
 * @file StringHash.h
 * @anthor:Croky.Zheng
 * @CreateTime:2013-5-29
 * @descirption:�ַ�����ϣ��
 *****************************************************************************************/
#ifndef BELIAL_COLLECT_STRINGHASH_H
#define BELIAL_COLLECT_STRINGHASH_H
#include "typedef.h"
#include "define.h"
#include <stdlib.h>
#include <string.h>


//��ϣ�㷨����ָ��
typedef unsigned long (*StringHashCode)(char*,unsigned long);

typedef struct __string_hash_node__
{
	//hash1 + hash2 ���������������ͬ���ַ���
	//�����ַ��������Ĺ�ϣֵһ���Ŀ���������˵�С
	unsigned long hash1;

	//�����ϣֵ����ʹ����index
	unsigned long hash2;

	//�ַ�������
	unsigned long length;

	//�ַ�����ֵ,��\0\0�������ַ���
	char value[];
} StringHashNode,*StringHashNodePtr;

typedef struct
{
	//��ǰ���е�Ԫ������
	unsigned long size;

	//������
	unsigned long modCount;

	StringHashCode hashCode1;

	StringHashCode hashCode2;
	//��ʽ
	//StringFormat fomrat;

	//�������ʼ��ַ
	void* address;
} StringHash,*StringHashPtr;


//ö���ַ����ص�ʱ��Ļص�����
//��ȡ�����ݸ�ʽΪ��
//4�ֽ��ַ������� + �ַ��� + 4�ֽ�\0������
typedef void (*StringHashCallBack)(StringHashPtr stringHash,char* key);

//������POOL���һ��Ӧ����ֻ��һ��ʵ��
//StringHash StringHash;
#ifdef __cplusplus
extern "C" {	//����C++
#endif

/**
 * ����һ���ַ����أ���ָ����ʼ���صĴ�С
 * @param capacity �ַ����ص�����
 * @return �����ַ�����ָ��
 */
EXTERN StringHashPtr CreateStringHash(unsigned long capacity);

/**
 * ����һ��������Ϊ16��Ĭ���ַ�����
 * @return �����ַ�����ָ��
 */
EXTERN StringHashPtr CreateDefaultStringHash();

/**
 * ���ַ�������д������
 * @param pool ���������ַ�����
 * @param key �������ַ���
 * @param len �ַ�������
 */
EXTERN long PutStringHash(StringHashPtr stringHash,char* key,unsigned long len);

/**
 * ���ַ�������д���ַ������ַ�����\0��β
 * @param pool ���������ַ�����
 * @param key �������ַ���
 */
EXTERN long PutConstStringHash(StringHashPtr stringHash,const char* key);


EXTERN long PutConstUnicodeStringHash(StringHashPtr stringHash,const wchar_t* key);


/**
 * ��ȡ�ַ������ַ������е�ָ��,Ҫ�ǳ��в����ڴ��ַ�������Ҫ�������put����
 * @param pool ���������ַ�����
 * @param key �������ַ���
 * @param len �ַ�������
 */
EXTERN char* GetStringHash(StringHashPtr stringHash,char* key,unsigned long len);


/**
 * ���ַ�������д���ַ������ַ�����\0��β
 * @param pool ���������ַ�����
 * @param key �������ַ���
 * @return ��ȡ�ַ����ڳ��е�ָ��
 */
EXTERN char* GetConstStringHash(StringHashPtr stringHash,char* key);

/**
 * ����ַ����Ƿ��Ѿ����ַ�������
 * @param pool ���������ַ�����
 * @param key �������ַ���
 * @param len �ַ�������
 * @return �����򷵻�True,���򷵻�False
 */
EXTERN Boolean ContairStringHash(StringHashPtr stringHash,char* key,unsigned long len);

/**
 * ����ַ����Ƿ��Ѿ����ַ������� �ַ�����\0��β
 * @param pool ���������ַ�����
 * @param key �������ַ���
 * @return �����򷵻�True,���򷵻�False
 */
EXTERN Boolean ContairConstStringHash(StringHashPtr stringHash,char* key);

/**
 * ɾ���ַ������а������ַ���
 * @param pool ���������ַ�����
 * @param key �������ַ���
 * @param len �ַ�������
 */
EXTERN void RemoveStringHash(StringHashPtr stringHash,char* key,unsigned long len);

/**
 * ɾ���ַ������а������ַ��� �ַ�����\0��β
 * @param pool ���������ַ�����
 * @param key �������ַ���
 */
EXTERN void RemoveConstStringHash(StringHashPtr stringHash,char* key);

/**
 * �������е��ַ������е�����
 * @param pool ���������ַ�����
 */
EXTERN void CleanStringHash(StringHashPtr stringHash);

/**
 * �����ַ����أ����ͷ����пռ�
 */
EXTERN void DestroyStringHash(StringHashPtr stringHash);

/** 
 * ʹ�ûص������������еĽڵ�����
 * ���ݵĸ�ʽ��ǰ��4���ֽڱ�ʾ�ַ������� +���ַ��������ĸ��ֽ�\0
 */
EXTERN void EnumStringHash(StringHashPtr stringHash,StringHashCallBack fnStringHash);
#ifdef __cplusplus
}//����C++
#endif
#endif