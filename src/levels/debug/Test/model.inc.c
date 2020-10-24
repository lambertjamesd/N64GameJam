Vtx Test_PlaneTest_mesh_vtx_0[4] = {
	{{{-256, 0, 256},0, {-16, 1008},{0x0, 0x7F, 0x0, 0xFF}}},
	{{{256, 0, 256},0, {1008, 1008},{0x0, 0x7F, 0x0, 0xFF}}},
	{{{256, 0, -256},0, {1008, -16},{0x0, 0x7F, 0x0, 0xFF}}},
	{{{-256, 0, -256},0, {-16, -16},{0x0, 0x7F, 0x0, 0xFF}}},
};

Gfx Test_PlaneTest_mesh_tri_0[] = {
	gsSPVertex(Test_PlaneTest_mesh_vtx_0 + 0, 4, 0),
	gsSP1Triangle(0, 1, 2, 0),
	gsSP1Triangle(0, 2, 3, 0),
	gsSPEndDisplayList(),
};


Gfx mat_Test_sm64_material[] = {
	gsDPPipeSync(),
	gsDPSetCombineLERP(0, 0, 0, PRIMITIVE, 0, 0, 0, 1, 0, 0, 0, PRIMITIVE, 0, 0, 0, 1),
	gsSPTexture(65535, 65535, 0, 0, 1),
	gsDPSetPrimColor(0, 0, 187, 187, 187, 255),
	gsSPEndDisplayList(),
};


Gfx Test_PlaneTest_mesh[] = {
	gsSPDisplayList(mat_Test_sm64_material),
	gsSPDisplayList(Test_PlaneTest_mesh_tri_0),
	gsDPPipeSync(),
	gsSPSetGeometryMode(G_LIGHTING),
	gsSPClearGeometryMode(G_TEXTURE_GEN),
	gsDPSetCombineLERP(0, 0, 0, SHADE, 0, 0, 0, ENVIRONMENT, 0, 0, 0, SHADE, 0, 0, 0, ENVIRONMENT),
	gsSPTexture(65535, 65535, 0, 0, 0),
	gsSPEndDisplayList(),
};



