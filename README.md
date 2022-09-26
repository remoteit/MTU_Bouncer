# UDP MTU Checker
Simple server to test the incoming MTU of a connection.   In most cases it is easy to check the outbound MTU of a network path, but not the incomming path.   This server allows you to connect to a UDP socket with netcat or similar to determine the incoming MTU of a network path.


# Usage
```
usage: ./udp_mtu_server [-h] [-v(erbose)] [-d][pid file] [-l listen_tcp_port]
         -h this output.
         -v console debug output.
         -d runs the program as a daemon with optional pid file.
         -l Listen port (defaults to 9999)
```

# Examples

Run the udp_mtu_server on a server:
```
./udp_mtu_server
```

Connect to it with netcat
```
nc -u <serverIP> 9999
```

Now you can ask the server to send you a UDP packet with a payload of size <user specified>
```
nc -u <serverIP> 9999
10<ret>
Must be between 16 and 1472
17<ret>
17 (45 MTU)
...
1472<ret>
1472 (1500 MTU)
..............................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................
```

# Building
Using the makefile in the src directory should built on most linux/unix platforms.   There is a win32 project for building on windows consol app also.

#License
MIT




