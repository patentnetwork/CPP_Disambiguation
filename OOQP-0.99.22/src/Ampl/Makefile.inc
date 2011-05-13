AMPLHOOKDIR = $(srcdir)/Ampl
AMPL_ENABLED = ampl_disabled # AMPL support has been disabled

AMPL_SOLVERS  := extras/ampl/solvers

ifeq ($(AMPL_ENABLED),ampl_enabled)
# Be very careful not to add a -I to the includes line
INCLUDES +=   $(patsubst %, -I%, $(AMPL_SOLVERS))
endif


OOQPAMPLOBJ = $(AMPLHOOKDIR)/qpgen-sparse-gondzio-ampl.o \
	      $(CQPGENSPARSEOBJ) \
	      $(libooqpgondzio_STATIC) $(libooqpgensparse_STATIC) \
	      $(libooqpsparse_STATIC) $(libooqpbase_STATIC) \
	      $(AMPLHOOKDIR)/funcadd0.o

