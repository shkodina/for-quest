###############################################################################
# Makefile for the project Modbus_168
###############################################################################

## General Flags
PROJECT = Modbus_168
MCU = atmega168
TARGET = Modbus_168.elf
CC = avr-gcc

CPP = avr-g++

## Options common to compile, link and assembly rules
COMMON = -mmcu=$(MCU)

## Compile options common for all C compilation units.
CFLAGS = $(COMMON)
CFLAGS += -Wall -gdwarf-2 -Os -std=gnu99 -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums
CFLAGS += -MD -MP -MT $(*F).o -MF dep/$(@F).d 

## Assembly specific flags
ASMFLAGS = $(COMMON)
ASMFLAGS += $(CFLAGS)
ASMFLAGS += -x assembler-with-cpp -Wa,-gdwarf2

## Linker flags
LDFLAGS = $(COMMON)
LDFLAGS +=  -Wl,-Map=Modbus_168.map


## Intel Hex file production flags
HEX_FLASH_FLAGS = -R .eeprom -R .fuse -R .lock -R .signature

HEX_EEPROM_FLAGS = -j .eeprom
HEX_EEPROM_FLAGS += --set-section-flags=.eeprom="alloc,load"
HEX_EEPROM_FLAGS += --change-section-lma .eeprom=0 --no-change-warnings


## Objects that must be built in order to link
OBJECTS = demo.o mbfunccoils.o mbfuncdiag.o mbfuncdisc.o mbfuncholding.o mbfuncinput.o mbfuncother.o mbutils.o mbcrc.o mbrtu.o portevent.o portserial.o porttimer.o mb.o 

## Objects explicitly added by the user
LINKONLYOBJECTS = 

## Build
all: $(TARGET) Modbus_168.hex Modbus_168.eep Modbus_168.lss size

## Compile
demo.o: ../demo.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

mbfunccoils.o: ../MB/functions/mbfunccoils.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

mbfuncdiag.o: ../MB/functions/mbfuncdiag.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

mbfuncdisc.o: ../MB/functions/mbfuncdisc.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

mbfuncholding.o: ../MB/functions/mbfuncholding.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

mbfuncinput.o: ../MB/functions/mbfuncinput.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

mbfuncother.o: ../MB/functions/mbfuncother.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

mbutils.o: ../MB/functions/mbutils.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

mbcrc.o: ../MB/rtu/mbcrc.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

mbrtu.o: ../MB/rtu/mbrtu.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

portevent.o: ../MB/port/portevent.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

portserial.o: ../MB/port/portserial.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

porttimer.o: ../MB/port/porttimer.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

mb.o: ../MB/mb.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

##Link
$(TARGET): $(OBJECTS)
	 $(CC) $(LDFLAGS) $(OBJECTS) $(LINKONLYOBJECTS) $(LIBDIRS) $(LIBS) -o $(TARGET)

%.hex: $(TARGET)
	avr-objcopy -O ihex $(HEX_FLASH_FLAGS)  $< $@

%.eep: $(TARGET)
	-avr-objcopy $(HEX_EEPROM_FLAGS) -O ihex $< $@ || exit 0

%.lss: $(TARGET)
	avr-objdump -h -S $< > $@

size: ${TARGET}
	@echo
	@avr-size -C --mcu=${MCU} ${TARGET}

## Clean target
.PHONY: clean
clean:
	-rm -rf $(OBJECTS) Modbus_168.elf dep/* Modbus_168.hex Modbus_168.eep Modbus_168.lss Modbus_168.map


## Other dependencies
-include $(shell mkdir dep 2>/dev/null) $(wildcard dep/*)

