# !/bin/bash

pushd src
make main
make clean
mv shell_talk.out ../
popd
