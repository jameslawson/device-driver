obj-m := jjl_driver.o

all:	
	@echo ">>" Compiling the Source $<
	@echo ".." $<
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
