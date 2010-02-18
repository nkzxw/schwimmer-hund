compressed_file_path=$1
src_dir=$2

#echo compressed_file_path: $compressed_file_path
#echo src_dir: $src_dir

#---manage files
# VERBOSE #tar xvjf $compressed_file_path -C $src_dir
tar xjf $compressed_file_path -C $src_dir

#---make

rm compilation.log

#g++ -I"$BOOST_ROOT" -I$src_dir -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"src/libs/os_services/test/test.d" -MT"src/libs/os_services/test/test.d" -o"src/libs/os_services/test/test.o" "$src_dir/libs/os_services/test/test.cpp"
g++ -I"$BOOST_ROOT" -I$src_dir -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$src_dir/libs/os_services/test/test.d" -MT"$src_dir/libs/os_services/test/test.d" -o"$src_dir/libs/os_services/test/test.o" "$src_dir/libs/os_services/test/test.cpp" &> compilation.log

# LINK: 
g++ -L"$BOOST_ROOT/stage/lib/" -o"$src_dir/../bin/PortableFileSystemWatcher"  $src_dir/libs/os_services/test/test.o   -lboost_thread-gcc44-mt-d-1_41 -lboost_system-gcc44-mt-d-1_41 -lboost_filesystem-gcc44-mt-d-1_41 &>> compilation.log

cat compilation.log | sed -e 's!/home/fernando/dev/schwimmer-hund/!.\\!g' | sed -e 's!/home/fernando/programs/!C:\\Program Files\\Boost\\!g' | tr "/" "\\"


rm compilation.log

#---delete temporaries
rm $compressed_file_path



echo --- COMPILATION FINISHED ---





# ------------------------------------------------------------------------------------
# OLDS
# ------------------------------------------------------------------------------------

# cat file | tr "/" "\\" | tr "home" "XXXX"
# sed -e s/:/-/g $a
# sed -e 's,bin,/home/sandeep/bin,' file
# cat file | sed -e 's!/home/fernando/dev/schwimmer-hund/!.\\!g' | tr "/" "\\"
# sed 's_c:/mydocuments/pictures_d:/mypics/personal_'




#echo 1 argument: $1
#echo 2 argument: $2
#echo 3 argument: $3

#echo --------------------------------------
#echo BOOST_ROOT: $BOOST_ROOT
#echo DEV_ROOT: $DEV_ROOT
#echo PATH: $PATH
#echo --------------------------------------

# g++ -I"$BOOST_ROOT" -I$src_dir -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP  -o"src/libs/os_services/test/test.o" "$src_dir/libs/os_services/test/test.cpp" &> compilation.log
# g++ -I"$BOOST_ROOT" -I/home/fernando/dev/schwimmer-hund/src -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"/home/fernando/dev/schwimmer-hund/src/libs/os_services/test/test.d" -MT"/home/fernando/dev/schwimmer-hund/src/libs/os_services/test/test.d" -o"/home/fernando/dev/schwimmer-hund/src/libs/os_services/test/test.o" "/home/fernando/dev/schwimmer-hund/src/libs/os_services/test/test.cpp"
