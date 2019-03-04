# Use the following for debugging with DrMemory, but you need to use 32 bit toolchain!
# gcc -std=c11 -Wall -m32 -g -fno-inline -fno-omit-frame-pointer -fopenmp -o mca_standalone.exe mca.c mca_io.c mca_standalone.c
# Use the following for debugging with gdb
# gcc -std=c99 -Wall -O3 -fopenmp -g -o mca_standalone.exe mca.c mca_io.c mca_standalone.c

FLAGS = -std=c11 -Wall -O3

all : mca_standalone mca_find_EP

mca_standalone : mca_standalone.c mca.c mca.h mca_io.h mca_io.c
	gcc $(FLAGS) -fopenmp -o mca_standalone.exe mca.c mca_io.c mca_standalone.c

mca_standalone_nomp : mca_standalone.c mca.c mca.h mca_io.h mca_io.c
	gcc $(FLAGS) -o mca_standalone_nomp.exe mca.c mca_io.c mca_standalone.c

mca_part : mca_part.c mca.c mca.h mca_io.h mca_io.c
	gcc $(FLAGS) -fopenmp -o mca_part.exe mca.c mca_io.c mca_part.c

debug : mca_standalone_debug mca_part_debug

mca_standalone_debug :
	gcc $(FLAGS) -fopenmp -g -o mca_standalone.exe mca.c mca_io.c mca_standalone.c

mca_part_debug :
	gcc $(FLAGS) -fopenmp -g -o mca_part.exe mca.c mca_io.c mca_part.c

mca_find_EP : mca_find_EP.c mca.c mca.h mca_io.h mca_io.c
	gcc $(FLAGS) -fopenmp -o mca_find_EP.exe mca.c mca_io.c mca_find_EP.c


clean :
	rm *.exe
