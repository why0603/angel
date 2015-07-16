#define _CRTDBG_MAP_ALLOC
#define _CRTDBG_MAP_ALLOC_NE
#include <stdlib.h>
#include <crtdbg.h>
#include <stdio.h>
#include <string.h>

#include "charUtils.h"
#include "array.h"
#include "computer.h"
#include "list.h"
#include "hashtable.h"
#include "stringpool.h"
#include "stringhash.h"
#include "tree.h"


#define WIN32
static void test_print_array(ArrayPtr array);
static void test_array();
static void test_print_list(ListPtr list);
static void test_print_stringpool(StringPool pool);
static void test_print_hashtable(HashTablePtr table);
static void test_list_string();
static void test_array_string();
static void test_tree();
static void test_print_tree(TreePtr tree);

static void print_stringpool(StringPoolPtr pool,char* key)
{
	//4字节长度
	long len = *(long*)key;
	printf("key_long=%d,context=%s\r\n",len,key);
}

void test_print_stringpool(StringPoolPtr pool)
{
	EnumStringPool(pool,print_stringpool);
}
static void print_hashtable(HashTablePtr table,char* keyValue)
{
	//测试的哈希表KEY和VALUE都是int类型
	long key = *(int*)keyValue;
	long value = *((int*)keyValue + 1);
	printf("key=%d,value=%d\r\n",key,value);
}
static void print_stringhash(StringHashPtr stringHash,char* value)
{
	printf("value=%s\r\n",value);
}

void test_print_hashtable(HashTablePtr table)
{
	EnumHashTable(table,print_hashtable);
}

void test_print_stringhash(StringHashPtr stringHash)
{
	EnumStringHash(stringHash,print_stringhash);
}

static void print_list_callback(ListPtr list,ListNodePtr node)
{
	long long data = 0;
	static int position = 0;
	if (NULL != node)
	{
//		memcpy(&data,node->data,list->elementByteSize);
			if ((list->dataType == Chars_Type) &&
				(list->elementByteSize == LongLong_Type))
			{
				printf("%s,",(char*)node->data);
			}
			else if((list->dataType == Unicode_Type) &&
				(list->elementByteSize == LongLong_Type))
			{
				wprintf(L"%s,",(wchar_t*)node->data);
			}
			else
			{
				printf("%I64d,",(long long)data);
			}
		//printf("%d,",data);
		if (++position % 10 == 0)
		{
			printf("\r\n");
		}
	}
}

void test_print_list(ListPtr list)
{
	int i;
	long long data = 0;
	ListNodePtr node;
	if (NULL != list)
	{
		EnumListNode(list,print_list_callback);
	}
	printf("\r\n------------------list size:%d\r\n",list->size);
}

void test_print_array(ArrayPtr array)
{
	int i;
	long long data = 0;
	if (NULL != array)
	{
		for (i=0; i<array->size; i++)
		{
			GetDataFromArray(array,&data,i);
			if ((array->dataType == Chars_Type) &&
				(array->elementByteSize == LongLong_Type))
			{
				printf("%s,",(char*)data);
			}
			else if((array->dataType == Unicode_Type) &&
				(array->elementByteSize == LongLong_Type))
			{
				wprintf(L"%s,",(wchar_t*)data);
			}
			else
			{
				printf("%I64d,",data);
			}
			if ((i!=0) && (i%10==0))
			{
				printf("\r\n");
			}
		}
	}
}

void test_array()
{
	long long i = 0;
	long long delData = 601;
	ArrayPtr sub_array = NULL;
	ArrayPtr array = CreateDefaultArray();
	for (i=1000; i<60000; i++)
	{
		InsertArray(array,&i);
	}
	for (i=300; i<600; i++)
	{
		InsertArray(array,&i);
	}
	for (i=1; i<300; i++)
	{
		InsertArray(array,&i);
	}
	for (i=600; i<1000; i++)
	{
		InsertArray(array,&i);
	}
	RemoveArrayMoreIndex(array,100,500);//删除501-600
	RemoveArrayIndex(array,NULL,499);//删除500
	RemoveDataArray(array,&delData);//删除601
//	test_print_array(array);
//	printf("\r\n--------------------------------------------------------\r\n");
	sub_array = SubArray(array,499,0);//将1-499放入子列表
	RemoveArrayMoreIndex(array,499,0);//删除1-499
	InsertArrayToArray(array,array->size,sub_array,sub_array->size,0);//将1-499加入列表尾部
	DestroyArray(sub_array);
	SortArray(array);
	test_print_array(array);
	printf("\r\n--------------------------------------------------------\r\n");
	CleanArray(array);//清理列表
	test_print_array(array);
	DestroyArray(array);
	printf("\r\ncomput word size:%d\r\n",GetCpuWordSize(NULL,NULL));
}

