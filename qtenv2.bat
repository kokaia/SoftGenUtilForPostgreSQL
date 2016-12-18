@echo off
echo Setting up environment for Qt usage...
set QTDIR=C:\Qt\QtSDK\Desktop\Qt\4.7.3\mingw
set PATH=C:\Qt\QtSDK\mingw\bin;%PATH%
set PATH=%QTDIR%\bin;%PATH%
cd /d D:\Documents\QT\SoftGen
copy /Y  /B test-build-desktop\debug\test.exe SoftGenUtil-build-desktop\debug\test.exe
cd /d "D:\Documents\QT\SoftGen\SoftGenUtil-build-desktop\debug"