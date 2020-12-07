
#include "allthemes.h"

#include "src/levelthemes/alienworld/theme.h"
#include "src/levelthemes/alienworldred/theme.h"

extern char _alienworldSegmentRomStart[], _alienworldSegmentRomEnd[];
extern char _alienworldredSegmentRomStart[], _alienworldredSegmentRomEnd[];

struct LevelThemeDefinition gAllThemes[] = {
    {_alienworldSegmentRomStart, _alienworldSegmentRomEnd, &gAlienWorldLevelTheme},
    {_alienworldredSegmentRomStart, _alienworldredSegmentRomEnd, &gAlienWorldRedLevelTheme},
};
