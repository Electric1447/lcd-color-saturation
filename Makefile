TITLE    = LCD Color Saturation
TITLE_ID = LCDSAT000
TARGET   = lcdsat
OBJS     = main.o

LIBS = -lvita2d -lScePgf_stub -lSceGxm_stub -lSceAppMgr_stub \
	-lSceCtrl_stub -lSceSysmodule_stub -lSceDisplay_stub -lSceAVConfig_stub \
	-lSceCommonDialog_stub -lSceRegistryMgr_stub -lpng -lm -lz

PREFIX  = arm-vita-eabi
CC      = $(PREFIX)-gcc
CFLAGS  = -Wl,-q -O3 -Wall
ASFLAGS = $(CFLAGS)

all: $(TARGET).vpk

%.vpk: $(TARGET).velf
	vita-make-fself -c -at 0x0E -m 0x10000 $< eboot.bin
	vita-mksfoex -s TITLE_ID=$(TITLE_ID) -s APP_VER="01.20" "$(TITLE)" param.sfo
	vita-pack-vpk -s param.sfo -b eboot.bin -a sce_sys=sce_sys $@

%.velf: %.elf
	$(PREFIX)-strip -g $<
	vita-elf-create $< $@

$(TARGET).elf: $(OBJS)
	$(CC) $(CFLAGS) $^ $(LIBS) -o $@

clean:
	@rm -rf $(TARGET).velf $(TARGET).elf $(OBJS) eboot.bin param.sfo