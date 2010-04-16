@echo off

REM TODO: usar sesiones que no se cierren
REM TODO: ver de conectarse usando las SAVED de putty para no tener que enviar el PWD por texto
REM TODO: meter algo en que medio que intercepte la salida standard y haga reemplazos en el texto de los nombres de los arhivos. Todo esto para el parser detecte bien los nombres de los archivos
REM TODO: eliminar los comentarios que pone el bash al hacer Login ( -l )

set temp_var = %TIME:~0,1%
IF [%temp_var%]==[] (set current_datetime=%DATE:~6,4%%DATE:~3,2%%DATE:~0,2%0%TIME:~1,1%%TIME:~3,2%%TIME:~6,2%%TIME:~9,2%) ELSE set current_datetime=%DATE:~6,4%%DATE:~3,2%%DATE:~0,2%%TIME:~0,2%%TIME:~3,2%%TIME:~6,2%%TIME:~9,2%

set host_uri=%1
set host_password=%2
set build_sh_file=%3

    
REM "C:\Program Files\putty\plink.exe" -pw %host_password% %host_uri% bash -l -c '. %host_make_sh% %host_project_dir%/%compressed_filename% %host_src_dir%'
"C:\Program Files\putty\plink.exe" -pw %host_password% %host_uri% /home/fernando/dev/schwimmer-hund/solution/scons/build.sh



REM echo %1
REM echo %current_datetime%
REM echo %DATE%
REM echo %TIME%
REM echo %host_uri%
REM echo %host_project_dir%
REM echo %host_password%
REM echo %current_datetime%



REM -------------------------------------------------------------------------------------------
REM Llamar:
REM call execute_remote_build.bat fernando@192.168.0.5 candombe /home/fernando/dev/schwimmer-hund/solution/scons/build.sh


REM -------------------------------------------------------------------------------------------
REM Referencias:

REM http://dotnetperls.com/7-zip-examples
REM http://www.storage-b.com/windows/22
REM http://the.earth.li/~sgtatham/putty/0.60/htmldoc/Chapter5.html
REM http://osr507doc.sco.com/en/OSUserG/_Passing_to_shell_script.html
REM http://cygwin.com/ml/cygwin/2004-05/msg00251.html
REM http://www.robvanderwoude.com/if.php



