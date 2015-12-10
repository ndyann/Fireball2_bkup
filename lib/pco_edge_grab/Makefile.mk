# Makefile for programs in this directory

PCODIR = ../pco_include
PCOLIBDIR = ../pco_lib
CPCODIR = ../pco_classes


CFLAGS += -O -Wall -DLINUX -I$(PCODIR) -I$(CPCODIR)

CLSERLIB = ../siso_lib/libclserme4.so
FGLIB = ../siso_lib/libfglib5.so

DISPLIB = $(PCOLIBDIR)/libpcodisp.a
LOGLIB = $(PCOLIBDIR)/libpcolog.a
FILELIB = $(PCOLIBDIR)/libpcofile.a


HEADERS = $(PCODIR)/VersionNo.h $(CPCODIR)/Cpco_cl_com.h $(CPCODIR)/Cpco_me4.h 
#HEADERS = $(PCODIR)/VersionNo.h $(CPCODIR)/Cpco_cl_com.h $(CPCODIR)/Cpco_me4.h $(CPCODIR)/Cpco_me4_GS.h

CPCOCLASS_SRC = Cpco_cl_com.cpp Cpco_me4.cpp reorder_func.cpp
CPCOCLASS = $(addprefix $(CPCODIR)/,$(CPCOCLASS_SRC) )


TARGETS = pco_edge_grab pcox_edge_grab



all:		$(TARGETS) copy

clean:
		$(RM) *~ $(TARGETS)  $(CPCODIR)/*~  *.log *.o

copy:
		@cp $(TARGETS) ../bin

%.o:		%.cpp
		$(CXX) $(CFLAGS) -c $< -o $@

pco_%:		pco_%.cpp $(FGLIB) $(CLSERLIB) $(CPCOCLASS) $(FILELIB) $(LOGLIB) $(HEADERS)
		$(CXX) $(CFLAGS) $@.cpp $(CPCOCLASS) $(FILELIB) $(LOGLIB) $(FGLIB) $(CLSERLIB) -o $@	
		./pco_edge_grab


pcox_%:		pcox_%.cpp $(FGLIB) $(CLSERLIB) $(CPCOCLASS) $(DISPLIB) $(LOGLIB) $(HEADERS)
		$(CXX) $(CFLAGS) $@.cpp $(CPCOCLASS) $(DISPLIB) $(LOGLIB) $(FGLIB) $(CLSERLIB) -lX11 -lXext -lpthread -o $@

