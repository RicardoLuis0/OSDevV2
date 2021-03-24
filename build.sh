#!/usr/bin/env bash

hasRBuild=$(command -v RBuild &> /dev/null)

set -eu -o pipefail

mkdir -p deps

[ -d "deps/lai" ] && echo "Updating LAI" && git -C deps/lai pull

[ ! -d "deps/lai" ] && echo "Cloning LAI" && git -C deps clone --depth=1 https://github.com/managarm/lai

if [ $hasRBuild ]
then
    RBuild --file=build_lai.json
    RBuild --file=build_kernel.json
else
    [ -d "deps/RBuild" ] && echo "Updating RBuild" && git -C deps/RBuild pull

    [ ! -d "deps/RBuild" ] && echo "Cloning RBuild" && git -C deps clone --depth=1 https://github.com/RicardoLuis0/RBuild
    
    if command -v ./.RBuild &> /dev/null
    then
        cd deps/RBuild
        ../../.RBuild --file=RBuild.json release
        cp build/lin/release/bin/RBuild ../../.RBuild
        cd ../..
    else
        cd deps/RBuild
        ./build.sh
        cp build/lin/release/bin/RBuild ../../.RBuild
        ../../.RBuild --file=RBuild.json release
        cp build/lin/release/bin/RBuild ../../.RBuild
        cd ../..
    fi
   ./.RBuild --file=build_lai.json
   ./.RBuild --file=build_kernel.json
fi

