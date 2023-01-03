.ONESHELL:
.PRECIOUS: src/%.s src/%.map
# Run 'make SYS=<target>'; or, set a SYS env.
# var. to build for another target system.
SYS ?= atarixl

NULLDEV = /dev/null
DEL = $(RM)
RMDIR = $(RM) -r

ifdef CC65_HOME
  AS = $(CC65_HOME)/bin/ca65
  CC = $(CC65_HOME)/bin/cc65
  CL = $(CC65_HOME)/bin/cl65
  LD = $(CC65_HOME)/bin/ld65
else
  AS := $(if $(wildcard /usr/bin/ca65*),/usr/bin/ca65,ca65)
  CC := $(if $(wildcard /usr/bin/ca65*),/usr/bin/cc65,cc65)
  CL := $(if $(wildcard /usr/bin/ca65*),/usr/bin/cl65,cl65)
  LD := $(if $(wildcard /usr/bin/ca65*),/usr/bin/ld65,ld65)
endif

taco: clean
	$(CL) -t atari -C cfg/tacobot.cfg --mapfile tmp/taco.map -O -Os -Oi -Or --start-addr 0x3000 -Wl "-D__RESERVED_MEMORY__=0x3000" -I include -o bin/taco src/font.c src/atari_lib.s src/splash.c src/taco.c 
clean:
	@$(DEL) bin/taco.* 2>$(NULLDEV)
dist: taco 
	cp assets/disk.atr bin/taco.atr
	franny -A bin/taco.atr add -i resources/RATA.fnt -o  RATA.FNT
	franny -A bin/taco.atr add -i resources/TACOBOT.BMP -o TACOBOT.BMP
	franny -A bin/taco.atr add -i bin/taco -o AUTORUN.SYS
test:
	$(CL) -t atari -C cfg/tacobot.cfg  -Wl "-D__RESERVED_MEMORY__=0x2000" -I include -o bin/test_g test/test_graphics.c src/atari_lib.s 
debug: clean
	$(CC) -t atari -O -I include -o tmp/taco.s src/taco.c
	$(CC) -t atari  -O -I include -o tmp/font.s src/font.c
	$(CC) -t atari  -O -I include -o tmp/splash.s src/splash.c
	$(AS) -t atari  -o tmp/taco.obj tmp/taco.s  
	$(AS) -t atari  -o tmp/font.obj tmp/font.s
	$(AS) -t atari  -o tmp/atari_lib.obj src/atari_lib.s
	$(AS) -t atari  -o tmp/splash.obj tmp/splash.s
	$(LD) -o bin/taco -C cfg/tacobot.cfg -D__RESERVED_MEMORY__=0x3000 --mapfile tmp/taco.map  tmp/taco.obj tmp/font.obj tmp/atari_lib.obj tmp/splash.obj atari.lib 
