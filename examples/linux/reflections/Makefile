# Things you might need to change in this Makefile:
#
# If you don't have the SensAble libraries installed, take out -lHD and -lHDU from the LIBS line

OBJECTS = ../../bcpp6/reflections/main.o

SOURCES = $(OBJECTS:.o=.cpp)
INCLUDE = -I../../../include 
LOCALOBJS = $(notdir $(OBJECTS) )
DEFS = -D_POSIX -D_MAX_PATH=260 -D_LINUX -DLINUX
CC   = g++ -c $(DEFS)
CFLAGS = -O3 $(INCLUDE)
LD   = g++ -v -o 
LIBS = -L. -L../../../lib/linux -lchai3d_linux -lGL -lglut -lGLU -ldhd -lpciscan -lpthread -lusb
# LIBS = -L. -L../../../lib/linux -lchai3d_linux -lGL -lglut -lGLU -lHDU -lHD -ldhd -lpciscan -lpthread

all: reflections_linux

%.o: %.cpp 
	$(CC) $(CFLAGS) $<
  
# Actual target and dependencies
reflections_linux: $(OBJECTS)
	$(LD) reflections_linux $(LOCALOBJS) $(LIBS)
	mv reflections_linux ../../../bin
	@echo "compilation done"

# Target deleting unwanted files
clean:
	\rm -f *.o *~ reflections_linux core 




