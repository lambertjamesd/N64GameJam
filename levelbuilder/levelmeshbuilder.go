package main

type FullVertexTransform func(int, int, int) VertexTransform

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

func ExtractTileMeshes(separateMeshes []*Mesh, x int, y int, tile *LevelTileSlot, level *LevelGrid, material MaterialType) []*Mesh {
	if tile.Tile != nil {
		for height := 0; height < 3; height = height + 1 {
			var block = tile.Tile.Blocks[height]

			if block != nil {
				if !tile.Tile.IsSolidAtHeight(height+1) && block.Top != nil && block.Top.Material == material {
					separateMeshes = append(
						separateMeshes,
						TransformMesh(block.Top.Mesh, FullTopTileTransform(x, y, height, tile.Rotation)),
					)
				}

				for i := 0; i < 4; i = i + 1 {
					var rotIndex = (i + 4 - tile.Rotation) % 4
					if !level.IsSolid(x+faceDir[rotIndex][0], y+faceDir[rotIndex][1], height) && block.Sides[i] != nil && block.Sides[i].Material == material {
						separateMeshes = append(
							separateMeshes,
							TransformMesh(block.Sides[i].Mesh, rotationTransforms[rotIndex](x, y, height)),
						)
					}
				}
			}
		}
	}

	return separateMeshes
}

func ExtractCombinedMesh(level *LevelGrid) map[MaterialType]*Mesh {
	var result = make(map[MaterialType]*Mesh)

	for _, material := range AllMaterials {
		var separateMeshes []*Mesh = nil

		for x, rowData := range level.Tiles {
			for y, cell := range rowData {
				separateMeshes = ExtractTileMeshes(separateMeshes, x, y, &cell, level, material)
			}
		}

		result[material] = JoinMesh(separateMeshes)
	}

	return result
}
