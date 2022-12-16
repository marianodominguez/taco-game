.ONESHELL:
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
	$(CL) -t atarixl -O --start-addr 0x3000 -I include -o bin/taco src/taco.c src/atari_lib.s src/splash.c
clean:
	@$(DEL) bin/taco.* 2>$(NULLDEV)
dist: taco 
	cp assets/disk.atr bin/taco.atr
	franny -A bin/taco.atr add -i resources/RATA.FNT -o  RATA.FNT
	franny -A bin/taco.atr add -i resources/DEFAULT.FNT -o DEFAULT.FNT
	franny -A bin/taco.atr add -i resources/TACOBOT.BMP -o TACOBOT.BMP
	franny -A bin/taco.atr add -i bin/taco -o AUTORUN.SYS
test:
	$(CL) -t atarixl -I include -o bin/test_g test/test_graphics.c src/atari_lib.s 
