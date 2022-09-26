/*!																www.mycal.com			
 *---------------------------------------------------------------------------
 *! \file arch.c
 *  \brief Platform dependent code for, contains abstaracted 
 *		patform independent interface interface.
 *																			
 *---------------------------------------------------------------------------
 * Version                                                                  -
 *		0.1 Original Version June 3, 2006									-        
 *
 *---------------------------------------------------------------------------    
 * Version                                                                  -
 * 0.1 Original Version August 31, 2006     							    -
 *																			-
 * (c)2006 mycal.net  All Rights Reserved									-
 *---------------------------------------------------------------------------
 *
 */

/*					*/ 
/* include files 	*/
/*					*/ 
//#include	"config.h"
#include	"mytypes.h"
#include	"arch.h"
#include	"debug.h"	


#if defined(MACOSX)
#include	<net/if_dl.h>
#include	<ifaddrs.h>	// getifaddrs() for get_mac_address()

#include	<sys/time.h>	// Mac OS X 10.3 and before lean towards Unix Sys V- no ftime().
#endif


void
Yoics_asc_2_uni(U8 *buffer,U8 *pass)
{
	int len,i,j,l;

	j=strlen( (char *) pass);
	len=strlen( (char *) buffer);

	for(l=0,i=len;l<j;i=i+2,l++)
	{
		buffer[i]	=pass[l];
		buffer[i+1]	=0;
	}
}


/*
unsigned char bin2hex(unsigned char c)
{
  if (c >= 10) return 'a' + c - 10;
  else return '0' + c;
}

unsigned char hex2bin(unsigned char c)
{
  if (c >= 'a') return c - 'a' + 10;
  if (c >= 'A') return c - 'A' + 10;
  return c - '0';
}
*/
U8
bin2hex(U8 bin)
{
	U8	ret='0';

	if(bin<0x0a)
		ret=bin+'0';
	else if(bin<0x10)
		ret=bin+('A'-10);

	return(ret);
}

U8
hex2bin(U8 c)
{
return  
		(('0'<=(c))&&('9'>=(c)))				\
                 ?((c)-'0')                     \
                 :( (('a'<=(c))&&('f'>=(c)))    \
                    ?((c)-'a'+10)               \
                    :( (('A'<=(c))&&('F'>=(c))) \
                       ?((c)-'A'+10)            \
                       :(0xFF) ) ) ;
					   
}
/**
 * Converts a binary buffer into a hexadecimal string.
 *
 * @param buf binary buffer of len bytes size
 * @param str string of at least 2*len+1 bytes size
 * @param len length of the buffer
 * @returns -
 */
void bin2hexstr(char *buf, char *str, int len)
{
  int i;

  /* convert the byte array to a hex string */
  for (i = 0; i < len; i++) 
  {
    str[i*2]	= bin2hex(  (U8)((buf[i] >> 4) & 0x0F) );
    str[i*2+1]	= bin2hex( (U8) (buf[i] & 0x0F) );\
  }
  str[2*i] = 0;			// str[i<<1] = 0;
}

/**
 * Converts a hexadecimal string into a binary buffer.
 *
 * @param str string of at least 2*len+1 bytes size
 * @param buf binary buffer of len bytes size
 * @param len length of the buffer
 * @returns -
 */
void hexstr2bin(char *str, char *buf, int len)
{
  int i;

  /* convert the hex string to a byte array */
  memset(buf, 0, len);
  for (i = strlen(str)-1; i >= 0; i -= 2) 
  {
    if (i > 0) 
		buf[i/2] = hex2bin(str[i-1])*16 + hex2bin(str[i]);
    else 
		buf[i/2] = hex2bin(str[i]);
  }
}




void
UID_2_ASC(U8*str, U8* uid)
{
	sprintf((char *) str,"%.2x:%.2x:%.2x:%.2x:%.2x:%.2x:%.2x:%.2x",uid[0],uid[1],uid[2],uid[3],uid[4],uid[5],uid[6],uid[7]);
}


