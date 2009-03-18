# CHAI makefile for Darwin
# Stephen Sinclair
# March 2007
#
# To disable Phantom support and compile away SensAble's libraries, you need to
# add -D_DISABLE_PHANTOM_SUPPORT to the DEFS line.  If you don't do this, the
# SensAble runtime may be required to run your applications.

OBJECTS = ../src/collisions/CCollisionAABB.o ../src/collisions/CCollisionAABBTree.o ../src/collisions/CCollisionBrute.o ../src/collisions/CCollisionSpheres.o ../src/collisions/CCollisionSpheresGeometry.o ../src/collisions/CGenericCollision.o ../src/devices/CDeltaDevices.o ../src/devices/CGenericDevice.o ../src/devices/CPhantomDevices.o ../src/files/CFileLoader3DS.o ../src/files/CFileLoaderBMP.o ../src/files/CFileLoaderOBJ.o ../src/files/CFileLoaderTGA.o ../src/files/CImageLoader.o ../src/files/CMeshLoader.o ../src/forces/CGenericPointForceAlgo.o ../src/forces/CPotentialFieldForceAlgo.o ../src/forces/CProxyPointForceAlgo.o ../src/graphics/CColor.o ../src/graphics/CDraw3D.o ../src/graphics/CMacrosGL.o ../src/graphics/CMaterial.o ../src/graphics/CTexture2D.o ../src/graphics/CTriangle.o ../src/graphics/CVertex.o ../src/math/CMaths.o ../src/math/CMatrix3d.o ../src/math/CString.o ../src/math/CVector3d.o ../src/scenegraph/CCamera.o ../src/scenegraph/CGenericObject.o ../src/scenegraph/CGenericPotentialField.o ../src/scenegraph/CLight.o ../src/scenegraph/CMesh.o ../src/scenegraph/CShapeSphere.o ../src/scenegraph/CShapeTorus.o ../src/scenegraph/CWorld.o ../src/timers/CPrecisionClock.o ../src/timers/CPrecisionTimer.o ../src/tools/CDelta3dofPointer.o ../src/tools/CGeneric3dofPointer.o ../src/tools/CGenericTool.o ../src/tools/CMeta3dofPointer.o ../src/tools/CPhantom3dofPointer.o ../src/widgets/CBitmap.o ../src/widgets/CFont.o ../src/widgets/CLabelPanel.o ../src/widgets/CPanel.o ../src/devices/PhantomDriverLinux.o ../src/devices/CLibNIFalconDevice.o

SOURCES = $(OBJECTS:.o=.cpp)
INCLUDE = -I../src/graphics -I../src/collisions -I../src/devices -I../src/display -I../src/files -I../src/forces -I../src/math -I../src/scenegraph -I../src/timers -I../src/tools -I../src/widgets -I../include/dhd -IGL -IGLUT

LOCALOBJS = $(notdir $(OBJECTS) )
LOCALSRCS = $(notdir $(SOURCES) )

# Optionally add -D_DISABLE_PHANTOM_SUPPORT to compile away SensAble's libraries
# Optionally add -D_DISABLE_DELTA_SUPPORT to compile away Force Dimension's libraries
# Optionally add -D_DISABLE_FALCON_DEVICE_SUPPORT to compile away Novint's libraries
# Optionally add -D_DISABLE_FREEDOM6S_SUPPORT to compile away MPBT's Freedom 6S libraries
# We exclude the Delta device because libdhd.a is not available on Darwin
DEFS = -D_POSIX -D_MAX_PATH=260 -D_DISABLE_PHANTOM_SUPPORT -D_DISABLE_DELTA_SUPPORT -D_DISABLE_FALCON_DEVICE_SUPPORT -D_DISABLE_FREEDOM6S_SUPPORT

CC   = g++ -c $(DEFS)
CFLAGS = -O3 $(INCLUDE)
LD   = g++ -o
AR   = ar rc
RANLIB = ranlib
LIBS = -L. -L../lib/darwin -lglut -lgl -lglu -framework OpenGL

INCLUDE += -I$(HOME)/.local/include \
	-I$(HOME)/.local/include/libusb-1.0 \
	-I$(HOME)/.local/include/falcon

all: localsrc chai3d

# On Darwin, the OpenGL and GLUT headers are in the corresponding frameworks' Headers folder.
# However, since the headers do not reside in a folder called "GL", this causes a problem.
# Here we create some symlinks which fix the problem, but it seems like there should be a better way.
# Likely that would mean using XCode and creating a proper Framework.
headers:
	if ! ( [ -d GL ] && [ -d GLUT ] ); then \
		mkdir GL; \
		mkdir GLUT; \
		ln -s /System/Library/Frameworks/OpenGL.framework/Headers GL/GL; \
		ln -s /System/Library/Frameworks/GLUT.framework/Headers GLUT/GL; \
	fi

localsrc: $(SOURCES)
	for i in $+; do if ! [ -h $$(basename $$i) ]; then ln -s $$i .; fi; done

%.o: %.cpp 
	$(CC) $(CFLAGS) $<
  
# Actual target and dependencies
chai3d: headers $(LOCALOBJS)
	$(AR) libchai3d_darwin.a $(LOCALOBJS) 
	$(RANLIB) libchai3d_darwin.a
	-mkdir ../lib/darwin
	mv libchai3d_darwin.a ../lib/darwin/
	@echo "compilation done"

# Target deleting unwanted files
clean:
	\rm -fr *.o *~ libchai3d_darwin.a core GL GLUT

