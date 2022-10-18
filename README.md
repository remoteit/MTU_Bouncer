# MTU Bouncer
Simple server to test the incoming MTU of a connection using UDP.   Connect the server with netcat or similar and request a UDP packet of data size specified to be returned with the DF flag set.   You may also specify clearing the DF flag (if supported in your OS).

It should be noted that MACOSX does not support setting the DF flag.

We have also used this software to find endpoints that do not allow fragmented packets.   Ask for a larger than MTU packet and ask for the fragment flag to be set on, if you do not get a response back then somewhere in your path may be dropping fragmented packets.


# Usage
```
usage: ./mtu_bouncer [-h] [-v(erbose)] [-d][pid file] [-l listen_tcp_port]
         -h this output.
         -v console debug output.
         -d runs the program as a daemon with optional pid file.
         -l Listen port (defaults to 9999)
```

# Examples

Run the udp_mtu_server on a server:
```
./mtu_bouncer
```

Connect to it with netcat
```
nc -u <serverIP> 9999
```

Now you can ask the server to send you a UDP packet with a payload of size <user specified>
```
ops@ops-mac-mini ~ % nc -u <serverIP> 9999
10
Must be between 20 and 4000 (add f on end to allow fragmentation)
100
100 (128 MTU DF=1)
..............................................................................
1472
1472 (1500 MTU DF=1)
.........................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................
1473
On size 1473 (1501 MTU dfrag=1) sender failed to send with error Message too long code 90
1473f
1473 (1501 MTU DF=0)
.........................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................

```

# Building
Using the makefile in the src directory should built on most linux/unix platforms.   There is a win32 project for building on windows consol app also.

#License
MIT




