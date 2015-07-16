			/*********************************************************************
 * @file CY_Crypt.h
 * @anthor:Croky.Zheng
 * @CreateTime:2012/2/20
 * @descirption:所有加密算法使用的一个公共头
 **********************************************************************/
#include "CY_Blowfish.h"
#include "CY_Tea.h"
#include "CY_Des.h"
#include "CY_Crc32.h"
#include "CY_Rsa.h"
#include "CY_Md5.h"
#include "CY_Sha.h"
#include "CY_Base64.h"
#include "CY_Rc.h"


#define BASENUM		0x012E703B



//字长 位为单位
#define WORDSIZE 32
//定义逻辑移位操作运算
//循环移位,最多循环移位31位
//将x循环左移n位
//#define ROTL(x,n) (((x)<<(n&(WORDSIZE-1))) | ((x)>>(WORDSIZE-(n&(WORDSIZE-1)))))

//将x循环右移n位
//#define ROTR(x,n) (((x)>>(n&(WORDSIZE-1))) | ((x)<<(WORDSIZE-(n&(WORDSIZE-1)))))
