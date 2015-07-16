/******************************************************************************************
 * @file trie.h
 * @anthor:Croky.Zheng
 * @CreateTime:2013-6-6
 * @descirption:trie�ṹ
 * ����TRIE�����νṹ
 *****************************************************************************************/
#ifndef BELIAL_COLLECT_TRIE_H
#define BELIAL_COLLECT_TRIE_H
#include "typedef.h"
#include "define.h"

#ifdef __cplusplus
extern "C" {	//����C++
#endif

typedef struct
{
	//�ʵĳ���
	unsigned char length;
}Word,*WordPtr;


//trie�ṹ
typedef struct
{
	//base ����
	long* base;

	//check����
	long* check;

	//�������
	unsigned long size;

	//�ڵ����
	unsigned long nodeCount;

	//��������
	unsigned long capacity;

	//�δ�С
	unsigned segSize;
} Trie,*TriePtr;

#ifdef __cplusplus
}//����C++
#endif
#endif