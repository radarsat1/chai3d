# Things you might need to change in this Makefile:
#
# If you don't have the SensAble libraries installed, take out -lHD and -lHDU from the LIBS line.
# This is the default when we ship CHAI.

OBJECTS = ../../bcpp6/earth/main.o

SOURCES = $(OBJECTS:.o=.cpp)
INCLUDE = -I../../../include 
LOCALOBJS = $(notdir $(OBJECTS) )
DEFS = -D_POSIX -D_MAX_PATH=260 -D_LINUX -DLINUX
CC   = g++ -c $(DEFS)
CFLAGS = -O3 $(INCLUDE)
LD   = g++ -v -o 

# With no Phantom support:
LIBS = -L. -L../../../lib/linux -lchai3d_linux -lGL -lglut -lGLU -ldhd -lpthread -lpciscan -lusb

# With Phantom support:
# LIBS = -L. -L../../../lib/linux -lchai3d_linux -lGL -lglut -lGLU -ldhd -lHD -lHDU -lpciscan -lusb -lpthread

all: earth_linux

%.o: %.cpp 
	$(CC) $(CFLAGS) $<
  
# Actual target and dependencies
earth_linux: $(OBJECTS)
	$(LD) earth_linux $(LOCALOBJS) $(LIBS)
	mv earth_linux ../../../bin
	@echo "compilation done"

# Target deleting unwanted files
clean:
	\rm -f *.o *~ earth_linux core 




