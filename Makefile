.ONESHELL:
.PRECIOUS: src/%.s src/%.map

#Atari or atarixl
SYS ?= atari

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
	$(CL) -t $(SYS) -C cfg/tacobot.cfg --mapfile bin/taco.map -O -Os -Oi -Or -Wl "-D__RESERVED_MEMORY__=4000" -I include -o bin/taco src/font.c src/atari_lib.s src/splash.c src/taco.c
clean:
	@$(DEL) bin/taco.* 2>$(NULLDEV)
dist: taco
	mkdir -p tmp
	rm -rf tmp/*
	cp bin/taco tmp/AUTO
	cp resources/RATA.fnt tmp/RATA.FNT
	cp resources/TACOBOT.BMP tmp/TACOBOT.BMP
	dir2atr -S -B assets/xbootdos.obx bin/taco.atr tmp
	./reset_scores bin/taco.atr 243
test:
# 	$(CL) -t $(SYS) -Wl "-D__RESERVED_MEMORY__=0x4000" -I include -o bin/test_g test/ test_graphics.c src/atari_lib.s
#	$(CL) -t $(SYS) -I include -o bin/test_s test/test_scores.c src/scores.c
	$(CL) -t $(SYS) -I include -o bin/test_s test/test_graphics.c src/splash.c src/atari_lib.s
	rm -rf tmp/*
	cp bin/test_s tmp/AUTO
	cp resources/abejuarez_4_cr.bmp tmp/TEST.BMP
	dir2atr -S -B assets/xbootdos.obx bin/test.atr tmp
#	./reset_scores bin/test.atr 243

debug: clean
	$(CC) -t $(SYS) -O -I include -o tmp/taco.s src/taco.c
	$(CC) -t $(SYS)  -O -I include -o tmp/font.s src/font.c
	$(CC) -t $(SYS)  -O -I include -o tmp/splash.s src/splash.c
	$(AS) -t $(SYS)  -o tmp/taco.obj tmp/taco.s
	$(AS) -t $(SYS)  -o tmp/font.obj tmp/font.s
	$(AS) -t $(SYS)  -o tmp/atari_lib.obj src/atari_lib.s
	$(AS) -t $(SYS)  -o tmp/splash.obj tmp/splash.s
	$(LD) -o bin/taco -C cfg/tacobot.cfg -D__RESERVED_MEMORY__=0x3000 --mapfile tmp/taco.map  tmp/taco.obj tmp/font.obj tmp/atari_lib.obj tmp/splash.obj atari.lib
