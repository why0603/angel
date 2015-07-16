/******************************************************************************************
 * @file list.c
 * @anthor:Croky.Zheng
 * @CreateTime:2013-5-20
 * @descirption:����ÿ����һ��Ԫ�أ�����Ҫ���¿���һ��Ԫ�ؿռ�
 *****************************************************************************************/
#include "list.h"
#include "common.h"
#include <stdlib.h>
#include <string.h>


//�������ݵ��ڵ�
/*
static void CopyDataToNode(ListPtr list,ListNodePtr node,void* data);
void CopyDataToNode(ListPtr list,ListNodePtr node,void* data)
{
	if (list->dataType == Chars_Type)
	{
		strcpy(node->data,(const char*)data);
		//strcpy(node->data,(const char*)data,list->elementByteSize);
	}
	else if (list->dataType == Unicode_Type)
	{
		wcscpy((wchar_t*)node->data,(const wchar_t*)data);
	}
	else
	{
		CopyData(node->data,data,list->dataType,list->elementByteSize);
	}
}
*/

//����һ��Ĭ�ϵ��б�Ԫ�صĴ�СΪָ���С
ListPtr CreateDefaultList()
{
	return CreateList(sizeof(void*));
}

ListPtr CreateList(unsigned long elementByteSize)
{
	ListPtr list = (ListPtr)calloc(1,sizeof(List));
	if (NULL != list)
	{
		//memset(list,0,sizeof(List));
		if (elementByteSize == 0)
		{
			list->elementByteSize = sizeof(void*);
		}
		else
		{
			list->elementByteSize = elementByteSize;
		}
		list->size = 0;
		list->head = list->tail = NULL;
		list->dataType = (DataType)list->elementByteSize;
		list->isSigned = False;
	}
	return list;
}
//----------------------------stack��ջ

//������ѹ��ջ
void PushStack(ListPtr list,void* data)
{
	InsertListHead(list,data);
}

//����������ѹ��ջ
void PushAllStack(ListPtr list,void* data,unsigned long count)
{
	InsertMoreListHead(list,data,count);
}

//���ݳ�ջ
void PopStack(ListPtr list,void* data)
{
	RemoveListHead(list,data);
}

//�������ݳ�ջ
void PopAllStack(ListPtr list)
{
	CleanList(list);
}

//--------------------------------------end stack

//--------------------------------------queue ����

//������ѹ�����
void OfferQueue(ListPtr list,void* data)
{
	InsertList(list,data);
}

//��ȡ���Ƴ�����
void PollQueue(ListPtr list,void* data)
{
	RemoveListHead(list,data);
}

//��ȡ���ǲ��Ƴ�����
void PeekQueue(ListPtr list,void* data)
{
	if ((NULL != list) && (NULL != list->tail) && (NULL != data))
	{
		memcpy(data,list->tail->data,list->elementByteSize);
	}
}
//--------------------------------------end queue
//�������ݵ�����ͷ��
void InsertListHead(ListPtr list,void* data)
{
	ListNodePtr node;
	unsigned long byteSize = 0;
	//Boolean ret = False;
	if ((NULL != list) && (NULL != data))
	{
		byteSize = GetByteSizeOfData(list->dataType,data);
		node = (ListNodePtr)malloc(sizeof(ListNode) + byteSize);
		if (NULL != node)
		{
			//memcpy(node->data,data,byteSize);
			CopyData(node->data,data,list->dataType,byteSize);
			node->next = list->head;
			list->head = node;
			if (NULL == list->tail)
			{
				list->tail = node;
			}
			list->size++;
		}
	}
}

//�������ݵ�����β��
void InsertList(ListPtr list,void* data)
{
	ListNodePtr node;
	unsigned long byteSize = 0;
	//Boolean ret = False;
	if ((NULL != list) && (NULL != data))
	{
		byteSize = GetByteSizeOfData(list->dataType,data);
		node = (ListNodePtr)malloc(sizeof(ListNode) + byteSize);
		if (NULL != node)
		{
			node->next = NULL;
			//memcpy(node->data,data,list->elementByteSize);
			CopyData(node->data,data,list->dataType,byteSize);
			if (NULL == list->head)
			{
				list->head = node;
			}
			if (NULL != list->tail)
			{
				list->tail->next = node;
			}
			list->tail = node;
			list->size++;
		}
	}
}

