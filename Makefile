#!smake
include $(ROOT)/usr/include/make/PRdefs

FINAL = YES

ifeq ($(FINAL), YES)
# OPTIMIZER       = -O2 -std=gnu90 -mno-shared
OPTIMIZER       = -g
LCDEFS			= -DNDEBUG -D_FINALROM -Werror
N64LIB          = -lultra_rom
else
OPTIMIZER       = -g -std=gnu90 -mno-shared
LCDEFS          = -DDEBUG -Werror
N64LIB          = -lultra_d
endif

APP =		game.out

TARGETS =	game.n64

GO_SOURCE = $(wildcard ./levelbuilder/*.go)

levelbuilder/levelbuilder: $(GO_SOURCE)
	go build -o levelbuilder/levelbuilder ./levelbuilder/

LEVELS = cadet_intro \
	robot_intro \
	robot_platform \
	lava_platform \
	switch_tutorial \
	loop_around \
	radioactive_river \
	double_crossing \
	raised_platform \
	moving_platforms \
	switch_mania \
	raise_robot \
	debug
	
LEVEL_GEO = $(foreach level, $(LEVELS), src/levels/$(level)/geo.c)
LEVEL_DATA = $(foreach level, $(LEVELS), levels/$(level).level)

src/levels/%/geo.c: levels/%.level levels/%.meta levelbuilder/levelbuilder
	@mkdir -p $(@D)
	levelbuilder/levelbuilder level $* $< $@

build/spec/level_segs build/spec/level_include src/levels/levels.c src/levels/levels.h: levelbuilder/levelbuilder $(LEVEL_DATA)
	@mkdir -p build/spec
	levelbuilder/levelbuilder levelpack all_levels 0 src/levels/levels.c build/spec/level_segs build/spec/level_include $(LEVELS)

COLLISION_SHAPES = solid_block tunnel_block ramp_block stair_block entrance_exit
COLLISION_GEO = $(foreach shape, $(COLLISION_SHAPES), src/collision/geo/$(shape).inc.c)

src/collision/geo/%.inc.c: collision/%.ply levelbuilder/levelbuilder
	@mkdir -p $(@D)
	levelbuilder/levelbuilder collision $* $< $@

src/collision/geo/geo.c: $(COLLISION_GEO)

PLAYER_SOUNDS = sound/clips/Jump.wav \
	sound/clips/JumpPeakBeep.wav \
	sound/clips/Landing.wav \
	sound/clips/AstronautFootsteps.aif \
	sound/clips/AstronautSlide1.aif \
	sound/clips/AstronautSlide2.aif \
	sound/clips/WarpAstronautOnly.aif \
	sound/clips/WarpRobotOnly.aif \
	sound/clips/WarpBoth.aif \
	sound/clips/AstronautDeath_Fall.wav \
	sound/clips/RobotDeath_Fall.wav \
	sound/clips/Land_onRobotHead.aif \
	sound/clips/MetalFootsteps_RoboHead.aif \
	sound/clips/RobotDestroy.aif \
	sound/clips/RobotMovement.aif \
	sound/clips/SwitchBackChar.wav \
	sound/clips/SwitchChar.wav \
	sound/clips/BigButton.aif \
	sound/clips/SmallButton.aif \
	sound/clips/WarpGoGreen_Big.wav \
	sound/clips/WarpGoGreen_Small.aif


build/audio/player.sounds: $(PLAYER_SOUNDS)
	@mkdir -p $(@D)
	/home/james/go/src/github.com/lambertjamesd/sfz2n64/sfz2n64 $@ $^

DEBUGGERHFILES = src/debugger/serial.h \
	src/debugger/debugger.h

DEBUGGERFILES = src/debugger/serial.c \
	src/debugger/debugger.c

HFILES = $(DEBUGGERHFILES) \
	src/game.h	\
	src/audio/audio.h		\
	src/boot.h		\
	src/graphics/graphics.h

CODEFILES = $(DEBUGGERFILES) \
	src/audio/audio.c		\
	src/audio/audiomgr.c	\
	src/audio/soundarray.c \
	src/audio/playersounds.c \
	src/cadet/cadet.c   	\
	src/cadet/geo/model.c \
	src/collision/collisiondata.c \
	src/collision/collisionscene.c \
	src/collision/boxcollision.c \
	src/collision/geo/geo.c \
	src/collision/levelcollisiongrid.c \
	src/collision/meshcollision.c \
	src/collision/sphereactor.c \
	src/collision/sparsecollisiongrid.c \
	src/effects/explosion.c	\
	src/effects/leveltitle.c	\
	src/effects/tutorial.c	\
	src/effects/shadow.c	\
	src/effects/teleport.c	\
	src/effects/walkanim.c \
	src/font/font.c	\
	src/font/fontrenderer.c	\
	src/font/buttons/buttons.c	\
	src/font/endlessbossbattle/endlessbossbattle.c	\
	src/graphics/dynamic.c	\
	src/graphics/graphics.c	\
	src/graphics/levelgraphics.c	\
	src/graphics/levelthemegraphics.c	\
	src/graphics/renderscene.c	\
	src/input/controller.c  \
	src/input/inputfocus.c  \
	src/level/level.c	\
	src/levels/levels.c \
	src/levelthemes/allthemes.c	\
	src/levelthemes/alienworld/theme.c	\
	src/math/basictransform.c   	\
	src/math/mathf.c   	\
	src/math/plane.c		\
	src/math/quaternion.c   \
	src/math/ray.c   		\
	src/math/vector.c   	\
	src/math/vector2.c   	\
	src/menu/menu.c   	\
	src/menu/pausemenu.c   	\
	src/robot/robot.c \
	src/robot/geo/model.c \
	src/puzzle/breakable.c  \
	src/puzzle/door.c  \
	src/puzzle/entranceexit.c  \
	src/puzzle/gem.c  \
	src/puzzle/movingplatform.c  \
	src/puzzle/signal.c  \
	src/puzzle/switch.c  \
	src/system/assert.c		\
	src/save/savefile.c	\
	src/time/time.c		\
	src/boot.c				\
	src/game.c				\
	src/system/memory.c

CODEOBJECTS =	$(CODEFILES:.c=.o)

CODESEGMENT =	codesegment.o

# Data files that have thier own segments:

DATAFILES =	$(LEVEL_GEO) \
	src/graphics/init.c \
	src/levelthemes/alienworld/materials.c \
	src/system/heapstart.c

DATAOBJECTS =	$(DATAFILES:.c=.o)

DEPS = $(CODEFILES:.c=.d) $(DATAFILES:.c=.d)

OBJECTS =	$(CODESEGMENT) $(DATAOBJECTS) build/audio/player.sounds

LCINCS =	-I. -I$(ROOT)/usr/include/PR -I $(ROOT)/usr/include
LCOPTS =	-mno-shared -G 0
LDFLAGS =	$(MKDEPOPT) -L$(ROOT)/usr/lib $(N64LIB) -L$(N64_LIBGCCDIR) -L$(N64_NEWLIBDIR) -lgcc -lc

LDIRT  =	$(APP)

default:	$(TARGETS)

%.d: %.c
	$(CC) $(GCINCS) $(LCINCS) -MF"$@" -MG -MM -MP -MT"$@" -MT"$(<:.c=.o)" "$<"

-include $(DEPS)

include $(COMMONRULES)

spec: build/spec/level_segs build/spec/level_include

$(CODESEGMENT):	$(CODEOBJECTS)
		$(LD) -o $(CODESEGMENT) -r $(CODEOBJECTS) $(LDFLAGS)

ifeq ($(FINAL), YES)
$(TARGETS) $(APP):      spec $(OBJECTS)
	$(MAKEROM) -s 9 -r $(TARGETS) spec
	makemask $(TARGETS)
else
$(TARGETS) $(APP):      spec $(OBJECTS)
	$(MAKEROM) -r $(TARGETS) spec
endif

cleanall: clean
	rm -f $(CODEOBJECTS) $(OBJECTS) $(DEPS)