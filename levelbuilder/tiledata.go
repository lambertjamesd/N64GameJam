package main

import (
	"io/ioutil"
	"log"
	"os"
	"path/filepath"
)

var meshCache map[string]*Mesh = make(map[string]*Mesh)

func buildFace(plyFile string) *LevelBlockFace {
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

	return &LevelBlockFace{mesh}
}

func buildBlock(
	isSolid bool,
	left string,
	right string,
	front string,
	back string,
	top string,
) *LevelBlock {
	return &LevelBlock{
		isSolid,
		buildFace(left),
		buildFace(right),
		buildFace(front),
		buildFace(back),
		buildFace(top),
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

	var basicBlock = buildBlock(
		true,
		"ply/flat_face.ply",
		"ply/flat_face.ply",
		"ply/flat_face.ply",
		"ply/flat_face.ply",
		"ply/flat_face.ply",
	)

	result.Tiles = make(map[string]*LevelTile)

	result.Tiles["Floor"] = buildTile([3]*LevelBlock{
		basicBlock,
		nil,
		nil,
	})

	result.Tiles["Platform"] = buildTile([3]*LevelBlock{
		basicBlock,
		basicBlock,
		nil,
	})

	result.Tiles["Barrier"] = buildTile([3]*LevelBlock{
		basicBlock,
		basicBlock,
		basicBlock,
	})

	return &result
}
