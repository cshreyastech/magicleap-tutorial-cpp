@echo off

rem mabu ctest.package -t device

mldb connect 192.168.86.247:5555
mldb uninstall com.rogerboesch.ctest
mldb install -u .out\ctest\ctest.mpk
mldb launch com.rogerboesch.ctest