//
// Convert a UID in asc format to binary (ie this format 00:00:2D:4A:7B:3E:1F:8B)
//
//
void
UID_Extract(U8 *uid,U8 *uid_ascii)
{
	int	i;
	U8	*subst;

	// scan out the bytes.
	subst=(U8 *) strtok((char *) uid_ascii,": \n");

	// Loop for 8 bytes
	for(i=0;i<8;i++)
	{
		uid[i]=	(U8)strtol((char *) subst,(char **)NULL, 16);
		subst= (U8 *) strtok(NULL,": \n");
		if(NULL==subst)
			break;
	}

	/*	Not portable		
	sscanf((const char *) uid_ascii,"%2x:%2x:%2x:%2x:%2x:%2x:%2x:%2x",(unsigned int *) &tid[0],
                                                           (unsigned int *) &tid[1],
                                                           (unsigned int *) &tid[2],
                                                           (unsigned int *) &tid[3],
                                                           (unsigned int *) &tid[4],
                                                           (unsigned int *) &tid[5],
                                                           (unsigned int *) &tid[6],
                                                           (unsigned int *) &tid[7]);
	memcpy(uid,tid,UID_SIZE);
	*/
	return;
}

void
IP_Extract(IPADDR *ip,U8 *ip_ascii)
{		
#if defined(WIN32) || defined(WINCE)	
	U8	*subst;

	// scan out the bytes.
	subst=(U8 *) strtok((char *) ip_ascii,". \n");

	while(1)
	{
		// get 4 bytes
		if(!subst) break;
		ip->ipb1 =	(U8)atoi((char *) subst);
		subst= (U8 *) strtok(NULL,". \n");
		if(!subst) break;
		ip->ipb2 =	(U8)atoi((char *) subst);
		subst= (U8 *) strtok(NULL,". \n");
		if(!subst) break;
		ip->ipb3 =	(U8)atoi((char *) subst);
		subst= (U8 *) strtok(NULL,". \n");
		if(!subst) break;
		ip->ipb4 =	(U8)atoi((char *) subst);
		break;
	}
#else
	if(0==	inet_aton((const char *) ip_ascii, (struct in_addr *) ip )) 
		ip->ip32=0;
#endif

	return;
}




int
set_sock_nonblock(SOCKET lsock)
{
	int ret;
#if defined(WIN32) || defined(WINCE)
	u_long	flags;

	flags=0x1;
	ret=ioctlsocket ( lsock, FIONBIO, (u_long FAR *) &flags);
#endif

#if defined (__ECOS)
    int tr = 1;
    ret=ioctl(lsock, FIONBIO, &tr); 
#endif

#if defined(LINUX) || defined(MACOSX)

	int flags;

	flags = fcntl(lsock, F_GETFL, 0);
	ret=fcntl(lsock, F_SETFL, O_NONBLOCK | flags);

#endif

	return(ret);
}

int
set_sock_send_timeout(SOCKET lsock)
{
	return(0);
}



int get_last_error()
{
#if defined(WIN32) || defined(WINCE)
	int err;
	err=errno;
	err=WSAGetLastError();
	return(err);
#endif

#if defined(LINUX) || defined(MACOSX) || defined(__ECOS)
	return(errno);
#endif
}

/*
void
log_lasterror(U8 *msg)
{
	printlog(LOG_MISC,"error in %s errno=%d\n",msg,get_last_error());
}
*/



#if defined(WIN32) || defined(WINCE)
#define		timeb	_timeb
#define		ftime	_ftime
#endif


