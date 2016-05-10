#!/bin/bash

# make temporary build dir
temp=`mktemp -d -t 'build_tmp'`
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

#remove old
rm -rf "${DIR}/build"
#build
xcodebuild -project "${DIR}/gpu-switch.xcodeproj" -scheme gpu-switch -configuration Release -derivedDataPath "$temp" | egrep '^(/.+:[0-9+:[0-9]+:.(error):|fatal|===)' -

#copy files back
mkdir -p "${DIR}/build"
cp "$temp/Build/Products/Release/gpu-switch" "${DIR}/build/"
printf "build successfull: ${DIR}/build/gpu-switch\n";

#delete temp
rm -rf $temp
