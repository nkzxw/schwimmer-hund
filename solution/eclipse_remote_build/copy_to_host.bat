@echo off

REM TODO pasar host_name por parámetro, asi esto puede servir tanto para Linux como para FreeBSD

echo 1
set current_datetime=%DATE:~6,4%%DATE:~3,2%%DATE:~0,2%%TIME:~0,2%%TIME:~3,2%%TIME:~6,2%%TIME:~9,2%
set tarfilename=%current_datetime%.tar
set compressedfilename=%tarfilename%.bz2
REM set compressedfilename=%tarfilename%.gz

echo 2
set host_uri=fernando@192.168.0.6
set host_target_dir=/home/fernando/dev/schwimmer-hund
set host_password=candombe

echo 3

REM del %tarfilename% >> log.txt
REM del %compressedfilename% >> log.txt

"C:\Program Files\7-Zip\7za.exe" a -ttar -r -x!*svn* %tarfilename% .\src\* >> log.txt

echo 4

REM "C:\Program Files\7-Zip\7za.exe" a -tgzip %compressedfilename% %tarfilename% >> log.txt
"C:\Program Files\7-Zip\7za.exe" a -tbzip2 %compressedfilename% %tarfilename% >> log.txt

echo 5

REM TODO cambiar variables
"C:\Program Files\putty\pscp.exe" -pw %host_password% %compressedfilename% %host_uri%:%host_target_dir%
echo 6
"C:\Program Files\putty\plink.exe" -pw candombe fernando@192.168.0.6 bash -l -c '. /home/fernando/dev/schwimmer-hund/make.sh /home/fernando/dev/schwimmer-hund/%compressedfilename%'

echo 7


del %tarfilename%
del %compressedfilename%
echo 8




REM echo %DATE%
REM echo %TIME%
REM echo %host_uri%
REM echo %host_target_dir%
REM echo %host_password%
REM echo %current_datetime%
REM echo %tarfilename%
REM echo %compressedfilename%




REM -------------------------------------------------------------------------------------------
REM Referencias:

REM http://dotnetperls.com/7-zip-examples
REM http://www.storage-b.com/windows/22
REM http://the.earth.li/~sgtatham/putty/0.60/htmldoc/Chapter5.html
REM http://osr507doc.sco.com/en/OSUserG/_Passing_to_shell_script.html
REM http://cygwin.com/ml/cygwin/2004-05/msg00251.html