//
// Returns a 32bit seconds count
//
U32 second_count(void)
{
	int	seconds;
	// grab a time value and return a 16 bit 10ms count used for timestamping - 1000=1second
	//#if 0
	//ftime() is obsolete everywhere except maybe embedded Linux?  
	//OS X didn't get ftime() until OS X 10.4.  So we do this...
	// BUT it doesn't work with Mike's code.  It does now....  defined(LINUX) ||
#if  defined(MACOSX) || defined(__ECOS)
	struct timeval   Tp;
	struct timezone  Tzp;
    gettimeofday( &Tp, &Tzp );     /* get timeofday */
	seconds=0;
	seconds=(U32)(Tp.tv_sec);
#endif
#if defined(LINUX) || defined(WIN32)
	struct timeb    timebuffer;
	ftime( &timebuffer );
	seconds=0;
	seconds=(U32)(timebuffer.time);
#endif
#if defined(WINCE)
	SYSTEMTIME tSystemtime;
	GetLocalTime(&tSystemtime);
	seconds=(U32)tSystemtime.wSecond;
#endif
	return(seconds);
}

//
// hund_ms_count(void) - returns a 16 bit 100ms tick count.
//
U16	hund_ms_count(void)
{	
	U16	ticks;
	// grab a time value and return a 16 bit 10ms count used for timestamping - 1000=1second
	//#if 0
	//ftime() is obsolete everywhere except maybe embedded Linux?  
	//OS X didn't get ftime() until OS X 10.4.  So we do this...
	// BUT it doesn't work with Mike's code.  It does now....  defined(LINUX) ||

// Should this all be like the MAC?

#if  defined(MACOSX) || defined(__ECOS) || defined(LINUX)
	struct timeval   Tp;
	struct timezone  Tzp;
    gettimeofday( &Tp, &Tzp );     /* get timeofday */
	ticks=0;
	ticks=(U16)(Tp.tv_sec*10);
	ticks=ticks + ((Tp.tv_usec)/100000);
#endif
#if defined(WIN32) 
	struct timeb    timebuffer; 
	ftime( &timebuffer );
	ticks=0;
	ticks=(U16)(timebuffer.time*10);
	ticks=ticks + ((timebuffer.millitm)/100);
#endif
#if defined(WINCE)
	SYSTEMTIME tSystemtime;
	GetLocalTime(&tSystemtime);
	ticks=0;
	ticks=(U16)(tSystemtime.wSecond*10);
	ticks=ticks+(tSystemtime.wMilliseconds/100);
#endif
	return(ticks);
}


//
// network_init()-
// If anything needs to be initialized before using the network, put it here, mostly this
//	is for windows.
//
S16
network_init()
{
#if defined(WIN32) || defined(WINCE)

	WSADATA w;								/* Used to open Windows connection */
	/* Open windows connection */
	if (WSAStartup(0x0101, &w) != 0)
	{
		fprintf(stderr, "Could not open Windows connection.\n");
		exit(0);
	}	

#endif
return(0);
}


#if WIN32a
static void GetMACaddress(void)
{
  IP_ADAPTER_INFO AdapterInfo[16];       // Allocate information
                                         // for up to 16 NICs
  DWORD dwBufLen = sizeof(AdapterInfo);  // Save memory size of buffer

  DWORD dwStatus = GetAdaptersInfo(      // Call GetAdapterInfo
    AdapterInfo,                 // [out] buffer to receive data
    &dwBufLen);                  // [in] size of receive data buffer
  assert(dwStatus == ERROR_SUCCESS);  // Verify return value is
                                      // valid, no buffer overflow

  PIP_ADAPTER_INFO pAdapterInfo = AdapterInfo; // Contains pointer to
                                               // current adapter info
  do {
    PrintMACaddress(pAdapterInfo->Address); // Print MAC address
    pAdapterInfo = pAdapterInfo->Next;    // Progress through
                                          // linked list
  }
  while(pAdapterInfo);                    // Terminate if last adapter
}
#endif



