
#include "allthemes.h"

#include "src/levelthemes/alienworld/theme.h"

extern char _alienworldSegmentRomStart[], _alienworldSegmentRomEnd[];

struct LevelThemeDefinition gAllThemes[] = {
    {_alienworldSegmentRomStart, _alienworldSegmentRomEnd, &gAlienWorldLevelTheme},
};
