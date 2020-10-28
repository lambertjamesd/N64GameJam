package main

import (
	"fmt"
	"log"
	"os"
)

func testLevelCombine() {
	var tileSet = BuildTileSet()
	var tileMap = ParseLevel(os.Args[1], tileSet)

	var joinedMesh = ExtractCombinedMesh(tileMap)

	for material, mesh := range joinedMesh {
		output, err := os.OpenFile(fmt.Sprintf("test_out_%d.ply", material), os.O_CREATE|os.O_WRONLY|os.O_TRUNC, 0664)

		if err != nil {
			log.Fatal(err)
		}

		defer output.Close()

		FlipZ(mesh).WritePly(output)
	}
}

func main() {
	var face = buildFace("ply/flat_face.ply", LowerFloor)

	output, err := os.OpenFile("output/test.inc.c", os.O_CREATE|os.O_WRONLY|os.O_TRUNC, 0664)

	if err != nil {
		log.Fatal(err)
	}

	defer output.Close()

	WriteMeshToC(output, Triangulate(face.Mesh), "test", writeColorVertex)
}