void test_array_string()
{
	ArrayPtr array = CreateDefaultArray();
	char* a = "aaaa";
	char* b = "bbbb";
	char* c = "cccc";
	char* d = "12121212";
	char* e = "abcdefg";
	array->dataType = Chars_Type;
	InsertArray(array,&a);
	InsertArray(array,&b);
	InsertArray(array,&c);
	InsertArray(array,&d);
	InsertArray(array,&e);
	test_print_array(array);
	printf("\r\n--------------------------------------------------------\r\n");
	SortArray(array);
	test_print_array(array);
	printf("\r\n--------------------------------------------------------\r\n");
	DestroyArray(array);
}

void test_list_string()
{
	ListPtr list = CreateDefaultList();
	char* a = "aaaa";
	char* b = "bbbb";
	char* c = "cccc";
	char* d = "12121212";
	char* e = "abcdefg";
	list->dataType = Chars_Type;
	InsertListSort(list,a);
	InsertListSort(list,b);
	InsertListSort(list,c);
	InsertListSort(list,d);
	InsertListSort(list,e);
	printf("\r\n--------------------------------------------------------\r\n");
	test_print_list(list);
	printf("\r\n--------------------------------------------------------\r\n");
	DestroyList(list);
}


void test_string_hash()
{
	StringHashPtr string_hash = CreateDefaultStringHash();
	char* a = "aaaa";
	char* b = "bbbb";
	char* c = "cccc";
	char* d = "12121212";
	char* e = "abcdefg";
	printf("%s=%d\r\n",a,PutConstStringHash(string_hash,a));
	printf("%s=%d\r\n",b,PutConstStringHash(string_hash,b));
	printf("%s=%d\r\n",c,PutConstStringHash(string_hash,c));
	printf("%s=%d\r\n",d,PutConstStringHash(string_hash,d));
	printf("%s=%d\r\n",e,PutConstStringHash(string_hash,e));
	printf("\r\n--------------------------------------------------------\r\n");
	test_print_stringhash(string_hash);
	printf("\r\n--------------------------------------------------------\r\n");
	DestroyStringHash(string_hash);
}
void test_stack()
{
	long long i = 0;
	long long delData = 601;
	ListPtr stack = CreateList(LongLong_Type);
	for (i=0; i<1000; i++)
	{
		PushStack(stack,&i);
	}
	for (i=0; i<100; i++)
	{
		PopStack(stack,&delData);
	}
	test_print_list(stack);
	PopAllStack(stack);
	test_print_list(stack);
	DestroyList(stack);
}


void test_queue()
{
	long long i = 0;
	long long delData = 601;
	ListPtr queue = CreateList(LongLong_Type);
	for (i=0; i<1000; i++)
	{
		OfferQueue(queue,&i);
	}
	for (i=0; i<200; i++)
	{
		PollQueue(queue,&delData);
	}
	test_print_list(queue);
	PopAllStack(queue);
	test_print_list(queue);
	DestroyList(queue);
}

static void test_string_pool()
{
	int i=0;
	char dec[10];
	StringPoolPtr pool = CreateDefaultStringPool();
	/*
	char* key1 = "1234";
	char* key2 = "23456";
	char* key3 = "345667";
	char* key4 = "6789910";
	char* key5 = "3456671";
	char* key6;
	PutConstStringPool(pool,key1);
	PutConstStringPool(pool,key2);
	PutConstStringPool(pool,key3);
	PutConstStringPool(pool,key4);
	PutConstStringPool(pool,key5);
	PutConstStringPool(pool,key5);
	key6 = key1;
	PutConstStringPool(pool,key6);
	key6 = key2;
	PutConstStringPool(pool,key6);
	key6 = key3;
	PutConstStringPool(pool,key6);
	key6 = key4;
	PutConstStringPool(pool,key6);
	key6 = key5;
	PutConstStringPool(pool,key6);
	*/
	for (i=10000; i<10032; i++)
	{
		Int2DecStr(i,dec);
		PutConstStringPool(pool,dec);
		memset(dec,0,10);
	}
	printf("Total:%d,%d\r\n",pool->size,pool->modCount);
	test_print_stringpool(pool);
	for (i=10000; i<10010; i++)
	{
		Int2DecStr(i,dec);
		RemoveStringPool(pool,dec,strlen(dec));
		memset(dec,0,10);
	}
	printf("Remove:%d,%d\r\n",pool->size,pool->modCount);
	for (i=10000; i<10032; i++)
	{
		Int2DecStr(i,dec);
		printf("%d,%s\r\n",i,GetConstStringPool(pool,dec));
		memset(dec,0,10);
	}
	CleanStringPool(pool);
	printf("Clean:%d,%d\r\n",pool->size,pool->modCount);
}


