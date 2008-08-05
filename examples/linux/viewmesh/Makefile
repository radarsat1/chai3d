# Things you might need to change in this Makefile:
#
# If you don't have the SensAble libraries installed, take out -lHD and -lHDU from the LIBS line.
# This is the default when we ship CHAI.

OBJECTS = ../../cygwin/viewmesh/main.o

SOURCES = $(OBJECTS:.o=.cpp)
INCLUDE = -I../../../include 
LOCALOBJS = $(notdir $(OBJECTS) )
DEFS = -D_POSIX -D_MAX_PATH=260 -D_LINUX -DLINUX
CC   = g++ -c $(DEFS)
CFLAGS = -O3 $(INCLUDE)
LD   = g++ -v -o 

# With no Phantom support:
LIBS = -L. -L../../../lib/linux -lchai3d_linux -lGL -lglut -lGLU -ldhd -lpciscan -lusb -lpthread

# With Phantom support:
# LIBS = -L. -L../../../lib/linux -lchai3d_linux -lGL -lglut -lGLU -ldhd -lHD -lHDU -lpciscan -lusb

all: viewmesh_linux

%.o: %.cpp 
	$(CC) $(CFLAGS) $<
  
# Actual target and dependencies
viewmesh_linux: $(OBJECTS)
	$(LD) viewmesh_linux $(LOCALOBJS) $(LIBS)
	mv viewmesh_linux ../../../bin
	@echo "compilation done"

# Target deleting unwanted files
clean:
	\rm -f *.o *~ viewmesh_linux core 




