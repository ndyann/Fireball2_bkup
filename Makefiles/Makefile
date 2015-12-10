# Makefile for programs in this directory

CXX = g++

CODE_PATH = ./src
PCODIR = ./lib/pco_include
PCOLIBDIR = ./lib/pco_lib
CPCODIR = ./lib/pco_classes
SISOLIB = ./lib/siso_lib

C11FLAG += -std=c++0x
CFLAGS += -O -Wall -Wno-comments -DLINUX -I$(PCODIR) -I$(CPCODIR) -lrt

CLSERLIB = $(SISOLIB)/libclserme4.so
FGLIB = $(SISOLIB)/libfglib5.so
 
DISPLIB = $(PCOLIBDIR)/libpcodisp.a
LOGLIB = $(PCOLIBDIR)/libpcolog.a
FILELIB = $(PCOLIBDIR)/libpcofile.a

HEADERS = $(PCODIR)/VersionNo.h $(CPCODIR)/Cpco_cl_com.h $(CPCODIR)/Cpco_me4.h $(CPCODIR)/Cpco_me4_GS.h

CPCOCLASS_SRC = Cpco_cl_com.cpp Cpco_me4.cpp reorder_func.cpp
CPCOCLASS = $(addprefix $(CPCODIR)/,$(CPCOCLASS_SRC) )

