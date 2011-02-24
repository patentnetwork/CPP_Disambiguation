#CXX=icpc
#CC=icc
#CFLAGS=-O3 -g -xsse4.1 -ipo -static-intel -Wall

CXX=g++
CC=gcc
CFLAGS=-Os -g -Wall -I/usr/local/include -L/usr/local/lib
OOQPINCLUDEDIR=/usr/local/include/ooqp


all:exedisambig

exedisambig: Disambigmain.o DisambigDefs.o DisambigRatios.o DisambigEngine.o DisambigFileOper.o sqlite3op.o strcmp95.o DisambigComp.o DisambigTraining.o Threading.o DisambigCluster.o DisambigRatioSmoothing.o DisambigNewCluster.o Array.o QuadProg++.o
	$(CXX) -o $@ $? $(CFLAGS) -lsqlite3 -looqpgensparse -looqpsparse -looqpgondzio -looqpbase -lblas -lMA27 -pg 

Disambigmain.o: Disambigmain.cpp
	$(CXX) -c $? $(CFLAGS)

DisambigDefs.o:	DisambigDefs.cpp DisambigDefs.h
	$(CXX) -c $? $(CFLAGS)

DisambigRatios.o: DisambigRatios.cpp DisambigRatios.h
	$(CXX) -c $? $(CFLAGS)

DisambigEngine.o: DisambigEngine.cpp DisambigEngine.h
	$(CXX) -c $? $(CFLAGS)

sqlite3op.o: sqlite3op.cpp sqlite3op.h
	$(CXX) -c $? $(CFLAGS)

DisambigFileOper.o: DisambigFileOper.cpp DisambigFileOper.h
	$(CXX) -c $? $(CFLAGS)	

DisambigComp.o: DisambigComp.cpp DisambigComp.h
	$(CXX) -c $? $(CFLAGS)

DisambigTraining.o: DisambigTraining.h DisambigTraining.cpp
	$(CXX) -c $? $(CFLAGS)

strcmp95.o: strcmp95.c strcmp95.h
	$(CC) -c $? $(CFLAGS)

Threading.o: Threading.cpp Threading.h
	$(CXX) -c $? $(CFLAGS)

DisambigCluster.o: DisambigCluster.cpp DisambigCluster.h
	$(CXX) -c $? $(CFLAGS)

DisambigRatioSmoothing.o: DisambigRatioSmoothing.cpp
	$(CXX) -c $? $(CFLAGS) -I$(OOQPINCLUDEDIR)

DisambigNewCluster.o: DisambigNewCluster.h DisambigNewCluster.cpp
	$(CXX) -c $? $(CFLAGS)

Array.o: Array.h Array.cpp
	$(CXX) -c $? $(CFLAGS)

QuadProg++.o: QuadProg++.h QuadProg++.cpp
	$(CXX) -c $? $(CFLAGS)

clean:
	rm *.o


