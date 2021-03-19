#Freethought Interface
PROJECT = fti
RELEASE_PROJECT = Release/$(PROJECT)
DEBUG_PROJECT = Debug/$(PROJECT).debug
CC=gcc
DATA_THISDIR = $(shell pwd | sed -e 's=^/tmp_mnt/=/=')
RELEASE_CFLAGS = -Wall -DLinux 
DEBUG_CFLAGS = -Wall -DLinux -g
OS := $(shell uname)
ifeq ($(OS),Darwin)
RELEASE_CFLAGS = -Wall -DLinux -DMacOS
DEBUG_CFLAGS = -Wall -DLinux -DMacOS -g
endif

#CFLAGS := -Wall -O2 -ffast-math -mpreferred-stack-boundary=2
#-fomit-frame-pointer 
#-mpreferred-stack-boundary=2
INCDIR = -I/usr/include -I/usr/X11/include -I/usr/local/include -I. -I../lib/src
#INCDIR = -I/usr/src/linux/include -I/usr/include -I. -I../lib/src
#LIBS = -L/lib -L/usr/lib64 -L/usr/lib -L/usr/X11R6/lib -lfreethought -ljpeg -lswscale -lavcodec -lmp3lame -lavformat -lavutil -lm -lz  -lpthread ./libX11.dll
#LIBS = -L/lib -L/usr/lib64 -L/usr/lib -L/usr/X11R6/lib -lfreethought -ljpeg -lswscale -lavcodec -lmp3lame -lavformat -lavutil -lm -lz -lpthread -lX11
LIBS = -L/lib -L/usr/lib64 -L/usr/lib -L/usr/X11R6/lib -L/usr/local/lib -lfreethought -ljpeg -lmp3lame -lm -lpthread -lX11  
#-lstdc++
#SRCS=fti.c win_ftimain.c win_openfile.c win_openproject.c win_outfile.c win_record.c win_saveproject.c win_prop.c win_control.c win_savewindow.c 
#OBJS=fti.o win_ftimain.o win_openfile.o win_openproject.o win_outfile.o win_record.o win_saveproject.o win_prop.o win_control.o win_savewindow.o
#SRC = $(shell echo src/*.c)
SRC = $(shell echo src/*.c)
#This means D_OBJECTS is the same as the variable SRC but replace each %.c with %.o
D_OBJECTS=$(SRC:src/%.c=Debug/%.o)
R_OBJECTS=$(SRC:src/%.c=Release/%.o)
#note that second line must be tabbed

RTARGET = $(RELEASE_PROJECT)
DTARGET = $(DEBUG_PROJECT)

all : dirs $(RTARGET)

debug : dirs $(DTARGET)

#test:
#	echo $(D_OBJECTS)

dirs :
	@mkdir -p Debug Release

install : dirs $(RTARGET)
	cp $(RELEASE_PROJECT) /usr/local/bin

uninstall : 
	rm -rf /usr/local/bin/$(PROJECT)

debug_install : dirs $(DTARGET)
	cp $(DEBUG_PROJECT) /usr/local/bin



#really important to put $(D_OBJECTS) before $(LIBS)
#$@ is the variable to the left of the :, $< is the variable to the right of the :
$(DTARGET) : $(D_OBJECTS)
	$(CC) $(DEBUG_CFLAGS) $(D_OBJECTS) $(LIBS)  -o $@ 

$(RTARGET) : $(R_OBJECTS)
	$(CC) $(RELEASE_CFLAGS) $(R_OBJECTS) $(LIBS) -o $@

Debug/%.o: src/%.c                    
	$(CC) $(DEBUG_CFLAGS) $(INCDIR) -o $@  -c  $<  

Release/%.o: src/%.c                    
	$(CC) $(RELEASE_CFLAGS) $(INCDIR) -o $@  -c  $<  

clean:
	rm -f *.o src/*.o Release/*.o Debug/*.o $(RELEASE_PROJECT) $(DEBUG_PROJECT)  

