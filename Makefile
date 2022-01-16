#!smake
include $(ROOT)/usr/include/make/PRdefs

SFZ2N64:=bin/sfz2n64
MIDICVT:=bin/midicvt

OPTIMIZER		:= -O0
LCDEFS			:= -g -Isrc/ -I/usr/include/n64/nustd -Werror
N64LIB			:= -lultra_rom -lnustd

LCINCS =	-I. -I/usr/include/n64/PR 

BASE_TARGET_NAME = build/telocation

LD_SCRIPT	= telocation.ld
CP_LD_SCRIPT	= build/telocation

ASMFILES    =	$(shell find asm/ -type f -name '*.s')

ASMOBJECTS  =	$(patsubst %.s, build/%.o, $(ASMFILES))

LDIRT  =	$(BASE_TARGET_NAME).elf $(CP_LD_SCRIPT) $(BASE_TARGET_NAME).z64 $(BASE_TARGET_NAME)_no.map $(ASMOBJECTS)
LDFLAGS =	-L/usr/lib/n64 $(N64LIB)  -L$(N64_LIBGCCDIR) -lgcc

GO_SOURCE = $(wildcard ./levelbuilder/*.go)

levelbuilder/levelbuilder: $(GO_SOURCE)
	go build -o levelbuilder/levelbuilder ./levelbuilder/

SLIDE_BUILDER_SOURCE = $(wildcard ./slidebuilder/*.go)

slidebuilder/slidebuilder: $(SLIDE_BUILDER_SOURCE)
	go build -o slidebuilder/slidebuilder ./slidebuilder/

LEVELS = cadet_intro \
	switch_intro \
	robot_intro \
	robot_platform \
	switch_sizes \
	lava_platform \
	connected_colors \
	switch_tagteam \
	inverted_doors \
	platform_intro \
	spiral \
	loop_around \
	radioactive_river \
	canyon \
	track_blockage \
	raised_platform \
	moving_platforms \
	switch_mania \
	raise_robot \
	final

	
LEVEL_GEO = $(foreach level, $(LEVELS), src/levels/$(level)/geo.c)
LEVEL_DATA = $(foreach level, $(LEVELS), levels/$(level).level)
LEVEL_META_DATA = $(foreach level, $(LEVELS), levels/$(level).meta)

src/levels/%/geo.c: levels/%.level levels/%.meta levelbuilder/levelbuilder
	@mkdir -p $(@D)
	levelbuilder/levelbuilder level $* $< $@

build/spec/level_segs src/levels/levels.c src/levels/levels.h: levelbuilder/levelbuilder $(LEVEL_DATA) $(LEVEL_META_DATA)
	@mkdir -p build/spec
	levelbuilder/levelbuilder levelpack all_levels 0 src/levels/levels.c build/spec/level_segs $(LEVELS)

IMAGE_SLIDES = _00_rocket \
	_01_insidecockpit \
	_01_insidecockpit_flash \
	_02_fuelgauge \
	_02_fuelgauge_flash \
	_03_mapcloseup \
	_04_seebutton \
	_05_pushbutton \
	_06_mapchanged \
	_08_rocket_turn \
	_09_see_planet \
	_00_bad_ending \
	_00_bad_ending_closeup \
	_01_see_robot \
	_02_sad_cadet \
	_03_leave_robot \
	_04_leave_planet \
	_00_good_ending \
	_00_good_ending_closeup \
	_02_leave_with_robot \
	_10_credits_text \
	_11_homebrew



SLIDE_IMAGES = $(foreach slide, $(IMAGE_SLIDES), imageslides/$(slide).jpeg)

src/cutscene/slides.h src/cutscene/slides.c build/spec/slide_segs: slidebuilder/slidebuilder $(SLIDE_IMAGES)
	@mkdir -p build/spec
	slidebuilder/slidebuilder src/cutscene/slides.h src/cutscene/slides.c build/spec/slide_segs $(IMAGE_SLIDES)

COLLISION_SHAPES = solid_block tunnel_block ramp_block stair_block entrance_exit rocket_collision large_switch
COLLISION_GEO = $(foreach shape, $(COLLISION_SHAPES), src/collision/geo/$(shape).inc.c)

src/collision/geo/%.inc.c: collision/%.ply levelbuilder/levelbuilder
	@mkdir -p $(@D)
	levelbuilder/levelbuilder collision $* $< $@

src/collision/geo/geo.c: $(COLLISION_GEO)

PLAYER_SOUNDS = sound/clips/Jump.aifc \
	sound/clips/JumpPeakBeep.aifc \
	sound/clips/Landing.aifc \
	sound/clips/AstronautFootsteps.ins \
	sound/clips/AstronautSlide1.aifc \
	sound/clips/AstronautSlide2.aifc \
	sound/clips/WarpAstronautOnly.aifc \
	sound/clips/WarpRobotOnly.aifc \
	sound/clips/WarpBoth.aifc \
	sound/clips/AstronautDeath_Fall.aifc \
	sound/clips/RobotDeath_Fall.aifc \
	sound/clips/Land_onRobotHead.aifc \
	sound/clips/MetalFootsteps_RoboHead.ins \
	sound/clips/RoboDestroy.aifc \
	sound/clips/RobotMovement.ins \
	sound/clips/RobotStartMoving.aifc \
	sound/clips/SwitchBackChar.aifc \
	sound/clips/SwitchChar.aifc \
	sound/clips/BigButton.aifc \
	sound/clips/SmallButton.aifc \
	sound/clips/WarpGoGreen_Big.aifc \
	sound/clips/WarpGoGreen_Small.aifc \
	sound/clips/UIScroll.aifc \
	sound/clips/UISelect.aifc \
	sound/clips/GemPickup.aifc \
	sound/clips/SlidingBlock4.aifc \
	sound/clips/PlatformMove1.ins \
	sound/clips/CameraSqueekL.aifc \
	sound/clips/CameraSqueekR.aifc \
	sound/clips/RocksBreak.aifc \
	sound/clips/ZoomIn.aifc \
	sound/clips/ZoomOut.aifc

INTRO_CUTSCENE_SOUNDS = sound/clips/CockpitAmbience.ins \
	sound/clips/processing.aifc \
	sound/clips/GOButtonPress.aifc \
	sound/clips/GemsNotice1.aifc \
	sound/clips/GemsNotice2.aifc \
	sound/clips/HUDglow.aifc \
	sound/clips/SpaceshipFlyby.aifc \
	sound/clips/LogoLaughing.aifc \
	sound/clips/Alarm1.aifc

BANK_SOUNDS = $(wildcard sound/ins/sounds/*.aiff)
BANK_SOUNDS_COMP = $(BANK_SOUNDS:.aiff=.aifc)

sound/clips/%.aif: sound/clips/%.wav
	$(SFZ2N64) $< -o $@

%.aif: %.aiff
	cp $< $@

%.table: %.aif
	$(SFZ2N64) $< -o $@

%.aifc: %.aif
	$(SFZ2N64) -o $@ $^

build/audio/player.sounds: $(PLAYER_SOUNDS)
	@mkdir -p $(@D)
	$(SFZ2N64) -o $@ $^

build/audio/intro_cutscene.sounds: $(INTRO_CUTSCENE_SOUNDS)
	@mkdir -p $(@D)
	$(SFZ2N64) -o $@ $^

SONG_FILES = build/music/AuroraBorealis.mid \
	build/music/CosmicDust.mid \
	build/music/DarkSuns.mid \
	build/music/EnceladusdelayCompensated.mid \
	build/music/GetToTheTele.mid \
	build/music/robottheme.mid \
	build/music/StepsAcrosstheSky.mid \
	build/music/TeamUltraRareLogoJingleWIP1.mid \
	build/music/ThermalImaging.mid \
	build/music/LostHasselbladBoogie.mid \
	build/music/ReducedGravity.mid

build/music/%.mid: sound/music/%.mid sound/music/%.meta
	@mkdir -p $(@D)
	# midicomp $< | grep PrCh > $@.instruments
	$(MIDICVT) $< $@ --metadata $(word 2,$^)

build/ins/Bank.ctl build/ins/Bank.tbl: sound/ins/Bank.ins $(BANK_SOUNDS_COMP)
	@mkdir -p $(@D)
	$(SFZ2N64) sound/ins/Bank.ins -o build/ins/Bank.ctl
	# cd sound/ins && wine /home/james/Documents/AudioTools/tools/ic.exe -OBank ./Bank.ins
	# mv sound/ins/Bank.ctl build/ins/Bank.ctl
	# mv sound/ins/Bank.tbl build/ins/Bank.tbl

DEBUGGERHFILES = src/debugger/serial.h \
	src/debugger/debugger.h

# DEBUGGERFILES = src/debugger/serial.c \
# 	src/debugger/debugger.c \
# 	src/debugger/usb.c

HFILES = $(DEBUGGERHFILES) \
	src/game.h	\
	src/audio/audio.h		\
	src/boot.h		\
	src/graphics/graphics.h

CODEFILES = $(DEBUGGERFILES) \
	src/audio/audio.c		\
	src/audio/audiomgr.c	\
	src/audio/playersounds.c \
	src/audio/soundarray.c \
	src/boot.c				\
	src/cadet/cadet.c   	\
	src/cadet/geo/model.c \
	src/collision/boxcollision.c \
	src/collision/collisiondata.c \
	src/collision/collisionscene.c \
	src/collision/geo/geo.c \
	src/collision/levelcollisiongrid.c \
	src/collision/meshcollision.c \
	src/collision/sparsecollisiongrid.c \
	src/collision/sphereactor.c \
	src/cutscene/badendingcutscene.c \
	src/cutscene/cutscene.c \
	src/cutscene/goodendingcutscene.c \
	src/cutscene/introcutscene.c \
	src/cutscene/jpegdecoder.c \
	src/cutscene/slides.c \
	src/effects/explosion.c	\
	src/effects/leveltitle.c	\
	src/effects/respawn.c \
	src/effects/rockettrail.c \
	src/effects/rockfragments.c	\
	src/effects/shadow.c	\
	src/effects/splash.c \
	src/effects/teleport.c	\
	src/effects/tutorial.c	\
	src/effects/walkanim.c \
	src/font/buttons/buttons.c	\
	src/font/endlessbossbattle/endlessbossbattle.c	\
	src/font/font.c	\
	src/font/fontrenderer.c	\
	src/game.c				\
	src/graphics/dynamic.c	\
	src/graphics/graphics.c	\
	src/graphics/levelgraphics.c	\
	src/graphics/levelthemegraphics.c	\
	src/graphics/renderscene.c	\
	src/input/controller.c  \
	src/input/inputfocus.c  \
	src/level/level.c	\
	src/levels/levels.c \
	src/levelthemes/alienworld/theme.c	\
	src/levelthemes/alienworldgrey/theme.c	\
	src/levelthemes/alienworldred/theme.c	\
	src/levelthemes/allthemes.c	\
	src/levelthemes/menu/theme.c	\
	src/math/basictransform.c   	\
	src/math/box.c   \
	src/math/mathf.c   	\
	src/math/plane.c		\
	src/math/quaternion.c   \
	src/math/ray.c   		\
	src/math/vector.c   	\
	src/math/vector2.c   	\
	src/menu/mainmenu.c   	\
	src/menu/menu.c   	\
	src/menu/pausemenu.c   	\
	src/menu/spinninglogo.c   	\
	src/puzzle/breakable.c  \
	src/puzzle/door.c  \
	src/puzzle/entranceexit.c  \
	src/puzzle/gem.c  \
	src/puzzle/movingplatform.c  \
	src/puzzle/signal.c  \
	src/puzzle/switch.c  \
	src/robot/geo/model.c \
	src/robot/robot.c \
	src/rocket/geo/model.c \
	src/rocket/rocket.c \
	src/save/savefile.c	\
	src/strings/strings.c \
	src/system/assert.c		\
	src/time/time.c		\
	src/tjpeg/tjpgd.c \
	src/system/memory.c

CODEOBJECTS = $(patsubst %.c, build/%.o, $(CODEFILES))

CODESEGMENT =	build/codesegment.o

# Data files that have thier own segments:

DATAFILES =	$(LEVEL_GEO) \
	src/graphics/init.c \
	src/levelthemes/alienworld/materials.c \
	src/levelthemes/alienworldred/materials.c \
	src/levelthemes/alienworldgrey/materials.c \
	src/menu/geo/spinninglogo.c \
	src/menu/geo/titlescreen.c

DATAOBJECTS = $(patsubst %.c, build/%.o, $(DATAFILES))

BOOT		=	/usr/lib/n64/PR/bootcode/boot.6102
BOOT_OBJ	=	build/boot.6102.o

OBJECTS =	$(CODESEGMENT) $(DATAOBJECTS) $(ASMOBJECTS) $(BOOT_OBJ) build/audio/player.sounds build/audio/intro_cutscene.sounds 

LCOPTS =	-mno-shared -G 0

LDIRT  =	$(APP)

default:	build/telocation.z64

build/%.o: %.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -MM $^ -MF "$(@:.o=.d)" -MT"$@"
	$(CC) $(CFLAGS) -c -o $@ $<

build/%.o: %.s
	@mkdir -p $(@D)
	$(AS) -Wa,-Iasm -o $@ $<

$(BOOT_OBJ): $(BOOT)
	$(OBJCOPY) -I binary -B mips -O elf32-bigmips $< $@

include $(COMMONRULES)

build/asm/sound_data.o: $(SONG_FILES) build/ins/Bank.ctl build/ins/Bank.tbl build/audio/player.sounds build/audio/intro_cutscene.sounds

build/asm/image_slide_data.o: build/spec/slide_segs

$(CODESEGMENT):	$(CODEOBJECTS)
		$(LD) -o $(CODESEGMENT) -r $(CODEOBJECTS) $(LDFLAGS)

$(CP_LD_SCRIPT).ld: $(LD_SCRIPT) build/spec/level_segs build/spec/slide_segs
	cpp -P -Wno-trigraphs $(LCDEFS) -DCODE_SEGMENT=$(CODESEGMENT) -o $@ $<

$(BASE_TARGET_NAME).z64: $(CODESEGMENT) \
	$(OBJECTS) \
	$(CP_LD_SCRIPT).ld \
	build/spec/slide_segs \
	$(SLIDE_IMAGES)
	$(LD) -L. -T $(CP_LD_SCRIPT).ld -Map $(BASE_TARGET_NAME).map -o $(BASE_TARGET_NAME).elf
	$(OBJCOPY) --pad-to=0x100000 --gap-fill=0xFF $(BASE_TARGET_NAME).elf $(BASE_TARGET_NAME).z64 -O binary
	makemask $(BASE_TARGET_NAME).z64

init: $(LEVEL_GEO)

cleanall: clean
	rm -f $(CODEOBJECTS) $(OBJECTS)