package main

type LevelBlockFace struct {
	Mesh *Mesh
}

type LevelBlock struct {
	IsSolid bool
	Left    *LevelBlockFace
	Right   *LevelBlockFace
	Front   *LevelBlockFace
	Back    *LevelBlockFace
	Top     *LevelBlockFace
}

type LevelTile struct {
	Blocks [3]*LevelBlock
}

type LevelTileSet struct {
	Tiles map[string]*LevelTile
}

type LevelTileSlot struct {
	Tile     *LevelTile
	Rotation int
}

type LevelGrid struct {
	Tiles [][]LevelTileSlot
}
