# build example x.c with:   make EXAMPLE=x

EXAMPLE=MFM
THUMB=no


LIB_DIR = ../../src

INC_DIR = ../../src
INC_MUSL = ./musl-1.0.0/include


LIBGCC = ../../../../arm-none-linux-gnueabi/lib/gcc/arm-none-linux-gnueabi/4.8.1/libgcc.a
LIBSPIN = $(LIB_DIR)/spin1_api_gnulib.o
MUSL_LIBC = ./musl-1.0.0/lib/libc.a


FILES = $(EXAMPLE).c network.c communication.c spin1_math.c hacks.c



CC := arm-none-linux-gnueabi-gcc
LD := arm-none-linux-gnueabi-ld 
OC := arm-none-linux-gnueabi-objcopy
OD := arm-none-linux-gnueabi-objdump
RM := /bin/rm -f
CAT := /bin/cat

CCFLAGS := -O1 -mthumb-interwork -fno-use-linker-plugin -march=armv5te -std=gnu99 -DDEBUG -I $(INC_MUSL) -I $(INC_DIR)
LDFLAGS := -T example.lnk -nostdlib -nostartfiles $(LIBSPIN) $(MUSL_LIBC) $(LIBGCC)




ifeq ($(THUMB),yes)
  CC += -mthumb -DTHUMB
endif


	
target: example.lnk
	$(CC) $(FILES) $(CCFLAGS) $(LDFLAGS)
	$(OC) --set-section-flags APLX=alloc,code,readonly a.out $(EXAMPLE).elf
	$(OC) -O binary -j APLX    $(EXAMPLE).elf APLX.bin
	$(OC) -O binary -j RO_DATA $(EXAMPLE).elf RO_DATA.bin
	$(OC) -O binary -j RW_DATA $(EXAMPLE).elf RW_DATA.bin
	$(OD) -dt $(EXAMPLE).elf > $(EXAMPLE).lst
	${CAT} APLX.bin RO_DATA.bin RW_DATA.bin > $(EXAMPLE).aplx
	${RM} APLX.bin RO_DATA.bin RW_DATA.bin a.out



clean:
	${RM} *.o *.aplx *.elf *.lst *~
	${RM} APLX.bin RO_DATA.bin RW_DATA.bin a.out

tidy:
	${RM} *.o *.elf *.lst *~
	${RM} APLX.bin RO_DATA.bin RW_DATA.bin a.out
