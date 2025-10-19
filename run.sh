#!/usr/bin/env bash 
mainFile=src/main.c 
testFile=test/test-1.pug

tcc \
-I./include \
$(pkg-config --cflags glib-2.0) \
$(pkg-config --libs glib-2.0) \
$mainFile -o ./bin/test && ./bin/test  \
$testFile 
