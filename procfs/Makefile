obj-m += procfs_seq.o

all:
	make -C /lib/modules/${shell uname -r}/build  M=${PWD} modules

clean:
	make -C /lib/modules/${shell uname -r}/build  M=${PWD} clean
