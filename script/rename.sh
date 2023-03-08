for name in `ls *.cc`
do
    mv $name ${name%.cc}.cpp
done