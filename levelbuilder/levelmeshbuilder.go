package main

func ExtractTileMeshes(separateMeshes []*Mesh, x int, y int, tile *LevelTileSlot, level *LevelGrid, material MaterialType) []*Mesh {
	if tile.Tile != nil {
		for height := 0; height < 3; height = height + 1 {
			var block = tile.Tile.Blocks[height]

			if block != nil {
				if !tile.Tile.IsSolidAtHeight(height+1) && block.Top != nil && block.Top.Material == material {
					separateMeshes = append(
						separateMeshes,
						TransformMesh(block.Top.Mesh, FullTopTileTransform(x, y, height)),
					)
				}

				if !level.IsSolid(x-1, y, height) && block.Left != nil && block.Left.Material == material {
					separateMeshes = append(
						separateMeshes,
						TransformMesh(block.Left.Mesh, FullLeftTileTransform(x, y, height)),
					)
				}

				if !level.IsSolid(x+1, y, height) && block.Right != nil && block.Right.Material == material {
					separateMeshes = append(
						separateMeshes,
						TransformMesh(block.Right.Mesh, FullRightTileTransform(x, y, height)),
					)
				}

				if !level.IsSolid(x, y+1, height) && block.Back != nil && block.Back.Material == material {
					separateMeshes = append(
						separateMeshes,
						TransformMesh(block.Back.Mesh, FullBackTileTransform(x, y, height)),
					)
				}

				if !level.IsSolid(x, y-1, height) && block.Front != nil && block.Front.Material == material {
					separateMeshes = append(
						separateMeshes,
						TransformMesh(block.Front.Mesh, FullFrontTileTransform(x, y, height)),
					)
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
