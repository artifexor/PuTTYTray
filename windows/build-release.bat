set trayversion=
for /f "tokens=2,3 delims=-t" %%i in ("%1") do set trayversion=%%i
if %trayversion%.==. echo Usage: %0 p0.61-t004 & goto end

nmake /F Makefile.vc "VER=/DRELEASE=%1 /DSVN_REV1=%trayversion%" ^
<<<<<<< HEAD
  putty.exe ^
  pageant.exe ^
  puttygen.exe

signtool sign /a ^
  putty.exe ^
  pageant.exe ^
  puttygen.exe
=======
  putty.exe

signtool sign /a ^
  putty.exe
>>>>>>> upstream/master
:end