//
// MAC address must be 8 bytes and will receive padded out to 8 bytes macaddress
//
S16
get_mac_address(U8 *adapter, U8 *macaddr)
{

// BSD way of getting mac address.
#if defined(MACOSX)
    struct ifaddrs     *ifaphead;
    unsigned char      *if_mac;
    int                 found = 0;
    struct ifaddrs     *ifap;
    struct sockaddr_dl *sdl = NULL;

    if_mac = NULL;		// set to NULL so we can safely check to deallocate l8r.

    if (getifaddrs(&ifaphead) != 0)
    {   perror("get_if_name: getifaddrs() failed");
        exit(1);
    }

    for (ifap = ifaphead; ifap && !found; ifap = ifap->ifa_next)
    {   if ((ifap->ifa_addr->sa_family == AF_LINK))
        {   if (strlen(ifap->ifa_name) == strlen( (char *) adapter))
            {   if (strcmp(ifap->ifa_name, (char *) adapter) == 0)
                {   found = 1;
                    sdl = (struct sockaddr_dl *)ifap->ifa_addr;
                    if (sdl)
                    {   if_mac = malloc(sdl->sdl_alen);
                        memcpy(if_mac, LLADDR(sdl), sdl->sdl_alen);
                    }
                }
            }
        }
    }


    if (if_mac)
        free(if_mac);

    if (!found)
    {   fprintf (stderr,"Can't find interface %s.\n",adapter);
        if(ifaphead)
            freeifaddrs(ifaphead);
        exit(1);
    }

    if(ifaphead)
        freeifaddrs(ifaphead);


    macaddr[0]=macaddr[1]=0;
    macaddr[2] = if_mac[0];
    macaddr[3] = if_mac[1];
    macaddr[4] = if_mac[2];
    macaddr[5] = if_mac[3];
    macaddr[6] = if_mac[4];
    macaddr[7] = if_mac[5];

    return(0);
#endif

// outdated linux way of getting mac address.  Above technique should work on GNU Linux.
#ifdef LINUX
	struct ifreq	if_hwaddr;
	int				fd,i;	


	fd = socket(PF_INET, SOCK_DGRAM, IPPROTO_IP);

	strcpy(if_hwaddr.ifr_name,(char*)adapter);
	/* Get the vitals from the master interface. */
	if (ioctl(fd, SIOCGIFHWADDR, &if_hwaddr) < 0) 
	{
		close(fd);
		fprintf(stderr, "SIOCGIFHWADDR on %s failed: %s\n", if_hwaddr.ifr_name,
				strerror(errno));
		return -1;
	} 
	else 
	{	
		close(fd);
		/* copy over MAC address */
		macaddr[0]=macaddr[1]=0;
		for(i=0;i<6;i++)
			macaddr[i+2]=if_hwaddr.ifr_hwaddr.sa_data[i];
		return(0);
	}
#endif

#ifdef WIN321
 unsigned char MACData[6];
 int			i;

  UUID uuid;
  UuidCreateSequential( &uuid );    // Ask OS to create UUID

  for (i=2; i<8; i++)  // Bytes 2 through 7 inclusive
                           // are MAC address
    macaddr[i - 2] = uuid.Data4[i];
#endif

	return(0);
}




int yrand(int max)
{
	return(1 + (int) (max * (rand() / (RAND_MAX + 1.0))));
}

void yrand_seed(void)
{
	srand(second_count());
	yrand(10);
}

//
// threadswitch() - force a threadswitch
//
void
threadswitch()
{
#if defined(WIN32) || defined(WINCE)
			Sleep(0);
#endif

#if defined(MACOSX)
			usleep(100);
			sched_yield();
#endif

#if defined(LINUX)
			sched_yield();

#endif
#if defined(__ECOS)
			cyg_thread_delay(1);		
#endif
}

//
// Platform ag
//
void ysleep_seconds(U16 duration)
{
#if defined(WIN32) || defined(WINCE)
			Sleep(duration*1000);  
#endif

#if defined(LINUX) || defined (MACOSX)
			sleep(duration);
#endif
#if defined(__ECOS)
			cyg_thread_delay(duration*1000);		
#endif
}



