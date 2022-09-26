#
# echo server Build File
#

EXEC	=  udpmtu 

all: $(EXEC)

# Which compiler
CC = gcc

STRIP=strip
BASEDIR=

LIBDIR=$(BASEDIR)/lib/
INCDIR=$(BASEDIR)/inc/


BIN_TARGET=../bin_targets

# Where are include files kept
INCLUDE = 


# Use this for Linux client software
CDEFINE = -DLINUX 

# Options for development
CFLAGS		= $(INCLUDE) $(CDEFINE) -g -Wall -Os -fPIE 
LDFLAGS		=
LINKFLAGS	= -o

# add libs for lib crypto TBD
#LIBS		=  -lcrypto -lssl

# Options for release
# CFLAGS = -O -Wall -ansi


$(shell [ -d "$(BIN_TARGET)" ] || mkdir -p $(BIN_TARGET))

#Dependancies for all
DEPENDALL	=  

#Objects put all your .c files here but with .o extentions
OBJ	= udpmtu.o daemonize.o arch.o


all: $(EXEC)
	mv $< $(BIN_TARGET)

$(EXEC): $(OBJ) $(DEPENDALL) 
	$(CC) $(LINKFLAGS) $(EXEC) $(OBJ) $(LIBS)
	$(STRIP) $(EXEC)

clean:
	-rm *.o $(MYLIB)
	 rm -f *.map
	 rm -f $(EXEC)