//�������,��С���������������
void InsertListSort(ListPtr list,void* data)
{
	ListNodePtr node,cur_node,prev_node;
	unsigned long byteSize = 0;
	if ((NULL != list) && (NULL != data))
	{
		byteSize = GetByteSizeOfData(list->dataType,data);
		node = (ListNodePtr)malloc(sizeof(ListNode) + byteSize);
		if (NULL != node)
		{
			node->next = NULL;
			CopyData(node->data,data,list->dataType,byteSize);
			prev_node = NULL;
			cur_node = list->head;
			while (NULL != cur_node)
			{
				//
				if (Compare_Less == Compare(node->data,cur_node->data,list->dataType,list->elementByteSize,list->isSigned))
				{
					break;
				}
				prev_node = cur_node;
				cur_node = cur_node->next;
			}

			//���ڵ���붮��prev��cur֮��
			if (NULL != prev_node)
			{
				prev_node->next = node;
				node->next = cur_node;
			}

			//�ڵ�Ϊͷ�ڵ�
			if (cur_node == list->head)
			{
				node->next = list->head;
				list->head = node;
			}

			//�ڵ�Ϊβ�ڵ�
			if (prev_node == list->tail)
			{
				/* node�ѱ���λ
				if (NULL != list->tail)
				{
					list->tail->next = node;
				}
				*/
				list->tail = node;
			}
			list->size++;
		}
	}
}

void InsertMoreListHead(ListPtr list,void* data,unsigned long count)
{
	unsigned long i;
	//Boolean ret = False;
	if ((NULL != list) && (NULL != data) && (count > 0))
	{
		for (i=0; i<count; i++)
		{
			InsertListHead(list,(char*)data + i * list->elementByteSize);
		}
	}
}

void InsertMoreList(ListPtr list,void* data,unsigned long count)
{
	unsigned long i;
	//Boolean ret = False;
	if ((NULL != list) && (NULL != data) && (count > 0))
	{
		for (i=0; i<count; i++)
		{
			InsertList(list,(char*)data + i * list->elementByteSize);
		}
	}
}

void InsertListToList(ListPtr dest,ListPtr src)
{
	//Boolean ret = False;
	if ((NULL != dest) && (NULL != src) && (dest->elementByteSize == src->elementByteSize))
	{
		//���destû��ͷָ��,��ʹ��src��ͷָ��
		if (NULL == dest->head)
		{
			dest->head = src->head;
		}
		//��������destβָ���nextָ��src��ͷ
		if ((NULL != dest->tail) && (NULL != src->head))
		{
			dest->tail->next = src->head;
		}
		dest->tail = src->tail;
		dest->size += src->size;
	}
}

void InsertListToListHead(ListPtr dest,ListPtr src)
{
	//Boolean ret = False;
	if ((NULL != dest) && (NULL != src) && (dest->elementByteSize == src->elementByteSize))
	{
		//���destû��ͷָ��,��ʹ��src��ͷָ��
		if (NULL != src->head)
		{
			dest->head = src->head;
		}
		//��������destβָ���nextָ��src��ͷ
		if (NULL != src->tail)
		{
			src->tail->next = dest->head;
		}
		dest->size += src->size;
	}
}

int IndexOfList(ListPtr list,void* data)
{
	ListNodePtr current;
	int index = -1;
	if((NULL != list) && (NULL != data))
	{
		current = list->head;
		while(NULL != current)
		{
			index++;
			if (memcmp(current->data,data,list->elementByteSize) == 0)
			{
				break;
			}
			current = current->next;
		}
	}
	return index;
}

