#!/usr/bin/env bash 
mainFile=src/main.c 
testFile=test/test-1.pug
#valgrind --leak-check=full --show-leak-kinds=all
tcc -DDEBUG \
-I./include \
-I./include/bellard \
$(pkg-config --cflags glib-2.0) \
$(pkg-config --libs glib-2.0) \
$mainFile include/bellard/cutils.c include/bellard/libregexp.c include/bellard/libunicode.c -o ./bin/test && ./bin/test  \
$testFile
