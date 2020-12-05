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

#include "build/spec/level_segs"
#include "build/spec/slide_segs"

beginseg
	name "alienworld"
	flags OBJECT
	number LEVEL_THEME_SEGMENT
	include "src/levelthemes/alienworld/materials.o"
endseg

beginseg
	name "spinning_logo"
	flags OBJECT
	number LEVEL_SEGMENT
	include "src/menu/geo/spinninglogo.o"
endseg

beginseg
	name "bank"
	flags RAW
	include "build/ins/Bank.ctl"
endseg

beginseg
	name "table"
	flags RAW
	include "build/ins/Bank.tbl"
endseg

beginseg
	name "logoJingle"
	flags RAW
	include "sound/music/TeamUltraRareLogoJingleWIP2Loud.mid"
endseg

beginseg
	name "cosmicDust"
	flags RAW
	include "sound/music/CosmicDustLoud.mid"
endseg

beginseg
	name "darkSuns"
	flags RAW
	include "sound/music/DarkSunsLoud.mid"
endseg

beginseg
	name "thermalImaging"
	flags RAW
	include "sound/music/ThermalImagingLoud.mid"
endseg

beginseg
	name "stepsAcrossTheSky"
	flags RAW
	include "sound/music/StepsAcrosstheSkyLoud.mid"
endseg

beginseg
	name "getToTheTele"
	flags RAW
	include "sound/music/GetToTheTeleLoud.mid"
endseg

beginseg
	name "robotTheme"
	flags RAW
	include "sound/music/robotthemeLoud.mid"
endseg

beginseg
	name "player_sounds"
	flags RAW
	include "build/audio/player.sounds"
endseg

beginseg
	name "player_sounds_table"
	flags RAW
	include "build/audio/player.sounds.tbl"
endseg

beginseg
	name "intro_cutscene"
	flags RAW
	include "build/audio/intro_cutscene.sounds"
endseg

beginseg
	name "intro_cutscene_table"
	flags RAW
	include "build/audio/intro_cutscene.sounds.tbl"
endseg

beginwave
	name "game"
	include "code"
	include "static"
#include "build/spec/level_include"
#include "build/spec/slide_include"
	include "alienworld"
	include "spinning_logo"
	include "table"
	include "bank"
	include "logoJingle"
	include "darkSuns"
	include "cosmicDust"
	include "thermalImaging"
	include "stepsAcrossTheSky"
	include "getToTheTele"
	include "robotTheme"
	include "player_sounds"
	include "player_sounds_table"
	include "intro_cutscene"
	include "intro_cutscene_table"
endwave
