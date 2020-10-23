#include "src/defs.h"

/*
 * ROM spec file
 */

beginseg
	name "code"
	flags BOOT OBJECT
	entry boot
	stack bootStack + STACKSIZEBYTES
	include "codesegment.o"
	include "$(ROOT)/usr/lib/PR/rspboot.o"
	include "$(ROOT)/usr/lib/PR/gspFast3D.o"
	include "$(ROOT)/usr/lib/PR/gspFast3D.dram.o"
	include "$(ROOT)/usr/lib/PR/aspMain.o"
endseg

beginseg
	name "static"
	flags OBJECT
	number STATIC_SEGMENT
	include "src/graphics/init.o"
endseg

beginseg
	name "bank"
	flags RAW
	include "$(ROOT)/usr/lib/soundbanks/GenMidiBank.ctl"
endseg

beginseg
	name "table"
	flags RAW
	include "$(ROOT)/usr/lib/soundbanks/GenMidiBank.tbl"
endseg

beginseg
	name "seq"
	flags RAW
	include "sound/music/RobotTheme.mid"
endseg

beginwave
	name "game"
	include "code"
	include "static"
	include "table"
	include "bank"
	include "seq"
endwave
