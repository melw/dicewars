rm -f dicewars.log
rm -f dicewars_server
make -f makefile.linux
while [ "$var1" != "end" ]     # while test "$var1" != "end"
do
	../../valgrind-local_test/bin/valgrind --log-file=valgrind.output --leak-check=yes ./dicewars_server
done

exit 0
