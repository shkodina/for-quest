###############################################################################
# Makefile for the project modbus
###############################################################################

## General Flags
PROJECT = modbus
MCU = atmega32
TARGET = modbus.elf
CC = avr-gcc

CPP = avr-g++

## Options common to compile, link and assembly rules
COMMON = -mmcu=$(MCU)

## Compile options common for all C compilation units.
CFLAGS = $(COMMON)
CFLAGS += -Wall -gdwarf-2 -std=gnu99 -DF_CPU=16000000UL -Os -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums
CFLAGS += -MD -MP -MT $(*F).o -MF dep/$(@F).d 

## Assembly specific flags
ASMFLAGS = $(COMMON)
ASMFLAGS += $(CFLAGS)
ASMFLAGS += -x assembler-with-cpp -Wa,-gdwarf2

## Linker flags
LDFLAGS = $(COMMON)
LDFLAGS +=  -Wl,-Map=modbus.map


## Intel Hex file production flags
HEX_FLASH_FLAGS = -R .eeprom -R .fuse -R .lock -R .signature

HEX_EEPROM_FLAGS = -j .eeprom
HEX_EEPROM_FLAGS += --set-section-flags=.eeprom="alloc,load"
HEX_EEPROM_FLAGS += --change-section-lma .eeprom=0 --no-change-warnings


## Objects that must be built in order to link
OBJECTS = main.o mb.o mbascii.o mbcrc.o mbfunccoils.o mbfuncdiag.o mbfuncdisc.o mbfuncholding.o mbfuncinput.o mbfuncother.o mbrtu.o mbtcp.o mbutils.o portevent.o portserial.o porttimer.o 

## Objects explicitly added by the user
LINKONLYOBJECTS = 

## Build
all: $(TARGET) modbus.hex modbus.eep modbus.lss size

## Compile
main.o: ../demo/main.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

mb.o: ../demo/mb.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

mbascii.o: ../demo/mbascii.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

mbcrc.o: ../demo/mbcrc.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

mbfunccoils.o: ../demo/mbfunccoils.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

mbfuncdiag.o: ../demo/mbfuncdiag.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

mbfuncdisc.o: ../demo/mbfuncdisc.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

mbfuncholding.o: ../demo/mbfuncholding.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

mbfuncinput.o: ../demo/mbfuncinput.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

mbfuncother.o: ../demo/mbfuncother.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

mbrtu.o: ../demo/mbrtu.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

mbtcp.o: ../demo/mbtcp.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

mbutils.o: ../demo/mbutils.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

portevent.o: ../demo/portevent.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

portserial.o: ../demo/portserial.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

porttimer.o: ../demo/porttimer.c
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
	-rm -rf $(OBJECTS) modbus.elf dep/* modbus.hex modbus.eep modbus.lss modbus.map


## Other dependencies
-include $(shell mkdir dep 2>NUL) $(wildcard dep/*)

