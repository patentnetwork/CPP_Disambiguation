all-depend: depend all

SHELL =    /bin/bash

CXX      = g++
F77      = gfortran
FC       = $(F77)
CXXFLAGS = -O
CC       = cc
CFLAGS   = -O
FFLAGS   = -O
INSTALL  = /usr/bin/install -c
CPPFLAGS =  $(INCLUDES) $(DEFS)
DEFS     = -DPACKAGE_NAME=\"\" -DPACKAGE_TARNAME=\"\" -DPACKAGE_VERSION=\"\" -DPACKAGE_STRING=\"\" -DPACKAGE_BUGREPORT=\"\" -DHAVE_EXPLICIT= -DSTDC_HEADERS=1 -DHAVE_GETRUSAGE=1
LDFLAGS  = 
FLIBS    =  -L/media/data/edwardspace/IntelCpl//tbb/ia32/cc4.1.0_libc2.4_kernel2.6.16.21/lib/../lib -L/usr/lib/gcc/i686-linux-gnu/4.4.5 -L/usr/lib/gcc/i686-linux-gnu/4.4.5/../../../../lib -L/lib/../lib -L/usr/lib/../lib -L/media/data/edwardspace/IntelCpl//lib/ia32 -L/media/data/edwardspace/IntelCpl//mkl/lib/32 -L/media/data/edwardspace/IntelCpl//tbb/ia32/cc4.1.0_libc2.4_kernel2.6.16.21/lib -L/usr/lib/gcc/i686-linux-gnu/4.4.5/../../.. -L/usr/lib/i686-linux-gnu -lgfortranbegin -lgfortran -lm
BLAS     = -lblas
LIBS     = 
RANLIB   = ranlib
MAKEDEPEND = makedepend

SUPERLU     = /sandbox/SuperLU
SUPERLULIB  = $(SUPERLU)/superluLinux-i386.a
OBLIOLIB    = /sandbox/oblio/lib/oblio.a
MA57LIB     = extras/MA57/libMA57.a
MA27LIB     = extras/MA27/libMA27.a

srcdir      = src
prefix      = /usr/local
includedir  = ${prefix}/include
libdir      = ${exec_prefix}/lib
bindir      = ${exec_prefix}/bin
datadir     = ${prefix}/share
docdir      = $(datadir)/doc/packages/OOQP
exec_prefix = ${prefix}
locallibdir = ./lib
localincludedir = ./include

SOURCES =
PROGRAMS =
INCLUDES :=

#include all modules (subdirectories directly below this one that have
#a makefile.inc.)

MODULEMAKEFILES  := $(wildcard $(srcdir)/*/Makefile.inc)
MODULES          := $(dir $(MODULEMAKEFILES))
INCLUDES         :=  $(patsubst %, -I%, $(MODULES))
SOURCES   := $(foreach MODULE, $(MODULES), $(wildcard $(MODULE)*.[cC])) 

include $(MODULEMAKEFILES)
include $(wildcard $(srcdir)/*/MakefileTargets.inc)

all: $(PROGRAMS) all_headers all_libs
all_ma57:

# No user-serviceable parts below here
clean mostlyclean: 
	-find . -name "*.o" -exec rm \{\} \;

veryclean: clean headers_veryclean libs_veryclean
	-find . -name "*.exe" -exec rm \{\} \;

