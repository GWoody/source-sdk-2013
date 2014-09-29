pushd %~dp0
devtools\bin\vpc.exe /hl2 /define:HOLODECK +everything /mksln everything.sln
popd
pause