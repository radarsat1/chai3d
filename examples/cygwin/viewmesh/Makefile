# CHAI application makefile for cygwin
# Dan Morris
# March 2006
#
# To disable Phantom support and compile away SensAble's libraries, you need to
# add -D_DISABLE_PHANTOM_SUPPORT to the DEFS line.  If you don't do this, the
# SensAble runtime may be required to run your applications.
#
# CHAI must also be compiled with this option if you want to disable Phantom
# support.

OBJECTS = ./main.o

SOURCES = $(OBJECTS:.o=.cpp)
INCLUDE = -I../../../include 
LOCALOBJS = $(notdir $(OBJECTS) )

# Optionally add -D_DISABLE_PHANTOM_SUPPORT to compile away SensAble's libraries
DEFS = -D_POSIX -D_MAX_PATH=260 -D_WIN32 -DNOMINMAX

CC   = g++ -c $(DEFS)
CFLAGS = -O3 $(INCLUDE)
LD   = g++ -v -o 
LIBS = -L. -L../../../lib/cygwin -lchai3d_cygwin -lglut32 -lopengl32 -lglu32

all: viewmesh_cygwin

%.o: %.cpp 
	$(CC) $(CFLAGS) $<
  
# Actual target and dependencies
viewmesh_cygwin: $(OBJECTS)
	$(LD) viewmesh_cygwin $(LOCALOBJS) $(LIBS)
	mv viewmesh_cygwin ../../../bin
	@echo "compilation done"

# Target deleting unwanted files
clean:
	\rm -f *.o *~ viewmesh_cygwin core 