headers_veryclean:
	-rm -f $(localincludedir)/*.h

libs_veryclean:
	-rm -f $(locallibdir)/*.a

distclean: veryclean
	-rm -f config.status config.log config.cache TAGS
	-find . -name "*.out" -exec rm \{\} \;
	-find . -name "*.bak" -exec rm \{\} \;
	-find . -name "*~" -exec rm \{\} \;
	-if [ -d doc-src ]; then cd doc-src; $(MAKE) distclean; fi
	-rm  GNUmakefile PetscMakefile

maintainer-clean: distclean
	-if [ -d doc-src ]; then cd doc-src; $(MAKE) maintainer-clean; fi
	-rm -f configure

depend:
	$(MAKEDEPEND) -f GNUmakefile $(INCLUDES) $(CPPFLAGS) \
		$(DEFS) $(SOURCES) > /dev/null 2>&1
	@echo " *** Done making depend"

install: all install_headers install_libs install_docs install_exe
install_headers: all_headers
	$(INSTALL) -d $(includedir)/ooqp	
	$(INSTALL) $(localincludedir)/*.h $(includedir)/ooqp

install_libs: all_libs
	$(INSTALL) -d $(libdir)
	$(INSTALL) $(locallibdir)/*.a $(libdir)

install_exe:
	$(INSTALL) -d $(bindir)
	$(INSTALL) *.exe $(bindir)

install_docs:
	if [ -d doc-src ]; then cd doc-src; fi; make
	$(INSTALL) -d $(docdir)
	$(INSTALL) doc/*.pdf doc/*.html $(docdir)
	$(INSTALL) -d $(docdir)/reference-manual
	$(INSTALL) doc/reference-manual/*.html \
		doc/reference-manual/*.png \
		doc/reference-manual/*.css \
		$(docdir)/reference-manual
	$(INSTALL) -d $(docdir)/formulations
	$(INSTALL) doc/formulations/*.html $(docdir)/formulations
	$(INSTALL) -d $(docdir)/distribution-docs
	$(INSTALL) doc/distribution-docs/*.html $(docdir)/distribution-docs 

GNUmakefile: GNUmakefile.in config.status
	./config.status

config.status: configure
	./config.status --recheck

rebuild-manifest:
	cd doc-src; make
	make distclean
	find * .cvsignore \
	\( -path "*CVS" -prune \) -o \( -path doc-src -prune \) \
	-o -type f -print > MANIFEST

rebuild-VERSION:
	make qpgen-sparse-gondzio.exe
	./qpgen-sparse-gondzio.exe --version > VERSION

.SUFFIXES:
.SUFFIXES: .o .c .cc .C .f


# DO NOT DELETE

src/Abstract/Data.o: src/Abstract/Data.h
src/Abstract/IotrRefCount.o: src/Abstract/IotrRefCount.h
src/Abstract/OoqpMonitor.o: src/Abstract/OoqpMonitor.h
src/Abstract/OoqpMonitor.o: src/Abstract/OoqpMonitorData.h
src/Abstract/OoqpMonitor.o: src/Abstract/Solver.h src/Abstract/Residuals.h
src/Abstract/OoqpPetscMonitor.o: src/Abstract/OoqpPetscMonitor.h
src/Abstract/OoqpPetscMonitor.o: src/Abstract/OoqpMonitor.h
src/Abstract/OoqpPetscMonitor.o: src/Abstract/OoqpMonitorData.h
src/Abstract/OoqpVersion.o: src/Abstract/OoqpVersion.h
src/Abstract/Solver.o: src/Abstract/Solver.h src/Abstract/OoqpMonitor.h
src/Abstract/Solver.o: src/Abstract/OoqpMonitorData.h src/Abstract/Status.h
src/Abstract/Solver.o: src/Vector/SimpleVector.h src/Vector/OoqpVector.h
src/Abstract/Solver.o: src/Abstract/IotrRefCount.h
src/Abstract/Solver.o: src/Vector/OoqpVectorHandle.h
src/Abstract/Solver.o: src/Abstract/SmartPointer.h
src/Abstract/Solver.o: src/Vector/SimpleVectorHandle.h src/Abstract/Data.h
src/Abstract/Solver.o: src/Abstract/Variables.h src/Abstract/Residuals.h
src/Abstract/Solver.o: src/Abstract/LinearSystem.h
src/Abstract/Solver.o: src/Abstract/OoqpStartStrategy.h
src/Abstract/Status.o: src/Abstract/Status.h src/Vector/SimpleVector.h
src/Abstract/Status.o: src/Vector/OoqpVector.h src/Abstract/IotrRefCount.h
src/Abstract/Status.o: src/Vector/OoqpVectorHandle.h
src/Abstract/Status.o: src/Abstract/SmartPointer.h
src/Abstract/Status.o: src/Vector/SimpleVectorHandle.h src/Abstract/Solver.h
src/Abstract/Variables.o: src/Abstract/Variables.h src/Abstract/Solver.h
src/Ampl/qpgen-sparse-gondzio-ampl.o: src/CInterface/cQpGenSparse.h
src/Ampl/qpgen-sparse-gondzio-ampl.o: src/CInterface/cQpGen.h
src/Ampl/qpgen-sparse-gondzio-ampl.o: src/Abstract/OoqpMonitorData.h
src/Ampl/qpgen-sparse-gondzio-ampl.o: src/Abstract/Status.h
src/CInterface/cBounds.o: src/CInterface/cBounds.h /usr/include/stdlib.h
src/CInterface/cBounds.o: /usr/include/features.h /usr/include/bits/predefs.h
src/CInterface/cBounds.o: /usr/include/sys/cdefs.h
src/CInterface/cBounds.o: /usr/include/bits/wordsize.h
src/CInterface/cBounds.o: /usr/include/gnu/stubs.h
src/CInterface/cBounds.o: /usr/include/gnu/stubs-32.h
src/CInterface/cBounds.o: /usr/include/bits/waitflags.h
src/CInterface/cBounds.o: /usr/include/bits/waitstatus.h
src/CInterface/cBounds.o: /usr/include/endian.h /usr/include/bits/endian.h
src/CInterface/cBounds.o: /usr/include/bits/byteswap.h
src/CInterface/cBounds.o: /usr/include/sys/types.h /usr/include/bits/types.h
src/CInterface/cBounds.o: /usr/include/bits/typesizes.h /usr/include/time.h
src/CInterface/cBounds.o: /usr/include/sys/select.h
src/CInterface/cBounds.o: /usr/include/bits/select.h
src/CInterface/cBounds.o: /usr/include/bits/sigset.h /usr/include/bits/time.h
src/CInterface/cBounds.o: /usr/include/sys/sysmacros.h
src/CInterface/cBounds.o: /usr/include/bits/pthreadtypes.h
src/CInterface/cBounds.o: /usr/include/alloca.h
src/CInterface/cMpsReader.o: src/Readers/MpsReader.h
src/CInterface/cMpsReader.o: src/Vector/OoqpVectorHandle.h
src/CInterface/cMpsReader.o: src/Abstract/IotrRefCount.h
src/CInterface/cMpsReader.o: src/Abstract/SmartPointer.h
src/CInterface/cMpsReader.o: src/Abstract/DoubleMatrixHandle.h
src/CInterface/cMpsReader.o: src/Vector/SimpleVectorHandle.h
src/CInterface/cMpsReader.o: src/Readers/hash.h src/CInterface/cMpsReader.h
src/CInterface/cMpsReaderPetsc.o: src/CInterface/cMpsReaderPetsc.h
src/CInterface/cMpsReaderPetsc.o: src/CInterface/cMpsReader.h
src/CInterface/cMpsReaderPetsc.o: src/SparseLinearAlgebra/SparseGenMatrix.h
src/CInterface/cMpsReaderPetsc.o: src/Vector/OoqpVectorHandle.h
src/CInterface/cMpsReaderPetsc.o: src/Abstract/IotrRefCount.h
src/CInterface/cMpsReaderPetsc.o: src/Abstract/SmartPointer.h
src/CInterface/cMpsReaderPetsc.o: src/Abstract/DoubleMatrix.h
src/CInterface/cMpsReaderPetsc.o: src/Abstract/DoubleMatrixHandle.h
src/CInterface/cMpsReaderPetsc.o: src/SparseLinearAlgebra/SparseStorage.h
src/CInterface/cMpsReaderPetsc.o: src/SparseLinearAlgebra/SparseStorageHandle.h
src/CInterface/cMpsReaderPetsc.o: src/SparseLinearAlgebra/SparseGenMatrixHandle.h
src/CInterface/cMpsReaderPetsc.o: src/SparseLinearAlgebra/SparseSymMatrix.h
src/CInterface/cMpsReaderPetsc.o: src/Vector/OoqpVector.h
src/CInterface/cMpsReaderPetsc.o: src/SparseLinearAlgebra/SparseSymMatrixHandle.h
src/CInterface/cMpsReaderPetsc.o: src/PetscLinearAlgebra/PetscSpSymMatrix.h
src/CInterface/cMpsReaderPetsc.o: src/PetscLinearAlgebra/PetscSparseStorage.h
src/CInterface/cMpsReaderPetsc.o: src/PetscLinearAlgebra/PetscVectorHandle.h
src/CInterface/cMpsReaderPetsc.o: src/PetscLinearAlgebra/PetscSpGenMatrix.h
src/CInterface/cMpsReaderPetsc.o: src/PetscLinearAlgebra/PetscSpGenMatrixHandle.h
src/CInterface/cMpsReaderPetsc.o: src/PetscLinearAlgebra/PetscSparseStorageHandle.h
src/CInterface/cMpsReaderPetsc.o: src/CInterface/cQpGenSparse.h
src/CInterface/cMpsReaderPetsc.o: src/CInterface/cQpGen.h
src/CInterface/cMpsReaderPetsc.o: src/Abstract/OoqpMonitorData.h
src/CInterface/cQpBound.o: src/CInterface/cQpBound.h
src/CInterface/cQpBound.o: src/Abstract/OoqpMonitorData.h
src/CInterface/cQpBound.o: src/QpBound/QpBoundData.h src/Abstract/Data.h
src/CInterface/cQpBound.o: src/Abstract/DoubleMatrixHandle.h
src/CInterface/cQpBound.o: src/Abstract/SmartPointer.h
src/CInterface/cQpBound.o: src/Abstract/IotrRefCount.h
src/CInterface/cQpBound.o: src/Vector/OoqpVectorHandle.h
src/CInterface/cQpBound.o: src/Vector/OoqpVector.h src/QpBound/QpBound.h
src/CInterface/cQpBound.o: src/Abstract/ProblemFormulation.h
src/CInterface/cQpBound.o: src/QpBound/QpBoundVars.h src/Abstract/Variables.h
src/CInterface/cQpBound.o: src/QpBound/QpBoundResiduals.h
src/CInterface/cQpBound.o: src/Abstract/Residuals.h src/Abstract/Solver.h
src/CInterface/cQpBound.o: src/Abstract/OoqpMonitor.h
src/CInterface/cQpBound.o: src/Vector/SimpleVector.h
src/CInterface/cQpBound.o: src/Vector/SimpleVectorHandle.h
src/CInterface/cQpBoundDense.o: src/CInterface/cQpBoundDense.h
src/CInterface/cQpBoundDense.o: src/CInterface/cQpBound.h
src/CInterface/cQpBoundDense.o: src/Abstract/OoqpMonitorData.h
src/CInterface/cQpBoundDense.o: src/QpBound/QpBoundDense.h
src/CInterface/cQpBoundDense.o: src/QpBound/QpBound.h
src/CInterface/cQpBoundDense.o: src/Abstract/ProblemFormulation.h
src/CInterface/cQpBoundDense.o: src/Vector/OoqpVectorHandle.h
src/CInterface/cQpBoundDense.o: src/Abstract/IotrRefCount.h
src/CInterface/cQpBoundDense.o: src/Abstract/SmartPointer.h
src/CInterface/cQpBoundDense.o: src/QpBound/QpBoundData.h src/Abstract/Data.h
src/CInterface/cQpBoundDense.o: src/Abstract/DoubleMatrixHandle.h
src/CInterface/cQpBoundDense.o: src/Vector/OoqpVector.h
src/CInterface/cQpBoundDense.o: src/QpSolvers/GondzioSolver.h
src/CInterface/cQpBoundDense.o: src/Abstract/Solver.h
src/CInterface/cQpBoundPetsc.o: src/CInterface/cQpBoundPetsc.h
src/CInterface/cQpBoundPetsc.o: src/Abstract/OoqpMonitorData.h
src/CInterface/cQpBoundPetsc.o: src/QpBound/QpBoundPetsc.h
src/CInterface/cQpBoundPetsc.o: src/QpBound/QpBound.h
src/CInterface/cQpBoundPetsc.o: src/Abstract/ProblemFormulation.h
src/CInterface/cQpBoundPetsc.o: src/Vector/OoqpVectorHandle.h
src/CInterface/cQpBoundPetsc.o: src/Abstract/IotrRefCount.h
src/CInterface/cQpBoundPetsc.o: src/Abstract/SmartPointer.h
src/CInterface/cQpBoundPetsc.o: src/QpBound/QpBoundData.h src/Abstract/Data.h
src/CInterface/cQpBoundPetsc.o: src/Abstract/DoubleMatrixHandle.h
src/CInterface/cQpBoundPetsc.o: src/Vector/OoqpVector.h
src/CInterface/cQpBoundPetsc.o: src/QpBound/QpBoundVars.h
src/CInterface/cQpBoundPetsc.o: src/Abstract/Variables.h
src/CInterface/cQpBoundPetsc.o: src/QpBound/QpBoundResiduals.h
src/CInterface/cQpBoundPetsc.o: src/Abstract/Residuals.h
src/CInterface/cQpBoundPetsc.o: src/QpSolvers/MehrotraSolver.h
src/CInterface/cQpBoundPetsc.o: src/Abstract/Solver.h
src/CInterface/cQpBoundPetsc.o: src/Abstract/OoqpMonitor.h
src/CInterface/cQpBoundPetsc.o: src/PetscLinearAlgebra/PetscVector.h
src/CInterface/cQpBoundPetsc.o: src/PetscLinearAlgebra/PetscVectorHandle.h
src/CInterface/cQpBoundPetsc.o: /usr/include/assert.h /usr/include/features.h
src/CInterface/cQpBoundPetsc.o: /usr/include/bits/predefs.h
src/CInterface/cQpBoundPetsc.o: /usr/include/sys/cdefs.h
src/CInterface/cQpBoundPetsc.o: /usr/include/bits/wordsize.h
src/CInterface/cQpBoundPetsc.o: /usr/include/gnu/stubs.h
src/CInterface/cQpBoundPetsc.o: /usr/include/gnu/stubs-32.h
src/CInterface/cQpBoundPetsc.o: src/Abstract/Status.h
src/CInterface/cQpBoundPetsc.o: src/CInterface/cQpBound.h
src/CInterface/cQpGen.o: src/CInterface/cQpGen.h
src/CInterface/cQpGen.o: src/Abstract/OoqpMonitorData.h src/QpGen/QpGenData.h
src/CInterface/cQpGen.o: src/Abstract/Data.h src/Vector/OoqpVectorHandle.h
src/CInterface/cQpGen.o: src/Abstract/IotrRefCount.h
src/CInterface/cQpGen.o: src/Abstract/SmartPointer.h src/Vector/OoqpVector.h
src/CInterface/cQpGen.o: src/Abstract/DoubleMatrixHandle.h src/QpGen/QpGen.h
src/CInterface/cQpGen.o: src/Abstract/ProblemFormulation.h
src/CInterface/cQpGen.o: src/Abstract/Solver.h src/QpGen/QpGenVars.h
src/CInterface/cQpGen.o: src/Abstract/Variables.h src/QpGen/QpGenResiduals.h
src/CInterface/cQpGen.o: src/Abstract/Residuals.h src/Vector/SimpleVector.h
src/CInterface/cQpGen.o: src/Vector/SimpleVectorHandle.h
src/CInterface/cQpGen.o: src/Abstract/OoqpMonitor.h
src/CInterface/cQpGenDense.o: src/CInterface/cQpGenDense.h
src/CInterface/cQpGenDense.o: src/CInterface/cQpGen.h
src/CInterface/cQpGenDense.o: src/Abstract/OoqpMonitorData.h
src/CInterface/cQpGenDense.o: src/QpGen/QpGenDense.h src/QpGen/QpGen.h
src/CInterface/cQpGenDense.o: src/Abstract/ProblemFormulation.h
src/CInterface/cQpGenDense.o: src/Vector/OoqpVectorHandle.h
src/CInterface/cQpGenDense.o: src/Abstract/IotrRefCount.h
src/CInterface/cQpGenDense.o: src/Abstract/SmartPointer.h
src/CInterface/cQpGenDense.o: src/QpSolvers/GondzioSolver.h
src/CInterface/cQpGenDense.o: src/Abstract/Solver.h src/QpGen/QpGenData.h
src/CInterface/cQpGenDense.o: src/Abstract/Data.h src/Vector/OoqpVector.h
src/CInterface/cQpGenDense.o: src/Abstract/DoubleMatrixHandle.h
src/CInterface/cQpGenDense.o: src/CInterface/cBounds.h
src/CInterface/cQpGenPetsc.o: src/QpGen/QpGenData.h src/Abstract/Data.h
src/CInterface/cQpGenPetsc.o: src/Vector/OoqpVectorHandle.h
src/CInterface/cQpGenPetsc.o: src/Abstract/IotrRefCount.h
src/CInterface/cQpGenPetsc.o: src/Abstract/SmartPointer.h
src/CInterface/cQpGenPetsc.o: src/Vector/OoqpVector.h
src/CInterface/cQpGenPetsc.o: src/Abstract/DoubleMatrixHandle.h
src/CInterface/cQpGenPetsc.o: src/QpGen/QpGenVars.h src/Abstract/Variables.h
src/CInterface/cQpGenPetsc.o: src/QpGen/QpGenResiduals.h
src/CInterface/cQpGenPetsc.o: src/Abstract/Residuals.h
src/CInterface/cQpGenPetsc.o: src/QpSolvers/MehrotraSolver.h
src/CInterface/cQpGenPetsc.o: src/Abstract/Solver.h
src/CInterface/cQpGenPetsc.o: src/Abstract/OoqpMonitor.h
src/CInterface/cQpGenPetsc.o: src/Abstract/OoqpMonitorData.h
src/CInterface/cQpGenPetsc.o: src/PetscLinearAlgebra/PetscVector.h
src/CInterface/cQpGenPetsc.o: src/PetscLinearAlgebra/PetscVectorHandle.h
src/CInterface/cQpGenPetsc.o: /usr/include/assert.h /usr/include/features.h
src/CInterface/cQpGenPetsc.o: /usr/include/bits/predefs.h
src/CInterface/cQpGenPetsc.o: /usr/include/sys/cdefs.h
src/CInterface/cQpGenPetsc.o: /usr/include/bits/wordsize.h
src/CInterface/cQpGenPetsc.o: /usr/include/gnu/stubs.h
src/CInterface/cQpGenPetsc.o: /usr/include/gnu/stubs-32.h
src/CInterface/cQpGenSparse.o: src/CInterface/cQpGenSparse.h
src/CInterface/cQpGenSparse.o: src/CInterface/cQpGen.h
src/CInterface/cQpGenSparse.o: src/Abstract/OoqpMonitorData.h
src/CInterface/cQpGenSparse.o: src/CInterface/cBounds.h
src/CInterface/cQpGenSparse.o: src/QpGen/QpGenSparseMa27.h
src/CInterface/cQpGenSparse.o: src/QpGen/QpGenSparseSeq.h src/QpGen/QpGen.h
src/CInterface/cQpGenSparse.o: src/Abstract/ProblemFormulation.h
src/CInterface/cQpGenSparse.o: src/QpSolvers/GondzioSolver.h
src/CInterface/cQpGenSparse.o: src/Abstract/Solver.h
src/DenseLinearAlgebra/DenseGenMatrix.o: src/DenseLinearAlgebra/DenseGenMatrix.h
src/DenseLinearAlgebra/DenseGenMatrix.o: src/Abstract/DoubleMatrix.h
src/DenseLinearAlgebra/DenseGenMatrix.o: src/Vector/OoqpVectorHandle.h
src/DenseLinearAlgebra/DenseGenMatrix.o: src/Abstract/IotrRefCount.h
src/DenseLinearAlgebra/DenseGenMatrix.o: src/Abstract/SmartPointer.h
src/DenseLinearAlgebra/DenseGenMatrix.o: src/Abstract/DoubleMatrixHandle.h
src/DenseLinearAlgebra/DenseGenMatrix.o: src/DenseLinearAlgebra/DenseStorage.h
src/DenseLinearAlgebra/DenseGenMatrix.o: src/DenseLinearAlgebra/DenseStorageHandle.h
src/DenseLinearAlgebra/DenseGenMatrix.o: src/DenseLinearAlgebra/DenseGenMatrixHandle.h
src/DenseLinearAlgebra/DenseGenMatrix.o: src/DenseLinearAlgebra/DenseSymMatrix.h
src/DenseLinearAlgebra/DenseGenMatrix.o: src/DenseLinearAlgebra/DenseSymMatrixHandle.h
src/DenseLinearAlgebra/DenseGenMatrix.o: src/Utilities/OoqpBlas.h
src/DenseLinearAlgebra/DenseGenMatrix.o: src/Vector/SimpleVector.h
src/DenseLinearAlgebra/DenseGenMatrix.o: src/Vector/OoqpVector.h
src/DenseLinearAlgebra/DenseGenMatrix.o: src/Vector/SimpleVectorHandle.h
src/DenseLinearAlgebra/DenseGenMatrix.o: src/Abstract/DoubleMatrixTypes.h
src/DenseLinearAlgebra/DenseLinearAlgebraPackage.o: src/DenseLinearAlgebra/DenseLinearAlgebraPackage.h
src/DenseLinearAlgebra/DenseLinearAlgebraPackage.o: src/Abstract/LinearAlgebraPackage.h
src/DenseLinearAlgebra/DenseLinearAlgebraPackage.o: src/Abstract/DoubleMatrixHandle.h
src/DenseLinearAlgebra/DenseLinearAlgebraPackage.o: src/Abstract/SmartPointer.h
src/DenseLinearAlgebra/DenseLinearAlgebraPackage.o: src/Abstract/IotrRefCount.h
src/DenseLinearAlgebra/DenseLinearAlgebraPackage.o: src/Vector/OoqpVectorHandle.h
src/DenseLinearAlgebra/DenseLinearAlgebraPackage.o: src/DenseLinearAlgebra/DenseSymMatrix.h
src/DenseLinearAlgebra/DenseLinearAlgebraPackage.o: src/DenseLinearAlgebra/DenseStorage.h
src/DenseLinearAlgebra/DenseLinearAlgebraPackage.o: src/Abstract/DoubleMatrix.h
src/DenseLinearAlgebra/DenseLinearAlgebraPackage.o: src/DenseLinearAlgebra/DenseStorageHandle.h
src/DenseLinearAlgebra/DenseLinearAlgebraPackage.o: src/DenseLinearAlgebra/DenseSymMatrixHandle.h
src/DenseLinearAlgebra/DenseLinearAlgebraPackage.o: src/DenseLinearAlgebra/DenseGenMatrix.h
src/DenseLinearAlgebra/DenseLinearAlgebraPackage.o: src/DenseLinearAlgebra/DenseGenMatrixHandle.h
src/DenseLinearAlgebra/DenseLinearAlgebraPackage.o: src/DenseLinearAlgebra/DeSymIndefSolver.h
src/DenseLinearAlgebra/DenseLinearAlgebraPackage.o: src/Abstract/DoubleLinearSolver.h
src/DenseLinearAlgebra/DenseLinearAlgebraPackage.o: src/Vector/SimpleVectorHandle.h
src/DenseLinearAlgebra/DenseLinearAlgebraPackage.o: src/Vector/SimpleVector.h
src/DenseLinearAlgebra/DenseLinearAlgebraPackage.o: src/Vector/OoqpVector.h
src/DenseLinearAlgebra/DenseStorage.o: src/Utilities/OoqpBlas.h
src/DenseLinearAlgebra/DenseStorage.o: src/DenseLinearAlgebra/DenseStorage.h
src/DenseLinearAlgebra/DenseStorage.o: src/Abstract/DoubleMatrix.h
src/DenseLinearAlgebra/DenseStorage.o: src/Vector/OoqpVectorHandle.h
src/DenseLinearAlgebra/DenseStorage.o: src/Abstract/IotrRefCount.h
src/DenseLinearAlgebra/DenseStorage.o: src/Abstract/SmartPointer.h
src/DenseLinearAlgebra/DenseStorage.o: src/Abstract/DoubleMatrixHandle.h
src/DenseLinearAlgebra/DenseStorage.o: src/DenseLinearAlgebra/DenseStorageHandle.h
src/DenseLinearAlgebra/DenseStorage.o: src/Vector/OoqpVector.h
src/DenseLinearAlgebra/DenseStorage.o: src/Vector/SimpleVector.h
src/DenseLinearAlgebra/DenseStorage.o: src/Vector/SimpleVectorHandle.h
src/DenseLinearAlgebra/DenseSymMatrix.o: src/DenseLinearAlgebra/DenseSymMatrix.h
src/DenseLinearAlgebra/DenseSymMatrix.o: src/DenseLinearAlgebra/DenseStorage.h
src/DenseLinearAlgebra/DenseSymMatrix.o: src/Abstract/DoubleMatrix.h
src/DenseLinearAlgebra/DenseSymMatrix.o: src/Vector/OoqpVectorHandle.h
src/DenseLinearAlgebra/DenseSymMatrix.o: src/Abstract/IotrRefCount.h
src/DenseLinearAlgebra/DenseSymMatrix.o: src/Abstract/SmartPointer.h
src/DenseLinearAlgebra/DenseSymMatrix.o: src/Abstract/DoubleMatrixHandle.h
src/DenseLinearAlgebra/DenseSymMatrix.o: src/DenseLinearAlgebra/DenseStorageHandle.h
src/DenseLinearAlgebra/DenseSymMatrix.o: src/DenseLinearAlgebra/DenseSymMatrixHandle.h
src/DenseLinearAlgebra/DenseSymMatrix.o: src/DenseLinearAlgebra/DeSymPSDSolver.h
src/DenseLinearAlgebra/DenseSymMatrix.o: src/Abstract/DoubleLinearSolver.h
src/DenseLinearAlgebra/DenseSymMatrix.o: src/DenseLinearAlgebra/DeSymIndefSolver.h
src/DenseLinearAlgebra/DenseSymMatrix.o: src/Vector/SimpleVectorHandle.h
src/DenseLinearAlgebra/DenseSymMatrix.o: src/Utilities/OoqpBlas.h
src/DenseLinearAlgebra/DenseSymMatrix.o: src/Vector/SimpleVector.h
src/DenseLinearAlgebra/DenseSymMatrix.o: src/Vector/OoqpVector.h
src/DenseLinearAlgebra/DenseSymMatrix.o: src/Abstract/DoubleMatrixTypes.h
src/DenseLinearAlgebra/DeSymIndefSolver.o: src/DenseLinearAlgebra/DeSymIndefSolver.h
src/DenseLinearAlgebra/DeSymIndefSolver.o: src/Abstract/DoubleLinearSolver.h
src/DenseLinearAlgebra/DeSymIndefSolver.o: src/Vector/OoqpVectorHandle.h
src/DenseLinearAlgebra/DeSymIndefSolver.o: src/Abstract/IotrRefCount.h
src/DenseLinearAlgebra/DeSymIndefSolver.o: src/Abstract/SmartPointer.h
src/DenseLinearAlgebra/DeSymIndefSolver.o: src/DenseLinearAlgebra/DenseSymMatrixHandle.h
src/DenseLinearAlgebra/DeSymIndefSolver.o: src/DenseLinearAlgebra/DenseStorageHandle.h
src/DenseLinearAlgebra/DeSymIndefSolver.o: src/Vector/SimpleVectorHandle.h
src/DenseLinearAlgebra/DeSymIndefSolver.o: src/Vector/SimpleVector.h
src/DenseLinearAlgebra/DeSymIndefSolver.o: src/Vector/OoqpVector.h
src/DenseLinearAlgebra/DeSymIndefSolver.o: src/DenseLinearAlgebra/DenseSymMatrix.h
src/DenseLinearAlgebra/DeSymIndefSolver.o: src/DenseLinearAlgebra/DenseStorage.h
src/DenseLinearAlgebra/DeSymIndefSolver.o: src/Abstract/DoubleMatrix.h
src/DenseLinearAlgebra/DeSymIndefSolver.o: src/Abstract/DoubleMatrixHandle.h
src/DenseLinearAlgebra/DeSymPSDSolver.o: src/DenseLinearAlgebra/DeSymPSDSolver.h
src/DenseLinearAlgebra/DeSymPSDSolver.o: src/Abstract/DoubleLinearSolver.h
src/DenseLinearAlgebra/DeSymPSDSolver.o: src/Vector/OoqpVectorHandle.h
src/DenseLinearAlgebra/DeSymPSDSolver.o: src/Abstract/IotrRefCount.h
src/DenseLinearAlgebra/DeSymPSDSolver.o: src/Abstract/SmartPointer.h
src/DenseLinearAlgebra/DeSymPSDSolver.o: src/DenseLinearAlgebra/DenseStorageHandle.h
src/DenseLinearAlgebra/DeSymPSDSolver.o: src/DenseLinearAlgebra/DenseSymMatrixHandle.h
src/DenseLinearAlgebra/DeSymPSDSolver.o: src/DenseLinearAlgebra/DenseStorage.h
src/DenseLinearAlgebra/DeSymPSDSolver.o: src/Abstract/DoubleMatrix.h
src/DenseLinearAlgebra/DeSymPSDSolver.o: src/Abstract/DoubleMatrixHandle.h
src/DenseLinearAlgebra/DeSymPSDSolver.o: src/DenseLinearAlgebra/DenseSymMatrix.h
src/DenseLinearAlgebra/DeSymPSDSolver.o: src/Vector/SimpleVector.h
src/DenseLinearAlgebra/DeSymPSDSolver.o: src/Vector/OoqpVector.h
src/DenseLinearAlgebra/DeSymPSDSolver.o: src/Vector/SimpleVectorHandle.h
src/Huber/Huber.o: src/Huber/Huber.h src/Abstract/ProblemFormulation.h
src/Huber/Huber.o: src/Huber/HuberData.h src/Abstract/Data.h
src/Huber/Huber.o: src/DenseLinearAlgebra/DenseGenMatrixHandle.h
src/Huber/Huber.o: src/Abstract/IotrRefCount.h src/Abstract/SmartPointer.h
src/Huber/Huber.o: src/DenseLinearAlgebra/DenseSymMatrixHandle.h
src/Huber/Huber.o: src/Vector/SimpleVectorHandle.h
src/Huber/Huber.o: src/Vector/OoqpVectorHandle.h src/Huber/HuberResiduals.h
src/Huber/Huber.o: src/Abstract/Residuals.h src/Huber/HuberVars.h
src/Huber/Huber.o: src/Abstract/Variables.h src/Huber/HuberLinsys.h
src/Huber/Huber.o: src/Abstract/LinearSystem.h
src/Huber/HuberData.o: src/Huber/HuberData.h src/Abstract/Data.h
src/Huber/HuberData.o: src/DenseLinearAlgebra/DenseGenMatrixHandle.h
src/Huber/HuberData.o: src/Abstract/IotrRefCount.h
src/Huber/HuberData.o: src/Abstract/SmartPointer.h
src/Huber/HuberData.o: src/DenseLinearAlgebra/DenseSymMatrixHandle.h
src/Huber/HuberData.o: src/Vector/SimpleVectorHandle.h
src/Huber/HuberData.o: src/Vector/OoqpVectorHandle.h
src/Huber/HuberData.o: src/DenseLinearAlgebra/DenseGenMatrix.h
src/Huber/HuberData.o: src/Abstract/DoubleMatrix.h
src/Huber/HuberData.o: src/Abstract/DoubleMatrixHandle.h
src/Huber/HuberData.o: src/DenseLinearAlgebra/DenseStorage.h
src/Huber/HuberData.o: src/DenseLinearAlgebra/DenseStorageHandle.h
src/Huber/HuberData.o: src/Vector/SimpleVector.h src/Vector/OoqpVector.h
src/Huber/HuberGondzioDriver.o: src/Huber/HuberData.h src/Abstract/Data.h
src/Huber/HuberGondzioDriver.o: src/DenseLinearAlgebra/DenseGenMatrixHandle.h
src/Huber/HuberGondzioDriver.o: src/Abstract/IotrRefCount.h
src/Huber/HuberGondzioDriver.o: src/Abstract/SmartPointer.h
src/Huber/HuberGondzioDriver.o: src/DenseLinearAlgebra/DenseSymMatrixHandle.h
src/Huber/HuberGondzioDriver.o: src/Vector/SimpleVectorHandle.h
src/Huber/HuberGondzioDriver.o: src/Vector/OoqpVectorHandle.h
src/Huber/HuberGondzioDriver.o: src/Huber/HuberVars.h
src/Huber/HuberGondzioDriver.o: src/Abstract/Variables.h
src/Huber/HuberGondzioDriver.o: src/Huber/HuberResiduals.h
src/Huber/HuberGondzioDriver.o: src/Abstract/Residuals.h
src/Huber/HuberGondzioDriver.o: src/Huber/HuberLinsys.h
src/Huber/HuberGondzioDriver.o: src/Abstract/LinearSystem.h
src/Huber/HuberGondzioDriver.o: src/QpSolvers/GondzioSolver.h
src/Huber/HuberGondzioDriver.o: src/Abstract/Solver.h src/Huber/Huber.h
src/Huber/HuberGondzioDriver.o: src/Abstract/ProblemFormulation.h
src/Huber/HuberGondzioDriver.o: src/Vector/SimpleVector.h
src/Huber/HuberGondzioDriver.o: src/Vector/OoqpVector.h
src/Huber/HuberLinsys.o: src/Huber/HuberLinsys.h src/Abstract/LinearSystem.h
src/Huber/HuberLinsys.o: src/DenseLinearAlgebra/DenseSymMatrixHandle.h
src/Huber/HuberLinsys.o: src/Abstract/IotrRefCount.h
src/Huber/HuberLinsys.o: src/Abstract/SmartPointer.h
src/Huber/HuberLinsys.o: src/Vector/SimpleVectorHandle.h
src/Huber/HuberLinsys.o: src/Vector/OoqpVectorHandle.h
src/Huber/HuberLinsys.o: src/DenseLinearAlgebra/DenseSymMatrix.h
src/Huber/HuberLinsys.o: src/DenseLinearAlgebra/DenseStorage.h
src/Huber/HuberLinsys.o: src/Abstract/DoubleMatrix.h
src/Huber/HuberLinsys.o: src/Abstract/DoubleMatrixHandle.h
src/Huber/HuberLinsys.o: src/DenseLinearAlgebra/DenseStorageHandle.h
src/Huber/HuberLinsys.o: src/DenseLinearAlgebra/DenseGenMatrix.h
src/Huber/HuberLinsys.o: src/DenseLinearAlgebra/DenseGenMatrixHandle.h
src/Huber/HuberLinsys.o: src/Huber/HuberData.h src/Abstract/Data.h
src/Huber/HuberLinsys.o: src/Huber/HuberVars.h src/Abstract/Variables.h
src/Huber/HuberLinsys.o: src/Huber/HuberResiduals.h src/Abstract/Residuals.h
src/Huber/HuberLinsys.o: src/Abstract/DoubleLinearSolver.h
src/Huber/HuberLinsys.o: src/DenseLinearAlgebra/DeSymPSDSolver.h
src/Huber/HuberLinsys.o: src/Vector/SimpleVector.h src/Vector/OoqpVector.h
src/Huber/HuberResiduals.o: src/Huber/HuberResiduals.h
src/Huber/HuberResiduals.o: src/Abstract/Residuals.h
src/Huber/HuberResiduals.o: src/Vector/SimpleVectorHandle.h
src/Huber/HuberResiduals.o: src/Vector/OoqpVectorHandle.h
src/Huber/HuberResiduals.o: src/Abstract/IotrRefCount.h
src/Huber/HuberResiduals.o: src/Abstract/SmartPointer.h src/Huber/HuberData.h
src/Huber/HuberResiduals.o: src/Abstract/Data.h
src/Huber/HuberResiduals.o: src/DenseLinearAlgebra/DenseGenMatrixHandle.h
src/Huber/HuberResiduals.o: src/DenseLinearAlgebra/DenseSymMatrixHandle.h
src/Huber/HuberResiduals.o: src/Huber/HuberVars.h src/Abstract/Variables.h
src/Huber/HuberResiduals.o: src/Vector/SimpleVector.h src/Vector/OoqpVector.h
src/Huber/HuberVars.o: src/Huber/HuberVars.h src/Vector/SimpleVectorHandle.h
src/Huber/HuberVars.o: src/Vector/OoqpVectorHandle.h
src/Huber/HuberVars.o: src/Abstract/IotrRefCount.h
src/Huber/HuberVars.o: src/Abstract/SmartPointer.h src/Abstract/Variables.h
src/Huber/HuberVars.o: src/Huber/HuberData.h src/Abstract/Data.h
src/Huber/HuberVars.o: src/DenseLinearAlgebra/DenseGenMatrixHandle.h
src/Huber/HuberVars.o: src/DenseLinearAlgebra/DenseSymMatrixHandle.h
src/Huber/HuberVars.o: src/Vector/SimpleVector.h src/Vector/OoqpVector.h
src/Mex/cfunc_ooqp_huber.o: src/Huber/HuberData.h src/Abstract/Data.h
src/Mex/cfunc_ooqp_huber.o: src/DenseLinearAlgebra/DenseGenMatrixHandle.h
src/Mex/cfunc_ooqp_huber.o: src/Abstract/IotrRefCount.h
src/Mex/cfunc_ooqp_huber.o: src/Abstract/SmartPointer.h
src/Mex/cfunc_ooqp_huber.o: src/DenseLinearAlgebra/DenseSymMatrixHandle.h
src/Mex/cfunc_ooqp_huber.o: src/Vector/SimpleVectorHandle.h
src/Mex/cfunc_ooqp_huber.o: src/Vector/OoqpVectorHandle.h
src/Mex/cfunc_ooqp_huber.o: src/Huber/HuberVars.h src/Abstract/Variables.h
src/Mex/cfunc_ooqp_huber.o: src/Huber/HuberResiduals.h
src/Mex/cfunc_ooqp_huber.o: src/Abstract/Residuals.h src/Huber/HuberLinsys.h
src/Mex/cfunc_ooqp_huber.o: src/Abstract/LinearSystem.h
src/Mex/cfunc_ooqp_huber.o: src/QpSolvers/GondzioSolver.h
src/Mex/cfunc_ooqp_huber.o: src/Abstract/Solver.h src/Huber/Huber.h
src/Mex/cfunc_ooqp_huber.o: src/Abstract/ProblemFormulation.h
src/Mex/cfunc_ooqp_huber.o: src/Abstract/OoqpMonitorData.h
src/Mex/cfunc_ooqp_huber.o: src/Abstract/OoqpMonitor.h
src/Mex/cfunc_ooqp_svm.o: src/Abstract/DoubleMatrix.h
src/Mex/cfunc_ooqp_svm.o: src/Vector/OoqpVectorHandle.h
src/Mex/cfunc_ooqp_svm.o: src/Abstract/IotrRefCount.h
src/Mex/cfunc_ooqp_svm.o: src/Abstract/SmartPointer.h
src/Mex/cfunc_ooqp_svm.o: src/Abstract/DoubleMatrixHandle.h
src/Mex/cfunc_ooqp_svm.o: src/Vector/SimpleVector.h src/Vector/OoqpVector.h
src/Mex/cfunc_ooqp_svm.o: src/Vector/SimpleVectorHandle.h src/Svm/SvmData.h
src/Mex/cfunc_ooqp_svm.o: src/Abstract/Data.h
src/Mex/cfunc_ooqp_svm.o: src/DenseLinearAlgebra/DenseGenMatrixHandle.h
src/Mex/cfunc_ooqp_svm.o: src/DenseLinearAlgebra/DenseSymMatrixHandle.h
src/Mex/cfunc_ooqp_svm.o: src/Svm/SvmVars.h src/Abstract/Variables.h
src/Mex/cfunc_ooqp_svm.o: src/Svm/SvmResiduals.h src/Abstract/Residuals.h
src/Mex/cfunc_ooqp_svm.o: src/Svm/SvmLinsys.h src/Abstract/LinearSystem.h
src/Mex/cfunc_ooqp_svm.o: src/QpSolvers/GondzioSolver.h src/Abstract/Solver.h
src/Mex/cfunc_ooqp_svm.o: src/Svm/Svm.h src/Abstract/ProblemFormulation.h
src/Mex/cfunc_ooqp_svm.o: src/Abstract/OoqpMonitorData.h
src/Mex/cfunc_ooqp_svm.o: src/Abstract/OoqpMonitor.h
src/Mex/mexUtility.o: src/Mex/mexUtility.h
src/Mex/ooqp_huber.o: src/Mex/mexUtility.h src/Abstract/OoqpMonitorData.h
src/Mex/ooqp_mex.o: src/Mex/mexUtility.h src/CInterface/cQpGenSparse.h
src/Mex/ooqp_mex.o: src/CInterface/cQpGen.h src/Abstract/OoqpMonitorData.h
src/Mex/ooqp_mex.o: /usr/include/string.h /usr/include/features.h
src/Mex/ooqp_mex.o: /usr/include/bits/predefs.h /usr/include/sys/cdefs.h
src/Mex/ooqp_mex.o: /usr/include/bits/wordsize.h /usr/include/gnu/stubs.h
src/Mex/ooqp_mex.o: /usr/include/gnu/stubs-32.h /usr/include/xlocale.h
src/Mex/ooqp_mex.o: /usr/include/sys/time.h /usr/include/bits/types.h
src/Mex/ooqp_mex.o: /usr/include/bits/typesizes.h /usr/include/time.h
src/Mex/ooqp_mex.o: /usr/include/bits/time.h /usr/include/sys/select.h
src/Mex/ooqp_mex.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
src/Mex/ooqp_mex.o: /usr/include/sys/resource.h /usr/include/bits/resource.h
src/Mex/ooqp_mex.o: /usr/include/unistd.h /usr/include/bits/posix_opt.h
src/Mex/ooqp_mex.o: /usr/include/bits/environments.h
src/Mex/ooqp_mex.o: /usr/include/bits/confname.h /usr/include/getopt.h
src/Mex/ooqp_start_cpp.o: src/CInterface/cQpGen.h
src/Mex/ooqp_start_cpp.o: src/Abstract/OoqpMonitorData.h
src/Mex/ooqp_start_cpp.o: src/QpGen/QpGenData.h src/Abstract/Data.h
src/Mex/ooqp_start_cpp.o: src/Vector/OoqpVectorHandle.h
src/Mex/ooqp_start_cpp.o: src/Abstract/IotrRefCount.h
src/Mex/ooqp_start_cpp.o: src/Abstract/SmartPointer.h src/Vector/OoqpVector.h
src/Mex/ooqp_start_cpp.o: src/Abstract/DoubleMatrixHandle.h src/QpGen/QpGen.h
src/Mex/ooqp_start_cpp.o: src/Abstract/ProblemFormulation.h
src/Mex/ooqp_start_cpp.o: src/Abstract/Solver.h src/QpGen/QpGenVars.h
src/Mex/ooqp_start_cpp.o: src/Abstract/Variables.h src/QpGen/QpGenResiduals.h
src/Mex/ooqp_start_cpp.o: src/Abstract/Residuals.h src/Vector/SimpleVector.h
src/Mex/ooqp_start_cpp.o: src/Vector/SimpleVectorHandle.h
src/Mex/ooqp_start_cpp.o: src/Abstract/OoqpMonitor.h
src/Mex/ooqp_start_cpp.o: src/Abstract/OoqpStartStrategy.h
src/Mex/ooqp_start_mex.o: src/Mex/mexUtility.h src/CInterface/cQpGenSparse.h
src/Mex/ooqp_start_mex.o: src/CInterface/cQpGen.h
src/Mex/ooqp_start_mex.o: src/Abstract/OoqpMonitorData.h
src/Mex/ooqp_start_mex.o: /usr/include/string.h /usr/include/features.h
src/Mex/ooqp_start_mex.o: /usr/include/bits/predefs.h
src/Mex/ooqp_start_mex.o: /usr/include/sys/cdefs.h
src/Mex/ooqp_start_mex.o: /usr/include/bits/wordsize.h
src/Mex/ooqp_start_mex.o: /usr/include/gnu/stubs.h
src/Mex/ooqp_start_mex.o: /usr/include/gnu/stubs-32.h /usr/include/xlocale.h
src/Mex/ooqp_start_mex.o: /usr/include/sys/time.h /usr/include/bits/types.h
src/Mex/ooqp_start_mex.o: /usr/include/bits/typesizes.h /usr/include/time.h
src/Mex/ooqp_start_mex.o: /usr/include/bits/time.h /usr/include/sys/select.h
src/Mex/ooqp_start_mex.o: /usr/include/bits/select.h
src/Mex/ooqp_start_mex.o: /usr/include/bits/sigset.h
src/Mex/ooqp_start_mex.o: /usr/include/sys/resource.h
src/Mex/ooqp_start_mex.o: /usr/include/bits/resource.h /usr/include/unistd.h
src/Mex/ooqp_start_mex.o: /usr/include/bits/posix_opt.h
src/Mex/ooqp_start_mex.o: /usr/include/bits/environments.h
src/Mex/ooqp_start_mex.o: /usr/include/bits/confname.h /usr/include/getopt.h
src/Mex/ooqp_svm.o: src/Mex/mexUtility.h src/Abstract/OoqpMonitorData.h
src/Mex/readmps_mex.o: src/Mex/mexUtility.h src/CInterface/cMpsReader.h
src/PetscLinearAlgebra/PetscIterativeSolver.o: src/PetscLinearAlgebra/PetscIterativeSolver.h
src/PetscLinearAlgebra/PetscIterativeSolver.o: src/Abstract/DoubleLinearSolver.h
src/PetscLinearAlgebra/PetscIterativeSolver.o: src/Vector/OoqpVectorHandle.h
src/PetscLinearAlgebra/PetscIterativeSolver.o: src/Abstract/IotrRefCount.h
src/PetscLinearAlgebra/PetscIterativeSolver.o: src/Abstract/SmartPointer.h
src/PetscLinearAlgebra/PetscIterativeSolver.o: src/PetscLinearAlgebra/PetscSpSymMatrixHandle.h
src/PetscLinearAlgebra/PetscIterativeSolver.o: src/PetscLinearAlgebra/PetscSparseStorageHandle.h
src/PetscLinearAlgebra/PetscIterativeSolver.o: src/PetscLinearAlgebra/PetscVectorHandle.h
src/PetscLinearAlgebra/PetscIterativeSolver.o: src/PetscLinearAlgebra/PetscSpSymMatrix.h
src/PetscLinearAlgebra/PetscIterativeSolver.o: src/Abstract/DoubleMatrix.h
src/PetscLinearAlgebra/PetscIterativeSolver.o: src/Abstract/DoubleMatrixHandle.h
src/PetscLinearAlgebra/PetscIterativeSolver.o: src/PetscLinearAlgebra/PetscSparseStorage.h
src/PetscLinearAlgebra/PetscIterativeSolver.o: src/PetscLinearAlgebra/PetscVector.h
src/PetscLinearAlgebra/PetscIterativeSolver.o: /usr/include/assert.h
src/PetscLinearAlgebra/PetscIterativeSolver.o: /usr/include/features.h
src/PetscLinearAlgebra/PetscIterativeSolver.o: /usr/include/bits/predefs.h
src/PetscLinearAlgebra/PetscIterativeSolver.o: /usr/include/sys/cdefs.h
src/PetscLinearAlgebra/PetscIterativeSolver.o: /usr/include/bits/wordsize.h
src/PetscLinearAlgebra/PetscIterativeSolver.o: /usr/include/gnu/stubs.h
src/PetscLinearAlgebra/PetscIterativeSolver.o: /usr/include/gnu/stubs-32.h
src/PetscLinearAlgebra/PetscIterativeSolver.o: src/Vector/OoqpVector.h
src/PetscLinearAlgebra/PetscLinearAlgebraPackage.o: src/PetscLinearAlgebra/PetscLinearAlgebraPackage.h
src/PetscLinearAlgebra/PetscLinearAlgebraPackage.o: src/Abstract/LinearAlgebraPackage.h
src/PetscLinearAlgebra/PetscLinearAlgebraPackage.o: src/Abstract/DoubleMatrixHandle.h
src/PetscLinearAlgebra/PetscLinearAlgebraPackage.o: src/Abstract/SmartPointer.h
src/PetscLinearAlgebra/PetscLinearAlgebraPackage.o: src/Abstract/IotrRefCount.h
src/PetscLinearAlgebra/PetscLinearAlgebraPackage.o: src/PetscLinearAlgebra/PetscSpSymMatrix.h
src/PetscLinearAlgebra/PetscLinearAlgebraPackage.o: src/Abstract/DoubleMatrix.h
src/PetscLinearAlgebra/PetscLinearAlgebraPackage.o: src/Vector/OoqpVectorHandle.h
src/PetscLinearAlgebra/PetscLinearAlgebraPackage.o: src/PetscLinearAlgebra/PetscSparseStorage.h
src/PetscLinearAlgebra/PetscLinearAlgebraPackage.o: src/PetscLinearAlgebra/PetscVectorHandle.h
src/PetscLinearAlgebra/PetscLinearAlgebraPackage.o: src/PetscLinearAlgebra/PetscSpGenMatrix.h
src/PetscLinearAlgebra/PetscLinearAlgebraPackage.o: src/PetscLinearAlgebra/PetscSpGenMatrixHandle.h
src/PetscLinearAlgebra/PetscLinearAlgebraPackage.o: src/PetscLinearAlgebra/PetscSparseStorageHandle.h
src/PetscLinearAlgebra/PetscLinearAlgebraPackage.o: src/PetscLinearAlgebra/PetscVector.h
src/PetscLinearAlgebra/PetscLinearAlgebraPackage.o: /usr/include/assert.h
src/PetscLinearAlgebra/PetscLinearAlgebraPackage.o: /usr/include/features.h
src/PetscLinearAlgebra/PetscLinearAlgebraPackage.o: /usr/include/bits/predefs.h
src/PetscLinearAlgebra/PetscLinearAlgebraPackage.o: /usr/include/sys/cdefs.h
src/PetscLinearAlgebra/PetscLinearAlgebraPackage.o: /usr/include/bits/wordsize.h
src/PetscLinearAlgebra/PetscLinearAlgebraPackage.o: /usr/include/gnu/stubs.h
src/PetscLinearAlgebra/PetscLinearAlgebraPackage.o: /usr/include/gnu/stubs-32.h
src/PetscLinearAlgebra/PetscLinearAlgebraPackage.o: src/Vector/OoqpVector.h
src/PetscLinearAlgebra/PetscSparseStorage.o: src/PetscLinearAlgebra/PetscSparseStorage.h
src/PetscLinearAlgebra/PetscSparseStorage.o: src/Abstract/DoubleMatrix.h
src/PetscLinearAlgebra/PetscSparseStorage.o: src/Vector/OoqpVectorHandle.h
src/PetscLinearAlgebra/PetscSparseStorage.o: src/Abstract/IotrRefCount.h
src/PetscLinearAlgebra/PetscSparseStorage.o: src/Abstract/SmartPointer.h
src/PetscLinearAlgebra/PetscSparseStorage.o: src/Abstract/DoubleMatrixHandle.h
src/PetscLinearAlgebra/PetscSparseStorage.o: src/PetscLinearAlgebra/PetscVectorHandle.h
src/PetscLinearAlgebra/PetscSparseStorage.o: src/Vector/OoqpVector.h
src/PetscLinearAlgebra/PetscSparseStorage.o: src/Vector/SimpleVector.h
src/PetscLinearAlgebra/PetscSparseStorage.o: src/Vector/SimpleVectorHandle.h
src/PetscLinearAlgebra/PetscSparseStorage.o: src/PetscLinearAlgebra/PetscVector.h
src/PetscLinearAlgebra/PetscSparseStorage.o: /usr/include/assert.h
src/PetscLinearAlgebra/PetscSparseStorage.o: /usr/include/features.h
src/PetscLinearAlgebra/PetscSparseStorage.o: /usr/include/bits/predefs.h
src/PetscLinearAlgebra/PetscSparseStorage.o: /usr/include/sys/cdefs.h
src/PetscLinearAlgebra/PetscSparseStorage.o: /usr/include/bits/wordsize.h
src/PetscLinearAlgebra/PetscSparseStorage.o: /usr/include/gnu/stubs.h
src/PetscLinearAlgebra/PetscSparseStorage.o: /usr/include/gnu/stubs-32.h
src/PetscLinearAlgebra/PetscSpGenMatrix.o: src/PetscLinearAlgebra/PetscSpGenMatrix.h
src/PetscLinearAlgebra/PetscSpGenMatrix.o: src/Abstract/DoubleMatrix.h
src/PetscLinearAlgebra/PetscSpGenMatrix.o: src/Vector/OoqpVectorHandle.h
src/PetscLinearAlgebra/PetscSpGenMatrix.o: src/Abstract/IotrRefCount.h
src/PetscLinearAlgebra/PetscSpGenMatrix.o: src/Abstract/SmartPointer.h
src/PetscLinearAlgebra/PetscSpGenMatrix.o: src/Abstract/DoubleMatrixHandle.h
src/PetscLinearAlgebra/PetscSpGenMatrix.o: src/PetscLinearAlgebra/PetscSpGenMatrixHandle.h
src/PetscLinearAlgebra/PetscSpGenMatrix.o: src/PetscLinearAlgebra/PetscSparseStorageHandle.h
src/PetscLinearAlgebra/PetscSpGenMatrix.o: src/PetscLinearAlgebra/PetscSparseStorage.h
src/PetscLinearAlgebra/PetscSpGenMatrix.o: src/PetscLinearAlgebra/PetscVectorHandle.h
src/PetscLinearAlgebra/PetscSpGenMatrix.o: src/Vector/SimpleVector.h
src/PetscLinearAlgebra/PetscSpGenMatrix.o: src/Vector/OoqpVector.h
src/PetscLinearAlgebra/PetscSpGenMatrix.o: src/Vector/SimpleVectorHandle.h
src/PetscLinearAlgebra/PetscSpGenMatrix.o: src/PetscLinearAlgebra/PetscVector.h
src/PetscLinearAlgebra/PetscSpGenMatrix.o: /usr/include/assert.h
src/PetscLinearAlgebra/PetscSpGenMatrix.o: /usr/include/features.h
src/PetscLinearAlgebra/PetscSpGenMatrix.o: /usr/include/bits/predefs.h
src/PetscLinearAlgebra/PetscSpGenMatrix.o: /usr/include/sys/cdefs.h
src/PetscLinearAlgebra/PetscSpGenMatrix.o: /usr/include/bits/wordsize.h
src/PetscLinearAlgebra/PetscSpGenMatrix.o: /usr/include/gnu/stubs.h
src/PetscLinearAlgebra/PetscSpGenMatrix.o: /usr/include/gnu/stubs-32.h
src/PetscLinearAlgebra/PetscSpGenMatrix.o: src/Abstract/DoubleMatrixTypes.h
src/PetscLinearAlgebra/PetscSpSymMatrix.o: src/PetscLinearAlgebra/PetscSpSymMatrix.h
src/PetscLinearAlgebra/PetscSpSymMatrix.o: src/Abstract/DoubleMatrix.h
src/PetscLinearAlgebra/PetscSpSymMatrix.o: src/Vector/OoqpVectorHandle.h
src/PetscLinearAlgebra/PetscSpSymMatrix.o: src/Abstract/IotrRefCount.h
src/PetscLinearAlgebra/PetscSpSymMatrix.o: src/Abstract/SmartPointer.h
src/PetscLinearAlgebra/PetscSpSymMatrix.o: src/Abstract/DoubleMatrixHandle.h
src/PetscLinearAlgebra/PetscSpSymMatrix.o: src/PetscLinearAlgebra/PetscSparseStorage.h
src/PetscLinearAlgebra/PetscSpSymMatrix.o: src/PetscLinearAlgebra/PetscVectorHandle.h
src/PetscLinearAlgebra/PetscSpSymMatrix.o: src/PetscLinearAlgebra/PetscVector.h
src/PetscLinearAlgebra/PetscSpSymMatrix.o: /usr/include/assert.h
src/PetscLinearAlgebra/PetscSpSymMatrix.o: /usr/include/features.h
src/PetscLinearAlgebra/PetscSpSymMatrix.o: /usr/include/bits/predefs.h
src/PetscLinearAlgebra/PetscSpSymMatrix.o: /usr/include/sys/cdefs.h
src/PetscLinearAlgebra/PetscSpSymMatrix.o: /usr/include/bits/wordsize.h
src/PetscLinearAlgebra/PetscSpSymMatrix.o: /usr/include/gnu/stubs.h
src/PetscLinearAlgebra/PetscSpSymMatrix.o: /usr/include/gnu/stubs-32.h
src/PetscLinearAlgebra/PetscSpSymMatrix.o: src/Vector/OoqpVector.h
src/PetscLinearAlgebra/PetscSpSymMatrix.o: src/Abstract/DoubleMatrixTypes.h
src/PetscLinearAlgebra/PetscVector.o: src/PetscLinearAlgebra/PetscVector.h
src/PetscLinearAlgebra/PetscVector.o: src/PetscLinearAlgebra/PetscVectorHandle.h
src/PetscLinearAlgebra/PetscVector.o: src/Abstract/IotrRefCount.h
src/PetscLinearAlgebra/PetscVector.o: src/Abstract/SmartPointer.h
src/PetscLinearAlgebra/PetscVector.o: /usr/include/assert.h
src/PetscLinearAlgebra/PetscVector.o: /usr/include/features.h
src/PetscLinearAlgebra/PetscVector.o: /usr/include/bits/predefs.h
src/PetscLinearAlgebra/PetscVector.o: /usr/include/sys/cdefs.h
src/PetscLinearAlgebra/PetscVector.o: /usr/include/bits/wordsize.h
src/PetscLinearAlgebra/PetscVector.o: /usr/include/gnu/stubs.h
src/PetscLinearAlgebra/PetscVector.o: /usr/include/gnu/stubs-32.h
src/PetscLinearAlgebra/PetscVector.o: src/Vector/OoqpVector.h
src/PetscLinearAlgebra/PetscVector.o: src/Vector/OoqpVectorHandle.h
src/PetscLinearAlgebra/PetscVector.o: src/Vector/SimpleVector.h
src/PetscLinearAlgebra/PetscVector.o: src/Vector/SimpleVectorHandle.h
src/PetscLinearAlgebra/PetscVector.o: src/Vector/VectorUtilities.h
src/QpBound/QpBound.o: src/QpBound/QpBound.h
src/QpBound/QpBound.o: src/Abstract/ProblemFormulation.h
src/QpBound/QpBound.o: src/Vector/OoqpVectorHandle.h
src/QpBound/QpBound.o: src/Abstract/IotrRefCount.h
src/QpBound/QpBound.o: src/Abstract/SmartPointer.h src/QpBound/QpBoundData.h
src/QpBound/QpBound.o: src/Abstract/Data.h src/Abstract/DoubleMatrixHandle.h
src/QpBound/QpBound.o: src/Vector/OoqpVector.h src/QpBound/QpBoundResiduals.h
src/QpBound/QpBound.o: src/Abstract/Residuals.h src/QpBound/QpBoundVars.h
src/QpBound/QpBound.o: src/Abstract/Variables.h src/Abstract/DoubleMatrix.h
src/QpBound/QpBound.o: src/Abstract/LinearAlgebraPackage.h
src/QpBound/QpBoundData.o: src/QpBound/QpBoundData.h src/Abstract/Data.h
src/QpBound/QpBoundData.o: src/Abstract/DoubleMatrixHandle.h
src/QpBound/QpBoundData.o: src/Abstract/SmartPointer.h
src/QpBound/QpBoundData.o: src/Abstract/IotrRefCount.h
src/QpBound/QpBoundData.o: src/Vector/OoqpVectorHandle.h
src/QpBound/QpBoundData.o: src/Vector/OoqpVector.h src/QpBound/QpBound.h
src/QpBound/QpBoundData.o: src/Abstract/ProblemFormulation.h
src/QpBound/QpBoundData.o: src/Abstract/DoubleMatrix.h
src/QpBound/QpBoundData.o: src/Vector/SimpleVector.h
src/QpBound/QpBoundData.o: src/Vector/SimpleVectorHandle.h
src/QpBound/QpBoundData.o: src/QpBound/QpBoundVars.h src/Abstract/Variables.h
src/QpBound/QpBoundData.o: src/Readers/MpsReader.h src/Readers/hash.h
src/QpBound/QpBoundData.o: src/DenseLinearAlgebra/DenseGenMatrix.h
src/QpBound/QpBoundData.o: src/DenseLinearAlgebra/DenseStorage.h
src/QpBound/QpBoundData.o: src/DenseLinearAlgebra/DenseStorageHandle.h
src/QpBound/QpBoundData.o: src/DenseLinearAlgebra/DenseGenMatrixHandle.h
src/QpBound/QpBoundData.o: src/DenseLinearAlgebra/DenseSymMatrix.h
src/QpBound/QpBoundData.o: src/DenseLinearAlgebra/DenseSymMatrixHandle.h
src/QpBound/QpBoundDense.o: src/QpBound/QpBoundDense.h src/QpBound/QpBound.h
src/QpBound/QpBoundDense.o: src/Abstract/ProblemFormulation.h
src/QpBound/QpBoundDense.o: src/Vector/OoqpVectorHandle.h
src/QpBound/QpBoundDense.o: src/Abstract/IotrRefCount.h
src/QpBound/QpBoundDense.o: src/Abstract/SmartPointer.h
src/QpBound/QpBoundDense.o: src/QpBound/QpBoundDenseLinsys.h
src/QpBound/QpBoundDense.o: src/QpBound/QpBoundLinsys.h
src/QpBound/QpBoundDense.o: src/Abstract/LinearSystem.h
src/QpBound/QpBoundDense.o: src/Abstract/DoubleMatrixHandle.h
src/QpBound/QpBoundDense.o: src/Vector/OoqpVector.h src/QpBound/QpBoundData.h
src/QpBound/QpBoundDense.o: src/Abstract/Data.h
src/QpBound/QpBoundDense.o: src/DenseLinearAlgebra/DeSymPSDSolver.h
src/QpBound/QpBoundDense.o: src/Abstract/DoubleLinearSolver.h
src/QpBound/QpBoundDense.o: src/DenseLinearAlgebra/DenseStorageHandle.h
src/QpBound/QpBoundDense.o: src/DenseLinearAlgebra/DenseSymMatrixHandle.h
src/QpBound/QpBoundDense.o: src/DenseLinearAlgebra/DenseSymMatrix.h
src/QpBound/QpBoundDense.o: src/DenseLinearAlgebra/DenseStorage.h
src/QpBound/QpBoundDense.o: src/Abstract/DoubleMatrix.h
src/QpBound/QpBoundDense.o: src/DenseLinearAlgebra/DenseLinearAlgebraPackage.h
src/QpBound/QpBoundDense.o: src/Abstract/LinearAlgebraPackage.h
src/QpBound/QpBoundDense.o: src/Vector/SimpleVector.h
src/QpBound/QpBoundDense.o: src/Vector/SimpleVectorHandle.h
src/QpBound/QpBoundDenseGondzioDriver.o: src/QpBound/QpBoundData.h
src/QpBound/QpBoundDenseGondzioDriver.o: src/Abstract/Data.h
src/QpBound/QpBoundDenseGondzioDriver.o: src/Abstract/DoubleMatrixHandle.h
src/QpBound/QpBoundDenseGondzioDriver.o: src/Abstract/SmartPointer.h
src/QpBound/QpBoundDenseGondzioDriver.o: src/Abstract/IotrRefCount.h
src/QpBound/QpBoundDenseGondzioDriver.o: src/Vector/OoqpVectorHandle.h
src/QpBound/QpBoundDenseGondzioDriver.o: src/Vector/OoqpVector.h
src/QpBound/QpBoundDenseGondzioDriver.o: src/QpBound/QpBoundVars.h
src/QpBound/QpBoundDenseGondzioDriver.o: src/Abstract/Variables.h
src/QpBound/QpBoundDenseGondzioDriver.o: src/QpBound/QpBoundResiduals.h
src/QpBound/QpBoundDenseGondzioDriver.o: src/Abstract/Residuals.h
src/QpBound/QpBoundDenseGondzioDriver.o: src/QpSolvers/GondzioSolver.h
src/QpBound/QpBoundDenseGondzioDriver.o: src/Abstract/Solver.h
src/QpBound/QpBoundDenseGondzioDriver.o: src/QpBound/QpBoundDense.h
src/QpBound/QpBoundDenseGondzioDriver.o: src/QpBound/QpBound.h
src/QpBound/QpBoundDenseGondzioDriver.o: src/Abstract/ProblemFormulation.h
src/QpBound/QpBoundDenseGondzioDriver.o: src/Readers/MpsReader.h
src/QpBound/QpBoundDenseGondzioDriver.o: src/Vector/SimpleVectorHandle.h
src/QpBound/QpBoundDenseGondzioDriver.o: src/Readers/hash.h
src/QpBound/QpBoundDenseLinsys.o: src/QpBound/QpBoundDenseLinsys.h
src/QpBound/QpBoundDenseLinsys.o: src/QpBound/QpBoundLinsys.h
src/QpBound/QpBoundDenseLinsys.o: src/Abstract/LinearSystem.h
src/QpBound/QpBoundDenseLinsys.o: src/Abstract/DoubleMatrixHandle.h
src/QpBound/QpBoundDenseLinsys.o: src/Abstract/SmartPointer.h
src/QpBound/QpBoundDenseLinsys.o: src/Abstract/IotrRefCount.h
src/QpBound/QpBoundDenseLinsys.o: src/Vector/OoqpVectorHandle.h
src/QpBound/QpBoundDenseLinsys.o: src/Vector/OoqpVector.h
src/QpBound/QpBoundDenseLinsys.o: src/QpBound/QpBoundData.h
src/QpBound/QpBoundDenseLinsys.o: src/Abstract/Data.h
src/QpBound/QpBoundDenseLinsys.o: src/DenseLinearAlgebra/DenseSymMatrix.h
src/QpBound/QpBoundDenseLinsys.o: src/DenseLinearAlgebra/DenseStorage.h
src/QpBound/QpBoundDenseLinsys.o: src/Abstract/DoubleMatrix.h
src/QpBound/QpBoundDenseLinsys.o: src/DenseLinearAlgebra/DenseStorageHandle.h
src/QpBound/QpBoundDenseLinsys.o: src/DenseLinearAlgebra/DenseSymMatrixHandle.h
src/QpBound/QpBoundLinsys.o: src/QpBound/QpBoundLinsys.h
src/QpBound/QpBoundLinsys.o: src/Abstract/LinearSystem.h
src/QpBound/QpBoundLinsys.o: src/Abstract/DoubleMatrixHandle.h
src/QpBound/QpBoundLinsys.o: src/Abstract/SmartPointer.h
src/QpBound/QpBoundLinsys.o: src/Abstract/IotrRefCount.h
src/QpBound/QpBoundLinsys.o: src/Vector/OoqpVectorHandle.h
src/QpBound/QpBoundLinsys.o: src/Vector/OoqpVector.h
src/QpBound/QpBoundLinsys.o: src/QpBound/QpBoundData.h src/Abstract/Data.h
src/QpBound/QpBoundLinsys.o: src/QpBound/QpBoundVars.h
src/QpBound/QpBoundLinsys.o: src/Abstract/Variables.h
src/QpBound/QpBoundLinsys.o: src/QpBound/QpBoundResiduals.h
src/QpBound/QpBoundLinsys.o: src/Abstract/Residuals.h
src/QpBound/QpBoundLinsys.o: src/Abstract/DoubleMatrix.h
src/QpBound/QpBoundLinsys.o: src/Abstract/DoubleLinearSolver.h
src/QpBound/QpBoundLinsys.o: src/QpBound/QpBound.h
src/QpBound/QpBoundLinsys.o: src/Abstract/ProblemFormulation.h
src/QpBound/QpBoundPetsc.o: src/QpBound/QpBoundPetsc.h src/QpBound/QpBound.h
src/QpBound/QpBoundPetsc.o: src/Abstract/ProblemFormulation.h
src/QpBound/QpBoundPetsc.o: src/Vector/OoqpVectorHandle.h
src/QpBound/QpBoundPetsc.o: src/Abstract/IotrRefCount.h
src/QpBound/QpBoundPetsc.o: src/Abstract/SmartPointer.h
src/QpBound/QpBoundPetsc.o: src/PetscLinearAlgebra/PetscIterativeSolver.h
src/QpBound/QpBoundPetsc.o: src/Abstract/DoubleLinearSolver.h
src/QpBound/QpBoundPetsc.o: src/PetscLinearAlgebra/PetscSpSymMatrixHandle.h
src/QpBound/QpBoundPetsc.o: src/PetscLinearAlgebra/PetscSparseStorageHandle.h
src/QpBound/QpBoundPetsc.o: src/PetscLinearAlgebra/PetscVectorHandle.h
src/QpBound/QpBoundPetsc.o: src/PetscLinearAlgebra/PetscSpSymMatrix.h
src/QpBound/QpBoundPetsc.o: src/Abstract/DoubleMatrix.h
src/QpBound/QpBoundPetsc.o: src/Abstract/DoubleMatrixHandle.h
src/QpBound/QpBoundPetsc.o: src/PetscLinearAlgebra/PetscSparseStorage.h
src/QpBound/QpBoundPetsc.o: src/PetscLinearAlgebra/PetscVector.h
src/QpBound/QpBoundPetsc.o: /usr/include/assert.h /usr/include/features.h
src/QpBound/QpBoundPetsc.o: /usr/include/bits/predefs.h
src/QpBound/QpBoundPetsc.o: /usr/include/sys/cdefs.h
src/QpBound/QpBoundPetsc.o: /usr/include/bits/wordsize.h
src/QpBound/QpBoundPetsc.o: /usr/include/gnu/stubs.h
src/QpBound/QpBoundPetsc.o: /usr/include/gnu/stubs-32.h
src/QpBound/QpBoundPetsc.o: src/Vector/OoqpVector.h
src/QpBound/QpBoundPetsc.o: src/PetscLinearAlgebra/PetscLinearAlgebraPackage.h
src/QpBound/QpBoundPetsc.o: src/Abstract/LinearAlgebraPackage.h
src/QpBound/QpBoundPetsc.o: src/QpBound/QpBoundLinsys.h
src/QpBound/QpBoundPetsc.o: src/Abstract/LinearSystem.h
src/QpBound/QpBoundPetsc.o: src/QpBound/QpBoundData.h src/Abstract/Data.h
src/QpBound/QpBoundPetsc.o: src/QpBound/QpBoundVars.h
src/QpBound/QpBoundPetsc.o: src/Abstract/Variables.h
src/QpBound/QpBoundPetsc.o: src/QpBound/QpBoundResiduals.h
src/QpBound/QpBoundPetsc.o: src/Abstract/Residuals.h
src/QpBound/QpBoundPetscMehrotraDriver.o: src/QpBound/QpBoundData.h
src/QpBound/QpBoundPetscMehrotraDriver.o: src/Abstract/Data.h
src/QpBound/QpBoundPetscMehrotraDriver.o: src/Abstract/DoubleMatrixHandle.h
src/QpBound/QpBoundPetscMehrotraDriver.o: src/Abstract/SmartPointer.h
src/QpBound/QpBoundPetscMehrotraDriver.o: src/Abstract/IotrRefCount.h
src/QpBound/QpBoundPetscMehrotraDriver.o: src/Vector/OoqpVectorHandle.h
src/QpBound/QpBoundPetscMehrotraDriver.o: src/Vector/OoqpVector.h
src/QpBound/QpBoundPetscMehrotraDriver.o: src/QpBound/QpBoundVars.h
src/QpBound/QpBoundPetscMehrotraDriver.o: src/Abstract/Variables.h
src/QpBound/QpBoundPetscMehrotraDriver.o: src/QpBound/QpBoundResiduals.h
src/QpBound/QpBoundPetscMehrotraDriver.o: src/Abstract/Residuals.h
src/QpBound/QpBoundPetscMehrotraDriver.o: src/QpSolvers/MehrotraSolver.h
src/QpBound/QpBoundPetscMehrotraDriver.o: src/Abstract/Solver.h
src/QpBound/QpBoundPetscMehrotraDriver.o: src/Abstract/OoqpMonitor.h
src/QpBound/QpBoundPetscMehrotraDriver.o: src/Abstract/OoqpMonitorData.h
src/QpBound/QpBoundPetscMehrotraDriver.o: src/QpBound/QpBoundPetsc.h
src/QpBound/QpBoundPetscMehrotraDriver.o: src/QpBound/QpBound.h
src/QpBound/QpBoundPetscMehrotraDriver.o: src/Abstract/ProblemFormulation.h
src/QpBound/QpBoundPetscMehrotraDriver.o: src/Readers/MpsReader.h
src/QpBound/QpBoundPetscMehrotraDriver.o: src/Vector/SimpleVectorHandle.h
src/QpBound/QpBoundPetscMehrotraDriver.o: src/Readers/hash.h
src/QpBound/QpBoundResiduals.o: src/QpBound/QpBoundResiduals.h
src/QpBound/QpBoundResiduals.o: src/Abstract/Residuals.h
src/QpBound/QpBoundResiduals.o: src/Vector/OoqpVectorHandle.h
src/QpBound/QpBoundResiduals.o: src/Abstract/IotrRefCount.h
src/QpBound/QpBoundResiduals.o: src/Abstract/SmartPointer.h
src/QpBound/QpBoundResiduals.o: src/QpBound/QpBoundData.h src/Abstract/Data.h
src/QpBound/QpBoundResiduals.o: src/Abstract/DoubleMatrixHandle.h
src/QpBound/QpBoundResiduals.o: src/Vector/OoqpVector.h
src/QpBound/QpBoundResiduals.o: src/QpBound/QpBoundVars.h
src/QpBound/QpBoundResiduals.o: src/Abstract/Variables.h
src/QpBound/QpBoundResiduals.o: src/QpBound/QpBound.h
src/QpBound/QpBoundResiduals.o: src/Abstract/ProblemFormulation.h
src/QpBound/QpBoundVars.o: src/QpBound/QpBoundVars.h src/Abstract/Variables.h
src/QpBound/QpBoundVars.o: src/Vector/OoqpVectorHandle.h
src/QpBound/QpBoundVars.o: src/Abstract/IotrRefCount.h
src/QpBound/QpBoundVars.o: src/Abstract/SmartPointer.h
src/QpBound/QpBoundVars.o: src/QpBound/QpBoundData.h src/Abstract/Data.h
src/QpBound/QpBoundVars.o: src/Abstract/DoubleMatrixHandle.h
src/QpBound/QpBoundVars.o: src/Vector/OoqpVector.h src/QpBound/QpBound.h
src/QpBound/QpBoundVars.o: src/Abstract/ProblemFormulation.h
src/QpExample/QpExampleData.o: src/QpExample/QpExampleData.h
src/QpExample/QpExampleData.o: src/Abstract/Data.h
src/QpExample/QpExampleData.o: src/Abstract/DoubleMatrixHandle.h
src/QpExample/QpExampleData.o: src/Abstract/SmartPointer.h
src/QpExample/QpExampleData.o: src/Abstract/IotrRefCount.h
src/QpExample/QpExampleData.o: src/DenseLinearAlgebra/DenseSymMatrixHandle.h
src/QpExample/QpExampleData.o: src/Vector/OoqpVectorHandle.h
src/QpExample/QpExampleData.o: src/Vector/OoqpVector.h
src/QpExample/QpExampleData.o: src/Abstract/DoubleMatrix.h
src/QpExample/QpExampleData.o: src/Abstract/LinearAlgebraPackage.h
src/QpExample/QpExampleData.o: src/DenseLinearAlgebra/DenseGenMatrix.h
src/QpExample/QpExampleData.o: src/DenseLinearAlgebra/DenseStorage.h
src/QpExample/QpExampleData.o: src/DenseLinearAlgebra/DenseStorageHandle.h
src/QpExample/QpExampleData.o: src/DenseLinearAlgebra/DenseGenMatrixHandle.h
src/QpExample/QpExampleData.o: src/DenseLinearAlgebra/DenseSymMatrix.h
src/QpExample/QpExampleData.o: src/Vector/SimpleVector.h
src/QpExample/QpExampleData.o: src/Vector/SimpleVectorHandle.h
src/QpExample/QpExampleDense.o: src/QpExample/QpExampleDense.h
src/QpExample/QpExampleDense.o: src/Abstract/ProblemFormulation.h
src/QpExample/QpExampleDense.o: src/QpExample/QpExampleData.h
src/QpExample/QpExampleDense.o: src/Abstract/Data.h
src/QpExample/QpExampleDense.o: src/Abstract/DoubleMatrixHandle.h
src/QpExample/QpExampleDense.o: src/Abstract/SmartPointer.h
src/QpExample/QpExampleDense.o: src/Abstract/IotrRefCount.h
src/QpExample/QpExampleDense.o: src/DenseLinearAlgebra/DenseSymMatrixHandle.h
src/QpExample/QpExampleDense.o: src/Vector/OoqpVectorHandle.h
src/QpExample/QpExampleDense.o: src/Vector/OoqpVector.h
src/QpExample/QpExampleDense.o: src/Abstract/DoubleMatrix.h
src/QpExample/QpExampleDense.o: src/QpExample/QpExampleVars.h
src/QpExample/QpExampleDense.o: src/Abstract/Variables.h
src/QpExample/QpExampleDense.o: src/QpExample/QpExampleResids.h
src/QpExample/QpExampleDense.o: src/Abstract/Residuals.h
src/QpExample/QpExampleDense.o: src/QpExample/QpExampleDenseLinsys.h
src/QpExample/QpExampleDense.o: src/Abstract/LinearSystem.h
src/QpExample/QpExampleDense.o: src/DenseLinearAlgebra/DeSymIndefSolver.h
src/QpExample/QpExampleDense.o: src/Abstract/DoubleLinearSolver.h
src/QpExample/QpExampleDense.o: src/DenseLinearAlgebra/DenseStorageHandle.h
src/QpExample/QpExampleDense.o: src/Vector/SimpleVectorHandle.h
src/QpExample/QpExampleDense.o: src/DenseLinearAlgebra/DenseSymMatrix.h
src/QpExample/QpExampleDense.o: src/DenseLinearAlgebra/DenseStorage.h
src/QpExample/QpExampleDense.o: src/DenseLinearAlgebra/DenseLinearAlgebraPackage.h
src/QpExample/QpExampleDense.o: src/Abstract/LinearAlgebraPackage.h
src/QpExample/QpExampleDenseLinsys.o: src/QpExample/QpExampleDenseLinsys.h
src/QpExample/QpExampleDenseLinsys.o: src/Abstract/LinearSystem.h
src/QpExample/QpExampleDenseLinsys.o: src/Vector/OoqpVectorHandle.h
src/QpExample/QpExampleDenseLinsys.o: src/Abstract/IotrRefCount.h
src/QpExample/QpExampleDenseLinsys.o: src/Abstract/SmartPointer.h
src/QpExample/QpExampleDenseLinsys.o: src/DenseLinearAlgebra/DenseSymMatrixHandle.h
src/QpExample/QpExampleDenseLinsys.o: /usr/include/memory.h
src/QpExample/QpExampleDenseLinsys.o: /usr/include/features.h
src/QpExample/QpExampleDenseLinsys.o: /usr/include/bits/predefs.h
src/QpExample/QpExampleDenseLinsys.o: /usr/include/sys/cdefs.h
src/QpExample/QpExampleDenseLinsys.o: /usr/include/bits/wordsize.h
src/QpExample/QpExampleDenseLinsys.o: /usr/include/gnu/stubs.h
src/QpExample/QpExampleDenseLinsys.o: /usr/include/gnu/stubs-32.h
src/QpExample/QpExampleDenseLinsys.o: /usr/include/string.h
src/QpExample/QpExampleDenseLinsys.o: /usr/include/xlocale.h
src/QpExample/QpExampleDenseLinsys.o: src/QpExample/QpExampleData.h
src/QpExample/QpExampleDenseLinsys.o: src/Abstract/Data.h
src/QpExample/QpExampleDenseLinsys.o: src/Abstract/DoubleMatrixHandle.h
src/QpExample/QpExampleDenseLinsys.o: src/Vector/OoqpVector.h
src/QpExample/QpExampleDenseLinsys.o: src/Abstract/DoubleMatrix.h
src/QpExample/QpExampleDenseLinsys.o: src/QpExample/QpExampleVars.h
src/QpExample/QpExampleDenseLinsys.o: src/Abstract/Variables.h
src/QpExample/QpExampleDenseLinsys.o: src/QpExample/QpExampleResids.h
src/QpExample/QpExampleDenseLinsys.o: src/Abstract/Residuals.h
src/QpExample/QpExampleDenseLinsys.o: src/Abstract/DoubleLinearSolver.h
src/QpExample/QpExampleDenseLinsys.o: src/DenseLinearAlgebra/DeSymIndefSolver.h
src/QpExample/QpExampleDenseLinsys.o: src/DenseLinearAlgebra/DenseStorageHandle.h
src/QpExample/QpExampleDenseLinsys.o: src/Vector/SimpleVectorHandle.h
src/QpExample/QpExampleDenseLinsys.o: src/DenseLinearAlgebra/DenseSymMatrix.h
src/QpExample/QpExampleDenseLinsys.o: src/DenseLinearAlgebra/DenseStorage.h
src/QpExample/QpExampleDenseLinsys.o: src/DenseLinearAlgebra/DenseGenMatrix.h
src/QpExample/QpExampleDenseLinsys.o: src/DenseLinearAlgebra/DenseGenMatrixHandle.h
src/QpExample/QpExampleDenseLinsys.o: src/Vector/SimpleVector.h
src/QpExample/QpExampleDenseLinsys.o: src/Abstract/LinearAlgebraPackage.h
src/QpExample/QpExampleGondzioDriver.o: src/Abstract/DoubleMatrix.h
src/QpExample/QpExampleGondzioDriver.o: src/Vector/OoqpVectorHandle.h
src/QpExample/QpExampleGondzioDriver.o: src/Abstract/IotrRefCount.h
src/QpExample/QpExampleGondzioDriver.o: src/Abstract/SmartPointer.h
src/QpExample/QpExampleGondzioDriver.o: src/Abstract/DoubleMatrixHandle.h
src/QpExample/QpExampleGondzioDriver.o: src/Vector/OoqpVector.h
src/QpExample/QpExampleGondzioDriver.o: src/Abstract/Data.h
src/QpExample/QpExampleGondzioDriver.o: src/Abstract/Variables.h
src/QpExample/QpExampleGondzioDriver.o: src/Abstract/Residuals.h
src/QpExample/QpExampleGondzioDriver.o: src/QpSolvers/GondzioSolver.h
src/QpExample/QpExampleGondzioDriver.o: src/Abstract/Solver.h
src/QpExample/QpExampleGondzioDriver.o: src/QpExample/QpExampleDense.h
src/QpExample/QpExampleGondzioDriver.o: src/Abstract/ProblemFormulation.h
src/QpExample/QpExampleResids.o: src/Abstract/DoubleMatrix.h
src/QpExample/QpExampleResids.o: src/Vector/OoqpVectorHandle.h
src/QpExample/QpExampleResids.o: src/Abstract/IotrRefCount.h
src/QpExample/QpExampleResids.o: src/Abstract/SmartPointer.h
src/QpExample/QpExampleResids.o: src/Abstract/DoubleMatrixHandle.h
src/QpExample/QpExampleResids.o: src/QpExample/QpExampleResids.h
src/QpExample/QpExampleResids.o: src/Abstract/Residuals.h
src/QpExample/QpExampleResids.o: src/QpExample/QpExampleData.h
src/QpExample/QpExampleResids.o: src/Abstract/Data.h
src/QpExample/QpExampleResids.o: src/DenseLinearAlgebra/DenseSymMatrixHandle.h
src/QpExample/QpExampleResids.o: src/Vector/OoqpVector.h
src/QpExample/QpExampleResids.o: src/QpExample/QpExampleVars.h
src/QpExample/QpExampleResids.o: src/Abstract/Variables.h
src/QpExample/QpExampleResids.o: src/Abstract/LinearAlgebraPackage.h
src/QpExample/QpExampleVars.o: src/QpExample/QpExampleVars.h
src/QpExample/QpExampleVars.o: src/Abstract/Variables.h
src/QpExample/QpExampleVars.o: src/Abstract/DoubleMatrix.h
src/QpExample/QpExampleVars.o: src/Vector/OoqpVectorHandle.h
src/QpExample/QpExampleVars.o: src/Abstract/IotrRefCount.h
src/QpExample/QpExampleVars.o: src/Abstract/SmartPointer.h
src/QpExample/QpExampleVars.o: src/Abstract/DoubleMatrixHandle.h
src/QpExample/QpExampleVars.o: src/QpExample/QpExampleData.h
src/QpExample/QpExampleVars.o: src/Abstract/Data.h
src/QpExample/QpExampleVars.o: src/DenseLinearAlgebra/DenseSymMatrixHandle.h
src/QpExample/QpExampleVars.o: src/Vector/OoqpVector.h /usr/include/memory.h
src/QpExample/QpExampleVars.o: /usr/include/features.h
src/QpExample/QpExampleVars.o: /usr/include/bits/predefs.h
src/QpExample/QpExampleVars.o: /usr/include/sys/cdefs.h
src/QpExample/QpExampleVars.o: /usr/include/bits/wordsize.h
src/QpExample/QpExampleVars.o: /usr/include/gnu/stubs.h
src/QpExample/QpExampleVars.o: /usr/include/gnu/stubs-32.h
src/QpExample/QpExampleVars.o: /usr/include/string.h /usr/include/xlocale.h
src/QpExample/QpExampleVars.o: src/Abstract/LinearAlgebraPackage.h
src/QpGen/QpGen.o: src/QpGen/QpGen.h src/Abstract/ProblemFormulation.h
src/QpGen/QpGen.o: src/QpGen/QpGenData.h src/Abstract/Data.h
src/QpGen/QpGen.o: src/Vector/OoqpVectorHandle.h src/Abstract/IotrRefCount.h
src/QpGen/QpGen.o: src/Abstract/SmartPointer.h src/Vector/OoqpVector.h
src/QpGen/QpGen.o: src/Abstract/DoubleMatrixHandle.h src/QpGen/QpGenVars.h
src/QpGen/QpGen.o: src/Abstract/Variables.h src/QpGen/QpGenResiduals.h
src/QpGen/QpGen.o: src/Abstract/Residuals.h
src/QpGen/QpGenData.o: src/QpGen/QpGenData.h src/Abstract/Data.h
src/QpGen/QpGenData.o: src/Vector/OoqpVectorHandle.h
src/QpGen/QpGenData.o: src/Abstract/IotrRefCount.h
src/QpGen/QpGenData.o: src/Abstract/SmartPointer.h src/Vector/OoqpVector.h
src/QpGen/QpGenData.o: src/Abstract/DoubleMatrixHandle.h
src/QpGen/QpGenData.o: src/QpGen/QpGenVars.h src/Abstract/Variables.h
src/QpGen/QpGenData.o: src/Abstract/DoubleMatrix.h src/Vector/SimpleVector.h
src/QpGen/QpGenData.o: src/Vector/SimpleVectorHandle.h
src/QpGen/QpGenData.o: src/Abstract/LinearAlgebraPackage.h
src/QpGen/QpGenData.o: src/Readers/MpsReader.h src/Readers/hash.h
src/QpGen/QpGenDense.o: src/QpGen/QpGenDense.h src/QpGen/QpGen.h
src/QpGen/QpGenDense.o: src/Abstract/ProblemFormulation.h
src/QpGen/QpGenDense.o: src/Vector/OoqpVectorHandle.h
src/QpGen/QpGenDense.o: src/Abstract/IotrRefCount.h
src/QpGen/QpGenDense.o: src/Abstract/SmartPointer.h
src/QpGen/QpGenDense.o: src/DenseLinearAlgebra/DenseLinearAlgebraPackage.h
src/QpGen/QpGenDense.o: src/Abstract/LinearAlgebraPackage.h
src/QpGen/QpGenDense.o: src/Abstract/DoubleMatrixHandle.h
src/QpGen/QpGenDense.o: src/DenseLinearAlgebra/DenseSymMatrix.h
src/QpGen/QpGenDense.o: src/DenseLinearAlgebra/DenseStorage.h
src/QpGen/QpGenDense.o: src/Abstract/DoubleMatrix.h
src/QpGen/QpGenDense.o: src/DenseLinearAlgebra/DenseStorageHandle.h
src/QpGen/QpGenDense.o: src/DenseLinearAlgebra/DenseSymMatrixHandle.h
src/QpGen/QpGenDense.o: src/DenseLinearAlgebra/DenseGenMatrix.h
src/QpGen/QpGenDense.o: src/DenseLinearAlgebra/DenseGenMatrixHandle.h
src/QpGen/QpGenDense.o: src/Vector/SimpleVector.h src/Vector/OoqpVector.h
src/QpGen/QpGenDense.o: src/Vector/SimpleVectorHandle.h
src/QpGen/QpGenDense.o: src/DenseLinearAlgebra/DeSymIndefSolver.h
src/QpGen/QpGenDense.o: src/Abstract/DoubleLinearSolver.h
src/QpGen/QpGenDense.o: src/QpGen/QpGenData.h src/Abstract/Data.h
src/QpGen/QpGenDense.o: src/QpGen/QpGenVars.h src/Abstract/Variables.h
src/QpGen/QpGenDense.o: src/QpGen/QpGenDenseLinsys.h src/QpGen/QpGenLinsys.h
src/QpGen/QpGenDense.o: src/Abstract/LinearSystem.h
src/QpGen/QpGenDenseGondzioDriver.o: src/QpGen/QpGenDriver.h
src/QpGen/QpGenDenseGondzioDriver.o: /usr/include/sys/time.h
src/QpGen/QpGenDenseGondzioDriver.o: /usr/include/features.h
src/QpGen/QpGenDenseGondzioDriver.o: /usr/include/bits/predefs.h
src/QpGen/QpGenDenseGondzioDriver.o: /usr/include/sys/cdefs.h
src/QpGen/QpGenDenseGondzioDriver.o: /usr/include/bits/wordsize.h
src/QpGen/QpGenDenseGondzioDriver.o: /usr/include/gnu/stubs.h
src/QpGen/QpGenDenseGondzioDriver.o: /usr/include/gnu/stubs-32.h
src/QpGen/QpGenDenseGondzioDriver.o: /usr/include/bits/types.h
src/QpGen/QpGenDenseGondzioDriver.o: /usr/include/bits/typesizes.h
src/QpGen/QpGenDenseGondzioDriver.o: /usr/include/time.h
src/QpGen/QpGenDenseGondzioDriver.o: /usr/include/bits/time.h
src/QpGen/QpGenDenseGondzioDriver.o: /usr/include/sys/select.h
src/QpGen/QpGenDenseGondzioDriver.o: /usr/include/bits/select.h
src/QpGen/QpGenDenseGondzioDriver.o: /usr/include/bits/sigset.h
src/QpGen/QpGenDenseGondzioDriver.o: /usr/include/sys/resource.h
src/QpGen/QpGenDenseGondzioDriver.o: /usr/include/bits/resource.h
src/QpGen/QpGenDenseGondzioDriver.o: /usr/include/unistd.h
src/QpGen/QpGenDenseGondzioDriver.o: /usr/include/bits/posix_opt.h
src/QpGen/QpGenDenseGondzioDriver.o: /usr/include/bits/environments.h
src/QpGen/QpGenDenseGondzioDriver.o: /usr/include/bits/confname.h
src/QpGen/QpGenDenseGondzioDriver.o: /usr/include/getopt.h
src/QpGen/QpGenDenseGondzioDriver.o: src/QpGen/QpGenVars.h
src/QpGen/QpGenDenseGondzioDriver.o: src/Abstract/Variables.h
src/QpGen/QpGenDenseGondzioDriver.o: src/Vector/OoqpVectorHandle.h
src/QpGen/QpGenDenseGondzioDriver.o: src/Abstract/IotrRefCount.h
src/QpGen/QpGenDenseGondzioDriver.o: src/Abstract/SmartPointer.h
src/QpGen/QpGenDenseGondzioDriver.o: src/QpGen/QpGenResiduals.h
src/QpGen/QpGenDenseGondzioDriver.o: src/Abstract/Residuals.h
src/QpGen/QpGenDenseGondzioDriver.o: src/Readers/MpsReader.h
src/QpGen/QpGenDenseGondzioDriver.o: src/Abstract/DoubleMatrixHandle.h
src/QpGen/QpGenDenseGondzioDriver.o: src/Vector/SimpleVectorHandle.h
src/QpGen/QpGenDenseGondzioDriver.o: src/Readers/hash.h
src/QpGen/QpGenDenseGondzioDriver.o: src/Vector/SimpleVector.h
src/QpGen/QpGenDenseGondzioDriver.o: src/Vector/OoqpVector.h
src/QpGen/QpGenDenseGondzioDriver.o: src/Abstract/Status.h
src/QpGen/QpGenDenseGondzioDriver.o: src/QpGen/QpGenData.h
src/QpGen/QpGenDenseGondzioDriver.o: src/Abstract/Data.h
src/QpGen/QpGenDenseGondzioDriver.o: src/Abstract/OoqpVersion.h
src/QpGen/QpGenDenseGondzioDriver.o: src/QpGen/QpGenDense.h src/QpGen/QpGen.h
src/QpGen/QpGenDenseGondzioDriver.o: src/Abstract/ProblemFormulation.h
src/QpGen/QpGenDenseGondzioDriver.o: src/QpSolvers/GondzioSolver.h
src/QpGen/QpGenDenseGondzioDriver.o: src/Abstract/Solver.h
src/QpGen/QpGenDenseLinsys.o: src/QpGen/QpGenDenseLinsys.h
src/QpGen/QpGenDenseLinsys.o: src/QpGen/QpGenLinsys.h
src/QpGen/QpGenDenseLinsys.o: src/Abstract/LinearSystem.h
src/QpGen/QpGenDenseLinsys.o: src/Abstract/DoubleMatrixHandle.h
src/QpGen/QpGenDenseLinsys.o: src/Abstract/SmartPointer.h
src/QpGen/QpGenDenseLinsys.o: src/Abstract/IotrRefCount.h
src/QpGen/QpGenDenseLinsys.o: src/Vector/OoqpVectorHandle.h
src/QpGen/QpGenDenseLinsys.o: src/Vector/OoqpVector.h
src/QpGen/QpGenDenseLinsys.o: src/DenseLinearAlgebra/DenseSymMatrixHandle.h
src/QpGen/QpGenDenseLinsys.o: src/QpGen/QpGenData.h src/Abstract/Data.h
src/QpGen/QpGenDenseLinsys.o: src/DenseLinearAlgebra/DenseSymMatrix.h
src/QpGen/QpGenDenseLinsys.o: src/DenseLinearAlgebra/DenseStorage.h
src/QpGen/QpGenDenseLinsys.o: src/Abstract/DoubleMatrix.h
src/QpGen/QpGenDenseLinsys.o: src/DenseLinearAlgebra/DenseStorageHandle.h
src/QpGen/QpGenDenseLinsys.o: src/Abstract/DoubleLinearSolver.h
src/QpGen/QpGenLinsys.o: src/QpGen/QpGenLinsys.h src/Abstract/LinearSystem.h
src/QpGen/QpGenLinsys.o: src/Abstract/DoubleMatrixHandle.h
src/QpGen/QpGenLinsys.o: src/Abstract/SmartPointer.h
src/QpGen/QpGenLinsys.o: src/Abstract/IotrRefCount.h
src/QpGen/QpGenLinsys.o: src/Vector/OoqpVectorHandle.h
src/QpGen/QpGenLinsys.o: src/Vector/OoqpVector.h src/QpGen/QpGenData.h
src/QpGen/QpGenLinsys.o: src/Abstract/Data.h src/QpGen/QpGenResiduals.h
src/QpGen/QpGenLinsys.o: src/Abstract/Residuals.h src/QpGen/QpGenVars.h
src/QpGen/QpGenLinsys.o: src/Abstract/Variables.h src/Abstract/DoubleMatrix.h
src/QpGen/QpGenLinsys.o: src/Abstract/DoubleLinearSolver.h
src/QpGen/QpGenLinsys.o: src/Vector/SimpleVector.h
src/QpGen/QpGenLinsys.o: src/Vector/SimpleVectorHandle.h
src/QpGen/QpGenLinsys.o: src/Abstract/LinearAlgebraPackage.h
src/QpGen/QpGenLinsys.o: src/QpGen/QpGen.h src/Abstract/ProblemFormulation.h
src/QpGen/QpGenResiduals.o: src/QpGen/QpGenResiduals.h
src/QpGen/QpGenResiduals.o: src/Abstract/Residuals.h
src/QpGen/QpGenResiduals.o: src/Vector/OoqpVectorHandle.h
src/QpGen/QpGenResiduals.o: src/Abstract/IotrRefCount.h
src/QpGen/QpGenResiduals.o: src/Abstract/SmartPointer.h src/QpGen/QpGenVars.h
src/QpGen/QpGenResiduals.o: src/Abstract/Variables.h src/QpGen/QpGenData.h
src/QpGen/QpGenResiduals.o: src/Abstract/Data.h src/Vector/OoqpVector.h
src/QpGen/QpGenResiduals.o: src/Abstract/DoubleMatrixHandle.h
src/QpGen/QpGenResiduals.o: src/Abstract/LinearAlgebraPackage.h
src/QpGen/QpGenSparseGondzioDriver.o: src/QpGen/QpGenDriver.h
src/QpGen/QpGenSparseGondzioDriver.o: /usr/include/sys/time.h
src/QpGen/QpGenSparseGondzioDriver.o: /usr/include/features.h
src/QpGen/QpGenSparseGondzioDriver.o: /usr/include/bits/predefs.h
src/QpGen/QpGenSparseGondzioDriver.o: /usr/include/sys/cdefs.h
src/QpGen/QpGenSparseGondzioDriver.o: /usr/include/bits/wordsize.h
src/QpGen/QpGenSparseGondzioDriver.o: /usr/include/gnu/stubs.h
src/QpGen/QpGenSparseGondzioDriver.o: /usr/include/gnu/stubs-32.h
src/QpGen/QpGenSparseGondzioDriver.o: /usr/include/bits/types.h
src/QpGen/QpGenSparseGondzioDriver.o: /usr/include/bits/typesizes.h
src/QpGen/QpGenSparseGondzioDriver.o: /usr/include/time.h
src/QpGen/QpGenSparseGondzioDriver.o: /usr/include/bits/time.h
src/QpGen/QpGenSparseGondzioDriver.o: /usr/include/sys/select.h
src/QpGen/QpGenSparseGondzioDriver.o: /usr/include/bits/select.h
src/QpGen/QpGenSparseGondzioDriver.o: /usr/include/bits/sigset.h
src/QpGen/QpGenSparseGondzioDriver.o: /usr/include/sys/resource.h
src/QpGen/QpGenSparseGondzioDriver.o: /usr/include/bits/resource.h
src/QpGen/QpGenSparseGondzioDriver.o: /usr/include/unistd.h
src/QpGen/QpGenSparseGondzioDriver.o: /usr/include/bits/posix_opt.h
src/QpGen/QpGenSparseGondzioDriver.o: /usr/include/bits/environments.h
src/QpGen/QpGenSparseGondzioDriver.o: /usr/include/bits/confname.h
src/QpGen/QpGenSparseGondzioDriver.o: /usr/include/getopt.h
src/QpGen/QpGenSparseGondzioDriver.o: src/QpGen/QpGenVars.h
src/QpGen/QpGenSparseGondzioDriver.o: src/Abstract/Variables.h
src/QpGen/QpGenSparseGondzioDriver.o: src/Vector/OoqpVectorHandle.h
src/QpGen/QpGenSparseGondzioDriver.o: src/Abstract/IotrRefCount.h
src/QpGen/QpGenSparseGondzioDriver.o: src/Abstract/SmartPointer.h
src/QpGen/QpGenSparseGondzioDriver.o: src/QpGen/QpGenResiduals.h
src/QpGen/QpGenSparseGondzioDriver.o: src/Abstract/Residuals.h
src/QpGen/QpGenSparseGondzioDriver.o: src/Readers/MpsReader.h
src/QpGen/QpGenSparseGondzioDriver.o: src/Abstract/DoubleMatrixHandle.h
src/QpGen/QpGenSparseGondzioDriver.o: src/Vector/SimpleVectorHandle.h
src/QpGen/QpGenSparseGondzioDriver.o: src/Readers/hash.h
src/QpGen/QpGenSparseGondzioDriver.o: src/Vector/SimpleVector.h
src/QpGen/QpGenSparseGondzioDriver.o: src/Vector/OoqpVector.h
src/QpGen/QpGenSparseGondzioDriver.o: src/Abstract/Status.h
src/QpGen/QpGenSparseGondzioDriver.o: src/QpGen/QpGenData.h
src/QpGen/QpGenSparseGondzioDriver.o: src/Abstract/Data.h
src/QpGen/QpGenSparseGondzioDriver.o: src/Abstract/OoqpVersion.h
src/QpGen/QpGenSparseGondzioDriver.o: src/QpGen/QpGenSparseMa27.h
src/QpGen/QpGenSparseGondzioDriver.o: src/QpGen/QpGenSparseSeq.h
src/QpGen/QpGenSparseGondzioDriver.o: src/QpGen/QpGen.h
src/QpGen/QpGenSparseGondzioDriver.o: src/Abstract/ProblemFormulation.h
src/QpGen/QpGenSparseGondzioDriver.o: src/QpSolvers/GondzioSolver.h
src/QpGen/QpGenSparseGondzioDriver.o: src/Abstract/Solver.h
src/QpGen/QpGenSparseLinsys.o: src/QpGen/QpGenSparseLinsys.h
src/QpGen/QpGenSparseLinsys.o: src/QpGen/QpGenLinsys.h
src/QpGen/QpGenSparseLinsys.o: src/Abstract/LinearSystem.h
src/QpGen/QpGenSparseLinsys.o: src/Abstract/DoubleMatrixHandle.h
src/QpGen/QpGenSparseLinsys.o: src/Abstract/SmartPointer.h
src/QpGen/QpGenSparseLinsys.o: src/Abstract/IotrRefCount.h
src/QpGen/QpGenSparseLinsys.o: src/Vector/OoqpVectorHandle.h
src/QpGen/QpGenSparseLinsys.o: src/Vector/OoqpVector.h
src/QpGen/QpGenSparseLinsys.o: src/SparseLinearAlgebra/SparseSymMatrixHandle.h
src/QpGen/QpGenSparseLinsys.o: src/Abstract/DoubleLinearSolver.h
src/QpGen/QpGenSparseLinsys.o: src/SparseLinearAlgebra/SparseSymMatrix.h
src/QpGen/QpGenSparseLinsys.o: src/Abstract/DoubleMatrix.h
src/QpGen/QpGenSparseLinsys.o: src/SparseLinearAlgebra/SparseStorage.h
src/QpGen/QpGenSparseLinsys.o: src/SparseLinearAlgebra/SparseStorageHandle.h
src/QpGen/QpGenSparseMa27.o: src/QpGen/QpGenSparseMa27.h
src/QpGen/QpGenSparseMa27.o: src/QpGen/QpGenSparseSeq.h src/QpGen/QpGen.h
src/QpGen/QpGenSparseMa27.o: src/Abstract/ProblemFormulation.h
src/QpGen/QpGenSparseMa27.o: src/QpGen/QpGenSparseLinsys.h
src/QpGen/QpGenSparseMa27.o: src/QpGen/QpGenLinsys.h
src/QpGen/QpGenSparseMa27.o: src/Abstract/LinearSystem.h
src/QpGen/QpGenSparseMa27.o: src/Abstract/DoubleMatrixHandle.h
src/QpGen/QpGenSparseMa27.o: src/Abstract/SmartPointer.h
src/QpGen/QpGenSparseMa27.o: src/Abstract/IotrRefCount.h
src/QpGen/QpGenSparseMa27.o: src/Vector/OoqpVectorHandle.h
src/QpGen/QpGenSparseMa27.o: src/Vector/OoqpVector.h
src/QpGen/QpGenSparseMa27.o: src/SparseLinearAlgebra/SparseSymMatrixHandle.h
src/QpGen/QpGenSparseMa27.o: src/QpGen/QpGenData.h src/Abstract/Data.h
src/QpGen/QpGenSparseMa27.o: src/SparseLinearAlgebra/SparseLinearAlgebraPackage.h
src/QpGen/QpGenSparseMa27.o: src/Abstract/LinearAlgebraPackage.h
src/QpGen/QpGenSparseMa27.o: src/SparseLinearAlgebra/SparseSymMatrix.h
src/QpGen/QpGenSparseMa27.o: src/Abstract/DoubleMatrix.h
src/QpGen/QpGenSparseMa27.o: src/SparseLinearAlgebra/SparseStorage.h
src/QpGen/QpGenSparseMa27.o: src/SparseLinearAlgebra/SparseStorageHandle.h
src/QpGen/QpGenSparseMa27.o: src/Vector/SimpleVector.h
src/QpGen/QpGenSparseMa27.o: src/Vector/SimpleVectorHandle.h
src/QpGen/QpGenSparseMa27.o: src/LinearSolvers/Ma27Solver/Ma27Solver.h
src/QpGen/QpGenSparseMa27.o: src/Abstract/DoubleLinearSolver.h
src/QpGen/QpGenSparseMa57.o: src/QpGen/QpGenSparseMa57.h
src/QpGen/QpGenSparseMa57.o: src/QpGen/QpGenSparseSeq.h src/QpGen/QpGen.h
src/QpGen/QpGenSparseMa57.o: src/Abstract/ProblemFormulation.h
src/QpGen/QpGenSparseMa57.o: src/QpGen/QpGenSparseLinsys.h
src/QpGen/QpGenSparseMa57.o: src/QpGen/QpGenLinsys.h
src/QpGen/QpGenSparseMa57.o: src/Abstract/LinearSystem.h
src/QpGen/QpGenSparseMa57.o: src/Abstract/DoubleMatrixHandle.h
src/QpGen/QpGenSparseMa57.o: src/Abstract/SmartPointer.h
src/QpGen/QpGenSparseMa57.o: src/Abstract/IotrRefCount.h
src/QpGen/QpGenSparseMa57.o: src/Vector/OoqpVectorHandle.h
src/QpGen/QpGenSparseMa57.o: src/Vector/OoqpVector.h
src/QpGen/QpGenSparseMa57.o: src/SparseLinearAlgebra/SparseSymMatrixHandle.h
src/QpGen/QpGenSparseMa57.o: src/QpGen/QpGenData.h src/Abstract/Data.h
src/QpGen/QpGenSparseMa57.o: src/SparseLinearAlgebra/SparseLinearAlgebraPackage.h
src/QpGen/QpGenSparseMa57.o: src/Abstract/LinearAlgebraPackage.h
src/QpGen/QpGenSparseMa57.o: src/SparseLinearAlgebra/SparseSymMatrix.h
src/QpGen/QpGenSparseMa57.o: src/Abstract/DoubleMatrix.h
src/QpGen/QpGenSparseMa57.o: src/SparseLinearAlgebra/SparseStorage.h
src/QpGen/QpGenSparseMa57.o: src/SparseLinearAlgebra/SparseStorageHandle.h
src/QpGen/QpGenSparseMa57.o: src/Vector/SimpleVector.h
src/QpGen/QpGenSparseMa57.o: src/Vector/SimpleVectorHandle.h
src/QpGen/QpGenSparseMa57.o: src/LinearSolvers/Ma57Solver/Ma57Solver.h
src/QpGen/QpGenSparseMa57.o: src/Abstract/DoubleLinearSolver.h
src/QpGen/QpGenSparseMa57GondzioDriver.o: src/QpGen/QpGenDriver.h
src/QpGen/QpGenSparseMa57GondzioDriver.o: /usr/include/sys/time.h
src/QpGen/QpGenSparseMa57GondzioDriver.o: /usr/include/features.h
src/QpGen/QpGenSparseMa57GondzioDriver.o: /usr/include/bits/predefs.h
src/QpGen/QpGenSparseMa57GondzioDriver.o: /usr/include/sys/cdefs.h
src/QpGen/QpGenSparseMa57GondzioDriver.o: /usr/include/bits/wordsize.h
src/QpGen/QpGenSparseMa57GondzioDriver.o: /usr/include/gnu/stubs.h
src/QpGen/QpGenSparseMa57GondzioDriver.o: /usr/include/gnu/stubs-32.h
src/QpGen/QpGenSparseMa57GondzioDriver.o: /usr/include/bits/types.h
src/QpGen/QpGenSparseMa57GondzioDriver.o: /usr/include/bits/typesizes.h
src/QpGen/QpGenSparseMa57GondzioDriver.o: /usr/include/time.h
src/QpGen/QpGenSparseMa57GondzioDriver.o: /usr/include/bits/time.h
src/QpGen/QpGenSparseMa57GondzioDriver.o: /usr/include/sys/select.h
src/QpGen/QpGenSparseMa57GondzioDriver.o: /usr/include/bits/select.h
src/QpGen/QpGenSparseMa57GondzioDriver.o: /usr/include/bits/sigset.h
src/QpGen/QpGenSparseMa57GondzioDriver.o: /usr/include/sys/resource.h
src/QpGen/QpGenSparseMa57GondzioDriver.o: /usr/include/bits/resource.h
src/QpGen/QpGenSparseMa57GondzioDriver.o: /usr/include/unistd.h
src/QpGen/QpGenSparseMa57GondzioDriver.o: /usr/include/bits/posix_opt.h
src/QpGen/QpGenSparseMa57GondzioDriver.o: /usr/include/bits/environments.h
src/QpGen/QpGenSparseMa57GondzioDriver.o: /usr/include/bits/confname.h
src/QpGen/QpGenSparseMa57GondzioDriver.o: /usr/include/getopt.h
src/QpGen/QpGenSparseMa57GondzioDriver.o: src/QpGen/QpGenVars.h
src/QpGen/QpGenSparseMa57GondzioDriver.o: src/Abstract/Variables.h
src/QpGen/QpGenSparseMa57GondzioDriver.o: src/Vector/OoqpVectorHandle.h
src/QpGen/QpGenSparseMa57GondzioDriver.o: src/Abstract/IotrRefCount.h
src/QpGen/QpGenSparseMa57GondzioDriver.o: src/Abstract/SmartPointer.h
src/QpGen/QpGenSparseMa57GondzioDriver.o: src/QpGen/QpGenResiduals.h
src/QpGen/QpGenSparseMa57GondzioDriver.o: src/Abstract/Residuals.h
src/QpGen/QpGenSparseMa57GondzioDriver.o: src/Readers/MpsReader.h
src/QpGen/QpGenSparseMa57GondzioDriver.o: src/Abstract/DoubleMatrixHandle.h
src/QpGen/QpGenSparseMa57GondzioDriver.o: src/Vector/SimpleVectorHandle.h
src/QpGen/QpGenSparseMa57GondzioDriver.o: src/Readers/hash.h
src/QpGen/QpGenSparseMa57GondzioDriver.o: src/Vector/SimpleVector.h
src/QpGen/QpGenSparseMa57GondzioDriver.o: src/Vector/OoqpVector.h
src/QpGen/QpGenSparseMa57GondzioDriver.o: src/Abstract/Status.h
src/QpGen/QpGenSparseMa57GondzioDriver.o: src/QpGen/QpGenData.h
src/QpGen/QpGenSparseMa57GondzioDriver.o: src/Abstract/Data.h
src/QpGen/QpGenSparseMa57GondzioDriver.o: src/Abstract/OoqpVersion.h
src/QpGen/QpGenSparseMa57GondzioDriver.o: src/QpGen/QpGenSparseMa57.h
src/QpGen/QpGenSparseMa57GondzioDriver.o: src/QpGen/QpGenSparseSeq.h
src/QpGen/QpGenSparseMa57GondzioDriver.o: src/QpGen/QpGen.h
src/QpGen/QpGenSparseMa57GondzioDriver.o: src/Abstract/ProblemFormulation.h
src/QpGen/QpGenSparseMa57GondzioDriver.o: src/QpSolvers/GondzioSolver.h
src/QpGen/QpGenSparseMa57GondzioDriver.o: src/Abstract/Solver.h
src/QpGen/QpGenSparseMehrotraDriver.o: src/QpGen/QpGenDriver.h
src/QpGen/QpGenSparseMehrotraDriver.o: /usr/include/sys/time.h
src/QpGen/QpGenSparseMehrotraDriver.o: /usr/include/features.h
src/QpGen/QpGenSparseMehrotraDriver.o: /usr/include/bits/predefs.h
src/QpGen/QpGenSparseMehrotraDriver.o: /usr/include/sys/cdefs.h
src/QpGen/QpGenSparseMehrotraDriver.o: /usr/include/bits/wordsize.h
src/QpGen/QpGenSparseMehrotraDriver.o: /usr/include/gnu/stubs.h
src/QpGen/QpGenSparseMehrotraDriver.o: /usr/include/gnu/stubs-32.h
src/QpGen/QpGenSparseMehrotraDriver.o: /usr/include/bits/types.h
src/QpGen/QpGenSparseMehrotraDriver.o: /usr/include/bits/typesizes.h
src/QpGen/QpGenSparseMehrotraDriver.o: /usr/include/time.h
src/QpGen/QpGenSparseMehrotraDriver.o: /usr/include/bits/time.h
src/QpGen/QpGenSparseMehrotraDriver.o: /usr/include/sys/select.h
src/QpGen/QpGenSparseMehrotraDriver.o: /usr/include/bits/select.h
src/QpGen/QpGenSparseMehrotraDriver.o: /usr/include/bits/sigset.h
src/QpGen/QpGenSparseMehrotraDriver.o: /usr/include/sys/resource.h
src/QpGen/QpGenSparseMehrotraDriver.o: /usr/include/bits/resource.h
src/QpGen/QpGenSparseMehrotraDriver.o: /usr/include/unistd.h
src/QpGen/QpGenSparseMehrotraDriver.o: /usr/include/bits/posix_opt.h
src/QpGen/QpGenSparseMehrotraDriver.o: /usr/include/bits/environments.h
src/QpGen/QpGenSparseMehrotraDriver.o: /usr/include/bits/confname.h
src/QpGen/QpGenSparseMehrotraDriver.o: /usr/include/getopt.h
src/QpGen/QpGenSparseMehrotraDriver.o: src/QpGen/QpGenVars.h
src/QpGen/QpGenSparseMehrotraDriver.o: src/Abstract/Variables.h
src/QpGen/QpGenSparseMehrotraDriver.o: src/Vector/OoqpVectorHandle.h
src/QpGen/QpGenSparseMehrotraDriver.o: src/Abstract/IotrRefCount.h
src/QpGen/QpGenSparseMehrotraDriver.o: src/Abstract/SmartPointer.h
src/QpGen/QpGenSparseMehrotraDriver.o: src/QpGen/QpGenResiduals.h
src/QpGen/QpGenSparseMehrotraDriver.o: src/Abstract/Residuals.h
src/QpGen/QpGenSparseMehrotraDriver.o: src/Readers/MpsReader.h
src/QpGen/QpGenSparseMehrotraDriver.o: src/Abstract/DoubleMatrixHandle.h
src/QpGen/QpGenSparseMehrotraDriver.o: src/Vector/SimpleVectorHandle.h
src/QpGen/QpGenSparseMehrotraDriver.o: src/Readers/hash.h
src/QpGen/QpGenSparseMehrotraDriver.o: src/Vector/SimpleVector.h
src/QpGen/QpGenSparseMehrotraDriver.o: src/Vector/OoqpVector.h
src/QpGen/QpGenSparseMehrotraDriver.o: src/Abstract/Status.h
src/QpGen/QpGenSparseMehrotraDriver.o: src/QpGen/QpGenData.h
src/QpGen/QpGenSparseMehrotraDriver.o: src/Abstract/Data.h
src/QpGen/QpGenSparseMehrotraDriver.o: src/Abstract/OoqpVersion.h
src/QpGen/QpGenSparseMehrotraDriver.o: src/QpGen/QpGenSparseMa27.h
src/QpGen/QpGenSparseMehrotraDriver.o: src/QpGen/QpGenSparseSeq.h
src/QpGen/QpGenSparseMehrotraDriver.o: src/QpGen/QpGen.h
src/QpGen/QpGenSparseMehrotraDriver.o: src/Abstract/ProblemFormulation.h
src/QpGen/QpGenSparseMehrotraDriver.o: src/QpSolvers/MehrotraSolver.h
src/QpGen/QpGenSparseMehrotraDriver.o: src/Abstract/Solver.h
src/QpGen/QpGenSparseMehrotraDriver.o: src/Abstract/OoqpMonitor.h
src/QpGen/QpGenSparseMehrotraDriver.o: src/Abstract/OoqpMonitorData.h
src/QpGen/QpGenSparseOblio.o: src/QpGen/QpGenSparseOblio.h
src/QpGen/QpGenSparseOblio.o: src/QpGen/QpGenSparseSeq.h src/QpGen/QpGen.h
src/QpGen/QpGenSparseOblio.o: src/Abstract/ProblemFormulation.h
src/QpGen/QpGenSparseOblio.o: src/QpGen/QpGenData.h src/Abstract/Data.h
src/QpGen/QpGenSparseOblio.o: src/Vector/OoqpVectorHandle.h
src/QpGen/QpGenSparseOblio.o: src/Abstract/IotrRefCount.h
src/QpGen/QpGenSparseOblio.o: src/Abstract/SmartPointer.h
src/QpGen/QpGenSparseOblio.o: src/Vector/OoqpVector.h
src/QpGen/QpGenSparseOblio.o: src/Abstract/DoubleMatrixHandle.h
src/QpGen/QpGenSparseOblio.o: src/QpGen/QpGenSparseLinsys.h
src/QpGen/QpGenSparseOblio.o: src/QpGen/QpGenLinsys.h
src/QpGen/QpGenSparseOblio.o: src/Abstract/LinearSystem.h
src/QpGen/QpGenSparseOblio.o: src/SparseLinearAlgebra/SparseSymMatrixHandle.h
src/QpGen/QpGenSparseOblio.o: src/SparseLinearAlgebra/SparseLinearAlgebraPackage.h
src/QpGen/QpGenSparseOblio.o: src/Abstract/LinearAlgebraPackage.h
src/QpGen/QpGenSparseOblio.o: src/SparseLinearAlgebra/SparseGenMatrix.h
src/QpGen/QpGenSparseOblio.o: src/Abstract/DoubleMatrix.h
src/QpGen/QpGenSparseOblio.o: src/SparseLinearAlgebra/SparseStorage.h
src/QpGen/QpGenSparseOblio.o: src/SparseLinearAlgebra/SparseStorageHandle.h
src/QpGen/QpGenSparseOblio.o: src/SparseLinearAlgebra/SparseGenMatrixHandle.h
src/QpGen/QpGenSparseOblio.o: src/Vector/SimpleVector.h
src/QpGen/QpGenSparseOblio.o: src/Vector/SimpleVectorHandle.h
src/QpGen/QpGenSparseOblio.o: src/SparseLinearAlgebra/SparseSymMatrix.h
src/QpGen/QpGenSparseSeq.o: src/QpGen/QpGenSparseSeq.h src/QpGen/QpGen.h
src/QpGen/QpGenSparseSeq.o: src/Abstract/ProblemFormulation.h
src/QpGen/QpGenSparseSeq.o: src/QpGen/QpGenData.h src/Abstract/Data.h
src/QpGen/QpGenSparseSeq.o: src/Vector/OoqpVectorHandle.h
src/QpGen/QpGenSparseSeq.o: src/Abstract/IotrRefCount.h
src/QpGen/QpGenSparseSeq.o: src/Abstract/SmartPointer.h
src/QpGen/QpGenSparseSeq.o: src/Vector/OoqpVector.h
src/QpGen/QpGenSparseSeq.o: src/Abstract/DoubleMatrixHandle.h
src/QpGen/QpGenSparseSeq.o: src/Vector/SimpleVector.h
src/QpGen/QpGenSparseSeq.o: src/Vector/SimpleVectorHandle.h
src/QpGen/QpGenSparseSeq.o: src/SparseLinearAlgebra/SparseGenMatrix.h
src/QpGen/QpGenSparseSeq.o: src/Abstract/DoubleMatrix.h
src/QpGen/QpGenSparseSeq.o: src/SparseLinearAlgebra/SparseStorage.h
src/QpGen/QpGenSparseSeq.o: src/SparseLinearAlgebra/SparseStorageHandle.h
src/QpGen/QpGenSparseSeq.o: src/SparseLinearAlgebra/SparseGenMatrixHandle.h
src/QpGen/QpGenSparseSeq.o: src/SparseLinearAlgebra/SparseSymMatrix.h
src/QpGen/QpGenSparseSeq.o: src/SparseLinearAlgebra/SparseSymMatrixHandle.h
src/QpGen/QpGenSparseSeq.o: src/SparseLinearAlgebra/SparseLinearAlgebraPackage.h
src/QpGen/QpGenSparseSeq.o: src/Abstract/LinearAlgebraPackage.h
src/QpGen/QpGenSparseSeq.o: src/QpGen/QpGenVars.h src/Abstract/Variables.h
src/QpGen/QpGenSparseSuperLu.o: src/QpGen/QpGenSparseSuperLu.h
src/QpGen/QpGenSparseSuperLu.o: src/QpGen/QpGenSparseSeq.h src/QpGen/QpGen.h
src/QpGen/QpGenSparseSuperLu.o: src/Abstract/ProblemFormulation.h
src/QpGen/QpGenSparseSuperLu.o: src/QpGen/QpGenSparseLinsys.h
src/QpGen/QpGenSparseSuperLu.o: src/QpGen/QpGenLinsys.h
src/QpGen/QpGenSparseSuperLu.o: src/Abstract/LinearSystem.h
src/QpGen/QpGenSparseSuperLu.o: src/Abstract/DoubleMatrixHandle.h
src/QpGen/QpGenSparseSuperLu.o: src/Abstract/SmartPointer.h
src/QpGen/QpGenSparseSuperLu.o: src/Abstract/IotrRefCount.h
src/QpGen/QpGenSparseSuperLu.o: src/Vector/OoqpVectorHandle.h
src/QpGen/QpGenSparseSuperLu.o: src/Vector/OoqpVector.h
src/QpGen/QpGenSparseSuperLu.o: src/SparseLinearAlgebra/SparseSymMatrixHandle.h
src/QpGen/QpGenSparseSuperLu.o: src/QpGen/QpGenData.h src/Abstract/Data.h
src/QpGen/QpGenSparseSuperLu.o: src/SparseLinearAlgebra/SparseLinearAlgebraPackage.h
src/QpGen/QpGenSparseSuperLu.o: src/Abstract/LinearAlgebraPackage.h
src/QpGen/QpGenSparseSuperLu.o: src/SparseLinearAlgebra/SparseGenMatrix.h
src/QpGen/QpGenSparseSuperLu.o: src/Abstract/DoubleMatrix.h
src/QpGen/QpGenSparseSuperLu.o: src/SparseLinearAlgebra/SparseStorage.h
src/QpGen/QpGenSparseSuperLu.o: src/SparseLinearAlgebra/SparseStorageHandle.h
src/QpGen/QpGenSparseSuperLu.o: src/SparseLinearAlgebra/SparseGenMatrixHandle.h
src/QpGen/QpGenSparseSuperLu.o: src/Vector/SimpleVector.h
src/QpGen/QpGenSparseSuperLu.o: src/Vector/SimpleVectorHandle.h
src/QpGen/QpGenVars.o: src/QpGen/QpGenVars.h src/Abstract/Variables.h
src/QpGen/QpGenVars.o: src/Vector/OoqpVectorHandle.h
src/QpGen/QpGenVars.o: src/Abstract/IotrRefCount.h
src/QpGen/QpGenVars.o: src/Abstract/SmartPointer.h src/QpGen/QpGenData.h
src/QpGen/QpGenVars.o: src/Abstract/Data.h src/Vector/OoqpVector.h
src/QpGen/QpGenVars.o: src/Abstract/DoubleMatrixHandle.h
src/QpGen/QpGenVars.o: src/QpGen/QpGenResiduals.h src/Abstract/Residuals.h
src/QpGen/QpGenVars.o: src/QpGen/QpGenLinsys.h src/Abstract/LinearSystem.h
src/QpGen/QpGenVars.o: src/Vector/SimpleVector.h
src/QpGen/QpGenVars.o: src/Vector/SimpleVectorHandle.h
src/QpGen/QpGenVars.o: src/Readers/MpsReader.h src/Readers/hash.h
src/QpGen/QpGenVars.o: src/Abstract/LinearAlgebraPackage.h
src/QpSolvers/GondzioSolver.o: src/QpSolvers/GondzioSolver.h
src/QpSolvers/GondzioSolver.o: src/Abstract/Solver.h src/Abstract/Variables.h
src/QpSolvers/GondzioSolver.o: src/Abstract/Residuals.h
src/QpSolvers/GondzioSolver.o: src/Abstract/LinearSystem.h
src/QpSolvers/GondzioSolver.o: src/Abstract/Status.h src/Abstract/Data.h
src/QpSolvers/GondzioSolver.o: src/Abstract/ProblemFormulation.h
src/QpSolvers/MehrotraSolver.o: src/QpSolvers/MehrotraSolver.h
src/QpSolvers/MehrotraSolver.o: src/Abstract/Solver.h
src/QpSolvers/MehrotraSolver.o: src/Abstract/OoqpMonitor.h
src/QpSolvers/MehrotraSolver.o: src/Abstract/OoqpMonitorData.h
src/QpSolvers/MehrotraSolver.o: src/Abstract/Variables.h
src/QpSolvers/MehrotraSolver.o: src/Abstract/Residuals.h
src/QpSolvers/MehrotraSolver.o: src/Abstract/LinearSystem.h
src/QpSolvers/MehrotraSolver.o: src/Abstract/Status.h src/Abstract/Data.h
src/QpSolvers/MehrotraSolver.o: src/Abstract/ProblemFormulation.h
src/QpSolvers/MehrotraSolver.o: src/Vector/OoqpVector.h
src/QpSolvers/MehrotraSolver.o: src/Abstract/IotrRefCount.h
src/QpSolvers/MehrotraSolver.o: src/Vector/OoqpVectorHandle.h
src/QpSolvers/MehrotraSolver.o: src/Abstract/SmartPointer.h
src/QpSolvers/MehrotraSolver.o: src/Abstract/DoubleMatrix.h
src/QpSolvers/MehrotraSolver.o: src/Abstract/DoubleMatrixHandle.h
src/Readers/MpsReader.o: src/Readers/MpsReader.h
src/Readers/MpsReader.o: src/Vector/OoqpVectorHandle.h
src/Readers/MpsReader.o: src/Abstract/IotrRefCount.h
src/Readers/MpsReader.o: src/Abstract/SmartPointer.h
src/Readers/MpsReader.o: src/Abstract/DoubleMatrixHandle.h
src/Readers/MpsReader.o: src/Vector/SimpleVectorHandle.h src/Readers/hash.h
src/Readers/MpsReader.o: src/Vector/OoqpVector.h src/Abstract/DoubleMatrix.h
src/Readers/MpsReader.o: src/Vector/SimpleVector.h
src/SparseLinearAlgebra/SparseGenMatrix.o: src/SparseLinearAlgebra/SparseGenMatrix.h
src/SparseLinearAlgebra/SparseGenMatrix.o: src/Vector/OoqpVectorHandle.h
src/SparseLinearAlgebra/SparseGenMatrix.o: src/Abstract/IotrRefCount.h
src/SparseLinearAlgebra/SparseGenMatrix.o: src/Abstract/SmartPointer.h
src/SparseLinearAlgebra/SparseGenMatrix.o: src/Abstract/DoubleMatrix.h
src/SparseLinearAlgebra/SparseGenMatrix.o: src/Abstract/DoubleMatrixHandle.h
src/SparseLinearAlgebra/SparseGenMatrix.o: src/SparseLinearAlgebra/SparseStorage.h
src/SparseLinearAlgebra/SparseGenMatrix.o: src/SparseLinearAlgebra/SparseStorageHandle.h
src/SparseLinearAlgebra/SparseGenMatrix.o: src/SparseLinearAlgebra/SparseGenMatrixHandle.h
src/SparseLinearAlgebra/SparseGenMatrix.o: src/Vector/SimpleVector.h
src/SparseLinearAlgebra/SparseGenMatrix.o: src/Vector/OoqpVector.h
src/SparseLinearAlgebra/SparseGenMatrix.o: src/Vector/SimpleVectorHandle.h
src/SparseLinearAlgebra/SparseGenMatrix.o: src/Abstract/DoubleMatrixTypes.h
src/SparseLinearAlgebra/SparseLinearAlgebraPackage.o: src/SparseLinearAlgebra/SparseLinearAlgebraPackage.h
src/SparseLinearAlgebra/SparseLinearAlgebraPackage.o: src/Abstract/LinearAlgebraPackage.h
src/SparseLinearAlgebra/SparseLinearAlgebraPackage.o: src/Abstract/DoubleMatrixHandle.h
src/SparseLinearAlgebra/SparseLinearAlgebraPackage.o: src/Abstract/SmartPointer.h
src/SparseLinearAlgebra/SparseLinearAlgebraPackage.o: src/Abstract/IotrRefCount.h
src/SparseLinearAlgebra/SparseLinearAlgebraPackage.o: src/Vector/OoqpVectorHandle.h
src/SparseLinearAlgebra/SparseLinearAlgebraPackage.o: src/SparseLinearAlgebra/SparseGenMatrix.h
src/SparseLinearAlgebra/SparseLinearAlgebraPackage.o: src/Abstract/DoubleMatrix.h
src/SparseLinearAlgebra/SparseLinearAlgebraPackage.o: src/SparseLinearAlgebra/SparseStorage.h
src/SparseLinearAlgebra/SparseLinearAlgebraPackage.o: src/SparseLinearAlgebra/SparseStorageHandle.h
src/SparseLinearAlgebra/SparseLinearAlgebraPackage.o: src/SparseLinearAlgebra/SparseGenMatrixHandle.h
src/SparseLinearAlgebra/SparseLinearAlgebraPackage.o: src/SparseLinearAlgebra/SparseSymMatrix.h
src/SparseLinearAlgebra/SparseLinearAlgebraPackage.o: src/Vector/OoqpVector.h
src/SparseLinearAlgebra/SparseLinearAlgebraPackage.o: src/SparseLinearAlgebra/SparseSymMatrixHandle.h
src/SparseLinearAlgebra/SparseLinearAlgebraPackage.o: src/Vector/SimpleVector.h
src/SparseLinearAlgebra/SparseLinearAlgebraPackage.o: src/Vector/SimpleVectorHandle.h
src/SparseLinearAlgebra/SparseStorage.o: src/SparseLinearAlgebra/SparseStorage.h
src/SparseLinearAlgebra/SparseStorage.o: src/Abstract/DoubleMatrix.h
src/SparseLinearAlgebra/SparseStorage.o: src/Vector/OoqpVectorHandle.h
src/SparseLinearAlgebra/SparseStorage.o: src/Abstract/IotrRefCount.h
src/SparseLinearAlgebra/SparseStorage.o: src/Abstract/SmartPointer.h
src/SparseLinearAlgebra/SparseStorage.o: src/Abstract/DoubleMatrixHandle.h
src/SparseLinearAlgebra/SparseStorage.o: src/SparseLinearAlgebra/SparseStorageHandle.h
src/SparseLinearAlgebra/SparseStorage.o: src/Vector/OoqpVector.h
src/SparseLinearAlgebra/SparseStorage.o: src/Vector/SimpleVector.h
src/SparseLinearAlgebra/SparseStorage.o: src/Vector/SimpleVectorHandle.h
src/SparseLinearAlgebra/SparseSymMatrix.o: src/SparseLinearAlgebra/SparseSymMatrix.h
src/SparseLinearAlgebra/SparseSymMatrix.o: src/Abstract/DoubleMatrix.h
src/SparseLinearAlgebra/SparseSymMatrix.o: src/Vector/OoqpVectorHandle.h
src/SparseLinearAlgebra/SparseSymMatrix.o: src/Abstract/IotrRefCount.h
src/SparseLinearAlgebra/SparseSymMatrix.o: src/Abstract/SmartPointer.h
src/SparseLinearAlgebra/SparseSymMatrix.o: src/Abstract/DoubleMatrixHandle.h
src/SparseLinearAlgebra/SparseSymMatrix.o: src/SparseLinearAlgebra/SparseStorage.h
src/SparseLinearAlgebra/SparseSymMatrix.o: src/SparseLinearAlgebra/SparseStorageHandle.h
src/SparseLinearAlgebra/SparseSymMatrix.o: src/Vector/OoqpVector.h
src/SparseLinearAlgebra/SparseSymMatrix.o: src/SparseLinearAlgebra/SparseSymMatrixHandle.h
src/SparseLinearAlgebra/SparseSymMatrix.o: src/Vector/SimpleVector.h
src/SparseLinearAlgebra/SparseSymMatrix.o: src/Vector/SimpleVectorHandle.h
src/SparseLinearAlgebra/SparseSymMatrix.o: src/Abstract/DoubleMatrixTypes.h
src/Svm/Svm.o: src/Svm/Svm.h src/Abstract/ProblemFormulation.h
src/Svm/Svm.o: src/Svm/SvmData.h src/Abstract/Data.h
src/Svm/Svm.o: src/DenseLinearAlgebra/DenseGenMatrixHandle.h
src/Svm/Svm.o: src/Abstract/IotrRefCount.h src/Abstract/SmartPointer.h
src/Svm/Svm.o: src/DenseLinearAlgebra/DenseSymMatrixHandle.h
src/Svm/Svm.o: src/Vector/SimpleVectorHandle.h src/Vector/OoqpVectorHandle.h
src/Svm/Svm.o: src/Svm/SvmResiduals.h src/Abstract/Residuals.h
src/Svm/Svm.o: src/Svm/SvmVars.h src/Abstract/Variables.h src/Svm/SvmLinsys.h
src/Svm/Svm.o: src/Abstract/LinearSystem.h src/Abstract/DoubleMatrix.h
src/Svm/Svm.o: src/Abstract/DoubleMatrixHandle.h
src/Svm/SvmData.o: src/Svm/SvmData.h src/Abstract/Data.h
src/Svm/SvmData.o: src/DenseLinearAlgebra/DenseGenMatrixHandle.h
src/Svm/SvmData.o: src/Abstract/IotrRefCount.h src/Abstract/SmartPointer.h
src/Svm/SvmData.o: src/DenseLinearAlgebra/DenseSymMatrixHandle.h
src/Svm/SvmData.o: src/Vector/SimpleVectorHandle.h
src/Svm/SvmData.o: src/Vector/OoqpVectorHandle.h
src/Svm/SvmData.o: src/DenseLinearAlgebra/DenseGenMatrix.h
src/Svm/SvmData.o: src/Abstract/DoubleMatrix.h
src/Svm/SvmData.o: src/Abstract/DoubleMatrixHandle.h
src/Svm/SvmData.o: src/DenseLinearAlgebra/DenseStorage.h
src/Svm/SvmData.o: src/DenseLinearAlgebra/DenseStorageHandle.h
src/Svm/SvmData.o: src/Vector/SimpleVector.h src/Vector/OoqpVector.h
src/Svm/SvmData.o: src/Abstract/LinearAlgebraPackage.h
src/Svm/SvmGondzioDriver.o: src/Svm/SvmData.h src/Abstract/Data.h
src/Svm/SvmGondzioDriver.o: src/DenseLinearAlgebra/DenseGenMatrixHandle.h
src/Svm/SvmGondzioDriver.o: src/Abstract/IotrRefCount.h
src/Svm/SvmGondzioDriver.o: src/Abstract/SmartPointer.h
src/Svm/SvmGondzioDriver.o: src/DenseLinearAlgebra/DenseSymMatrixHandle.h
src/Svm/SvmGondzioDriver.o: src/Vector/SimpleVectorHandle.h
src/Svm/SvmGondzioDriver.o: src/Vector/OoqpVectorHandle.h src/Svm/SvmVars.h
src/Svm/SvmGondzioDriver.o: src/Abstract/Variables.h src/Svm/SvmResiduals.h
src/Svm/SvmGondzioDriver.o: src/Abstract/Residuals.h src/Svm/SvmLinsys.h
src/Svm/SvmGondzioDriver.o: src/Abstract/LinearSystem.h
src/Svm/SvmGondzioDriver.o: src/QpSolvers/GondzioSolver.h
src/Svm/SvmGondzioDriver.o: src/Abstract/Solver.h src/Svm/Svm.h
src/Svm/SvmGondzioDriver.o: src/Abstract/ProblemFormulation.h
src/Svm/SvmGondzioDriver.o: src/Vector/SimpleVector.h src/Vector/OoqpVector.h
src/Svm/SvmLinsys.o: src/Svm/SvmLinsys.h src/Abstract/LinearSystem.h
src/Svm/SvmLinsys.o: src/Vector/SimpleVectorHandle.h
src/Svm/SvmLinsys.o: src/Vector/OoqpVectorHandle.h
src/Svm/SvmLinsys.o: src/Abstract/IotrRefCount.h src/Abstract/SmartPointer.h
src/Svm/SvmLinsys.o: src/DenseLinearAlgebra/DenseSymMatrixHandle.h
src/Svm/SvmLinsys.o: src/DenseLinearAlgebra/DenseSymMatrix.h
src/Svm/SvmLinsys.o: src/DenseLinearAlgebra/DenseStorage.h
src/Svm/SvmLinsys.o: src/Abstract/DoubleMatrix.h
src/Svm/SvmLinsys.o: src/Abstract/DoubleMatrixHandle.h
src/Svm/SvmLinsys.o: src/DenseLinearAlgebra/DenseStorageHandle.h
src/Svm/SvmLinsys.o: src/DenseLinearAlgebra/DenseGenMatrix.h
src/Svm/SvmLinsys.o: src/DenseLinearAlgebra/DenseGenMatrixHandle.h
src/Svm/SvmLinsys.o: src/Svm/SvmData.h src/Abstract/Data.h src/Svm/SvmVars.h
src/Svm/SvmLinsys.o: src/Abstract/Variables.h src/Svm/SvmResiduals.h
src/Svm/SvmLinsys.o: src/Abstract/Residuals.h
src/Svm/SvmLinsys.o: src/Abstract/DoubleLinearSolver.h
src/Svm/SvmLinsys.o: src/DenseLinearAlgebra/DeSymPSDSolver.h
src/Svm/SvmLinsys.o: src/Vector/SimpleVector.h src/Vector/OoqpVector.h
src/Svm/SvmResiduals.o: src/Svm/SvmResiduals.h src/Abstract/Residuals.h
src/Svm/SvmResiduals.o: src/Vector/SimpleVectorHandle.h
src/Svm/SvmResiduals.o: src/Vector/OoqpVectorHandle.h
src/Svm/SvmResiduals.o: src/Abstract/IotrRefCount.h
src/Svm/SvmResiduals.o: src/Abstract/SmartPointer.h src/Svm/SvmData.h
src/Svm/SvmResiduals.o: src/Abstract/Data.h
src/Svm/SvmResiduals.o: src/DenseLinearAlgebra/DenseGenMatrixHandle.h
src/Svm/SvmResiduals.o: src/DenseLinearAlgebra/DenseSymMatrixHandle.h
src/Svm/SvmResiduals.o: src/Svm/SvmVars.h src/Abstract/Variables.h
src/Svm/SvmResiduals.o: src/Vector/SimpleVector.h src/Vector/OoqpVector.h
src/Svm/SvmResiduals.o: src/Abstract/DoubleMatrix.h
src/Svm/SvmResiduals.o: src/Abstract/DoubleMatrixHandle.h
src/Svm/SvmVars.o: src/Svm/SvmVars.h src/Vector/SimpleVectorHandle.h
src/Svm/SvmVars.o: src/Vector/OoqpVectorHandle.h src/Abstract/IotrRefCount.h
src/Svm/SvmVars.o: src/Abstract/SmartPointer.h src/Abstract/Variables.h
src/Svm/SvmVars.o: src/Svm/SvmData.h src/Abstract/Data.h
src/Svm/SvmVars.o: src/DenseLinearAlgebra/DenseGenMatrixHandle.h
src/Svm/SvmVars.o: src/DenseLinearAlgebra/DenseSymMatrixHandle.h
src/Svm/SvmVars.o: src/Vector/SimpleVector.h src/Vector/OoqpVector.h
src/Svm/SvmVars.o: src/Abstract/DoubleMatrix.h
src/Svm/SvmVars.o: src/Abstract/DoubleMatrixHandle.h
src/Vector/OoqpVector.o: src/Vector/OoqpVector.h src/Abstract/IotrRefCount.h
src/Vector/OoqpVector.o: src/Vector/OoqpVectorHandle.h
src/Vector/OoqpVector.o: src/Abstract/SmartPointer.h
src/Vector/SimpleVector.o: src/Vector/SimpleVector.h src/Vector/OoqpVector.h
src/Vector/SimpleVector.o: src/Abstract/IotrRefCount.h
src/Vector/SimpleVector.o: src/Vector/OoqpVectorHandle.h
src/Vector/SimpleVector.o: src/Abstract/SmartPointer.h
src/Vector/SimpleVector.o: src/Vector/SimpleVectorHandle.h
src/Vector/SimpleVector.o: src/Vector/VectorUtilities.h
src/Vector/SimpleVector.o: src/Utilities/OoqpBlas.h
src/Vector/VectorUtilities.o: src/Vector/VectorUtilities.h
src/LinearSolvers/Ma27Solver/Ma27Solver.o: src/LinearSolvers/Ma27Solver/Ma27Solver.h
src/LinearSolvers/Ma27Solver/Ma27Solver.o: src/Abstract/DoubleLinearSolver.h
src/LinearSolvers/Ma27Solver/Ma27Solver.o: src/Vector/OoqpVectorHandle.h
src/LinearSolvers/Ma27Solver/Ma27Solver.o: src/Abstract/IotrRefCount.h
src/LinearSolvers/Ma27Solver/Ma27Solver.o: src/Abstract/SmartPointer.h
src/LinearSolvers/Ma27Solver/Ma27Solver.o: src/SparseLinearAlgebra/SparseSymMatrixHandle.h
src/LinearSolvers/Ma27Solver/Ma27Solver.o: src/SparseLinearAlgebra/SparseStorageHandle.h
src/LinearSolvers/Ma27Solver/Ma27Solver.o: src/SparseLinearAlgebra/SparseStorage.h
src/LinearSolvers/Ma27Solver/Ma27Solver.o: src/Abstract/DoubleMatrix.h
src/LinearSolvers/Ma27Solver/Ma27Solver.o: src/Abstract/DoubleMatrixHandle.h
src/LinearSolvers/Ma27Solver/Ma27Solver.o: src/SparseLinearAlgebra/SparseSymMatrix.h
src/LinearSolvers/Ma27Solver/Ma27Solver.o: src/Vector/OoqpVector.h
src/LinearSolvers/Ma27Solver/Ma27Solver.o: src/Vector/SimpleVector.h
src/LinearSolvers/Ma27Solver/Ma27Solver.o: src/Vector/SimpleVectorHandle.h
src/LinearSolvers/Ma27Solver/Ma27Solver.o: /usr/include/sys/time.h
src/LinearSolvers/Ma27Solver/Ma27Solver.o: /usr/include/features.h
src/LinearSolvers/Ma27Solver/Ma27Solver.o: /usr/include/bits/predefs.h
src/LinearSolvers/Ma27Solver/Ma27Solver.o: /usr/include/sys/cdefs.h
src/LinearSolvers/Ma27Solver/Ma27Solver.o: /usr/include/bits/wordsize.h
src/LinearSolvers/Ma27Solver/Ma27Solver.o: /usr/include/gnu/stubs.h
src/LinearSolvers/Ma27Solver/Ma27Solver.o: /usr/include/gnu/stubs-32.h
src/LinearSolvers/Ma27Solver/Ma27Solver.o: /usr/include/bits/types.h
src/LinearSolvers/Ma27Solver/Ma27Solver.o: /usr/include/bits/typesizes.h
src/LinearSolvers/Ma27Solver/Ma27Solver.o: /usr/include/time.h
src/LinearSolvers/Ma27Solver/Ma27Solver.o: /usr/include/bits/time.h
src/LinearSolvers/Ma27Solver/Ma27Solver.o: /usr/include/sys/select.h
src/LinearSolvers/Ma27Solver/Ma27Solver.o: /usr/include/bits/select.h
src/LinearSolvers/Ma27Solver/Ma27Solver.o: /usr/include/bits/sigset.h
src/LinearSolvers/Ma27Solver/Ma27Solver.o: /usr/include/sys/resource.h
src/LinearSolvers/Ma27Solver/Ma27Solver.o: /usr/include/bits/resource.h
src/LinearSolvers/Ma27Solver/Ma27Solver.o: /usr/include/unistd.h
src/LinearSolvers/Ma27Solver/Ma27Solver.o: /usr/include/bits/posix_opt.h
src/LinearSolvers/Ma27Solver/Ma27Solver.o: /usr/include/bits/environments.h
src/LinearSolvers/Ma27Solver/Ma27Solver.o: /usr/include/bits/confname.h
src/LinearSolvers/Ma27Solver/Ma27Solver.o: /usr/include/getopt.h
src/LinearSolvers/Ma57Solver/Ma57Solver.o: src/LinearSolvers/Ma57Solver/Ma57Solver.h
src/LinearSolvers/Ma57Solver/Ma57Solver.o: src/Abstract/DoubleLinearSolver.h
src/LinearSolvers/Ma57Solver/Ma57Solver.o: src/Vector/OoqpVectorHandle.h
src/LinearSolvers/Ma57Solver/Ma57Solver.o: src/Abstract/IotrRefCount.h
src/LinearSolvers/Ma57Solver/Ma57Solver.o: src/Abstract/SmartPointer.h
src/LinearSolvers/Ma57Solver/Ma57Solver.o: src/SparseLinearAlgebra/SparseSymMatrixHandle.h
src/LinearSolvers/Ma57Solver/Ma57Solver.o: src/SparseLinearAlgebra/SparseStorageHandle.h
src/LinearSolvers/Ma57Solver/Ma57Solver.o: src/SparseLinearAlgebra/SparseStorage.h
src/LinearSolvers/Ma57Solver/Ma57Solver.o: src/Abstract/DoubleMatrix.h
src/LinearSolvers/Ma57Solver/Ma57Solver.o: src/Abstract/DoubleMatrixHandle.h
src/LinearSolvers/Ma57Solver/Ma57Solver.o: src/SparseLinearAlgebra/SparseSymMatrix.h
src/LinearSolvers/Ma57Solver/Ma57Solver.o: src/Vector/OoqpVector.h
src/LinearSolvers/Ma57Solver/Ma57Solver.o: src/Vector/SimpleVector.h
src/LinearSolvers/Ma57Solver/Ma57Solver.o: src/Vector/SimpleVectorHandle.h
src/LinearSolvers/Ma57Solver/Ma57Solver.o: /usr/include/sys/time.h
src/LinearSolvers/Ma57Solver/Ma57Solver.o: /usr/include/features.h
src/LinearSolvers/Ma57Solver/Ma57Solver.o: /usr/include/bits/predefs.h
src/LinearSolvers/Ma57Solver/Ma57Solver.o: /usr/include/sys/cdefs.h
src/LinearSolvers/Ma57Solver/Ma57Solver.o: /usr/include/bits/wordsize.h
src/LinearSolvers/Ma57Solver/Ma57Solver.o: /usr/include/gnu/stubs.h
src/LinearSolvers/Ma57Solver/Ma57Solver.o: /usr/include/gnu/stubs-32.h
src/LinearSolvers/Ma57Solver/Ma57Solver.o: /usr/include/bits/types.h
src/LinearSolvers/Ma57Solver/Ma57Solver.o: /usr/include/bits/typesizes.h
src/LinearSolvers/Ma57Solver/Ma57Solver.o: /usr/include/time.h
src/LinearSolvers/Ma57Solver/Ma57Solver.o: /usr/include/bits/time.h
src/LinearSolvers/Ma57Solver/Ma57Solver.o: /usr/include/sys/select.h
src/LinearSolvers/Ma57Solver/Ma57Solver.o: /usr/include/bits/select.h
src/LinearSolvers/Ma57Solver/Ma57Solver.o: /usr/include/bits/sigset.h
src/LinearSolvers/Ma57Solver/Ma57Solver.o: /usr/include/sys/resource.h
src/LinearSolvers/Ma57Solver/Ma57Solver.o: /usr/include/bits/resource.h
src/LinearSolvers/Ma57Solver/Ma57Solver.o: /usr/include/unistd.h
src/LinearSolvers/Ma57Solver/Ma57Solver.o: /usr/include/bits/posix_opt.h
src/LinearSolvers/Ma57Solver/Ma57Solver.o: /usr/include/bits/environments.h
src/LinearSolvers/Ma57Solver/Ma57Solver.o: /usr/include/bits/confname.h
src/LinearSolvers/Ma57Solver/Ma57Solver.o: /usr/include/getopt.h