/*
	NetConnect() Connect a TCP socket with timeout

	pcServer		:	String of name or IP address of host to connect to
	usPort			:	Port to connect to
	iMillSecTimeout	:	Timeout in MS to try connect
	iOut_fd			:	a socket handle if creation was good

	Not that the socket is left in non-blocking mode when returned

	communication with IPCam by HTTP
	Return: -1, network error
			0, success
*/
int NetConnect1(const char *pcServer, unsigned short usPort, int iMillSecTimeout, int *iOut_fd)
{
	int fd;
	//int iFl;
	struct timeval tv;
	fd_set fdsRead;
	fd_set fdsWrite;

	struct hostent* sHostent = NULL;
//	struct hostent* sHost = NULL;
//	unsigned long ulAddr;
	struct sockaddr_in sin;
	char **ppc;

	if (iOut_fd == NULL || pcServer == NULL || iMillSecTimeout <= 0)
	{
		DEBUG3("YOICS_CONFIG:NetConnect1:Illegal parameters while call NetConnect!\n");
		return -1;
	}
	*iOut_fd = -1;

	if ((sHostent = gethostbyname(pcServer)) == NULL)
	{
		DEBUG3("YOICS_CONFIG:NetConnect1:Failed to resolve host %s!\n",pcServer);
		return -1;
	}

	tv.tv_sec = iMillSecTimeout / 1000;
	tv.tv_usec = (iMillSecTimeout - 1000 * tv.tv_sec) * 1000;

	for (ppc=sHostent->h_addr_list; *ppc; ppc++)
	{

		/* create socket */
		fd = socket(PF_INET,SOCK_STREAM,0);
		if (fd == INVALID_SOCKET)
		{
			DEBUG3("YOICS_CONFIG:NetConnect:Can not create socket!\n");
			return -1;
		}

		set_sock_nonblock(fd);
		/* set socket to O_NDELAY */
/*		iFl = fcntl(fd, F_GETFL, 0);
		if (fcntl(fd, F_SETFL, iFl | O_NDELAY) != 0)
		{
			fprintf(stderr, "YOICS_CONFIG:NetConnect:Can not set socket fd to O_NDELAY mode.\n");
			close(fd);
			return -1;
		}
*/
		FD_ZERO(&fdsRead);
		FD_SET(fd, &fdsRead);
		FD_ZERO(&fdsWrite);
		FD_SET(fd, &fdsWrite);

		/* Connect to server */
		sin.sin_family = AF_INET;
		sin.sin_port = htons(usPort);	//smtp port number
		sin.sin_addr.s_addr = *(unsigned long*)*ppc;

		DEBUG3("resolved %s to %s\n",pcServer,inet_ntoa(sin.sin_addr));

		connect(fd,(struct sockaddr *)&sin,sizeof(sin));

		if (select(fd+1, &fdsRead, &fdsWrite, NULL, &tv) > 0)
		{
			if (FD_ISSET(fd, &fdsWrite) || FD_ISSET(fd, &fdsRead))
			{
				int iErrorCode;
				socklen_t iErrorCodeLen = sizeof(iErrorCode);
				if (getsockopt(fd, SOL_SOCKET, SO_ERROR, (char *)&iErrorCode, &iErrorCodeLen) == 0)
				{
					if (iErrorCode == 0)
					{
						*iOut_fd = fd;
						// Turn nonblock off
						//set_sock_nonblock(fd);
						//fcntl(fd, F_SETFL, iFl);
						return 0;
					}
				}
			}
		}

		DEBUG3("NetConnect1:Can not connect %s:%d in %d millsecond.\n", pcServer,usPort, iMillSecTimeout);
		close(fd);
		DEBUG3("closefd\n");
	}

	return -1;
}

