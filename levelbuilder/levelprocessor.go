package main

import (
	"fmt"
	"log"
	"os"
	"path/filepath"
)

func writeOutTiles(levelName string, outputFile string, level *LevelGrid) {
	outputLevel, err := os.OpenFile(outputFile, os.O_CREATE|os.O_WRONLY|os.O_TRUNC, 0664)

	if err != nil {
		log.Fatal(err)
	}

	defer outputLevel.Close()

	width, height := level.GetSize()

	outputLevel.WriteString(fmt.Sprintf("\nstruct LevelCollisionCell _%s_collision_cells[] = {\n", levelName))

	for x := 0; x < width; x = x + 1 {
		for y := 0; y < height; y = y + 1 {
			var tileName string = ""
			var rotation int = 0

			if x < len(level.Tiles) {
				var row = level.Tiles[x]

				if y < len(row) && row[y].Tile != nil {
					tileName = row[y].Tile.CollisionTileName
					rotation = row[y].Rotation
				}
			}

			if tileName != "" {
				outputLevel.WriteString(fmt.Sprintf("    {&%s, %d},\n", tileName, rotation))
			} else {
				outputLevel.WriteString("    {0, 0},\n")
			}
		}
	}

	outputLevel.WriteString("    (void*)0xDEADBEEF,\n")

	outputLevel.WriteString("};\n")

	outputLevel.WriteString(fmt.Sprintf(`
struct LevelCollisionGrid _level_%s_collision_grid = {
	_%s_collision_cells,
	%d,
	%d,
};
`, levelName, levelName, width, height))
}