SOURCES = $(CODE_PATH)/*.cpp

TARGETS = main ctemp zguide zfind zstatus

all:    clean $(TARGETS)

clean:
		@echo "Cleaning make (safety reasons)..."
		@$(RM) *~ bin/$(TARGETS)  $(CPCODIR)/*~  *.log *.o
        
%.o: src/%.cpp
		@echo -n "Compiling1 "$<"..."
		@$(CXX) $(C11FLAG) $(CFLAGS) src/log.cpp $< -c
		@echo " Done1"
        
%.o: src/%.c
		@echo -n "Compiling2 "$<"..."
		@$(CC) $(CFLAGS) $< -c
		@echo " Done2"
    
camera.o: $(CODE_PATH)/camera.cpp $(CODE_PATH)/camera.hpp
		@echo -n "Compiling3 "$<"..."
		@$(CXX) $(CFLAGS) $< -c
		@echo " Done3"

main: src/main.cpp camera.o log.o fitswrapper.o gps.o sun.o $(FGLIB) $(CLSERLIB) $(CPCOCLASS) $(FILELIB) $(LOGLIB) $(HEADERS)
		@echo -n "Compiling MINT..."
		@mkdir -p -v ./bin
		@$(CXX) $(C11FLAG) $(CFLAGS) $? -o bin/mint -I./src/ $(FILELIB) $(LOGLIB) $(FGLIB) $(CLSERLIB) -lcfitsio
		@echo " Done4"

ctemp: camera.o log.o src/ctemp.cpp $(FGLIB) $(CLSERLIB) $(CPCOCLASS) $(FILELIB) $(LOGLIB) $(HEADERS)
		@echo -n "Compiling CTemp..."
		@mkdir -p -v ./bin
		@$(CXX) $(CFLAGS) $? -o bin/ctemp -I./src/ $(FILELIB) $(LOGLIB) $(FGLIB) $(CLSERLIB)
		@echo " Done5"
		@echo "Type bin/mint to run program"

zguide: src/zguide.cpp camera.o log.o fitswrapper.o bloblist.o frameblob.o $(FGLIB) $(CLSERLIB) $(CPCOCLASS) $(FILELIB) $(LOGLIB) $(HEADERS)
		@echo -n "Compiling zguide..."
		@mkdir -p -v ./bin
		@$(CXX) $(CFLAGS) $? -o bin/zguide -I./src/ $(FILELIB) $(LOGLIB) $(FGLIB) $(CLSERLIB) -lcfitsio
		@echo " Done"

zfind: src/zfind.cpp camera.o log.o fitswrapper.o $(FGLIB) $(CLSERLIB) $(CPCOCLASS) $(FILELIB) $(LOGLIB) $(HEADERS)
		@echo -n "Compiling zfind..."
		@mkdir -p -v ./bin
		@$(CXX) $(CFLAGS) $? -o bin/zfind -I./src/ $(FILELIB) $(LOGLIB) $(FGLIB) $(CLSERLIB) -lcfitsio
		@echo " Done"

zstatus: src/zstatus.cpp  $(FGLIB) $(CLSERLIB) $(CPCOCLASS) $(FILELIB) $(LOGLIB) $(HEADERS)
		@echo -n "Compiling zstatus..."
		@mkdir -p -v ./bin
		@$(CXX) $(CFLAGS) $? -o bin/zstatus -I./src/ $(FILELIB) $(LOGLIB) $(FGLIB) $(CLSERLIB) -lcfitsio
		@echo " Done"

zperformance: src/zperformance.cpp  $(FGLIB) $(CLSERLIB) $(CPCOCLASS) $(FILELIB) $(LOGLIB) $(HEADERS)
		@echo -n "Compiling zperformance..."
		@mkdir -p -v ./bin
		@$(CXX) $(C11FLAG) $(CFLAGS) $? -o bin/zperformance -I./src/ $(FILELIB) $(LOGLIB) $(FGLIB) $(CLSERLIB) -lcfitsio
		@echo " Done"

testcam:	src/testcam.cpp fireball_camera.o   $(FGLIB) $(CLSERLIB) $(CPCOCLASS) $(FILELIB) $(LOGLIB) $(HEADERS)
		@echo "Compiling testcam..."
		@$(CXX) $(C11FLAG) $(CFLAGS) $? -o bin/testcam -I./src/ $(FILELIB) $(LOGLIB) $(FGLIB) $(CLSERLIB) 


simplecam:	src/simplecam.cpp   $(FGLIB) $(CLSERLIB) $(CPCOCLASS) $(FILELIB) $(LOGLIB) $(HEADERS)
		@echo "Compiling simplecam..."
		@$(CXX) $(C11FLAG) $(CFLAGS) $? -o bin/simplecam -I./src/ $(FILELIB) $(LOGLIB) $(FGLIB) $(CLSERLIB) 

testcam2:	src/testcam2.cpp fireball_camera2.o frameblob.o bloblist.o image_analysis.o  $(FGLIB) $(CLSERLIB) $(CPCOCLASS) $(FILELIB) $(LOGLIB) $(HEADERS)
		@echo "Compiling testcam2..."
		@$(CXX) $(C11FLAG) $(CFLAGS) $? -o bin/testcam2 -I./src/ $(FILELIB) $(LOGLIB) $(FGLIB) $(CLSERLIB) 


run:	clean main
		@echo "Running MINT..."
		@bash -l -c "if [ `pgrep mint | wc -l` -lt 2 ]; then ./bin/mint; else echo 'Instance already running!'; fi;"

run_zguide:	clean zguide
		@echo "Running zguide..."
		@bash -l -c "if [ `pgrep zguide | wc -l` -lt 2 ]; then ./bin/zguide; else echo 'Instance already running!'; fi;"

run_zfind:	clean zfind
		@echo "Running zfind..."
		@bash -l -c "if [ `pgrep zfind | wc -l` -lt 2 ]; then ./bin/zfind; else echo 'Instance already running!'; fi;"

run_testcam:	clean testcam
		@echo "Running testcam..."
		@bash -l -c "if [ `pgrep testcam | wc -l` -lt 2 ]; then ./bin/testcam; else echo 'Instance already running!'; fi;"

run_simplecam:	clean simplecam
		@echo "Running simplecam..."
		@bash -l -c "if [ `pgrep simplecam | wc -l` -lt 2 ]; then ./bin/simplecam; else echo 'Instance already running!'; fi;"


run_testcam2:	clean testcam2
		@echo "Running testcam2..."
		@bash -l -c "if [ `pgrep testcam2 | wc -l` -lt 2 ]; then ./bin/testcam2; else echo 'Instance already running!'; fi;"



run_zstatus:	clean zstatus
		@echo "Running zstatus..."
		@bash -l -c "if [ `pgrep zstatus | wc -l` -lt 2 ]; then ./bin/zstatus; else echo 'Instance already running!'; fi;"

run_zperformance:	clean zperformance
		@echo "Running zperformance..."
		@bash -l -c "if [ `pgrep zperformance | wc -l` -lt 2 ]; then ./bin/zperformance; else echo 'Instance already running!'; fi;"

run_all:
		@echo "Running all..."
		@bash -l -c "if [ `pgrep mint | wc -l` -lt 2 ]; then ./bin/mint; else echo 'Instance already running!'; fi;"
        


install:clean all
		@echo "Installing MINT..."
		@mkdir -p /usr/rubble/bin/
		@cp bin/* /usr/rubble/bin/

