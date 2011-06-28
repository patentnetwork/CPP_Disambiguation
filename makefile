#CXX=icpc
#CC=icc
#CFLAGS=-O3 -g -xsse4.1 -ipo -static-intel -Wall

CXX=g++-4.4
CC=gcc
CFLAGS=-Os -g -Wall -I/usr/local/include -L/usr/local/lib
OOQPINCLUDEDIR=/usr/local/include/ooqp
ILOGINSTALLDIR=/home/ysun/ILOG/CPLEX_Studio_AcademicResearch122

CPLEXINCLUDE=$(ILOGINSTALLDIR)/cplex/include
CONCERTINCLUDE=$(ILOGINSTALLDIR)/concert/include
CPLEXLIB=$(ILOGINSTALLDIR)/cplex/lib/x86_sles10_4.1/static_pic
CONCERTLIB=$(ILOGINSTALLDIR)/concert/lib/x86_sles10_4.1/static_pic

all:exedisambig

exedisambig: Disambigmain.o DisambigDefs.o DisambigRatios.o DisambigEngine.o DisambigFileOper.o strcmp95.o DisambigComp.o DisambigTraining.o Threading.o DisambigCluster.o DisambigRatioSmoothing.o DisambigNewCluster.o Array.o QuadProg++.o DisambigCustomizedDefs.o DisambigPostProcess.o DisambigUtilities.o
	$(CXX) -o $@ $? $(CFLAGS) -lsqlite3 -looqpgensparse -looqpsparse -looqpgondzio -looqpbase -lblas -lMA27 -L$(CPLEXLIB) -lilocplex -lcplex -L$(CONCERTLIB) -lconcert -lm -lpthread -pg

Disambigmain.o: Disambigmain.cpp
	$(CXX) -c $? $(CFLAGS)

DisambigDefs.o:	DisambigDefs.cpp DisambigDefs.h
	$(CXX) -c $? $(CFLAGS)

DisambigRatios.o: DisambigRatios.cpp DisambigRatios.h
	$(CXX) -c $? $(CFLAGS)

DisambigEngine.o: DisambigEngine.cpp DisambigEngine.h
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
	$(CXX) -c $? $(CFLAGS) -I$(OOQPINCLUDEDIR) -I$(CPLEXINCLUDE) -I$(CONCERTINCLUDE) -DIL_STD -DNDEBUG -w

DisambigNewCluster.o: DisambigNewCluster.h DisambigNewCluster.cpp
	$(CXX) -c $? $(CFLAGS)

Array.o: Array.h Array.cpp
	$(CXX) -c $? $(CFLAGS)

QuadProg++.o: QuadProg++.h QuadProg++.cpp
	$(CXX) -c $? $(CFLAGS)

DisambigCustomizedDefs.o: DisambigCustomizedDefs.h DisambigCustomizedDefs.cpp
	$(CXX) -c $? $(CFLAGS)

DisambigPostProcess.o: DisambigPostProcess.h DisambigPostProcess.cpp
	$(CXX) -c $? $(CFLAGS)

DisambigUtilities.o: DisambigUtilities.h DisambigUtilities.cpp
	$(CXX) -c $? $(CFLAGS)

clean:
	rm *.o *.gch


