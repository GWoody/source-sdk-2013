pushd %~dp0
devtools\bin\vpc.exe /hl2 /define:HOLODECK +shaders /mksln shaders.sln
popd
pause