
Gfx** _level_debug_row_0[] = {
    _level_debug_tile_0_0,
    _level_debug_tile_0_1,
    _level_debug_tile_0_2,
    _level_debug_tile_0_3,
};

Gfx** _level_debug_row_1[] = {
    _level_debug_tile_1_0,
    _level_debug_tile_1_1,
    _level_debug_tile_1_2,
    _level_debug_tile_1_3,
};

Gfx** _level_debug_row_2[] = {
    _level_debug_tile_2_0,
    _level_debug_tile_2_1,
    _level_debug_tile_2_2,
    _level_debug_tile_2_3,
};

Gfx*** _level_debug_tileGrid[] = {
    _level_debug_row_0,
    _level_debug_row_1,
    _level_debug_row_2,
};

struct LevelGraphics _level_debug_levelGraphics = {
	{
		_level_debug_tileGrid,
		3, 4,
		5,
		8,	
	},
};
