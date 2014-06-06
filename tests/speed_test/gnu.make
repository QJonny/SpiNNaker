# build example x.c with:   make EXAMPLE=x

EXAMPLE=speed_test
THUMB=no


LIB_DIR = ../../../../src

INC_DIR = ../../../../src


LIBSPIN = $(LIB_DIR)/spin1_api_gnulib.o


FILES = $(EXAMPLE).c



CC := arm-none-linux-gnueabi-gcc
LD := arm-none-linux-gnueabi-ld 
OC := arm-none-linux-gnueabi-objcopy
OD := arm-none-linux-gnueabi-objdump
RM := /bin/rm -f
CAT := /bin/cat

CCFLAGS := -O1 -mthumb-interwork -fno-use-linker-plugin -march=armv5te -std=gnu99 -DDEBUG -I $(INC_DIR)
LDFLAGS := -T example.lnk -nostdlib -nostartfiles $(LIBSPIN) 




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
