CC=g++
CXX=g++
LD=g++

EXESRC=SampleClient.cpp
EXEOBJ=$(EXESRC:.cpp=.o)

INCS=-I.
CFLAGS = -Wall -std=c++11 -g $(INCS)
CXXFLAGS = -Wall -std=c++11 -g $(INCS)
LDFLAGS = -L. -lpthread -lMapReduceFramework

EXE = SampleClient
TARGETS = $(EXE)

TAR=tar
TARFLAGS=-cvf
TARNAME=sampleclient.tar
TARSRCS=$(EXESRC) Makefile

all: $(TARGETS)

$(TARGETS): $(EXEOBJ)
	$(LD) $(LDFLAGS) $(CXXFLAGS) $(EXEOBJ) -o $(EXE)

clean:
	$(RM) $(TARGETS) $(EXE) $(OBJ) $(EXEOBJ) *~ *core

depend:
	makedepend -- $(CFLAGS) -- $(SRC) $(LIBSRC)

tar:
	$(TAR) $(TARFLAGS) $(TARNAME) $(TARSRCS)
