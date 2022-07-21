#    
#    File: app_service.mak


ifndef SRC_DIR
SRC_DIR=..
endif
ifndef OBJ_DIR
OBJ_DIR=.
endif
ifndef EXE_DIR
EXE_DIR=.
endif
#ifndef GOOGLE_INCLUDE
#GOOGLE_INCLUDE=/usr/local/include/g
#endif
ifndef LOCAL_LIB
LOCAL_LIB=/usr/local/lib
endif
ifndef TARGET_MACHINE_TYPE
TARGET_MACHINE_TYPE= x64
endif


LIBSRC= $(SRC_DIR)/src
S= $(SRC_DIR)/application_service
O= $(OBJ_DIR)
US= .
I= $(SRC_DIR)/include
INCLUDE= -I$(I) -I/usr/local/opt/openssl@1.1/include/

CFLAGS=$(INCLUDE) -O3 -g -Wall -std=c++11 -Wno-unused-variable -D X64
CFLAGS1=$(INCLUDE) -O1 -g -Wall -std=c++11 -Wno-unused-variable -D X64
CC=g++
LINK=g++
PROTO=/usr/local/bin/protoc
AR=ar
#export LD_LIBRARY_PATH=/usr/local/lib
LDFLAGS= -L $(LOCAL_LIB) -lprotobuf -lgtest -lgflags -lpthread -L/usr/local/opt/openssl@1.1/lib/ -lcrypto -lssl

dobj=	$(O)/app_service.o $(O)/certifier.pb.o $(O)/certifier.o $(O)/support.o \
$(O)/simulated_enclave.o $(O)/application_enclave.o


all:	app_service.exe
clean:
	@echo "removing object files"
	rm $(O)/*.o
	@echo "removing executable file"
	rm $(EXE_DIR)/app_service.exe

app_service.exe: $(dobj) 
	@echo "linking executable files"
	$(LINK) -o $(EXE_DIR)/app_service.exe $(dobj) $(LDFLAGS)

$(S)/certifier.pb.cc $(LIBSRC)/certifier.pb.h: $(LIBSRC)/certifier.proto
	$(PROTO) -I$(I) --cpp_out=$(S) $(LIBSRC)/certifier.proto
	mv certifier.pb.h $(I)

$(O)/app_service.o: $(US)/app_service.cc $(I)/certifier.pb.h $(I)/certifier.h
	@echo "compiling app_service.cc"
	$(CC) $(CFLAGS) -c -o $(O)/app_service.o $(US)/app_service.cc

$(O)/certifier.pb.o: $(LIBSRC)/certifier.pb.cc $(I)/certifier.pb.h
	@echo "compiling certifier.pb.cc"
	$(CC) $(CFLAGS) -c -o $(O)/certifier.pb.o $(LIBSRC)/certifier.pb.cc

$(O)/certifier.o: $(LIBSRC)/certifier.cc $(I)/certifier.pb.h $(I)/certifier.h
	@echo "compiling certifier.cc"
	$(CC) $(CFLAGS) -c -o $(O)/certifier.o $(LIBSRC)/certifier.cc

$(O)/support.o: $(LIBSRC)/support.cc $(I)/support.h
	@echo "compiling support.cc"
	$(CC) $(CFLAGS) -c -o $(O)/support.o $(LIBSRC)/support.cc

$(O)/simulated_enclave.o: $(LIBSRC)/simulated_enclave.cc $(I)/simulated_enclave.h
	@echo "compiling simulated_enclave.cc"
	$(CC) $(CFLAGS) -c -o $(O)/simulated_enclave.o $(LIBSRC)/simulated_enclave.cc

$(O)/application_enclave.o: $(LIBSRC)/application_enclave.cc $(I)/application_enclave.h
	@echo "compiling application_enclave.cc"
	$(CC) $(CFLAGS) -c -o $(O)/application_enclave.o $(LIBSRC)/application_enclave.cc