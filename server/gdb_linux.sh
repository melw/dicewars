rm -f dicewars_server
make -f makefile.linux
../../gdb-6.6/gdb/gdb dicewars_server
