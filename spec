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
	name "cadet_intro"
	flags OBJECT
	number LEVEL_SEGMENT
	include "src/levels/cadet_intro/geo.o"
endseg

beginseg
	name "robot_intro"
	flags OBJECT
	number LEVEL_SEGMENT
	include "src/levels/robot_intro/geo.o"
endseg

beginseg
	name "switch_tutorial"
	flags OBJECT
	number LEVEL_SEGMENT
	include "src/levels/switch_tutorial/geo.o"
endseg

beginseg
	name "robot_platform"
	flags OBJECT
	number LEVEL_SEGMENT
	include "src/levels/robot_platform/geo.o"
endseg

beginseg
	name "lava_platform"
	flags OBJECT
	number LEVEL_SEGMENT
	include "src/levels/lava_platform/geo.o"
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
	include "cadet_intro"
	include "robot_intro"
	include "switch_tutorial"
	include "lava_platform"
	include "robot_platform"
	include "alienworld"
	include "table"
	include "bank"
	include "seq"
endwave
