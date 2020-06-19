#---------------------------------------------------------------------------
#   Name  : Makefile
#   Author: Ken.Yuan
#   Brief : Just for test
#   Date  : 2009-03-03
#---------------------------------------------------------------------------

MAKEFILE=Makefile

CC      =gcc
CXX     =g++

CXXFLGS =-g -lpthread -lrt  -D_FILE_OFFSET_BITS=64 -std=c++11
CFLAGS  += -W -Wall -Wpointer-arith -pipe -static
CCFLAGS	+= $(CFLAGS) 
DCFLAGS	+= 

OCFLAGS	+= -O2 -g -Wno-deprecated  -D_FILE_OFFSET_BITS=64 -std=c++11
PIC	    =-fPIC

INCDIR  =-I./include  -I/data/tools/curl7700//include/
LIBDIR  = -L/data/tools/curl7700/lib/ -lcurl -lrt


BINDIR  =./bin
OBJDIR  =./obj

CPPSRCS =$(wildcard ./src/*.cc)
OBJS    =$(patsubst ./src/%.cc,$(OBJDIR)/%.o, $(CPPSRCS))


TARGET  =estest

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) -o $@ $(CXXFLGS) $(OCFLAGS) $(LIBDIR) $(OBJDIR)/*.o

$(OBJDIR)/%o:./src/%cc
	$(CXX) $(OCFLAGS) $(INCDIR) -c $< -o $@
#	@cp *.o $(OBJDIR)/

clean:
	@rm -f $(OBJDIR)/*.o
	@rm -f $(TARGET)
