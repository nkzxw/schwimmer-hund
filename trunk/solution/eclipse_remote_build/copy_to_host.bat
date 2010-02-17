@echo off

REM TODO: usar sesiones que no se cierren
REM TODO: ver de conectarse usando las SAVED de putty para no tener que enviar el PWD por texto
REM TODO: meter algo en que medio que intercepte la salida standard y haga reemplazos en el texto de los nombres de los arhivos. Todo esto para el parser detecte bien los nombres de los archivos
REM TODO: eliminar los comentarios que pone el bash al hacer Login ( -l )

set temp_var = %TIME:~0,1%
IF [%temp_var%]==[] (set current_datetime=%DATE:~6,4%%DATE:~3,2%%DATE:~0,2%0%TIME:~1,1%%TIME:~3,2%%TIME:~6,2%%TIME:~9,2%) ELSE set current_datetime=%DATE:~6,4%%DATE:~3,2%%DATE:~0,2%%TIME:~0,2%%TIME:~3,2%%TIME:~6,2%%TIME:~9,2%

set tar_filename=%current_datetime%.tar
set compressed_filename=%tar_filename%.bz2


set src_dir=%1
set host_uri=%2
set host_password=%3
set host_project_dir=%4
set host_make_sh=%5
set host_src_dir=%6

    

REM del %tar_filename% >> log.txt
REM del %compressed_filename% >> log.txt

"C:\Program Files\7-Zip\7za.exe" a -ttar -r -x!*svn* %tar_filename% %src_dir%\* >> log.txt
"C:\Program Files\7-Zip\7za.exe" a -tbzip2 %compressed_filename% %tar_filename% >> log.txt

"C:\Program Files\putty\pscp.exe" -pw %host_password% %compressed_filename% %host_uri%:%host_project_dir% >> log.txt
REM "C:\Program Files\putty\pscp.exe" -pw candombe 2010021702225403.tar.bz2 fernando@192.168.120.36:/home/fernando/dev/schwimmer-hund

"C:\Program Files\putty\plink.exe" -pw %host_password% %host_uri% bash -l -c '. %host_make_sh% %host_project_dir%/%compressed_filename% %host_src_dir%'



del %tar_filename%
del %compressed_filename%
del log.txt



REM echo %1
REM echo %current_datetime%
REM echo %DATE%
REM echo %TIME%
REM echo %host_uri%
REM echo %host_project_dir%
REM echo %host_password%
REM echo %current_datetime%
REM echo %tar_filename%
REM echo %compressed_filename%



REM -------------------------------------------------------------------------------------------
REM Llamar desde Eclipse:
REM copy_to_host.bat ${workspace_loc:/boost_file_system_monitor_remote_build}\..\..\src fernando@192.168.120.36 candombe /home/fernando/dev/schwimmer-hund /home/fernando/dev/schwimmer-hund/solution/eclipse_remote_build/make.sh $DEV_ROOT/schwimmer-hund/src/


REM -------------------------------------------------------------------------------------------
REM Referencias:

REM http://dotnetperls.com/7-zip-examples
REM http://www.storage-b.com/windows/22
REM http://the.earth.li/~sgtatham/putty/0.60/htmldoc/Chapter5.html
REM http://osr507doc.sco.com/en/OSUserG/_Passing_to_shell_script.html
REM http://cygwin.com/ml/cygwin/2004-05/msg00251.html
REM http://www.robvanderwoude.com/if.php



