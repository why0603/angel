			/*********************************************************************
 * @file CY_Crypt.h
 * @anthor:Croky.Zheng
 * @CreateTime:2012/2/20
 * @descirption:���м����㷨ʹ�õ�һ������ͷ
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



//�ֳ� λΪ��λ
#define WORDSIZE 32
//�����߼���λ��������
//ѭ����λ,���ѭ����λ31λ
//��xѭ������nλ
//#define ROTL(x,n) (((x)<<(n&(WORDSIZE-1))) | ((x)>>(WORDSIZE-(n&(WORDSIZE-1)))))

//��xѭ������nλ
//#define ROTR(x,n) (((x)>>(n&(WORDSIZE-1))) | ((x)<<(WORDSIZE-(n&(WORDSIZE-1)))))
