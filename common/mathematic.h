/******************************************************************************************
 * @file mathematic.h
 * @anthor:Croky.Zheng
 * @CreateTime:2010-05-31
 * @descirption:用于一些C标准库没有自带，或者不能满足需求的数学计算函数
 *****************************************************************************************/
#ifndef BELIAL_COMMON_MATHEMATIC_H
#define BELIAL_COMMON_MATHEMATIC_H
#include "typedef.h"
#include "define.h"


#ifdef __cplusplus
extern "C" {	//兼容C++
#endif

/**************************************************************
 * 求参数x的y次方
 * @param x		基数
 * @param y		指数
 * @return x^y
 */
EXTERN	UINT64 Pow(int x,int y);


/*************************************************************
 * 计算x的y次方与m的模
 * @param x		基数
 * @param y		指数
 * @param m		模数
 * @return	(x^y) % m
 */
EXTERN	int	PowMod(int x,int y,int m);
#ifdef __cplusplus
}	//兼容C++
#endif

#endif