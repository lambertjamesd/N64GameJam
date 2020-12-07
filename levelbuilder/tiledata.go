package main

import (
	"io/ioutil"
	"log"
	"os"
	"path/filepath"
)

type Color struct {
	R, G, B, A uint8
}

type ColorTheme struct {
	Floor    Color
	Platform Color
	Barrier  Color
	Wall     Color
}

var meshCache map[string]*Mesh = make(map[string]*Mesh)

var ThemeColors = []ColorTheme{
	ColorTheme{
		Color{185, 47, 169, 255},
		Color{210, 90, 162, 255},
		Color{160, 117, 162, 255},
		Color{128, 71, 140, 255},
	},
	ColorTheme{
		Color{70, 161, 176, 255},
		Color{92, 207, 178, 255},
		Color{144, 173, 186, 255},
		Color{73, 120, 154, 255},
	},
	ColorTheme{
		Color{203, 43, 18, 255},
		Color{214, 87, 44, 255},
		Color{160, 112, 95, 255},
		Color{132, 54, 28, 255},
	},
}

func recolorMesh(input *Mesh, color Color) *Mesh {
	var vertices []MeshVertex = nil

	for _, vertex := range input.vertices {
		vertices = append(vertices, MeshVertex{
			vertex.x, vertex.y, vertex.z,
			vertex.nx, vertex.ny, vertex.nz,
			vertex.s, vertex.t,
			color.R, color.G, color.B, color.A,
		})
	}

	return &Mesh{
		vertices,
		input.faces,
	}
}

func buildFace(plyFile string, matType MaterialType, color Color) *LevelBlockFace {
	if plyFile == "" {
		return nil
	}

	mesh, ok := meshCache[plyFile]

	if !ok {
		content, err := ioutil.ReadFile(filepath.Join(filepath.Dir(os.Args[0]), plyFile))

		if err != nil {
			log.Fatal(err)
		}

		mesh, _, err = ParsePly(string(content))

		if err != nil {
			log.Fatal(err)
		}

		meshCache[plyFile] = mesh
	}

	return &LevelBlockFace{recolorMesh(mesh, color), matType}
}

func buildBlock(
	isSolid bool,
	left string,
	leftMat MaterialType,
	leftColor Color,
	front string,
	frontMat MaterialType,
	frontColor Color,
	right string,
	rightMat MaterialType,
	rightColor Color,
	back string,
	backMat MaterialType,
	backColor Color,
	top string,
	topMat MaterialType,
	topColor Color,
) *LevelBlock {
	return &LevelBlock{
		isSolid,
		[4]*LevelBlockFace{
			buildFace(left, leftMat, leftColor),
			buildFace(front, frontMat, frontColor),
			buildFace(right, rightMat, rightColor),
			buildFace(back, backMat, backColor),
		},
		buildFace(top, topMat, topColor),
	}
}

func buildTile(
	blocks [3]*LevelBlock,
	collisionName string,
	switchType int,
) *LevelTile {
	return &LevelTile{
		blocks,
		collisionName,
		switchType,
	}
}

