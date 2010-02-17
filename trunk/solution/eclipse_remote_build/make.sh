echo 1 argument: $1
echo 2 argument: $2
echo 3 argument: $3

#echo --------------------------------------
#echo BOOST_ROOT: $BOOST_ROOT
#echo DEV_ROOT: $DEV_ROOT
#echo PATH: $PATH
#echo --------------------------------------

compressed_file_path=$1
src_dir=$2

#echo compressed_file_path: $compressed_file_path
#echo src_dir: $src_dir

#---manage files
#TODO: ver que el directorio no este hardcodeado
# VERBOSE #tar xvjf $compressed_file_path -C $src_dir
tar xjf $compressed_file_path -C $src_dir

#---make
#TODO: PROJECT_ROOT

#g++ -I"$BOOST_ROOT" -I$src_dir -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"src/libs/os_services/test/test.d" -MT"src/libs/os_services/test/test.d" -o"src/libs/os_services/test/test.o" "$DEV_ROOT/schwimmer-hund/src/libs/os_services/test/test.cpp" | tr "/" "\\"
g++ -I"$BOOST_ROOT" -I$src_dir -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"src/libs/os_services/test/test.d" -MT"src/libs/os_services/test/test.d" -o"src/libs/os_services/test/test.o" "$DEV_ROOT/schwimmer-hund/src/libs/os_services/test/test.cpp" | tr "/" "\\"


#echo "/home/fernando/dev/schwimmer-hund/src/boost/os_services/details/freebsd_impl.hpp:330:" | tr "/" "\\"

#---delete temporaries
rm $compressed_file_path
