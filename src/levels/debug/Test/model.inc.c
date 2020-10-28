Vtx Test_PlaneTest_mesh_vtx_0[4] = {
    {{{-256, 0, -256}, 0, {0, 0}, {255, 255, 255, 255}}},
    {{{256, 0, 256}, 0, {32, 32}, {255, 255, 255, 255}}},
    {{{-256, 0, 256}, 0, {0, 32}, {255, 255, 255, 255}}},
    {{{256, 0, -256}, 0, {32, 0}, {255, 255, 255, 255}}},
};

Gfx Test_PlaneTest_mesh_tri_0[] = {
    gsSPVertex(&Test_PlaneTest_mesh_vtx_0[0], 4, 0),
    gsSP1Triangle(0, 1, 2, 0),
    gsSP1Triangle(0, 3, 1, 0),
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



