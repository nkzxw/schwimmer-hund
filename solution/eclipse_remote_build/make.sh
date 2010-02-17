#echo 1 argument: $1
#echo 2 argument: $2
#echo 3 argument: $3

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

rm compilation.log

#g++ -I"$BOOST_ROOT" -I$src_dir -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"src/libs/os_services/test/test.d" -MT"src/libs/os_services/test/test.d" -o"src/libs/os_services/test/test.o" "$DEV_ROOT/schwimmer-hund/src/libs/os_services/test/test.cpp"
g++ -I"$BOOST_ROOT" -I$src_dir -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"src/libs/os_services/test/test.d" -MT"src/libs/os_services/test/test.d" -o"src/libs/os_services/test/test.o" "$DEV_ROOT/schwimmer-hund/src/libs/os_services/test/test.cpp" &> compilation.log

#cat compilation.log | tr "/" "\\" | tr "/" "\\"
cat compilation.log | sed -e 's!/home/fernando/dev/schwimmer-hund/!.\\!g' | tr "/" "\\"


rm compilation.log




#echo "/home/fernando/dev/schwimmer-hund/src/boost/os_services/details/freebsd_impl.hpp:330:" | tr "/" "\\"

#---delete temporaries
rm $compressed_file_path



# ------------------------------------------------------------------------------------
# OLDS
# ------------------------------------------------------------------------------------

# cat file | tr "/" "\\" | tr "home" "XXXX"
# sed -e s/:/-/g $a
# sed -e 's,bin,/home/sandeep/bin,' file
# cat file | sed -e 's!/home/fernando/dev/schwimmer-hund/!.\\!g' | tr "/" "\\"
# sed 's_c:/mydocuments/pictures_d:/mypics/personal_'
