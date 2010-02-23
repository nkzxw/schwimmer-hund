# http://www.freeos.com/guides/lsst/ch03sec06.html


dir=~/temp1
mkdir $dir


if [ "$1" = "" ]
then
  count=100
else
  count=$1
fi

#echo $count


if [ "$2" = "" ]
then
  wait=0
else
  wait=$2
fi

#echo $wait



touch $dir/file1
touch $dir/file2
touch $dir/file3

mkdir $dir/dir1
mkdir $dir/dir2
mkdir $dir/dir3

ln $dir/file1 $dir/hardlink1
ln $dir/file1 $dir/hardlink2
ln $dir/file1 $dir/hardlink3

ln -s $dir/file1 $dir/symlink1
ln -s $dir/file1 $dir/symlink2
ln -s $dir/file1 $dir/symlink3


echo Environment configured, press any key
read key


mv $dir/file1 $dir/file1B
ln $dir/file1B $dir/hardlink1B


echo Test finished, press any key
read key

rm -rf $dir/*





