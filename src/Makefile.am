
top_srcdir = ../posix
top_builddir = ../posix

SOURCE_FILES = scenegraph/CShapeSphere.cpp scenegraph/CLight.cpp		\
    scenegraph/CCamera.cpp scenegraph/CGenericObject.cpp				\
    scenegraph/CMesh.cpp scenegraph/CWorld.cpp							\
    scenegraph/CGenericPotentialField.cpp scenegraph/CShapeTorus.cpp	\
    forces/CPotentialFieldForceAlgo.cpp									\
    forces/CGenericPointForceAlgo.cpp forces/CProxyPointForceAlgo.cpp	\
    timers/CPrecisionTimer.cpp timers/CPrecisionClock.cpp				\
    widgets/CPanel.cpp widgets/CLabelPanel.cpp widgets/CFont.cpp		\
    widgets/CBitmap.cpp files/CMeshLoader.cpp files/CFileLoaderBMP.cpp	\
    files/CImageLoader.cpp files/CFileLoaderOBJ.cpp						\
    files/CFileLoaderTGA.cpp files/CFileLoader3DS.cpp math/CMaths.cpp	\
    math/CString.cpp math/CMatrix3d.cpp math/CQuaternion.cpp			\
    math/CVector3d.cpp tools/CFreedom6S3dofPointer.cpp					\
    tools/CGeneric3dofPointer.cpp tools/CMeta3dofPointer.cpp			\
    tools/CPhantom3dofPointer.cpp tools/CGenericTool.cpp				\
    tools/CDelta3dofPointer.cpp collisions/CCollisionAABBBox.cpp		\
    collisions/CCollisionAABB.cpp collisions/CCollisionAABBTree.cpp		\
    collisions/CCollisionBrute.cpp collisions/CGenericCollision.cpp		\
    collisions/CCollisionSpheresGeometry.cpp							\
    collisions/CCollisionSpheres.cpp graphics/CVertex.cpp				\
    graphics/CTriangle.cpp graphics/CMacrosGL.cpp graphics/CDraw3D.cpp	\
    graphics/CTexture2D.cpp graphics/CColor.cpp graphics/CMaterial.cpp	\
    devices/CDeltaDevices.cpp devices/CGenericDevice.cpp				\
    devices/CFreedom6SDevice.cpp devices/CPhantomDevices.cpp			\
    devices/CCallback.cpp devices/PhantomDriverLinux.cpp				\
    devices/CFalconDevice.cpp devices/CLibNIFalconDevice.cpp

# TODO, configure.ac needs to be given tests for support for the
# following files:
#
# scenegraph/CVBOMesh.cpp
# display/CViewport.cpp
# graphics/CShaders.cpp
# devices/PhantomDriver.cpp
# devices/CDriverServotogo.cpp
# devices/CDriverSensoray626.cpp
# devices/CVirtualDevice.cpp

# TODO: configure.ac needs to be given tests for devices and
# OS-specific flags

DEFS = -D_POSIX -D_MAX_PATH=260 -D_LINUX -DLINUX						\
-D_DISABLE_PHANTOM_SUPPORT -D_DISABLE_DELTA_SUPPORT						\
-D_DISABLE_FALCON_DEVICE_SUPPORT -D_DISABLE_LIBNIFALCON_DEVICE_SUPPORT

lib_LTLIBRARIES = libchai3d.la

libchai3d_la_CXXFLAGS = -Wall -I$(top_srcdir)/../include $(DEFS)
libchai3d_la_SOURCES = $(SOURCE_FILES)
libchai3d_la_LDFLAGS  = -export-dynamic -version-info @SO_VERSION@
