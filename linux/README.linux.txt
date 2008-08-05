Notes on running CHAI under Linux:

* Since we can't ship SensAble's libraries, we use compile-time switches to
  link against or not link against them.  If you don't have SensAble's libs
  installed on your machine, you should make sure _DISABLE_PHANTOM_SUPPORT
  is -D'd in the Makefile in this directory.  If you want to use a Phantom,
  make sure _DISABLE_PHANTOM_SUPPORT is _not_ -D'd.

* Similarly, when you build the examples, each Makefile should have or
  not have -lHD and -lHDU depending on whether you have SensAble's HD 
  libs installed.
