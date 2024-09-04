# define some Makefile variables for the compiler and compiler flags
# to use Makefile variables later in the Makefile: $()
#
#  -g    adds debugging information to the executable file
#  -Wall turns on most, but not all, compiler warnings
#
# for C++ define  CC = g++
CC = gcc
CFLAGS  = -g -Wall
# typing 'make' will invoke the first target entry in the file 
# (in this case the default target entry)
# you can name this target entry anything, but "default" or "all"
# are the most commonly used names by convention
#
default: all

# To create the executable file all we need the object files
# serverM.o, serverL.o, serverH.o, serverS.o, and client.o:
#
all: serverM serverL serverH serverS client

# To create the object file serverM.o, we need the source file serverM.c:
#
serverM: serverM.o
	$(CC) $(CFLAGS) -o serverM serverM.o

# To create the object file serverM.o, we need the source file serverM.c:
#
serverL: serverL.o
	$(CC) $(CFLAGS) -o serverL serverL.o

# Similarly, create rules for serverH, serverS, and client.

# To create the object file serverM.o, we need the source file serverM.c:
#
serverH: serverH.o
	$(CC) $(CFLAGS) -o serverH serverH.o

# To create the object file serverM.o, we need the source file serverM.c:
#
serverS: serverS.o
	$(CC) $(CFLAGS) -o serverS serverS.o

# To create the object file serverM.o, we need the source file serverM.c:
#
client: client.o
	$(CC) $(CFLAGS) -o client client.o
# Similarly, create rules for serverL, serverH, serverS, and client.

# To create the object files, use the pattern rule:
%.o: %.c
	$(CC) $(CFLAGS) -c $<

# To start over from scratch, type 'make clean'.  This
# removes the executable files, as well as old .o object
# files and *~ backup files:
#
clean: 
	$(RM) serverM serverL serverH serverS client *.o *~
