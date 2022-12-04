# Run 'make SYS=<target>'; or, set a SYS env.
# var. to build for another target system.
SYS ?= atarixl

NULLDEV = /dev/null
DEL = $(RM)
RMDIR = $(RM) -r
LD65_LIB=/usr/share/cc65/lib
LD65_CFG=/usr/share/cc65/cfg

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
	$(CL) -t atarixl -O -o bin/taco src/taco.c
	
clean:
	@$(DEL) hello 2>$(NULLDEV)