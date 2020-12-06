


Vtx Splash_mesh_vtx_0[16] = {
	{{{0, 0, -51},0, {-5408, -26},{0xFF, 0xFF, 0xFF, 0xFF}}},
	{{{40, 0, -32},0, {-3708, -26},{0xFF, 0xFF, 0xFF, 0xFF}}},
	{{{50, 0, 11},0, {-2009, -26},{0xFF, 0xFF, 0xFF, 0xFF}}},
	{{{22, 0, 46},0, {-310, -26},{0xFF, 0xFF, 0xFF, 0xFF}}},
	{{{-22, 0, 46},0, {1390, -26},{0xFF, 0xFF, 0xFF, 0xFF}}},
	{{{-50, 0, 11},0, {3089, -26},{0xFF, 0xFF, 0xFF, 0xFF}}},
	{{{-40, 0, -32},0, {4788, -26},{0xFF, 0xFF, 0xFF, 0xFF}}},
	{{{0, 0, -51},0, {6488, -26},{0xFF, 0xFF, 0xFF, 0xFF}}},

	{{{0, 0, -51},0, {-5408, 946},{0xFF, 0xFF, 0xFF, 0xFF}}},
	{{{40, 0, -32},0, {-3708, 946},{0xFF, 0xFF, 0xFF, 0xFF}}},
	{{{50, 0, 11},0, {-2009, 946},{0xFF, 0xFF, 0xFF, 0xFF}}},
	{{{22, 0, 46},0, {-310, 946},{0xFF, 0xFF, 0xFF, 0xFF}}},
	{{{-22, 0, 46},0, {1390, 946},{0xFF, 0xFF, 0xFF, 0xFF}}},
	{{{-50, 0, 11},0, {3089, 946},{0xFF, 0xFF, 0xFF, 0xFF}}},
	{{{-40, 0, -32},0, {4788, 946},{0xFF, 0xFF, 0xFF, 0xFF}}},
	{{{0, 0, -51},0, {6488, 946},{0xFF, 0xFF, 0xFF, 0xFF}}},
};


Gfx Splash_mesh_tri_0[] = {
	gsSPVertex(Splash_mesh_vtx_0 + 0, 8, 0),
	gsSPEndDisplayList(),
};

Gfx Splash_mesh_tri_1[] = {
	gsSPVertex(Splash_mesh_vtx_0 + 8, 8, 8),
	gsSP1Triangle(0, 1, 9, 0),
	gsSP1Triangle(9, 8, 0, 0),
	gsSP1Triangle(1, 2, 10, 0),
	gsSP1Triangle(10, 9, 1, 0),
	gsSP1Triangle(2, 3, 11, 0),
	gsSP1Triangle(11, 10, 2, 0),
	gsSP1Triangle(3, 4, 12, 0),
	gsSP1Triangle(12, 11, 3, 0),
	gsSP1Triangle(4, 5, 13, 0),
	gsSP1Triangle(13, 12, 4, 0),
	gsSP1Triangle(5, 6, 14, 0),
	gsSP1Triangle(14, 13, 5, 0),
	gsSP1Triangle(6, 7, 15, 0),
	gsSP1Triangle(15, 14, 6, 0),
	gsSPEndDisplayList(),
};