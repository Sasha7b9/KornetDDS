del Debug\Device.lib
del Device.out

@echo Build Device-VS      ... %TIME%

"c:\Program Files (x86)\Microsoft Visual Studio\2017\Professional\Common7\IDE\devenv.exe" Device.vcxproj /Out Device.out /Rebuild
if exist Debug\Device.lib (
@echo Build Ok!            ... %TIME%
) else (
@echo !!! Build Failed !!! ... %TIME%
"c:\Program Files (x86)\Notepad++\notepad++.exe" -multiInst -alwaysOnTop Device.out -n1000 -x0 -y0
)