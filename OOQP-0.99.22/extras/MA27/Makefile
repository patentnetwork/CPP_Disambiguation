F77      = f95
FC       = $(F77)
FFLAGS   = -O

OBJS = fd05.o  id05.o  ma27.o za02.o

libMA27.a:  $(OBJS) Makefile
	ar ru $@ $(OBJS)

$(OBJS) : Makefile

