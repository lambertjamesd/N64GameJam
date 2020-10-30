package main

import (
	"fmt"
	"log"
	"os"
	"path/filepath"
)

func processLevel(levelName string, levelFile string, outputFile string) {
	var tileSet = BuildTileSet()
	var tileMap = ParseLevel(levelFile, tileSet)

	var joinedMesh = ExtractCombinedMesh(tileMap)

	var geoSources []string = nil
	var exportedGfx []string = nil

	for material, mesh := range joinedMesh {
		var cFile = fmt.Sprintf("geo_%d.inc.c", material)

		outputGeo, err := os.OpenFile(filepath.Join(filepath.Dir(outputFile), cFile), os.O_CREATE|os.O_WRONLY|os.O_TRUNC, 0664)

		geoSources = append(geoSources, cFile)

		if err != nil {
			log.Fatal(err)
		}

		defer outputGeo.Close()

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

		var gfxName = WriteMeshToC(outputGeo, mesh, fmt.Sprintf("_level_"+levelName+"_geo_%d", material), writeColorVertex)

		exportedGfx = append(exportedGfx, gfxName)
	}

	WriteGeoFile(outputFile, geoSources)
	WriteGeoHeader(outputFile, exportedGfx)
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
			processLevel(os.Args[2], os.Args[3], os.Args[4])
		} else {
			printUsage()
		}
	} else {
		printUsage()
	}
}
