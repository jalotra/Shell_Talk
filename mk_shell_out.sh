# !/bin/bash

pushd src
make main
mv shell_talk.out ../
popd
