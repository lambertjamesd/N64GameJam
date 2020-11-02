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

		mesh, err = ParsePly(string(content))

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
) *LevelTile {
	return &LevelTile{
		blocks,
		collisionName,
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
	}, "gCollideTileFloor")

	result.Tiles["Platform"] = buildTile([3]*LevelBlock{
		floorBlock,
		platformBlock,
		nil,
	}, "")

	result.Tiles["Barrier"] = buildTile([3]*LevelBlock{
		floorBlock,
		platformBlock,
		barrierBlock,
	}, "")

	result.Tiles["Ramp"] = buildTile([3]*LevelBlock{
		rampLowerBlock,
		rampUpperBlock,
		nil,
	}, "")

	result.Tiles["Stair"] = buildTile([3]*LevelBlock{
		stairLowerBlock,
		stairUpperBlock,
		nil,
	}, "")

	result.Tiles["Tunnel"] = buildTile([3]*LevelBlock{
		stairLowerBlock,
		tunnelBlock,
		nil,
	}, "")

	result.Tiles["Overhang"] = buildTile([3]*LevelBlock{
		floorBlock,
		nil,
		barrierBlock,
	}, "")

	result.Tiles["PlatformFragile"] = buildTile([3]*LevelBlock{
		floorBlock,
		nil,
		nil,
	}, "")

	result.Tiles["BarrierFragile"] = buildTile([3]*LevelBlock{
		floorBlock,
		nil,
		nil,
	}, "")

	result.Tiles["Lava"] = buildTile([3]*LevelBlock{
		lavaBlock,
		nil,
		nil,
	}, "")

	result.Tiles["OverhangLava"] = buildTile([3]*LevelBlock{
		lavaBlock,
		nil,
		barrierBlock,
	}, "")

	result.Tiles["LargeSwitch"] = buildTile([3]*LevelBlock{
		floorBlock,
		nil,
		nil,
	}, "")

	result.Tiles["SmallSwitch"] = buildTile([3]*LevelBlock{
		floorBlock,
		nil,
		nil,
	}, "")

	result.Tiles["Door"] = buildTile([3]*LevelBlock{
		floorHole,
		nil,
		nil,
	}, "")

	result.Tiles["MovingPlatform"] = buildTile([3]*LevelBlock{
		trackBlock,
		nil,
		nil,
	}, "")

	result.Tiles["PlatformTrack"] = buildTile([3]*LevelBlock{
		trackBlock,
		nil,
		nil,
	}, "")

	return &result
}
