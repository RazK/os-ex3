CC=g++
CXX=g++
LD=g++

EXESRC=atomicdemo.cpp
EXEOBJ=$(EXESRC:.cpp=.o)

INCS=-I.
CFLAGS = -Wall -std=c++11 -g $(INCS)
CXXFLAGS = -Wall -std=c++11 -g $(INCS)
LDFLAGS = -pthread

EXE = atomicdemo
TARGETS = $(EXE)

TAR=tar
TARFLAGS=-cvf
TARNAME=atomicdemo.tar
TARSRCS=$(EXESRC) Makefile README

all: $(TARGETS)

$(TARGETS): $(EXEOBJ)
	$(LD) $(LDFLAGS) $(CXXFLAGS) $(EXEOBJ) -o $(EXE)

clean:
	$(RM) $(TARGETS) $(EXE) $(OBJ) $(EXEOBJ) *~ *core

depend:
	makedepend -- $(CFLAGS) -- $(SRC) $(LIBSRC)

tar:
	$(TAR) $(TARFLAGS) $(TARNAME) $(TARSRCS)

