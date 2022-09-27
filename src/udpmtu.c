/*!						
-----------------------------------------------------------------------------
udpmtu.c

an RX MTU checker

Use NC to send a packet to the server, the server will parse the first line 
of data looking for a number, if the number is between 1 and 1600 will create
a UDP frame of that size and send it back.

The frame will start with UDP MTU xxxx -- then random ASCII to padd out frame

-----------------------------------------------------------------------------
*/
#include "udpmtu.h"
#include "config.h"
#include "arch.h"
#include "yselect.h"
#include "daemonize.h"
//#include "file_config.h"
#include "debug.h"


#if defined(WIN32)
#include "wingetopt.h"
#endif

#define BUFFER_SIZE 1024

#define on_error(...) { fprintf(stderr, __VA_ARGS__); fflush(stderr); exit(1); }

int global_flag=0;
volatile int go=0;






#if defined(WIN32)
BOOL WINAPI ConsoleHandler(DWORD CEvent)
{
    switch(CEvent)
    {
    case CTRL_C_EVENT:
        yprintf("CTRL+C received!\n");
        break;
    case CTRL_BREAK_EVENT:
        yprintf("CTRL+BREAK received!\n");
        break;
    case CTRL_CLOSE_EVENT:
        yprintf("program is being closed received!\n");
        break;
    case CTRL_SHUTDOWN_EVENT:
        yprintf("machine is being shutdown!\n");
        break;
    case CTRL_LOGOFF_EVENT:
        return FALSE;
    }
    go=0;

    return TRUE;
}
#else
void
termination_handler (int signum)
{
    go=0;

    if((SIGFPE==signum) || (SIGSEGV==signum) || (11==signum))
    {
        yprintf("Echo Server Terminated from Signal %d\n",signum);
        if(global_flag&GF_DAEMON) syslog(LOG_ERR,"Echo Server Terminated from Signal 11\n");

        exit(11);
    }
}
#endif

void
startup_banner()
{
    //------------------------------------------------------------------
    // Print Banner
    //------------------------------------------------------------------
    printf("UDP MTU Server Server built " __DATE__ " at " __TIME__ "\n");
    printf("   Version " VERSION " - (c)2022 Remot3.it\n");
    fflush(stdout);
}


void usage(int argc, char **argv)
{
  startup_banner();

  printf("usage: %s [-h] [-v(erbose)] [-d][pid file] [-f config_file] [-l listen_tcp_port] [-u user_2_run_as] [-c control_port] \n",argv[0]);
  printf("\t -h this output.\n");
  printf("\t -v console debug output.\n");
  printf("\t -d runs the program as a daemon with optional pid file.\n");
  //printf("\t -f specify a config file.\n");
  printf("\t -l Listen port (defaults to 9999)\n");
  exit(2);
}

int
set_sock_recv_timeout(SOCKET lsock, int secs)
{
    int ret = -1;

#if defined(WIN32)
    int timeout = secs * 100;
    ret = setsockopt(lsock, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(timeout));

#else
    struct timeval tv;
    tv.tv_sec = secs;
    tv.tv_usec = 0;
    if ((ret = setsockopt(lsock, SOL_SOCKET, SO_RCVTIMEO, (char*)&tv, sizeof(tv))) < 0)
    {

    }
#endif
    return(ret);
}


// do not fragment = fragment =1
int
set_sock_doNotFragment(SOCKET lsock, int fragment)
{
    int ret = 0, i = 0;

    if (fragment)
        i = 1;
#if defined(WIN32)
    ret = setsockopt(lsock, IPPROTO_IP, IP_DONTFRAGMENT, (char*)&i, sizeof(int));
#elif defined(LINUX)
    if (fragment)
        i = IP_PMTUDISC_DO;
    else
        i = IP_PMTUDISC_DONT;
    ret = setsockopt(lsock, IPPROTO_IP, IP_MTU_DISCOVER, (const void*)&i, sizeof(int));

#elif defined(MACOSX) || defined(IOS)
    // do nothings mac does not support this	
#pragma message("OSX does not support IP_DONTFRAGMENT")
#elif	defined(BSD_TYPE)
    ret = setsockopt(lsock, IPPROTO_IP, IP_DONTFRAGMENT, &i, sizeof(int));
#endif
    DEBUG1("do not fragment returened %d\n", ret);
    return(ret);
}

