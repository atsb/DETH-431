#! /bin/sh
#
# We are not distributing Borland BGI fonts with GRX.
# To add them to the library put .CHR files in this
# directory (where this script is located) and run it
# after building GRX
#

rm -f *.o
gcc -O2 ../src/bgi/bin2c.c -o bin2c.exe 
for x in *.chr; do
	echo "Processing $x ...";
	name=`basename $x | sed -e 's,\.chr,,g'`
	./bin2c $x _${name}_font $name.c
	gcc -c -O2 $name.c
	rm $name.c
done

ar rc ../lib/dj2/libgrx20.a *.o
ranlib ../lib/dj2/libgrx20.a
rm -f *.o
