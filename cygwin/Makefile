# CHAI makefile for cygwin
# Dan Morris
# March 2006
#
# To disable Phantom support and compile away SensAble's libraries, you need to
# add -D_DISABLE_PHANTOM_SUPPORT to the DEFS line.  If you don't do this, the
# SensAble runtime may be required to run your applications.

OBJECTS = ../src/collisions/CCollisionAABB.o ../src/collisions/CCollisionAABBTree.o ../src/collisions/CCollisionBrute.o ../src/collisions/CCollisionSpheres.o ../src/collisions/CCollisionSpheresGeometry.o ../src/collisions/CGenericCollision.o ../src/devices/CDeltaDevices.o ../src/devices/CFreedom6SDevice.o ../src/devices/CGenericDevice.o ../src/devices/CPhantomDevices.o ../src/devices/CVirtualDevice.o ../src/files/CFileLoader3DS.o ../src/files/CFileLoaderBMP.o ../src/files/CFileLoaderOBJ.o ../src/files/CFileLoaderTGA.o ../src/files/CImageLoader.o ../src/files/CMeshLoader.o ../src/forces/CGenericPointForceAlgo.o ../src/forces/CPotentialFieldForceAlgo.o ../src/forces/CProxyPointForceAlgo.o ../src/graphics/CColor.o ../src/graphics/CDraw3D.o ../src/graphics/CMacrosGL.o ../src/graphics/CMaterial.o ../src/graphics/CShaders.o ../src/graphics/CTexture2D.o ../src/graphics/CTriangle.o ../src/graphics/CVertex.o ../src/math/CMaths.o ../src/math/CMatrix3d.o ../src/math/CString.o ../src/math/CVector3d.o ../src/scenegraph/CCamera.o ../src/scenegraph/CGenericObject.o ../src/scenegraph/CGenericPotentialField.o ../src/scenegraph/CLight.o ../src/scenegraph/CMesh.o ../src/scenegraph/CShapeSphere.o ../src/scenegraph/CShapeTorus.o ../src/scenegraph/CVBOMesh.o ../src/scenegraph/CWorld.o ../src/timers/CPrecisionClock.o ../src/timers/CPrecisionTimer.o ../src/tools/CDelta3dofPointer.o ../src/tools/CFreedom6S3dofPointer.o ../src/tools/CGeneric3dofPointer.o ../src/tools/CGenericTool.o ../src/tools/CMeta3dofPointer.o ../src/tools/CPhantom3dofPointer.o ../src/widgets/CBitmap.o ../src/widgets/CFont.o ../src/widgets/CLabelPanel.o ../src/widgets/CPanel.o ../src/devices/PhantomDriver.o

SOURCES = $(OBJECTS:.o=.cpp)

INCLUDE = -I../src/graphics -I../src/collisions -I../src/devices -I../src/display -I../src/files -I../src/forces -I../src/math -I../src/scenegraph -I../src/timers -I../src/tools -I../src/widgets  

LOCALOBJS = $(notdir $(OBJECTS) )

# Optionally add -D_DISABLE_PHANTOM_SUPPORT to compile away SensAble's libraries
DEFS = -D_POSIX -D_MAX_PATH=260 -D_WIN32 -DNOMINMAX

CC   = g++ -c $(DEFS)
CFLAGS = -O3 $(INCLUDE)
LD   = g++ -o
AR   = ar rc
RANLIB = ranlib
LIBS = -L. -L../../../lib/cygwin -lglut32 -lopengl32 -lglu32 

all: chai3d

%.o: %.cpp 
	$(CC) $(CFLAGS) $<
  
# Actual target and dependencies
chai3d: $(OBJECTS)
	$(AR) libchai3d_cygwin.a $(LOCALOBJS) 
	$(RANLIB) libchai3d_cygwin.a
	mv libchai3d_cygwin.a ../lib/cygwin
	@echo "compilation done"

# Target deleting unwanted files
clean:
	\rm -f *.o *~ libchai3d_cygwin.a core 

