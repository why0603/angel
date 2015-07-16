/******************************************************************************************
 * @file trie.h
 * @anthor:Croky.Zheng
 * @CreateTime:2013-6-6
 * @descirption:trie结构
 * 基于TRIE的树形结构
 *****************************************************************************************/
#ifndef BELIAL_COLLECT_TRIE_H
#define BELIAL_COLLECT_TRIE_H
#include "typedef.h"
#include "define.h"

#ifdef __cplusplus
extern "C" {	//兼容C++
#endif

typedef struct
{
	//词的长度
	unsigned char length;
}Word,*WordPtr;


//trie结构
typedef struct
{
	//base 数组
	long* base;

	//check数组
	long* check;

	//词组个数
	unsigned long size;

	//节点个数
	unsigned long nodeCount;

	//数组容量
	unsigned long capacity;

	//段大小
	unsigned segSize;
} Trie,*TriePtr;

#ifdef __cplusplus
}//兼容C++
#endif
#endif