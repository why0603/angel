/******************************************************************************************
 * @file keccak.c
 * @anthor:Croky.Zheng
 * @CreateTime:2013-5-15
 * @descirption: 一个类似MD5，SHA算法
 * 其实为SHA-3算法,有224，256,384,512四种输出长度
 * SHA3-224("") = f71837502ba8e10837bdd8d365adb85591895602fc552b48b7390abd
 * SHA3-256("") = c5d2460186f7233c927e7db2dcc703c0e500b653ca82273b7bfad8045d85a470
 * SHA3-384("") = 2c23146a63a29acf99e73b88f8c24eaa7dc60aa771780ccc006afbfa8fe2479b2dd2b21362337441ac12b515911957ff
 * SHA3-512("") = 0eab42de4c3ceb9235fc91acffe746b29c29a8c366b7c60e4e67c466f36a4304c00fa9caf9d87976ba469bcbe06713b435f091ef2769fb160cdab33d3670680e
 * NTLM($pass) = MD4(Unicode($pass))
 * LM($pass) = DES("KGS!@#$%",$pass));
 *****************************************************************************************/
#include "keccak.h"