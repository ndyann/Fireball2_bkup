# Makefile for programs in this directory
#INCLUDEDIR = /usr/include

PCODIR = ../pco_include
PCOLIBDIR = ../pco_lib
CPCODIR = ../pco_classes


CFLAGS += -O -Wall -DLINUX -I$(PCODIR) -I$(CPCODIR)

CLSERLIB = ../siso_lib/libclserme4.so
FGLIB = ../siso_lib/libfglib5.so

DISPLIB = $(PCOLIBDIR)/libpcodisp.a
LOGLIB = $(PCOLIBDIR)/libpcolog.a

HEADERS = $(PCODIR)/VersionNo.h $(CPCODIR)/Cpco_cl_com.h


TARGETS = pco_ser_con

CPCOCLASS_SRC = Cpco_cl_com.cpp
CPCOCLASS = $(addprefix $(CPCODIR)/,$(CPCOCLASS_SRC) )

PCOFILE = $(CPCODIR)/file12.cpp


all:		$(TARGETS) copy

clean:
		$(RM)  *~ $(TARGETS) $(CPCODIR)/*~  *.log *.o

copy:
		@cp $(TARGETS) ../bin

%.o:		%.cpp
		$(CXX) $(CFLAGS) -c $< -o $@

pco_%:		pco_%.cpp $(CLSERLIB) $(CPCOCLASS) $(HEADERS)
		$(CXX) $(CFLAGS) $@.cpp $(CPCOCLASS) $(LOGLIB) $(CLSERLIB) -o $@


