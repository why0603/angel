/******************************************************************************************
 * @file mathematic.h
 * @anthor:Croky.Zheng
 * @CreateTime:2010-05-31
 * @descirption:����һЩC��׼��û���Դ������߲��������������ѧ���㺯��
 *****************************************************************************************/
#ifndef BELIAL_COMMON_MATHEMATIC_H
#define BELIAL_COMMON_MATHEMATIC_H
#include "typedef.h"
#include "define.h"


#ifdef __cplusplus
extern "C" {	//����C++
#endif

/**************************************************************
 * �����x��y�η�
 * @param x		����
 * @param y		ָ��
 * @return x^y
 */
EXTERN	UINT64 Pow(int x,int y);


/*************************************************************
 * ����x��y�η���m��ģ
 * @param x		����
 * @param y		ָ��
 * @param m		ģ��
 * @return	(x^y) % m
 */
EXTERN	int	PowMod(int x,int y,int m);
#ifdef __cplusplus
}	//����C++
#endif

#endif