static void test_hash_table()
{
	int i = 0;
	int v = 0;
	int *value;
	char dec[10];
	HashTablePtr table = CreateHashTable(16,Long_Type,Long_Type);
	for (i=323410; i<323460; i++)
	{
		v = i * 100;
		PutHashTable(table,&i,&v);
		//memset(dec,0,10);
	}
	printf("%d,%d,%d\r\n",table->bucketCount,table->bucketUseCount,table->size);
	for (i=323422; i<323442; i++)
	{
		//Int2DecStr(i,dec);
		RemoveHashTable(table,&i);
		//memset(dec,0,10);
	}
	printf("%d,%d,%d\r\n",table->bucketCount,table->bucketUseCount,table->size);
	for (i=323410; i<323460; i++)
	{
		//Int2DecStr(i,dec);
		value = (int*)GetHashTable(table,&i);
		if (NULL != value)
		{
			printf("%d,%d\r\n",i,*value);
		}
		else
		{
			printf("%d,NULL\r\n",i);
		}
		//memset(dec,0,10);
	}
	test_print_hashtable(table);
	CleanHashTable(table);
	printf("%d,%d,%d\r\n",table->bucketCount,table->bucketUseCount,table->size);
}

static void test_print_tree_node(TreePtr tree,TreeNodePtr node)
{
	int layer = 0;
	long parentKey = 0;
	if (NULL != node)
	{
		layer = GetTreeNodeLayer(tree,node->keyValue);
		if (NULL != node->parent)
		{
			parentKey = *((long*)node->parent->keyValue);
		}
		printf("key=%d,value=%d,color=%d,parent=%d,layer=%d\r\n",*(long*)node->keyValue,*((long*)node->keyValue + 1),node->color,parentKey,layer);
		test_print_tree_node(tree,node->left);
		test_print_tree_node(tree,node->right);
	}
}

void test_print_tree(TreePtr tree)
{
	test_print_tree_node(tree,tree->root);
}

void test_tree()
{
	int i = 0;
	int v = 0;
	int *value;
	char dec[10];
	TreePtr tree = CreateTree(Long_Type,Long_Type);
	for (i=1; i<320; i++)
	{
		v = i * 100;
		InsertTree(tree,&i,&v);
		//memset(dec,0,10);
	}
	for (i=1; i<300; i++)
	{
		RemoveTree(tree,&i);
	}
	test_print_tree(tree);
	printf("\r\n------------------------------clean before\r\n");
	CleanTree(tree);
	test_print_tree(tree);
	DestroyTree(tree);
	printf("\r\n---------------tree size:%d\r\n",tree->size);
}
int main()
{
	long long u ;
	unsigned __int64 time;
	unsigned int i,pos = 0;
	_CrtDumpMemoryLeaks();
	//test_array();
	//test_stack();
	////test_queue();
	//test_string_pool();
	test_string_hash();
	//test_hash_table();
	//test_array_string();
	//test_list_string();
	//test_tree();
	//u = (__int64)*poolNode;
	//FixedCpuRun(NULL);
	time = GetCpuTimeStampCounter();
	pos++;
	printf("CpuTimeStampCounter:%I64u\r\n",GetCpuTimeStampCounter() - time);
	//printf("%I64u\r\n",);//linux llu
	//printf("%I64u\r\n",GetRegFlags());
	printf("CpuTimeStampCounter:%I64u\r\n",GetCpuTimeStampCounter() - time);
	getchar();
	_CrtDumpMemoryLeaks();
}