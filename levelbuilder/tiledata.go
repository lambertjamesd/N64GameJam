package main

import (
	"io/ioutil"
	"log"
	"os"
	"path/filepath"
)

var meshCache map[string]*Mesh = make(map[string]*Mesh)

func buildFace(plyFile string, matType MaterialType) *LevelBlockFace {
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

	return &LevelBlockFace{mesh, matType}
}

func buildBlock(
	isSolid bool,
	left string,
	leftMat MaterialType,
	front string,
	frontMat MaterialType,
	right string,
	rightMat MaterialType,
	back string,
	backMat MaterialType,
	top string,
	topMat MaterialType,
) *LevelBlock {
	return &LevelBlock{
		isSolid,
		[4]*LevelBlockFace{
			buildFace(left, leftMat),
			buildFace(front, frontMat),
			buildFace(right, rightMat),
			buildFace(back, backMat),
		},
		buildFace(top, topMat),
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

func BuildTileSet() *LevelTileSet {
	var result LevelTileSet

	var floorBlock = buildBlock(
		true,
		"ply/wall_color.ply", Underhang,
		"ply/wall_color.ply", Underhang,
		"ply/wall_color.ply", Underhang,
		"ply/wall_color.ply", Underhang,
		"ply/floor_color.ply", LowerFloor,
	)

	var platformBlock = buildBlock(
		true,
		"ply/wall_color.ply", Wall,
		"ply/wall_color.ply", Wall,
		"ply/wall_color.ply", Wall,
		"ply/wall_color.ply", Wall,
		"ply/platform_color.ply", UpperFloor,
	)

	var barrierBlock = buildBlock(
		true,
		"ply/wall_color.ply", Wall,
		"ply/wall_color.ply", Wall,
		"ply/wall_color.ply", Wall,
		"ply/wall_color.ply", Wall,
		"ply/barrier_color.ply", BarrierTop,
	)

	var rampLowerBlock = buildBlock(
		true,
		"ply/wall_color.ply", Underhang,
		"ply/wall_color.ply", Underhang,
		"ply/wall_color.ply", Underhang,
		"ply/wall_color.ply", Underhang,
		"ply/ramp.ply", BarrierTop,
	)

	var rampUpperBlock = buildBlock(
		false,
		"ply/ramp_side_left.ply", Wall,
		"ply/wall_color.ply", Wall,
		"ply/ramp_side_right.ply", Wall,
		"", Wall,
		"", BarrierTop,
	)

	var stairLowerBlock = buildBlock(
		true,
		"ply/wall_color.ply", Underhang,
		"ply/wall_color.ply", Underhang,
		"ply/wall_color.ply", Underhang,
		"ply/wall_color.ply", Underhang,
		"ply/stair.ply", BarrierTop,
	)

	var stairUpperBlock = buildBlock(
		false,
		"ply/stair_side_left.ply", Wall,
		"ply/wall_color.ply", Wall,
		"ply/stair_side_right.ply", Wall,
		"", Wall,
		"", BarrierTop,
	)

	var tunnelBlock = buildBlock(
		true,
		"ply/wall_color.ply", Wall,
		"ply/tunnel_face.ply", Wall,
		"ply/wall_color.ply", Wall,
		"ply/tunnel_face.ply", Wall,
		"ply/platform_color.ply", UpperFloor,
	)

	var lavaBlock = buildBlock(
		true,
		"ply/wall_color.ply", Underhang,
		"ply/wall_color.ply", Underhang,
		"ply/wall_color.ply", Underhang,
		"ply/wall_color.ply", Underhang,
		"ply/floor_color.ply", Lava,
	)

	var floorHole = buildBlock(
		true,
		"ply/wall_color.ply", Underhang,
		"ply/wall_color.ply", Underhang,
		"ply/wall_color.ply", Underhang,
		"ply/wall_color.ply", Underhang,
		"", LowerFloor,
	)

	var trackBlock = buildBlock(
		true,
		"ply/wall_color.ply", Underhang,
		"ply/wall_color.ply", Underhang,
		"ply/wall_color.ply", Underhang,
		"ply/wall_color.ply", Underhang,
		"", Track,
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
	}, "gCollideTileFloor", DynamicTypeNone)

	result.Tiles["BarrierFragile"] = buildTile([3]*LevelBlock{
		floorBlock,
		nil,
		nil,
	}, "gCollideTileFloor", DynamicTypeNone)

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

	result.Tiles["Door"] = buildTile([3]*LevelBlock{
		floorHole,
		nil,
		nil,
	}, "", DynamicTypeDoor)

	result.Tiles["MovingPlatform"] = buildTile([3]*LevelBlock{
		trackBlock,
		nil,
		nil,
	}, "", DynamicTypeNone)

	result.Tiles["PlatformTrack"] = buildTile([3]*LevelBlock{
		trackBlock,
		nil,
		nil,
	}, "", DynamicTypeNone)

	return &result
}
