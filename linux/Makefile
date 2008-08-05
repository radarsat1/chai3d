# CHAI makefile for Linux
# Dan Morris
# March 2006
#
# To disable Phantom support and compile away SensAble's libraries, you need to
# add -D_DISABLE_PHANTOM_SUPPORT to the DEFS line.  This is the default when we ship CHAI.
#
# If you don't have DISABLE_PHANTOM_SUPPORT defined, the SensAble runtime may be required to run
# your applications.
#
# Likewise, to disable Delta support and compile away Force Dimension's libraries, you need to
# add -D_DISABLE_DELTA_SUPPORT to the DEFS line.  This is the default when we ship CHAI.
#
# If you do want to use a Phantom or Delta and have the relevant libraries, you should remove
# the disable DEF for that device.

OBJECTS = ../src/collisions/CCollisionAABB.o ../src/collisions/CCollisionAABBTree.o ../src/collisions/CCollisionBrute.o \
../src/collisions/CCollisionSpheres.o ../src/collisions/CCollisionSpheresGeometry.o ../src/collisions/CGenericCollision.o \
../src/devices/CGenericDevice.o ../src/devices/CPhantomDevices.o ../src/files/CFileLoader3DS.o ../src/files/CFileLoaderBMP.o \
../src/files/CFileLoaderOBJ.o ../src/files/CFileLoaderTGA.o ../src/files/CImageLoader.o ../src/files/CMeshLoader.o \
../src/forces/CGenericPointForceAlgo.o ../src/forces/CPotentialFieldForceAlgo.o ../src/forces/CProxyPointForceAlgo.o \
../src/graphics/CColor.o ../src/graphics/CDraw3D.o ../src/graphics/CMacrosGL.o ../src/graphics/CMaterial.o \
../src/graphics/CTexture2D.o ../src/graphics/CTriangle.o ../src/graphics/CVertex.o ../src/math/CMaths.o ../src/math/CMatrix3d.o \
../src/math/CString.o ../src/math/CVector3d.o ../src/scenegraph/CCamera.o ../src/scenegraph/CGenericObject.o \
../src/scenegraph/CGenericPotentialField.o ../src/scenegraph/CLight.o ../src/scenegraph/CMesh.o ../src/scenegraph/CShapeSphere.o \
../src/scenegraph/CShapeTorus.o ../src/scenegraph/CWorld.o ../src/timers/CPrecisionClock.o ../src/timers/CPrecisionTimer.o \
../src/tools/CGeneric3dofPointer.o ../src/tools/CGenericTool.o ../src/tools/CMeta3dofPointer.o ../src/tools/CPhantom3dofPointer.o \
../src/tools/CDelta3dofPointer.o ../src/widgets/CBitmap.o ../src/widgets/CFont.o ../src/widgets/CLabelPanel.o \
../src/widgets/CPanel.o ../src/devices/PhantomDriverLinux.o ../src/devices/CDeltaDevices.o

SOURCES = $(OBJECTS:.o=.cpp)
INCLUDE = -I../src/graphics -I../src/collisions -I../src/devices -I../src/display -I../src/files -I../src/forces -I../src/math -I../src/scenegraph -I../src/timers -I../src/tools -I../src/widgets -I../include/dhd

LOCALOBJS = $(notdir $(OBJECTS) )

# Optionally add -D_DISABLE_PHANTOM_SUPPORT to compile away SensAble's libraries
# Optionally add -D_DISABLE_DELTA_SUPPORT to compile away Force Dimension's libraries
DEFS = -D_POSIX -D_MAX_PATH=260 -D_LINUX -DLINUX -D_DISABLE_PHANTOM_SUPPORT -D_DISABLE_DELTA_SUPPORT

CC   = g++ -c $(DEFS)
CFLAGS = -O3 $(INCLUDE)
LD   = g++ -o
AR   = ar rc
RANLIB = ranlib

LIBS = -L. -L../lib/linux -lglut -lgl -lglu -ldhd

all: chai3d

%.o: %.cpp 
	$(CC) $(CFLAGS) $<
  
# Actual target and dependencies
chai3d: $(OBJECTS)
	$(AR) libchai3d_linux.a $(LOCALOBJS) 
	$(RANLIB) libchai3d_linux.a
	mv libchai3d_linux.a ../lib/linux
	@echo "compilation done"

# Target deleting unwanted files
clean:
	\rm -f *.o *~ libchai3d_linux.a core 

