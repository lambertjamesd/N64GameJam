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
	include "src/system/heapstart.o"
endseg

beginseg
	name "static"
	flags OBJECT
	number STATIC_SEGMENT
	include "src/graphics/init.o"
endseg

beginseg
	name "debuglevel"
	flags OBJECT
	number LEVEL_SEGMENT
	include "src/levels/debug/geo.o"
endseg

beginseg
	name "00_cadet_intro"
	flags OBJECT
	number LEVEL_SEGMENT
	include "src/levels/00_cadet_intro/geo.o"
endseg

beginseg
	name "01_robot_intro"
	flags OBJECT
	number LEVEL_SEGMENT
	include "src/levels/01_robot_intro/geo.o"
endseg

beginseg
	name "alienworld"
	flags OBJECT
	number LEVEL_THEME_SEGMENT
	include "src/levelthemes/alienworld/materials.o"
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
	include "debuglevel"
	include "00_cadet_intro"
	include "01_robot_intro"
	include "alienworld"
	include "table"
	include "bank"
	include "seq"
endwave
