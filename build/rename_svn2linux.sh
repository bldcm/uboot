#!/bin/bash
#
# The U-Boot source contains some files which exist in the same directory
# as well with uppercase names as with lowercase names. 
# The contents of the files with the 'same' name is different.
#
# The following list gives a complete overview of the files concerned: 
#
# Directory doc:
#  README.SBC8560    README.sbc8560
#
# Directory include/configs:
#  EBONY.h           ebony.h
#  SBC8560.h         sbc8560.h
#  OCOTEA.h          ocotea.h
#
# The sources from the SVN repository may be checked out on a Windows machine
# as well as on a Linux machine. Unfortunately on a Windows file system,
# two files having the same name, differing only in case, can't exist in one
# directory. Therefore a checkout of the directories listed above will always
# fail on a Windows machine.
#
# Remedy: Each file having an uppercase name has been renamed in the SVN 
# repository. The renaming scheme is: FILENAME.ext -> FILENAME.ext.off, in
# other words, the extension '.off' has been added to the name of each of these
# files.
#
# In order to be able to compile these files,
# they should be renamed to their original names. This script will do 
# that for you! Actually, it will COPY those files to their original names. 
# This will prevent SVN complaining that some files have been deleted if 
# you do a commit.
#
# Please note, that if you modify one of these files (having their original 
# names), you should copy them back to their SVN names, otherwise your 
# modifications won't be checked in when you commit. Please do this manually!
#
# Nanko Verwaal, 02.08.2007

result=0

# The script should be executed from the root of the source project
root=$(pwd)

cd $root/doc
for s in README.SBC8560 ; do
	cp -v $s.off $s
	result=$((result + $?))
done
 
cd $root/include/configs
for s in EBONY SBC8560 OCOTEA ; do
	cp -v $s.h.off $s.h
	result=$((result + $?))
done
 

cd $root

if [ $result -eq 0 ] ; then
	echo "Renamed successfully"
else
	echo "Errors occurred during copying; couldn't copy $result files" >&2
fi

exit $result

