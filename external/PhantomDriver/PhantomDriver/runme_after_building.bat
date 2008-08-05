copy PhantomDriver.h ..\..\..\src\devices\
copy PhantomDriver.h ..\..\..\include\

copy Release\PhantomDriver.lib ..\..\..\lib\msvc6
copy Release\PhantomDriver.lib ..\..\..\lib\msvc7


cd ..\..\..\bin\
implib PhantomDriver.lib PhantomDriver.dll
copy PhantomDriver.lib ..\lib\bbcp6\
del PhantomDriver.lib