/*! \fn S16 test_udp_bind_sock(port)  - test if we can bind this socket
    \brief 

    \param 
	\return -1 no bind,  else port number of bound port
*/
int
test_udp_bind(U16 port)
{
	SOCKET				new_soc;
	int					ret,len;
	struct sockaddr_in	client;		/* Information about the client */
    //int                 s,ttl;
    
	// bind a UDP port
	/* Open a datagram socket */
	new_soc = socket(AF_INET, SOCK_DGRAM, 0);

	if (new_soc == INVALID_SOCKET)
	{
		DEBUG1("Could not create socket.\n");
		return(-1);
	}

	// clear out socket structure
	memset((void *)&client, '\0', sizeof(struct sockaddr_in));
    
	client.sin_family = AF_INET;					// host byte order
	client.sin_port =  htons(port);					// Try to use this port  htons? +++
	/* Set server address */

	client.sin_addr.s_addr= INADDR_ANY;				// Check all IP

	ret=bind(new_soc, (struct sockaddr *)&client, sizeof(struct sockaddr_in));
	
	if(-1==ret)
	{
		// can't use this keep ret=-1
		//ret=1;
	}
	else 
	{
		// Get port!
		len=sizeof(client);
		ret=getsockname(new_soc,(struct sockaddr *)&client,(socklen_t *) &len);
		if(-1!=ret)
		{
			ret=htons(client.sin_port);
		}
	}
	// always close the socket 
	closesocket(new_soc);
	return(ret);
}

/*! \fn S16 test_udp_bind_sock(port)  - test if we can bind this socket
    \brief 

    \param 
	\return -1 no bind,  else port number of bound port
*/
SOCKET
udp_bind(U16 port)
{
	SOCKET				new_soc;
	int					ret,len;
	struct sockaddr_in	client;		/* Information about the client */
    //int                 s,ttl;
    
	// bind a UDP port
	/* Open a datagram socket */
	new_soc = socket(AF_INET, SOCK_DGRAM, 0);

	if (new_soc == INVALID_SOCKET)
	{
		DEBUG1("Could not create socket.\n");
		return(-1);
	}

	// clear out socket structure
	memset((void *)&client, '\0', sizeof(struct sockaddr_in));
    
	client.sin_family = AF_INET;					// host byte order
	client.sin_port =  htons(port);					// Try to use this port  htons? +++
	/* Set server address */

	client.sin_addr.s_addr= INADDR_ANY;				// Check all IP

	ret=bind(new_soc, (struct sockaddr *)&client, sizeof(struct sockaddr_in));
	
	if(-1==ret)
	{
		// can't use this keep ret=-1
		//ret=1;
	}
	else 
	{
		// Get port!
		len=sizeof(client);
		ret=getsockname(new_soc,(struct sockaddr *)&client,(socklen_t *) &len);
		if(-1!=ret)
		{
			ret=htons(client.sin_port);
		}
	}
	// always close the socket 
	return(new_soc);
}

#if defined(WIN32)

#if 0

#define MAX_KEY_LENGTH 128
#define MAX_VALUE_LENGTH 128

