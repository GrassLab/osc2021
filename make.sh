make clean

if [ "$1" = "-D" ]
then
    export DEBUG=1
fi

make