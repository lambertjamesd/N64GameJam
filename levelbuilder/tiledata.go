package main

import (
	"io/ioutil"
	"log"
	"os"
	"path/filepath"
)

var meshCache map[string]*Mesh = make(map[string]*Mesh)

func buildFace(plyFile string, matType MaterialType) *LevelBlockFace {
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
	right string,
	rightMat MaterialType,
	front string,
	frontMat MaterialType,
	back string,
	backMat MaterialType,
	top string,
	topMat MaterialType,
) *LevelBlock {
	return &LevelBlock{
		isSolid,
		buildFace(left, leftMat),
		buildFace(right, rightMat),
		buildFace(front, frontMat),
		buildFace(back, backMat),
		buildFace(top, topMat),
	}
}

func buildTile(
	blocks [3]*LevelBlock,
) *LevelTile {
	return &LevelTile{
		blocks,
	}
}

func BuildTileSet() *LevelTileSet {
	var result LevelTileSet

	var floorBlock = buildBlock(
		true,
		"ply/flat_face.ply", Underhang,
		"ply/flat_face.ply", Underhang,
		"ply/flat_face.ply", Underhang,
		"ply/flat_face.ply", Underhang,
		"ply/flat_face.ply", LowerFloor,
	)

	var platformBlock = buildBlock(
		true,
		"ply/flat_face.ply", Wall,
		"ply/flat_face.ply", Wall,
		"ply/flat_face.ply", Wall,
		"ply/flat_face.ply", Wall,
		"ply/flat_face.ply", UpperFloor,
	)

	var barrierBlock = buildBlock(
		true,
		"ply/flat_face.ply", Wall,
		"ply/flat_face.ply", Wall,
		"ply/flat_face.ply", Wall,
		"ply/flat_face.ply", Wall,
		"ply/flat_face.ply", BarrierTop,
	)

	result.Tiles = make(map[string]*LevelTile)

	result.Tiles["Floor"] = buildTile([3]*LevelBlock{
		floorBlock,
		nil,
		nil,
	})

	result.Tiles["Platform"] = buildTile([3]*LevelBlock{
		floorBlock,
		platformBlock,
		nil,
	})

	result.Tiles["Barrier"] = buildTile([3]*LevelBlock{
		floorBlock,
		platformBlock,
		barrierBlock,
	})

	return &result
}