int getDefaultGateway(IPADDR *addr)
{
	HKEY networkCardsKey;
	HKEY networkCardKey;
	HKEY interfacesKey;
	HKEY interfaceKey;
	DWORD i = 0;
	DWORD numSubKeys = 0;
	TCHAR keyName[MAX_KEY_LENGTH];
	DWORD keyNameLength = MAX_KEY_LENGTH;
	TCHAR keyValue[MAX_VALUE_LENGTH];
	DWORD keyValueLength = MAX_VALUE_LENGTH;
	DWORD keyValueType = REG_SZ;
	TCHAR localIPValue[MAX_VALUE_LENGTH];
	TCHAR gatewayValue[MAX_VALUE_LENGTH];
	DWORD gatewayValueLength = MAX_VALUE_LENGTH;
	DWORD gatewayValueType = REG_MULTI_SZ;
	int done = 0;
	
	char networkCardsPath[] = "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\NetworkCards";
	char interfacesPath[] = "SYSTEM\\CurrentControlSet\\Services\\Tcpip\\Parameters\\Interfaces";
	
	// The windows registry lists its primary network devices in the following location:
	// HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows NT\CurrentVersion\NetworkCards
	// 
	// Each network device has its own subfolder, named with an index, with various properties:
	// -NetworkCards
	//   -5
	//     -Description = Broadcom 802.11n Network Adapter
	//     -ServiceName = {E35A72F8-5065-4097-8DFE-C7790774EE4D}
	//   -8
	//     -Description = Marvell Yukon 88E8058 PCI-E Gigabit Ethernet Controller
	//     -ServiceName = {86226414-5545-4335-A9D1-5BD7120119AD}
	// 
	// The above service name is the name of a subfolder within:
	// HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Services\Tcpip\Parameters\Interfaces
	// 
	// There may be more subfolders in this interfaces path than listed in the network cards path above:
	// -Interfaces
	//   -{3a539854-6a70-11db-887c-806e6f6e6963}
	//     -DhcpIPAddress = 0.0.0.0
	//     -[more]
	//   -{E35A72F8-5065-4097-8DFE-C7790774EE4D}
	//     -DhcpIPAddress = 10.0.1.4
	//     -DhcpDefaultGateway = 10.0.1.1
	//     -[more]
	//   -{86226414-5545-4335-A9D1-5BD7120119AD}
	//     -DhcpIpAddress = 10.0.1.5
	//     -DhcpDefaultGateay = 10.0.1.1
	//     -[more]
	// 
	// In order to extract this information, we enumerate each network card, and extract the ServiceName value.
	// This is then used to open the interface subfolder, and attempt to extract a DhcpDefaultGateway value.
	// Once one is found, we're done.
	// 
	// It may be possible to simply enumerate the interface folders until we find one with a DhcpDefaultGateway value.
	// However, the technique used is the technique most cited on the web, and we assume it to be more correct.
	
	if(ERROR_SUCCESS != RegOpenKeyEx(HKEY_LOCAL_MACHINE, // Open registry key or predifined key 
	                                 networkCardsPath,   // Name of registry subkey to open
	                                 0,                  // Reserved - must be zero
	                                 KEY_READ,           // Mask - desired access rights
	                                 &networkCardsKey))  // Pointer to output key
	{
		// Unable to open network cards keys
		return -1;
	}
	
	if(ERROR_SUCCESS != RegOpenKeyEx(HKEY_LOCAL_MACHINE, // Open registry key or predefined key
	                                 interfacesPath,     // Name of registry subkey to open
	                                 0,                  // Reserved - must be zero
	                                 KEY_READ,           // Mask - desired access rights
	                                 &interfacesKey))    // Pointer to output key
	{
		// Unable to open interfaces key
		RegCloseKey(networkCardsKey);
		return -1;
	}
	
	// Figure out how many subfolders are within the NetworkCards folder
	RegQueryInfoKey(networkCardsKey, NULL, NULL, NULL, &numSubKeys, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
	
	//printf( "Number of subkeys: %u\n", (unsigned int)numSubKeys);
	
	// Enumrate through each subfolder within the NetworkCards folder
	for(i = 0; i < numSubKeys && !done; i++)
	{
		keyNameLength = MAX_KEY_LENGTH;
		if(ERROR_SUCCESS == RegEnumKeyEx(networkCardsKey, // Open registry key
		                                 i,               // Index of subkey to retrieve
		                                 keyName,         // Buffer that receives the name of the subkey
		                                 &keyNameLength,  // Variable that receives the size of the above buffer
		                                 NULL,            // Reserved - must be NULL
		                                 NULL,            // Buffer that receives the class string
		                                 NULL,            // Variable that receives the size of the above buffer
		                                 NULL))           // Variable that receives the last write time of subkey
		{
			if(RegOpenKeyEx(networkCardsKey,  keyName, 0, KEY_READ, &networkCardKey) == ERROR_SUCCESS)
			{
				keyValueLength = MAX_VALUE_LENGTH;
				if(ERROR_SUCCESS == RegQueryValueEx(networkCardKey,   // Open registry key
				                                    "ServiceName",    // Name of key to query
				                                    NULL,             // Reserved - must be NULL
				                                    &keyValueType,    // Receives value type
				                                    keyValue,         // Receives value
				                                    &keyValueLength)) // Receives value length in bytes
				{
					yprintf("keyValue: %s\n", keyValue);
					
					if(RegOpenKeyEx(interfacesKey, keyValue, 0, KEY_READ, &interfaceKey) == ERROR_SUCCESS)
					{
						// Query for DHCP gateway first
						gatewayValueLength = MAX_VALUE_LENGTH;
						if(ERROR_SUCCESS == RegQueryValueEx(interfaceKey,         // Open registry key
						                                    "DhcpDefaultGateway", // Name of key to query
						                                    NULL,                 // Reserved - must be NULL
						                                    &gatewayValueType,    // Receives value type
						                                    gatewayValue,         // Receives value
						                                    &gatewayValueLength)) // Receives value length in bytes
						{
							// Check to make sure it's a string
							if(gatewayValueType == REG_MULTI_SZ || gatewayValueType == REG_SZ)
							{
								yprintf("DHCP gatewayValue: %s\n", gatewayValue);
								done = 1;
								//
								// get the local IP
								//
								if(ERROR_SUCCESS == RegQueryValueEx(interfaceKey,         // Open registry key
						                                    "DhcpIPAddress",	// Name of key to query
						                                    NULL,                 // Reserved - must be NULL
						                                    &gatewayValueType,    // Receives value type
						                                    localIPValue,         // Receives value
						                                    &gatewayValueLength)) // Receives value length in bytes
								{
									if(gatewayValueType == REG_MULTI_SZ || gatewayValueType == REG_SZ)
									{
										yprintf("DHCP ip is: %s\n", localIPValue);
										//done = 1;
									}
								}
							}
						}
						else if(ERROR_SUCCESS == RegQueryValueEx(interfaceKey,         // Open registry key
						                                    "DefaultGateway",		// Name of key to query
						                                    NULL,                 // Reserved - must be NULL
						                                    &gatewayValueType,    // Receives value type
						                                    gatewayValue,         // Receives value
						                                    &gatewayValueLength)) // Receives value length in bytes
						{
							// Check to make sure it's a string
							if((gatewayValueType == REG_MULTI_SZ || gatewayValueType == REG_SZ) && (gatewayValueLength>1))
							{
								// Get the local IP also
								yprintf("Static gatewayValue: %s\n", gatewayValue);
								done = 1;
								//
								// get the local IP
								//
								if(ERROR_SUCCESS == RegQueryValueEx(interfaceKey,         // Open registry key
						                                    "DhcpIPAddress",	// Name of key to query
						                                    NULL,                 // Reserved - must be NULL
						                                    &gatewayValueType,    // Receives value type
						                                    localIPValue,         // Receives value
						                                    &gatewayValueLength)) // Receives value length in bytes
								{
									if(gatewayValueType == REG_MULTI_SZ || gatewayValueType == REG_SZ)
									{
										yprintf("DHCP ip is: %s\n", localIPValue);
										//done = 1;
									}
								}
							}
						}						
						RegCloseKey(interfaceKey);
					}
				}
				RegCloseKey(networkCardKey);
			}
		}
	}
	
	RegCloseKey(interfacesKey);
	RegCloseKey(networkCardsKey);
	
	if(done)
	{
		addr->ip32 = inet_addr(gatewayValue);
		return 0;
	}
	
	return -1;
}

#endif /* if 0 */

#endif /* #ifdef USE_WIN32_CODE */




