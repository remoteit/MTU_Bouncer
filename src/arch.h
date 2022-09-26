#ifndef __ARCH_H__
#define __ARCH_H__
/*! \file arch.h
    \brief Platform specific functions, or custom non standard lib functions
*/

//---------------------------------------------------------------------------
// arch.h - platform functions							-
//---------------------------------------------------------------------------
// Version                                                                  -
//		0.1 Original Version August 31, 2006     							-
//																			-
// (c)2006 mycal.net 								-
//---------------------------------------------------------------------------

#include "config.h"

int get_last_error(void);
int	set_sock_nonblock(SOCKET);
U32 second_count(void);
U16	hund_ms_count(void);
void ysleep_seconds(U16 duration);
S16 get_mac_address(U8 *adapter, U8 *macaddr);
IPADDR	get_local_ip(U8 *adapter);
void bin2hexstr(char *buf, char *str, int len);
void hexstr2bin(char *str, char *buf, int len);
void IP_Extract(IPADDR *ip,U8 *ip_ascii);
void UID_2_ASC(U8*str, U8* uid);
void Yoics_asc_2_uni(U8 *buffer,U8 *pass);

S16 network_init(void);
void threadswitch(void);
int  NetConnect1(const char *pcServer, unsigned short usPort, int iMillSecTimeout, int *iOut_fd);

int yrand(int max);
void yrand_seed(void);

int test_udp_bind(U16 port);

#endif




