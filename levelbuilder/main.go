package main

import (
	"fmt"
	"log"
	"os"
	"path/filepath"
)

func processLevel(levelFile string, outputDirectory string) {
	var tileSet = BuildTileSet()
	var tileMap = ParseLevel(levelFile, tileSet)

	var joinedMesh = ExtractCombinedMesh(tileMap)

	var writtenMeshes []string = nil
	var exportedGfx []string = nil

	for material, mesh := range joinedMesh {
		var cFile = fmt.Sprintf(filepath.Join(filepath.Dir(outputDirectory), "geo_%d.inc.c"), material)

		output, err := os.OpenFile(cFile, os.O_CREATE|os.O_WRONLY|os.O_TRUNC, 0664)

		writtenMeshes = append(writtenMeshes, cFile)

		if err != nil {
			log.Fatal(err)
		}

		defer output.Close()

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

		var gfxName = WriteMeshToC(output, mesh, fmt.Sprintf("_level_test_geo_%d", material), writeColorVertex)

		exportedGfx = append(exportedGfx, gfxName)
	}
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
		if len(os.Args) == 4 {
			processLevel(os.Args[2], os.Args[3])
		} else {
			printUsage()
		}
	} else {
		printUsage()
	}
}