SHELL = /bin/sh

OOQP=../../..
OOQPINCLUDEDIR=$(OOQP)/include
OOQPLIBDIR=$(OOQP)/lib


CXX      = g++

CXXFLAGS =-O -Wall
CPPFLAGS =-I$(OOQPINCLUDEDIR)
LDFLAGS  =-L$(OOQPLIBDIR)
BLAS     = #$(OOQP)/extras/blas/libblas.a
MA27LIB  = $(OOQP)/extras/MA27/libMA27.a
FLIBS    = #-lg2c
LIBS     = -looqpgensparse -looqpsparse  -looqpgondzio -looqpbase \
                $(BLAS) -lblas $(MA27LIB) $(FLIBS) -L/usr/lib -L/usr/local/lib

cxx_example.exe : example.o
	$(CXX) $(CPPFLAGS) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

clean:
	rm -f *.o

veryclean: clean
	rm -f *.exe

distclean: veryclean

.C.o:
	$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) -o $@ $<
