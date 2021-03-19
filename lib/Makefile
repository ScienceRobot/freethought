#PROJECT Freethought Library
PROJECT = libfreethought.a
RELEASE_PROJECT = Release/$(PROJECT)
DEBUG_PROJECT = Debug/$(PROJECT)
CC=gcc
DATA_THISDIR = $(shell pwd | sed -e 's=^/tmp_mnt/=/=')
#add -DUSE_FFMPEG to CFLAGS for FFMPEG code
RELEASE_CFLAGS = -c -Wall -DLinux 
DEBUG_CFLAGS = -c -Wall -DLinux -g

OS := $(shell uname)
ifeq ($(OS),Darwin)
RELEASE_CFLAGS = -c -Wall -DLinux -DMacOS
DEBUG_CFLAGS = -c -Wall -DLinux -DMacOS -g
endif

#CFLAGS := -Wall -O2 -ffast-math -DLinux
#
#-fomit-frame-pointer 
#-mpreferred-stack-boundary=2
INCDIR :=  -I/usr/include -I/usr/X11/include -I/usr/local/include -I. 
#INCDIR := -I/usr/src/linux/include -I/usr/include -I.
#LIBS = -L/lib -L/usr/lib64 -L/usr/lib -L/usr/X11R6/lib -lX11 -ljpeg -lm -lavutil  -lavformat -lavcodec -lmp3lame -lswscale
#LIBS = -L/lib -L/usr/lib64 -L/usr/lib -L/usr/X11R6/lib -lX11 -ljpeg -lm -lavutil -lavformat -lavcodec -lmp3lame -lswscale
LIBS = -L/lib  -L/usr/lib -L/usr/lib64 -L/usr/X11R6/lib -lX11 -ljpeg -lmp3lame -lm
#-lstdc++
#-lstdc++ is needed just because the extension is .cpp not .c
SRC = $(shell echo src/*.c)
#This means D_OBJECTS is the same as the variable SRC but replace each %.c with %.o
D_OBJECTS=$(SRC:src/%.c=Debug/%.o)
R_OBJECTS=$(SRC:src/%.c=Release/%.o)
#note that second line must be tabbed

RTARGET = $(RELEASE_PROJECT)
DTARGET = $(DEBUG_PROJECT)

all : dirs $(RTARGET)

debug : dirs $(DTARGET)

#SRCS=freethought.c
#OBJS=freethought.o

dirs :
	@mkdir -p Debug Release

install : dirs $(RTARGET)
#	cp $(RELEASE_PROJECT) /usr/local/bin
	cp $(RELEASE_PROJECT) /usr/lib64
	cp $(RELEASE_PROJECT) /usr/lib
	cp $(RELEASE_PROJECT) /usr/local/lib
ifeq ($(OS),Darwin)
	ln -sf $(DATA_THISDIR)/src/freethought.h /usr/local/include/freethought.h
else 
	ln -sf $(DATA_THISDIR)/src/freethought.h /usr/include/freethought.h
endif

uninstall:
	unlink /usr/include/freethought.h
	unlink /usr/local/include/freethought.h
	rm -rf /usr/local/lib/libfreethought.a
	rm -rf /usr/lib/libfreethought.a
	rm -rf /usr/lib64/libfreethought.a

debug_install : dirs $(DTARGET)
	cp $(DEBUG_PROJECT) /usr/lib64
	cp $(DEBUG_PROJECT) /usr/lib
	cp $(DEBUG_PROJECT) /usr/local/lib
ifeq ($(OS),Darwin)
	ln -sf $(DATA_THISDIR)/src/freethought.h /usr/local/include/freethought.h
else 
	ln -sf $(DATA_THISDIR)/src/freethought.h /usr/include/freethought.h
endif

#really important to put $(D_OBJECTS) before $(LIBS)
#$@ is the variable to the left of the :, $< is the variable to the right of the :
#$(CC) $(DEBUG_CFLAGS) $(D_OBJECTS) $(LIBS)  -o $@ 
$(DTARGET) : $(D_OBJECTS)
ifeq ($(OS),Darwin)
	libtool -static -o $(DEBUG_PROJECT) $(D_OBJECTS)
else
	ar rcs $(DEBUG_PROJECT) $(D_OBJECTS)
endif
#	cp $(DEBUG_PROJECT) /usr/lib64
#	cp $(DEBUG_PROJECT) /usr/lib
#	cp $(DEBUG_PROJECT) /usr/local/lib
#ifeq ($(OS),Darwin)
#	ln -sf $(DATA_THISDIR)/src/freethought.h /usr/local/include/freethought.h
#else 
#	ln -sf $(DATA_THISDIR)/src/freethought.h /usr/include/freethought.h
#endif

$(RTARGET) : $(R_OBJECTS)
ifeq ($(OS),Darwin)
	libtool -static -o $(RELEASE_PROJECT) $(R_OBJECTS)
else
	ar rcs $(RELEASE_PROJECT) $(R_OBJECTS)
endif
#	cp $(RELEASE_PROJECT) /usr/lib64
#	cp $(RELEASE_PROJECT) /usr/lib
#	cp $(RELEASE_PROJECT) /usr/local/lib
#ifeq ($(OS),Darwin)
#	ln -sf $(DATA_THISDIR)/src/freethought.h /usr/local/include/freethought.h
#else 
#	ln -sf $(DATA_THISDIR)/src/freethought.h /usr/include/freethought.h
#endif

Debug/%.o: src/%.c                    
	$(CC) $(DEBUG_CFLAGS) $(INCDIR) -o $@  -c  $<  

Release/%.o: src/%.c                    
	$(CC) $(RELEASE_CFLAGS) $(INCDIR) -o $@  -c  $<  

clean:
	rm -f src/*.o Release/*.o Debug/*.o Release/*.a Debug/*.a $(RELEASE_PROJECT) $(DEBUG_PROJECT)  	


#OLD CODE:
#EXE=$(project)
#possibly should just put lib locally- not in /usr/lib
#I just worry that people will make and wonder why it doesn't work
#all: $(OBJS)
#	ar rcs libfreethought.a freethought.o
#	cp libfreethought.a /usr/lib64
#	cp libfreethought.a /usr/lib
#	ln -sf $(DATA_THISDIR)/freethought.h /usr/include/freethought.h

#make freethought library (libfreethought.a)
#lib: $(OBJS)
#	cp libfreethought.a /usr/lib64
#	cp libfreethought.a /usr/lib
#	ln -sf $(DATA_THISDIR)/freethought.h /usr/include/freethought.h

#install: $(OBJS)
#	ar rcs libfreethought.a freethought.o
#	cp libfreethought.a /usr/lib64
#	cp libfreethought.a /usr/lib
#	ln -sf $(DATA_THISDIR)/freethought.h /usr/include/freethought.h

#uninstall:
#	unlink /usr/include/freethought.h
#	rm -rf /usr/lib/libfreethought.a
#	rm -rf /usr/lib64/libfreethought.a

#debug: $(OBJS)
#	$(CC) $(CFLAGS) -c -g $(INCDIR) $(OBJS) $(LIBS) $(SRCS)
#	ar rcs libfreethought.a freethought.o
#	cp libfreethought.a /usr/lib64
#	cp libfreethought.a /usr/lib
#	ln -sf $(DATA_THISDIR)/freethought.h /usr/include/freethought.h


#.c.o:	$(SRCS)
#	$(CC) -c $(CFLAGS) $(INCDIR) $<

#clean:
#	rm -f *.o $(PROJECT)
