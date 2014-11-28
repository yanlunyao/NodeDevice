@echo off
md ..\Release

C:\Keil\ARM\BIN40\fromelf.exe --bin -o ./OUTPUT/NodeDevice.bin ./OUTPUT/NodeDevice.axf

echo copy necessary files to release folder
copy .\OUTPUT\NodeDevice.bin ..\Release\NodeDevice" "V01-C20130705-PCB.bin
copy .\OUTPUT\NodeDevice.hex ..\Release\NodeDevice" "V01-C20130705-PCB.hex




