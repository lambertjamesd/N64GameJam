#!smake
include $(ROOT)/usr/include/make/PRdefs

FINAL = YES

ifeq ($(FINAL), YES)
# OPTIMIZER       = -O2 -std=gnu90 -mno-shared
OPTIMIZER       = -g
LCDEFS			= -DNDEBUG -D_FINALROM
N64LIB          = -lultra_rom
else
OPTIMIZER       = -g -std=gnu90 -mno-shared
LCDEFS          = -DDEBUG
N64LIB          = -lultra_d
endif

APP =		game.out

TARGETS =	game.n64

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
	src/graphics/graphics.c	\
	src/input/controller.c  \
	src/math/plane.c		\
	src/math/quaternion.c   \
	src/math/ray.c   		\
	src/math/vector.c   	\
	src/system/assert.c		\
	src/boot.c				\
	src/game.c				\
	src/memory.c

CODEOBJECTS =	$(CODEFILES:.c=.o)

DEPS = $(CODEFILES:.c=.d) $(DATAFILES:.c=.d)

CODESEGMENT =	codesegment.o

# Data files that have thier own segments:

DATAFILES =		\
	src/graphics/init.c \
	src/levels/debug/geo.c

DATAOBJECTS =	$(DATAFILES:.c=.o)

OBJECTS =	$(CODESEGMENT) $(DATAOBJECTS)

LCINCS =	-I. -I$(ROOT)/usr/include/PR -I $(ROOT)/usr/include
LCOPTS =	-mno-shared -G 0
LDFLAGS =	$(MKDEPOPT) -L$(ROOT)/usr/lib $(N64LIB) -L$(N64_LIBGCCDIR) -L$(N64_NEWLIBDIR) -lgcc -lc

LDIRT  =	$(APP)

default:	$(TARGETS)

%.d: %.c
	$(CC) $(GCINCS) $(LCINCS) -MF"$@" -MG -MM -MP -MT"$@" -MT"$(<:.c=.o)" "$<"

-include $(DEPS)

include $(COMMONRULES)

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