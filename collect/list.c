/******************************************************************************************
 * @file list.c
 * @anthor:Croky.Zheng
 * @CreateTime:2013-5-20
 * @descirption:链表，每加入一个元素，则需要重新开辟一次元素空间
 *****************************************************************************************/
#include "list.h"
#include "common.h"
#include <stdlib.h>
#include <string.h>


//拷贝数据到节点
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

//建立一个默认的列表，元素的大小为指针大小
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
//----------------------------stack堆栈

//将数据压入栈
void PushStack(ListPtr list,void* data)
{
	InsertListHead(list,data);
}

//将所有数据压入栈
void PushAllStack(ListPtr list,void* data,unsigned long count)
{
	InsertMoreListHead(list,data,count);
}

//数据出栈
void PopStack(ListPtr list,void* data)
{
	RemoveListHead(list,data);
}

//所有数据出栈
void PopAllStack(ListPtr list)
{
	CleanList(list);
}

//--------------------------------------end stack

//--------------------------------------queue 队列

//将数据压入队列
void OfferQueue(ListPtr list,void* data)
{
	InsertList(list,data);
}

//获取和移除数据
void PollQueue(ListPtr list,void* data)
{
	RemoveListHead(list,data);
}

//获取但是不移除数据
void PeekQueue(ListPtr list,void* data)
{
	if ((NULL != list) && (NULL != list->tail) && (NULL != data))
	{
		memcpy(data,list->tail->data,list->elementByteSize);
	}
}
//--------------------------------------end queue
//插入数据到链表头部
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

//插入数据到链表尾部
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

//排序插入,从小到大排序插入数据
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

			//将节点插入懂啊prev和cur之间
			if (NULL != prev_node)
			{
				prev_node->next = node;
				node->next = cur_node;
			}

			//节点为头节点
			if (cur_node == list->head)
			{
				node->next = list->head;
				list->head = node;
			}

			//节点为尾节点
			if (prev_node == list->tail)
			{
				/* node已被定位
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
		//如果dest没有头指针,则使用src的头指针
		if (NULL == dest->head)
		{
			dest->head = src->head;
		}
		//重新设置dest尾指针的next指向src的头
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
		//如果dest没有头指针,则使用src的头指针
		if (NULL != src->head)
		{
			dest->head = src->head;
		}
		//重新设置dest尾指针的next指向src的头
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

//获取指定序号的数据，这种方法的效率那是相当的低
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

//删除头节点，这个还是很快的
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
		//清理内存
		free(tmp);
		//删除到最后一个,则需要重置tail的值
		if (list->size == 0)
		{
			list->tail = NULL;
		}
	}
}

//删除尾节点，需要重新定位尾部节点
void RemoveListTail(ListPtr list,void* data)
{
	ListNodePtr tmp;
	ListNodePtr current = NULL;
	/*不从尾节点拷贝数据
	if ((NULL != list) && (NULL != list->tail))
	{
		if (NULL != data)
		{
			memcpy(data,list->tail,list->elementByteSize);
		}
		--list->size;
		//删除到最后一个,则需要重置tail的值
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
			//这个表示是尾节点
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

//删除指定数据的节点
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


//清理所有的节点
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
				//获取下一个节点
				next = current->next;
				//释放当前节点
				free(current);
				//从下一个节点开始
				current = next;
			}
		}
		//就不一个个去删除了
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