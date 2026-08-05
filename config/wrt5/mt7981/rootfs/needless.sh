#!/bin/bash

# delete the needless init
lookdir="./etc/init.d"
exception="boot sysctl done preland land"
for file in `ls ${lookdir}`
do
    save=0
    for ext in ${exception}
    do
        if [ ${file} == ${ext} ]
        then
            save=1
        fi
    done
    if [ $save != 1 ]
    then
        rm -f ${lookdir}/${file}
    fi
done


# ln the var mnt to /tmp
rm -fr ./mnt
ln -s /tmp/mnt ./mnt
# mv /lib/firmware to /usr/share/firmware make /lib/firmwre can write
#rm -fr ./usr/share/firmware
#mv ./lib/firmware .//usr/share/
#ln -s /tmp/firmware .//lib/firmware

# delete self
rm -fr ./needless.sh

