#ifndef __Y_MTU_SERVER_H__
#define __Y_MTU_SERVER_H__
//---------------------------------------------------------------------------
// echo_server.h - UDP MTU  Server                                          -
//---------------------------------------------------------------------------
// Version                                                                  -
//      0.1 Original Version October 21, 2022                               -
//                                                                          -
// (c)2022 remot3.it                                                        -
//---------------------------------------------------------------------------

#include "config.h"
#include "mytypes.h"


#define MTU_DEFALT_LISTEN_PORT      9999
#define MTU_REPLIES_PER_SECOND	    2



//
// GF flags, global flags
//
#define GF_GO           0x01                /* go */
#define GF_DAEMON       0x02                /* we are a daemon */
#define GF_QUIET        0x04                /* no output */





// Custom File config for each product here
typedef struct mtu_config_
{
    IPADDR		bind_IP;
    U16         listen_port;
    SOCKET      listen_soc;
    //
    int         verbose;
    int         log_level;

    // Stat Values
    long        requests;
	long		replies;
    long        bad_requests;
    long        tx_err;

    char        pidfile[MAX_PATH];
}MTU;

#endif




