@ECHO OFF
setlocal

rem how does not works:
rem 1.) directory uboot/build will be checked out on windows build-server
rem 2.) The windows build server starts this file
rem 3.) The bash script cygwin_shell ist used to copy this directory to the linux build server (10.20.30.69)
rem 4.) Script checkout_and_build is started on the linux build server. It gets linux-kernel-tc18 from the SVN and builds it
rem 5.) The script cygwin_shell copies the result back to the windows build server
rem

echo start make %*
ECHO TOPCALL BuildCentral build environment: make.bat
ECHO.
ECHO System Information:
ECHO Host:       %COMPUTERNAME%
ECHO Domain:     %USERDNSDOMAIN%
ECHO User:       %USERNAME%
ECHO.

PATH %PATH%;C:\cygwin\bin
SET executable=./build/checkout_and_build
SET buildserver=10.20.30.69
SET builduser=tcbuild 
SET buildversion=%1

ECHO Passing the build script to the cygwin-shell
ECHO. 
ECHO bash cygwin_shell %executable% %buildserver% %builduser% %buildversion%
bash ./build/cygwin_shell %executable% %buildserver% %builduser% %buildversion%
SET exitcode=%errorlevel%

ECHO.
ECHO Finished make.bat with exit code %exitcode%
ECHO.
   
EXIT /b %exitcode%

