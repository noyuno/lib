# lib

[![Build Status](https://travis-ci.org/noyuno/lib.svg?branch=master)](https://travis-ci.org/noyuno/lib)

## cc

C++14向けのライブラリ．UTF-32を中心とした文字列操作関数，標準入出力可能なプロセス関数など．

コンパイルするには以下が必要．

- clang++ 3.8
- boost 1.58
- libc++ 3.7

    sudo apt-get -qq install libc++-dev libc++abi-dev libboost-exception-dev libboost-program-options-dev

    cd cc
    cmake .
    make

アサートするには`./bin/main`を実行．

Doxygenでマニュアルを見ることができる．

