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

taco:
	$(CL) -t atarixl  -O -o bin/taco src/taco.c src/atari_lib.s
clean:
	@$(DEL) bin/taco bin/taco.atr 2>$(NULLDEV)
dist: taco 
	cp assets/disk.atr bin/taco.atr
	franny -A bin/taco.atr add -i resources/RATA.FNT -o  RATA.FNT
	franny -A bin/taco.atr add -i resources/DEFAULT.FNT -o DEFAULT.FNT
	franny -A bin/taco.atr add -i resources/TACOBOT.IM8 -o TACOBOT.IM8
	franny -A bin/taco.atr add -i resources/RATA.FNT -i bin/taco -o AUTORUN.SYS