//
// return -1 on error or socket
//
SOCKET
udp_listener(U16 port, IPADDR ip)
{
	int                 ret;
	SOCKET              new_soc;
	struct sockaddr_in  client;

	new_soc = socket(AF_INET, SOCK_DGRAM, 0);

	if (new_soc == INVALID_SOCKET)
	{
		return(new_soc);
	}
	// clear out socket structure
	memset((void*)&client, '\0', sizeof(struct sockaddr_in));
	client.sin_family = AF_INET;
	client.sin_port = htons(port);             // Use the next port
	/* Set server address */
	client.sin_addr.s_addr = ip.ip32; // any

	ret = bind(new_soc, (struct sockaddr*)&client, sizeof(struct sockaddr_in));

	if (SOCKET_ERROR == ret)
	{
		perror("error on bind\n");
		printf("error on bind %d\n", get_last_error());
		closesocket(new_soc);
		return(-1);
	}
	//
	// We are ready to go!
	//
	return(new_soc);
}

int main(int argc, char **argv) {
  int c,optval = 1;
  int size,ret,len,dfrag;
  int slen;
  char message[4096];
  struct sockaddr_in client,server;
  MTU	mtu;

 
  //
  // Banner
  startup_banner();
  //
  // Startup Network
  network_init();
  //Y_Init_Select();												// were going to use the select engine
  //
  // Set defaults
  memset(&mtu, 0, sizeof(MTU));
  mtu.listen_port	= MTU_DEFALT_LISTEN_PORT;					// service defaults to port 9999
  mtu.bind_IP.ip32 = 0;


  //------------------------------------------------------------------
  // Initialize error handling and signals
  //------------------------------------------------------------------
#if defined(WIN32) 
if (SetConsoleCtrlHandler((PHANDLER_ROUTINE)ConsoleHandler,TRUE)==FALSE)
{
  // unable to install handler... 
  // display message to the user
  yprintf("Error - Unable to install control handler!\n");
  exit(0);
}
#else
#if !defined(WINCE)
  //  SetConsoleCtrlHandle(termination_handler,TRUE);

  if (signal (SIGINT, termination_handler) == SIG_IGN)
      signal (SIGINT, SIG_IGN);
  if (signal (SIGTERM, termination_handler) == SIG_IGN)
      signal (SIGTERM, SIG_IGN);
  if (signal (SIGILL , termination_handler) == SIG_IGN)
      signal (SIGILL , SIG_IGN);
  if (signal (SIGFPE , termination_handler) == SIG_IGN)
      signal (SIGFPE , SIG_IGN);
  //if (signal (SIGSEGV , termination_handler) == SIG_IGN)
  //  signal (SIGSEGV , SIG_IGN);
#if defined(LINUX) || defined(MACOSX) || defined(IOS)
  if (signal (SIGXCPU , termination_handler) == SIG_IGN)
      signal (SIGXCPU , SIG_IGN);
  if (signal (SIGXFSZ , termination_handler) == SIG_IGN)
      signal (SIGXFSZ , SIG_IGN);
#endif
#endif
#endif

    //
    // Override with command line, after config file is loaded
    //
    while ((c = getopt(argc, argv, "p:d:vh")) != EOF)
    {
    	switch (c)
        {
            case 0:
                break;
            case 'd':
                // Startup as daemon with pid file
                if(0!=optarg)
                {
                    strncpy(mtu.pidfile,optarg,MAX_PATH-1);
                    printf("Starting up as daemon with pidfile %s\n",mtu.pidfile);
                }
                else
                {
                    printf("Starting up as daemon with no pidfile.\n");
                    mtu.pidfile[0]=0;
                }
                global_flag|=GF_DAEMON;
            
                break;
            case 'p':
                // Override Port
                mtu.listen_port=atoi(optarg);
                break;
            case 'v':
                mtu.verbose++;
                break;
            case 'h':
                usage (argc,argv);
                break;
            default:
                usage (argc,argv);
                break;
   		}
	}

    // 
    // Socket must be bound before Daemonize because we may drop privilages here
    //
	mtu.listen_soc=udp_listener(mtu.listen_port, mtu.bind_IP);

	if (mtu.listen_soc < 0) {
		perror("bind failed");
		exit(1);
	}

#if !defined(WIN32)
    //
    // Should Daemonize here,  
    //
    if(global_flag&GF_DAEMON)
    {
        // Daemonize this
        daemonize(mtu.pidfile,0,0,0,0,0,0);
        // Setup logging
        openlog("mtu_server",LOG_PID|LOG_CONS,LOG_USER);
        syslog(LOG_INFO,"UDP MTU Server built "__DATE__ " at " __TIME__ "\n");
        syslog(LOG_INFO,"   Version " VERSION " - (c)2022 mycal.net\n");
        syslog(LOG_INFO,"Starting up as daemon\n");
    }
#endif




  // add the listen socket to select for rx
  //Y_Set_Select_rx(mtu.listen_soc);

  printf("UDP MTU started up on port %d\n",mtu.listen_port);

  set_sock_recv_timeout(mtu.listen_soc, 1);

  // This code only accepts one socket at a time
  go = 1;
  while (go)
  {
      // by default DF bit set
      dfrag = 1;

       // receive here
      memset(&client, '\0', sizeof(struct sockaddr));
      slen = sizeof(struct sockaddr_in);
      memset(message, '\0', 4096);
      ret = (int)recvfrom(mtu.listen_soc, (char*)message, 1024, 0, (struct sockaddr*)&client, (socklen_t*)&slen);

      if (ret > 0)
      {
          if (strchr(message,'f') || strchr(message,'F'))
          {
              dfrag = 0;
              if (mtu.verbose) printf("DF Cleared\n");
          }
          // parse number support F# for just #
          size = atoi(message);
          //if ((ret >= 16) && (ret <= 1472))
          if ((size >= 0) && (size <= 2000))
          {
              if (size < 17)
                  size = 17;
              memset(message, 46, 4096);
              sprintf(message, "%d (%d MTU DF=%d)\n", size, size + 28,dfrag);
              //len = strlen(message);
              len = size;              
              message[size-1] = '\n';
          }
          else
          {
              //sprintf(message, "Must be between 16 and 1472\n");
              sprintf(message, "Must be between 20 and 2000 (add f on end to allow fragmentation)\n");
              len = strlen(message);
          }
          //
          // Send message always non encrypted
          //
          memset(&server, '\0', (sizeof(struct sockaddr)));
          server.sin_family = AF_INET;
          server.sin_addr.s_addr = client.sin_addr.s_addr;
          server.sin_port = client.sin_port;
          //
          set_sock_doNotFragment(mtu.listen_soc, dfrag);
          //
          ret = sendto(mtu.listen_soc, (char*)message, len, 0, (struct sockaddr*)&server, sizeof(struct sockaddr));
          if (ret < 0)
          {
              if (mtu.verbose) printf("Socket error %s, code %d\n", get_last_error_str(), get_last_error());
              sprintf(message, "On size %d (%d MTU dfrag=%d) sender failed to send with error %s code %d\n", size, size + 28, dfrag, get_last_error_str(), get_last_error());
              len = strlen(message);
              memset(&server, '\0', (sizeof(struct sockaddr)));
              server.sin_family = AF_INET;
              server.sin_addr.s_addr = client.sin_addr.s_addr;
              server.sin_port = client.sin_port;
              sendto(mtu.listen_soc, (char*)message, len, 0, (struct sockaddr*)&server, sizeof(struct sockaddr));
          }
      }
      else
          if(mtu.verbose>1) printf(".");


  }
}


