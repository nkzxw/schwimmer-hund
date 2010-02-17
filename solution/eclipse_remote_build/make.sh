echo 1 argument: $1
echo 2 argument: $2

#echo --------------------------------------
#echo BOOST_ROOT: $BOOST_ROOT
#echo DEV_ROOT: $DEV_ROOT
#echo PATH: $PATH
#echo --------------------------------------

#---manage files
#TODO: ver que el directorio no este hardcodeado
tar xvjf $1 -C $DEV_ROOT/schwimmer-hund/src/

#---make
#TODO: PROJECT_ROOT

g++ -I"$BOOST_ROOT" -I$DEV_ROOT/schwimmer-hund/src -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"src/libs/os_services/test/test.d" -MT"src/libs/os_services/test/test
.d" -o"src/libs/os_services/test/test.o" "$DEV_ROOT/schwimmer-hund/src/libs/os_services/test/test.cpp"

#---delete temporaries
rm $1
