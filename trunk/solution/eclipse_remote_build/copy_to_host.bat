@echo off

set temp_var = %TIME:~0,1%
IF [%temp_var%]==[] (set current_datetime=%DATE:~6,4%%DATE:~3,2%%DATE:~0,2%0%TIME:~1,1%%TIME:~3,2%%TIME:~6,2%%TIME:~9,2%) ELSE set current_datetime=%DATE:~6,4%%DATE:~3,2%%DATE:~0,2%%TIME:~0,2%%TIME:~3,2%%TIME:~6,2%%TIME:~9,2%

set tarfilename=%current_datetime%.tar
set compressedfilename=%tarfilename%.bz2


set src_directory=%1
set host_uri=%2
set host_password=%3
set host_target_dir=%4

REM /home/fernando/dev/schwimmer-hund
REM del %tarfilename% >> log.txt
REM del %compressedfilename% >> log.txt

"C:\Program Files\7-Zip\7za.exe" a -ttar -r -x!*svn* %tarfilename% %src_directory%\* >> log.txt
"C:\Program Files\7-Zip\7za.exe" a -tbzip2 %compressedfilename% %tarfilename% >> log.txt

"C:\Program Files\putty\pscp.exe" -pw %host_password% %compressedfilename% %host_uri%:%host_target_dir%
REM "C:\Program Files\putty\pscp.exe" -pw candombe 2010021702225403.tar.bz2 fernando@192.168.120.36:/home/fernando/dev/schwimmer-hund

"C:\Program Files\putty\plink.exe" -pw %host_password% %host_uri% bash -l -c '. /home/fernando/dev/schwimmer-hund/solution/eclipse_remote_build/make.sh /home/fernando/dev/schwimmer-hund/%compressedfilename%'



del %tarfilename%
del %compressedfilename%
del log.txt



REM echo %1
REM echo %current_datetime%
REM echo %DATE%
REM echo %TIME%
REM echo %host_uri%
REM echo %host_target_dir%
REM echo %host_password%
REM echo %current_datetime%
REM echo %tarfilename%
REM echo %compressedfilename%



REM -------------------------------------------------------------------------------------------
REM Llamar desde Eclipse:
REM copy_to_host.bat ${workspace_loc:/boost_file_system_monitor_remote_build}\..\..\src fernando@192.168.120.36 candombe /home/fernando/dev/schwimmer-hund


REM -------------------------------------------------------------------------------------------
REM Referencias:

REM http://dotnetperls.com/7-zip-examples
REM http://www.storage-b.com/windows/22
REM http://the.earth.li/~sgtatham/putty/0.60/htmldoc/Chapter5.html
REM http://osr507doc.sco.com/en/OSUserG/_Passing_to_shell_script.html
REM http://cygwin.com/ml/cygwin/2004-05/msg00251.html
REM http://www.robvanderwoude.com/if.php



