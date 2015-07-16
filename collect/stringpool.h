/******************************************************************************************
 * @file stringpool.h
 * @anthor:Croky.Zheng
 * @CreateTime:2013-5-29
 * @descirption:�ַ�����,���ڱ������е��ַ�������֤һ����ͬ���ַ���ֻ��һ��ʵ��
 * �ַ����ػὫ������ַ��������ֽ�������ȫ��������������һ�����ݣ������Ժ�ʹ��
 * ֧�������ַ�����ʽ
 * 1:��\0��β����ͨ�ַ���
 * 2:��\0\0��β���ַ���
 * 3:ǰ�����ֽ�Ϊ�ַ������ȵ��ַ���
 * 4:ǰ�ĸ��ֽ�Ϊ�ַ������ȵ��ַ���
 * 5:�޸�ʽ��ʾ���ַ���
 *****************************************************************************************/
#ifndef BELIAL_COLLECT_STRINGPOOL_H
#define BELIAL_COLLECT_STRINGPOOL_H
#include "typedef.h"
#include "define.h"


//��ϣ�㷨����ָ��
typedef unsigned long (*HashCode)(char*,unsigned long);

//�����ʽ
typedef enum
{
	//��\0�����ַ���
	Char_Format = 1,
	//��\0\0�����ַ���
	Wchar_Format = 2,
	//�����ֽڶ����ͱ�ʾ���ȿ�ʼ���ַ���
	Short_Head_Format = 3,
	//�����ֽ����ͱ�ʾ���ȿ�ʼ���ַ���
	Int_Head_Format = 4
}StringFormat;

typedef struct
{
	//��ǰ���е�Ԫ������
	unsigned long size;

	//������
	unsigned long modCount;

	//Ͱʹ����
	unsigned long modUseCount;

	HashCode hashCode1;

	HashCode hashCode2;

	HashCode hashCode3;
	//��ʽ
	//StringFormat fomrat;

	//�������ʼ��ַ
	void* address;
} StringPool,*StringPoolPtr;


//ö���ַ����ص�ʱ��Ļص�����
//��ȡ�����ݸ�ʽΪ��
//4�ֽ��ַ������� + �ַ��� + 4�ֽ�\0������
typedef void (*StringPoolCallBack)(StringPoolPtr pool,char* key);

//������POOL���һ��Ӧ����ֻ��һ��ʵ��
//StringPool stringPool;
#ifdef __cplusplus
extern "C" {	//����C++
#endif

/**
 * ����һ���ַ����أ���ָ����ʼ���صĴ�С
 * @param capacity �ַ����ص�����
 * @return �����ַ�����ָ��
 */
EXTERN StringPoolPtr CreateStringPool(unsigned long capacity);

/**
 * ����һ��������Ϊ16��Ĭ���ַ�����
 * @return �����ַ�����ָ��
 */
EXTERN StringPoolPtr CreateDefaultStringPool();

/**
 * ���ַ�������д������
 * @param pool ���������ַ�����
 * @param key �������ַ���
 * @param len �ַ�������
 */
EXTERN char* PutStringPool(StringPoolPtr pool,char* key,unsigned long len);

/**
 * ���ַ�������д���ַ������ַ�����\0��β
 * @param pool ���������ַ�����
 * @param key �������ַ���
 */
EXTERN char* PutConstStringPool(StringPoolPtr pool,const char* key);

//EXTERN void PutUnicodeToStringPool(StringPoolPtr pool,const wchar_t* key);


/**
 * ��ȡ�ַ������ַ������е�ָ��,Ҫ�ǳ��в����ڴ��ַ�������Ҫ�������put����
 * @param pool ���������ַ�����
 * @param key �������ַ���
 * @param len �ַ�������
 */
EXTERN char* GetStringPool(StringPoolPtr pool,char* key,unsigned long len);


/**
 * ���ַ�������д���ַ������ַ�����\0��β
 * @param pool ���������ַ�����
 * @param key �������ַ���
 * @return ��ȡ�ַ����ڳ��е�ָ��
 */
EXTERN char* GetConstStringPool(StringPoolPtr pool,char* key);

/**
 * ����ַ����Ƿ��Ѿ����ַ�������
 * @param pool ���������ַ�����
 * @param key �������ַ���
 * @param len �ַ�������
 * @return �����򷵻�True,���򷵻�False
 */
EXTERN Boolean ContairStringPool(StringPoolPtr pool,char* key,unsigned long len);

/**
 * ����ַ����Ƿ��Ѿ����ַ������� �ַ�����\0��β
 * @param pool ���������ַ�����
 * @param key �������ַ���
 * @return �����򷵻�True,���򷵻�False
 */
EXTERN Boolean ContairConstStringPool(StringPoolPtr pool,char* key);

/**
 * ɾ���ַ������а������ַ���
 * @param pool ���������ַ�����
 * @param key �������ַ���
 * @param len �ַ�������
 */
EXTERN void RemoveStringPool(StringPoolPtr pool,char* key,unsigned long len);

/**
 * ɾ���ַ������а������ַ��� �ַ�����\0��β
 * @param pool ���������ַ�����
 * @param key �������ַ���
 */
EXTERN void RemoveConstStringPool(StringPoolPtr pool,char* key);

/**
 * �������е��ַ������е�����
 * @param pool ���������ַ�����
 */
EXTERN void CleanStringPool(StringPoolPtr pool);

/**
 * �����ַ����أ����ͷ����пռ�
 */
EXTERN void DestroyStringPool(StringPoolPtr pool);

/** 
 * ʹ�ûص������������еĽڵ�����
 * ���ݵĸ�ʽ��ǰ��4���ֽڱ�ʾ�ַ������� +���ַ��������ĸ��ֽ�\0
 */
EXTERN void EnumStringPool(StringPoolPtr pool,StringPoolCallBack fnStringPool);
#ifdef __cplusplus
}//����C++
#endif
#endif