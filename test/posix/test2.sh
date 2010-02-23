# http://www.freeos.com/guides/lsst/ch03sec06.html


#TODO: asegurarse que los directorios existan

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



for ((  i = 0 ;  i <= $count;  i++  ))
do
  #echo "Welcome $i times"
  filename="$dir/autofile_$i"
  #echo $filename
  touch $filename
  #sleep $wait
done



for ((  i = 0 ;  i <= $count;  i++  ))
do
  filename="$dir/autofile_$i"
  rm $filename
  #sleep $wait
done

