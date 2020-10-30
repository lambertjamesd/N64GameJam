package main

import (
	"fmt"
	"log"
	"os"
	"path/filepath"
)

func processLevel(levelName string, levelFile string, outputFile string, gridSize int) {
	var tileSet = BuildTileSet()
	var tileMap = ParseLevel(levelFile, tileSet)

	var joinedMesh = ExtractMeshTiles(tileMap, gridSize)

	var geoSources []string = nil
	var materialCount = 0

	var leveIncFile = "level.inc.c"

	outputLevel, err := os.OpenFile(filepath.Join(filepath.Dir(outputFile), leveIncFile), os.O_CREATE|os.O_WRONLY|os.O_TRUNC, 0664)

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

			for material, mesh := range tile {
				if len(mesh.faces) > 0 {
					mesh = TransformMesh(mesh, RoundToN64)
					mesh = RemoveDuplicates(mesh)
					mesh = FlipZ(mesh)
					mesh = Triangulate(mesh)

					var lx, ly, lz float32
					lx = -1
					ly = -2
					lz = 3

					Normalize3f(&lx, &ly, &lz)

					mesh = TransformMesh(mesh, ApplyLighting(0.25, lx, ly, lz))

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

			outputGeo.WriteString("};\n")

			outputLevel.WriteString(fmt.Sprintf("    _level_%s_tile_%d_%d,\n", levelName, x, y))
		}

		outputLevel.WriteString("};\n")
	}

	outputLevel.WriteString(fmt.Sprintf("\nGfx*** _level_%s_tileGrid[] = {\n", levelName))

	var height = 0

	for x, row := range joinedMesh.Tiles {
		height = len(row)
		outputLevel.WriteString(fmt.Sprintf("    _level_%s_row_%d,\n", levelName, x))
	}

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

	geoSources = append(geoSources, leveIncFile)

	WriteGeoFile(outputFile, geoSources)
	WriteGeoHeader(outputFile, []string{
		fmt.Sprintf("struct LevelGraphics _level_%s_levelGraphics", levelName),
	})
}

func printUsage() {
	log.Print(`
Usage:
levelbuilder level inputFile.level outputDir/
`)
}

func main() {
	if len(os.Args) < 2 {
		printUsage()
	} else if os.Args[1] == "level" {
		if len(os.Args) == 5 {
			processLevel(os.Args[2], os.Args[3], os.Args[4], 8)
		} else {
			printUsage()
		}
	} else {
		printUsage()
	}
}
