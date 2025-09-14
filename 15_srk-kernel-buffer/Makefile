# Compile and sign srk-kernel-buffer.c
srk-kernel-buffer.ko: srk-kernel-buffer.c
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
	sh sign-module.sh

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean

.PHONY: all clean
