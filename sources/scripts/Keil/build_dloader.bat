@echo off
@echo %TIME%   Compile Device Loader ...

c:\Keil_v5\UV4\UV4 -b ..\..\DLoader\DLoader.uvprojx -j0 -z -o DLoader.out
set BUILD_STATUS=%ERRORLEVEL%
if %BUILD_STATUS%==0 goto Exit

echo ERROR!!! Device Loader   failed!!!

type ..\..\DLoader\DLoader.out

:Exit

@echo %TIME%   Complete