func processLevel(levelName string, levelFile string, outputFile string, gridSize int) {
	var tileSet = BuildTileSet()
	var tileMap = ParseLevel(levelFile, tileSet)

	var joinedMesh = ExtractMeshTiles(tileMap, gridSize)

	var geoSources []string = nil
	var materialCount = 0

	var levelIncFile = "level.inc.c"
	var tileIncFile = "tiles.inc.c"

	outputLevel, err := os.OpenFile(filepath.Join(filepath.Dir(outputFile), levelIncFile), os.O_CREATE|os.O_WRONLY|os.O_TRUNC, 0664)

	if err != nil {
		log.Fatal(err)
	}

	defer outputLevel.Close()

	for x, row := range joinedMesh.Tiles {
		outputLevel.WriteString(fmt.Sprintf("\nGfx** _level_%s_row_%d[] = {\n", levelName, x))

		for y, tile := range row {
			var cFile = fmt.Sprintf("geo_%d_%d.inc.c", x, y)

			outputGeo, err := os.OpenFile(filepath.Join(filepath.Dir(outputFile), cFile), os.O_CREATE|os.O_WRONLY|os.O_TRUNC, 0664)

			geoSources = append(geoSources, cFile)

			if err != nil {
				log.Fatal(err)
			}

			defer outputGeo.Close()

			var tileMeshNames []string = nil

			for _, meshPair := range tile {
				var mesh = meshPair.Mesh
				var material = meshPair.Material

				if len(mesh.faces) > 0 {
					mesh = TransformMesh(mesh, RoundToN64)
					mesh = RemoveDuplicates(mesh)
					mesh = FlipZ(mesh)

					var dissolved = LimitedDissolve(mesh)

					mesh = Triangulate(mesh)

					var lx, ly, lz float32
					lx = -1
					ly = -2
					lz = 3

					Normalize3f(&lx, &ly, &lz)

					mesh = TransformMesh(mesh, ApplyLighting(0.25, lx, ly, lz))

					debug_mesh, _ := os.OpenFile(filepath.Join(filepath.Dir(outputFile), fmt.Sprintf("_level_%s_geo_%d_%d_%d.ply", levelName, x, y, material)), os.O_CREATE|os.O_WRONLY|os.O_TRUNC, 0664)
					dissolved.WritePly(debug_mesh)
					defer debug_mesh.Close()

					var gfxName = WriteMeshToC(outputGeo, mesh, fmt.Sprintf("_level_%s_geo_%d_%d_%d", levelName, x, y, material), writeColorVertex)

					tileMeshNames = append(tileMeshNames, gfxName)
				} else {
					tileMeshNames = append(tileMeshNames, "0")
				}
			}

			materialCount = len(tileMeshNames)

			outputGeo.WriteString(fmt.Sprintf("\nGfx* _level_%s_tile_%d_%d[] = {\n", levelName, x, y))

			for _, name := range tileMeshNames {
				outputGeo.WriteString(fmt.Sprintf("    %s,\n", name))
			}

			outputGeo.WriteString("    (void*)0xDEADBEEF,\n")

			outputGeo.WriteString("};\n")

			outputLevel.WriteString(fmt.Sprintf("    _level_%s_tile_%d_%d,\n", levelName, x, y))
		}

		outputLevel.WriteString("    (void*)0xDEADBEEF,\n")
		outputLevel.WriteString("};\n")
	}

	outputLevel.WriteString(fmt.Sprintf("\nGfx*** _level_%s_tileGrid[] = {\n", levelName))

	var height = 0

	for x, row := range joinedMesh.Tiles {
		height = len(row)
		outputLevel.WriteString(fmt.Sprintf("    _level_%s_row_%d,\n", levelName, x))
	}

	outputLevel.WriteString("    (void*)0xDEADBEEF,\n")

	outputLevel.WriteString("};\n")

	outputLevel.WriteString(fmt.Sprintf(`
struct LevelGraphics _level_%s_levelGraphics = {
	{
		_level_%s_tileGrid,
		%d, %d,
		%d,
		%d,	
	},
};
`, levelName, levelName, len(joinedMesh.Tiles), height, materialCount, gridSize))

	outputLevel.WriteString(fmt.Sprintf(`
struct LevelSwitchDef _level_%s_levelSwitches[] = {
`, levelName))

	for _, puzzleSwitch := range tileMap.Switches {
		outputLevel.WriteString(fmt.Sprintf("    {{%.6f, %.6f, %.6f}, %d, %d},\n", puzzleSwitch.Pos.X, puzzleSwitch.Pos.Y, puzzleSwitch.Pos.Z, puzzleSwitch.Type, puzzleSwitch.Color))
	}

	outputLevel.WriteString("};\n")

	outputLevel.WriteString(fmt.Sprintf(`
struct LevelDoorDef _level_%s_levelDoors[] = {
`, levelName))

	for _, door := range tileMap.Doors {
		outputLevel.WriteString(fmt.Sprintf("    {{%.6f, %.6f, %.6f}, %d},\n", door.Pos.X, door.Pos.Y, door.Pos.Z, door.Color))
	}

	outputLevel.WriteString("};\n")

	outputLevel.WriteString(fmt.Sprintf(`
struct LevelBreakableDef _level_%s_levelBreakables[] = {
`, levelName))

	for _, breakable := range tileMap.Breakables {
		outputLevel.WriteString(fmt.Sprintf("    {{%.6f, %.6f, %.6f}, %d},\n", breakable.Pos.X, breakable.Pos.Y, breakable.Pos.Z, breakable.Type))
	}

	outputLevel.WriteString("};\n")

	outputLevel.WriteString(fmt.Sprintf(`
struct LevelPlatformDef _level_%s_movingPlatforms[] = {
`, levelName))

	for _, platform := range tileMap.Platforms {
		outputLevel.WriteString(fmt.Sprintf("    {{%.6f, %.6f, %.6f}, %d, %d},\n", platform.Pos.X, platform.Pos.Y, platform.Pos.Z, platform.Color, platform.SlotIndex))
	}

	outputLevel.WriteString("};\n")

	outputLevel.WriteString(fmt.Sprintf(`
struct LevelPlatformSlotDef _level_%s_platformSlots[] = {
`, levelName))

	for _, slot := range tileMap.PlatformSlots {
		outputLevel.WriteString(fmt.Sprintf("    {{%.6f, %.6f, %.6f}},\n", slot.Pos.X, slot.Pos.Y, slot.Pos.Z))
	}

	outputLevel.WriteString("};\n")

	outputLevel.WriteString(fmt.Sprintf(`
struct LevelData _level_%s_levelData = {
	&_level_%s_levelGraphics,
	&_level_%s_collision_grid,
	{%.6f, %.6f, %.6f},
	{%.6f, %.6f, %.6f},
	_level_%s_levelSwitches,
	%d,
	_level_%s_levelDoors,
	%d,
	_level_%s_levelBreakables,
	%d,
	_level_%s_movingPlatforms,
	%d,
	_level_%s_platformSlots,
	%d,
};
`, levelName, levelName, levelName,
		tileMap.PlayerPosX, 0.1, -tileMap.PlayerPosY,
		tileMap.RobotPosX, 0.1, -tileMap.RobotPosY,
		levelName, len(tileMap.Switches),
		levelName, len(tileMap.Doors),
		levelName, len(tileMap.Breakables),
		levelName, len(tileMap.Platforms),
		levelName, len(tileMap.PlatformSlots),
	))

	geoSources = append(geoSources, tileIncFile)
	geoSources = append(geoSources, levelIncFile)

	writeOutTiles(levelName, filepath.Join(filepath.Dir(outputFile), tileIncFile), tileMap)

	WriteGeoFile(outputFile, levelName, geoSources)
	WriteGeoHeader(outputFile, []string{
		fmt.Sprintf("struct LevelData _level_%s_levelData", levelName),
	})
}
