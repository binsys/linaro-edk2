

@REM Example usage of this script. default is a DEBUG build
@REM b
@REM b clean
@REM b release 
@REM b release clean
@REM b -v -y build.log

@rem ECHO OFF
@REM Setup Build environment. Sets WORKSPACE and puts build in path
CALL ..\..\edksetup.bat

@REM Set for tools chain. Currently RVCT
SET TARGET_TOOLS=RVCT
SET TARGET=DEBUG

@if /I "%1"=="RELEASE" (
  @REM If 1st argument is release set TARGET to RELEASE and shift arguments to remove it 
  SET TARGET=RELEASE
  shift /1
)

SET BUILD_ROOT=%WORKSPACE%\Build\QcomPkg\Msm8960Pkg\%TARGET%_%TARGET_TOOLS%

@REM Build the Beagle Board firmware and creat an FD (FLASH Device) Image.
CALL build -p QcomPkg\Msm8960Pkg\Msm8960Pkg.dsc -a ARM -t %TARGET_TOOLS% -b %TARGET% -y %BUILD_ROOT%\PCD.log -Y PCD %1 %2 %3 %4 %5 %6 %7 %8
@if ERRORLEVEL 1 goto Exit

@if /I "%1"=="CLEAN" goto Clean

@REM
@REM Ram starts at 0x80000000
@REM
@cd Tools

ECHO Building tools...
CALL nmake 

ECHO Gen Android Recovery Image...
CALL GenerateImage --cmdline 'console=null' --kernel %BUILD_ROOT%\FV\MSM8960_EFI.fd --ramdisk %BUILD_ROOT%\FV\Guid.xref -o %BUILD_ROOT%\FV\recovery.img --base 80200000 --pagesize 4096 --kernel_offset 00100000

START %BUILD_ROOT%\PCD.log
ECHO Build ok.
cd ..
:Exit
EXIT /B

:Clean
cd Tools
CALL nmake clean
cd ..
