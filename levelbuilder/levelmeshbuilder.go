package main

type MeshMaterial struct {
	Mesh     *Mesh
	Material MaterialType
}

type MeshTile struct {
	Tiles [][][]MeshMaterial
}

type FullVertexTransform func(int, int, int, int, int) VertexTransform

var rotationTransforms = []FullVertexTransform{
	FullLeftTileTransform,
	FullFrontTileTransform,
	FullRightTileTransform,
	FullBackTileTransform,
}

var faceDir = [][2]int{
	[2]int{-1, 0},
	[2]int{0, -1},
	[2]int{1, 0},
	[2]int{0, 1},
}

func ExtractTileMeshes(separateMeshes []*Mesh, x int, y int, s int, t int, tile *LevelTileSlot, level *LevelGrid, material MaterialType) []*Mesh {
	if tile.Tile != nil {
		for height := 0; height < 3; height = height + 1 {
			var block = tile.Tile.Blocks[height]

			if block != nil {
				if !tile.Tile.IsSolidAtHeight(height+1) && block.Top != nil && block.Top.Material == material {
					separateMeshes = append(
						separateMeshes,
						TransformMesh(block.Top.Mesh, FullTopTileTransform(x, y, s, t, height, tile.Rotation)),
					)
				}

				for i := 0; i < 4; i = i + 1 {
					var rotIndex = (i + 4 - tile.Rotation) % 4
					if !level.IsSolid(x+faceDir[rotIndex][0], y+faceDir[rotIndex][1], height) && block.Sides[i] != nil && block.Sides[i].Material == material {
						separateMeshes = append(
							separateMeshes,
							TransformMesh(block.Sides[i].Mesh, rotationTransforms[rotIndex](x, y, s, t, height)),
						)
					}
				}
			}
		}
	}

	return separateMeshes
}

func ExtractCombinedMesh(level *LevelGrid, startX, startY, groupSize int) []MeshMaterial {
	var result []MeshMaterial = nil

	for _, material := range AllMaterials {
		var separateMeshes []*Mesh = nil

		for x := startX; x < startX+groupSize && x < len(level.Tiles); x = x + 1 {
			var row = level.Tiles[x]
			for y := startY; y < startY+groupSize && y < len(row); y = y + 1 {
				separateMeshes = ExtractTileMeshes(separateMeshes, x, y, x-startX, y-startY, &row[y], level, material)
			}
		}

		result = append(result, MeshMaterial{JoinMesh(separateMeshes), material})
	}

	return result
}

func ExtractMeshTiles(level *LevelGrid, groupSize int) *MeshTile {
	var result MeshTile

	width, height := level.GetSize()

	for x := 0; x < width; x = x + groupSize {
		var row [][]MeshMaterial = nil

		for y := 0; y < height; y = y + groupSize {
			row = append(row, ExtractCombinedMesh(level, x, y, groupSize))
		}

		result.Tiles = append(result.Tiles, row)
	}

	return &result
}
