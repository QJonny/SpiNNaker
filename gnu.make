# build example x.c with:   make EXAMPLE=x

EXAMPLE=MFM
THUMB=no
CFLAGS=-DDEBUG

#LIB_DIR = /home/amulinks/spinnaker/code/lib
#INC_DIR = /home/amulinks/spinnaker/code/include
LIB_DIR = ../../src
INC_DIR = ../../src
LIBGCC = ../../../../arm-none-linux-gnueabi/lib/gcc/arm-none-linux-gnueabi/4.8.1/libgcc.a


FILES = fixed_float.c math.c network.c communication.c
OBJ_FILES = fixed_float.o math.o network.o communication.o

CC := arm-none-linux-gnueabi-gcc -c -O1 -nostdlib -mthumb-interwork \
      -march=armv5te -std=gnu99 -I $(INC_DIR) $(CFLAGS)
CT := $(CC)
LD := arm-none-linux-gnueabi-ld 
OC := arm-none-linux-gnueabi-objcopy
OD := arm-none-linux-gnueabi-objdump
RM := /bin/rm -f
CAT := /bin/cat

ifeq ($(THUMB),yes)
  CT += -mthumb -DTHUMB
  AS += --defsym THUMB=1
endif


OBJS = $(EXAMPLE).o $(OBJ_FILES)

target: $(OBJS) example.lnk
	$(LD) -T example.lnk $(LIB_DIR)/spin1_api_gnulib.o $(OBJS) $(LIBGCC)
	$(OC) --set-section-flags APLX=alloc,code,readonly a.out $(EXAMPLE).elf
	$(OC) -O binary -j APLX    $(EXAMPLE).elf APLX.bin
	$(OC) -O binary -j RO_DATA $(EXAMPLE).elf RO_DATA.bin
	$(OC) -O binary -j RW_DATA $(EXAMPLE).elf RW_DATA.bin
	$(OD) -dt $(EXAMPLE).elf > $(EXAMPLE).lst
	${CAT} APLX.bin RO_DATA.bin RW_DATA.bin > $(EXAMPLE).aplx
	${RM} APLX.bin RO_DATA.bin RW_DATA.bin a.out

$(EXAMPLE).o: $(EXAMPLE).c
	$(CT) $(EXAMPLE).c

$(OBJ_FILES): $(FILES)
	$(CT) $(FILES)

clean:
	${RM} *.o *.aplx *.elf *.lst *~
	${RM} APLX.bin RO_DATA.bin RW_DATA.bin a.out

tidy:
	${RM} *.o *.elf *.lst *~
	${RM} APLX.bin RO_DATA.bin RW_DATA.bin a.out
