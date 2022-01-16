.include "macros.inc"

.section .data

glabel _bankSegmentRomStart
.incbin "build/ins/Bank.ctl"
.balign 16
glabel _bankSegmentRomEnd

glabel _tableSegmentRomStart
.incbin "build/ins/Bank.tbl"
.balign 16
glabel _tableSegmentRomEnd

glabel _logoJingleSegmentRomStart
.incbin "build/music/TeamUltraRareLogoJingleWIP1.mid"
.balign 16
glabel _logoJingleSegmentRomEnd

glabel _cosmicDustSegmentRomStart
.incbin "build/music/CosmicDust.mid"
.balign 16
glabel _cosmicDustSegmentRomEnd

glabel _darkSunsSegmentRomStart
.incbin "build/music/DarkSuns.mid"
.balign 16
glabel _darkSunsSegmentRomEnd

glabel _thermalImagingSegmentRomStart
.incbin "build/music/ThermalImaging.mid"
.balign 16
glabel _thermalImagingSegmentRomEnd

glabel _stepsAcrossTheSkySegmentRomStart
.incbin "build/music/StepsAcrosstheSky.mid"
.balign 16
glabel _stepsAcrossTheSkySegmentRomEnd

glabel _getToTheTeleSegmentRomStart
.incbin "build/music/GetToTheTele.mid"
.balign 16
glabel _getToTheTeleSegmentRomEnd

glabel _robotThemeSegmentRomStart
.incbin "build/music/robottheme.mid"
.balign 16
glabel _robotThemeSegmentRomEnd

glabel _enceladusSegmentRomStart
.incbin "build/music/EnceladusdelayCompensated.mid"
.balign 16
glabel _enceladusSegmentRomEnd

glabel _auroraBorealisSegmentRomStart
.incbin "build/music/AuroraBorealis.mid"
.balign 16
glabel _auroraBorealisSegmentRomEnd

glabel _boogieSegmentRomStart
.incbin "build/music/LostHasselbladBoogie.mid"
.balign 16
glabel _boogieSegmentRomEnd

glabel _reducedGravitySegmentRomStart
.incbin "build/music/ReducedGravity.mid"
.balign 16
glabel _reducedGravitySegmentRomEnd

glabel _player_soundsSegmentRomStart
.incbin "build/audio/player.sounds"
.balign 16
glabel _player_soundsSegmentRomEnd

glabel _player_sounds_tableSegmentRomStart
.incbin "build/audio/player.sounds.tbl"
.balign 16
glabel _player_sounds_tableSegmentRomEnd

glabel _intro_cutsceneSegmentRomStart
.incbin "build/audio/intro_cutscene.sounds"
.balign 16
glabel _intro_cutsceneSegmentRomEnd

glabel _intro_cutscene_tableSegmentRomStart
.incbin "build/audio/intro_cutscene.sounds.tbl"
.balign 16
glabel _intro_cutscene_tableSegmentRomEnd