func BuildTileSet(theme *ColorTheme) *LevelTileSet {
	var result LevelTileSet

	var floorBlock = buildBlock(
		true,
		"ply/wall_color.ply", Underhang, theme.Wall,
		"ply/wall_color.ply", Underhang, theme.Wall,
		"ply/wall_color.ply", Underhang, theme.Wall,
		"ply/wall_color.ply", Underhang, theme.Wall,
		"ply/floor_color.ply", LowerFloor, theme.Floor,
	)

	var platformBlock = buildBlock(
		true,
		"ply/wall_color.ply", Wall, theme.Wall,
		"ply/wall_color.ply", Wall, theme.Wall,
		"ply/wall_color.ply", Wall, theme.Wall,
		"ply/wall_color.ply", Wall, theme.Wall,
		"ply/platform_color.ply", UpperFloor, theme.Platform,
	)

	var barrierBlock = buildBlock(
		true,
		"ply/wall_color.ply", Wall, theme.Wall,
		"ply/wall_color.ply", Wall, theme.Wall,
		"ply/wall_color.ply", Wall, theme.Wall,
		"ply/wall_color.ply", Wall, theme.Wall,
		"ply/barrier_color.ply", BarrierTop, theme.Barrier,
	)

	var rampLowerBlock = buildBlock(
		true,
		"ply/wall_color.ply", Underhang, theme.Wall,
		"ply/wall_color.ply", Underhang, theme.Wall,
		"ply/wall_color.ply", Underhang, theme.Wall,
		"ply/wall_color.ply", Underhang, theme.Wall,
		"ply/ramp.ply", BarrierTop, theme.Platform,
	)

	var rampUpperBlock = buildBlock(
		false,
		"ply/ramp_side_left.ply", Wall, theme.Wall,
		"ply/wall_color.ply", Wall, theme.Wall,
		"ply/ramp_side_right.ply", Wall, theme.Wall,
		"", Wall, theme.Wall,
		"", BarrierTop, theme.Barrier,
	)

	var stairLowerBlock = buildBlock(
		true,
		"ply/wall_color.ply", Underhang, theme.Wall,
		"ply/wall_color.ply", Underhang, theme.Wall,
		"ply/wall_color.ply", Underhang, theme.Wall,
		"ply/wall_color.ply", Underhang, theme.Wall,
		"ply/stair.ply", UpperFloor, theme.Platform,
	)

	var stairUpperBlock = buildBlock(
		false,
		"ply/stair_side_left.ply", Wall, theme.Wall,
		"ply/wall_color.ply", Wall, theme.Wall,
		"ply/stair_side_right.ply", Wall, theme.Wall,
		"", Wall, theme.Wall,
		"", BarrierTop, theme.Barrier,
	)

	var tunnelBlock = buildBlock(
		true,
		"ply/wall_color.ply", Wall, theme.Wall,
		"ply/tunnel_face.ply", Wall, theme.Wall,
		"ply/wall_color.ply", Wall, theme.Wall,
		"ply/tunnel_face.ply", Wall, theme.Wall,
		"ply/platform_color.ply", UpperFloor, theme.Platform,
	)

	var lavaBlock = buildBlock(
		true,
		"ply/wall_color.ply", Underhang, theme.Wall,
		"ply/wall_color.ply", Underhang, theme.Wall,
		"ply/wall_color.ply", Underhang, theme.Wall,
		"ply/wall_color.ply", Underhang, theme.Wall,
		"ply/lava_color.ply", Lava, Color{255, 255, 255, 255},
	)

	var floorHole = buildBlock(
		true,
		"ply/wall_color.ply", Underhang, theme.Wall,
		"ply/wall_color.ply", Underhang, theme.Wall,
		"ply/wall_color.ply", Underhang, theme.Wall,
		"ply/wall_color.ply", Underhang, theme.Wall,
		"", LowerFloor, theme.Floor,
	)

	var trackBlock = buildBlock(
		false,
		"", Underhang, theme.Wall,
		"", Underhang, theme.Wall,
		"", Underhang, theme.Wall,
		"", Underhang, theme.Wall,
		"ply/track.ply", Track, Color{255, 255, 255, 255},
	)

	var halfBlock = buildBlock(
		false,
		"ply/half_wall_color.ply", Wall, theme.Wall,
		"ply/half_wall_color.ply", Wall, theme.Wall,
		"ply/half_wall_color.ply", Wall, theme.Wall,
		"ply/half_wall_color.ply", Wall, theme.Wall,
		"ply/low_floor_color.ply", UpperFloor, theme.Platform,
	)

	result.Tiles = make(map[string]*LevelTile)

	result.Tiles["Floor"] = buildTile([3]*LevelBlock{
		floorBlock,
		nil,
		nil,
	}, "gCollideTileFloor", DynamicTypeNone)

	result.Tiles["Platform"] = buildTile([3]*LevelBlock{
		floorBlock,
		platformBlock,
		nil,
	}, "gCollideTilePlatform", DynamicTypeNone)

	result.Tiles["Barrier"] = buildTile([3]*LevelBlock{
		floorBlock,
		platformBlock,
		barrierBlock,
	}, "gCollideTileBarrier", DynamicTypeNone)

	result.Tiles["Ramp"] = buildTile([3]*LevelBlock{
		rampLowerBlock,
		rampUpperBlock,
		nil,
	}, "gCollideTileRamp", DynamicTypeNone)

	result.Tiles["Stair"] = buildTile([3]*LevelBlock{
		stairLowerBlock,
		stairUpperBlock,
		nil,
	}, "gCollideTileStair", DynamicTypeNone)

	result.Tiles["Tunnel"] = buildTile([3]*LevelBlock{
		stairLowerBlock,
		tunnelBlock,
		nil,
	}, "gCollideTileTunnel", DynamicTypeNone)

	result.Tiles["Overhang"] = buildTile([3]*LevelBlock{
		floorBlock,
		nil,
		barrierBlock,
	}, "gCollideTileOverhang", DynamicTypeNone)

	result.Tiles["PlatformFragile"] = buildTile([3]*LevelBlock{
		floorBlock,
		nil,
		nil,
	}, "gCollideTileFloor", DynamicTypeBreakablePlatform)

	result.Tiles["BarrierFragile"] = buildTile([3]*LevelBlock{
		floorBlock,
		nil,
		nil,
	}, "gCollideTileFloor", DynamicTypeBreakableBarrier)

	result.Tiles["Lava"] = buildTile([3]*LevelBlock{
		lavaBlock,
		nil,
		nil,
	}, "gCollideTileLava", DynamicTypeNone)

	result.Tiles["OverhangLava"] = buildTile([3]*LevelBlock{
		lavaBlock,
		nil,
		barrierBlock,
	}, "gCollideTileLavaOverhang", DynamicTypeNone)

	result.Tiles["LargeSwitch"] = buildTile([3]*LevelBlock{
		floorBlock,
		nil,
		nil,
	}, "gCollideTileFloor", DynamicTypeLargeSwitch)

	result.Tiles["SmallSwitch"] = buildTile([3]*LevelBlock{
		floorBlock,
		nil,
		nil,
	}, "gCollideTileFloor", DynamicTypeSmallSwitch)

	result.Tiles["PlatformSwitch"] = buildTile([3]*LevelBlock{
		floorBlock,
		halfBlock,
		nil,
	}, "gCollideHalfPlatform", DynamicTypePlatformSwitch)

	result.Tiles["Door"] = buildTile([3]*LevelBlock{
		floorHole,
		nil,
		nil,
	}, "", DynamicTypeDoor)

	result.Tiles["MovingPlatform"] = buildTile([3]*LevelBlock{
		trackBlock,
		nil,
		nil,
	}, "", DynamicTypeMovingPlatform)

	result.Tiles["PlatformTrack"] = buildTile([3]*LevelBlock{
		trackBlock,
		nil,
		nil,
	}, "", DynamicTypePlatformSlot)

	return &result
}
