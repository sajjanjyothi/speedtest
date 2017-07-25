CURLIDIR = ../curl-7.54.1/include
LCURLDIR = ../curl-7.54.1/lib/.libs
LDIR = ./ 
LIBS = -lcurl -lspeedtest
CC = gcc
AR = ar
OUTPUT = speedtest
SPEEDTESTOBJ = speedtest.o 
OBJECTFILES = main.o 
DEPS = libspeedtest.a 

CFLAGS = -I $(CURLIDIR) -L $(LDIR) -L$(LCURLDIR) $(LIBS)

all:$(OUTPUT)
%.o : %.c 
	$(CC) -c -o $@ $^ $(CFLAGS)

$(OUTPUT):$(SPEEDTESTOBJ) $(DEPS) $(OBJECTFILES) 
	$(CC) -o $@ $(OBJECTFILES) $(CFLAGS)

$(DEPS):
	$(AR) rc $(DEPS) $(SPEEDTESTOBJ)
clean:
	rm -rf $(OBJECTFILES) $(SPEEDTESTOBJ) $(DEPS) $(OUTPUT)