//��ȡָ����ŵ����ݣ����ַ�����Ч�������൱�ĵ�
void GetDataFromList(ListPtr list,void* data,unsigned long index)
{
	ListNodePtr current = NULL;
	unsigned long postion = 0;
	//Boolean ret = False;
	if((NULL != list) && (NULL != data) && (index < list->size))
	{
		current = list->head;
		while((NULL != current) && (postion++ < index))
		{
			current = current->next;
		}
		if (NULL != current)
		{
			memcpy(data,current->data,list->elementByteSize);
			//ret = True;
		}
	}
	//return ret;
}

void GetDataFromListNode(ListNodePtr node,void* data,unsigned long byteSize)
{
	if ((NULL != node) && (NULL != data) && (byteSize > 0))
	{
		memcpy(data,node->data,byteSize);
	}
}

//ɾ��ͷ�ڵ㣬������Ǻܿ��
void RemoveListHead(ListPtr list,void* data)
{
	ListNodePtr tmp;
	if ((NULL != list) && (NULL != list->head))
	{
		if (NULL != data)
		{
			memcpy(data,list->head,list->elementByteSize);
		}
		--list->size;
		tmp = list->head;
		list->head = list->head->next;
		//�����ڴ�
		free(tmp);
		//ɾ�������һ��,����Ҫ����tail��ֵ
		if (list->size == 0)
		{
			list->tail = NULL;
		}
	}
}

//ɾ��β�ڵ㣬��Ҫ���¶�λβ���ڵ�
void RemoveListTail(ListPtr list,void* data)
{
	ListNodePtr tmp;
	ListNodePtr current = NULL;
	/*����β�ڵ㿽������
	if ((NULL != list) && (NULL != list->tail))
	{
		if (NULL != data)
		{
			memcpy(data,list->tail,list->elementByteSize);
		}
		--list->size;
		//ɾ�������һ��,����Ҫ����tail��ֵ
		if (list->size == 0)
		{
			list->head = NULL;
			list->tail = NULL;
		}
		else
		{
			current = list->head;
			while(list->tail != current->next)
			{
				current = current->next;
			}
			if (NULL != current)
			{
				current = NULL;
				list->tail = current;
			}
		}
	}
	*/
	if ((NULL != list) && (NULL != list->tail))
	{
		tmp = current = list->head;
		while (NULL != current)
		{
			//�����ʾ��β�ڵ�
			if (NULL == current->next)
			{
				if (NULL != data)
				{
					memcpy(data,current,list->elementByteSize);
				}
				break;
			}
			tmp = current;
			current = current->next;
		}
		list->tail = tmp;
		free(current);
		--list->size;
		if (list->size == 0)
		{
			list->head = NULL;
		}
	}
}

//ɾ��ָ�����ݵĽڵ�
void RemoveDataList(ListPtr list,void* data)
{
	ListNodePtr current;
	int index = -1;
	if((NULL != list) && (NULL != data))
	{
		current = list->head;
		if ((NULL != current) && memcmp(current->data,data,list->elementByteSize) == 0)
		{
			list->head = current->next;
		} else 
		{
			while(NULL != current->next)
			{
				if (memcmp(current->next->data,data,list->elementByteSize) == 0)
				{

					break;
				}
				current = current->next;
			}
		}
	}
}


//�������еĽڵ�
void CleanList(ListPtr list)
{
	ListNodePtr current;
	ListNodePtr next;
	if (NULL != list)
	{
		if (NULL != list->head)
		{
			current = list->head;
			while(NULL != current)
			{
				//��ȡ��һ���ڵ�
				next = current->next;
				//�ͷŵ�ǰ�ڵ�
				free(current);
				//����һ���ڵ㿪ʼ
				current = next;
			}
		}
		//�Ͳ�һ����ȥɾ����
		list->size = 0;
		list->head = NULL;
		list->tail = NULL;
	}
}

void DestroyList(ListPtr list)
{
	CleanList(list);
	if (NULL != list)
	{
		free(list);
	}
}

void EnumListNode(ListPtr list,ListCallBack fnList)
{
	ListNodePtr current;
	if ((NULL != list) && (NULL != fnList))
	{
		current = list->head;
		while (NULL != current)
		{
			fnList(list,current);
			current = current->next;
		}
	}
}