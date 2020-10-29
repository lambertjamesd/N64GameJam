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
		output, err := os.OpenFile(fmt.Sprintf("output/geo_%d.inc.c", material), os.O_CREATE|os.O_WRONLY|os.O_TRUNC, 0664)

		if err != nil {
			log.Fatal(err)
		}

		defer output.Close()

		mesh = TransformMesh(mesh, RoundToN64)
		mesh = RemoveDuplicates(mesh)
		mesh = FlipZ(mesh)
		mesh = Triangulate(mesh)
		mesh = TransformMesh(mesh, ApplyLighting(0.5, -0.57735, -0.57735, -0.57735))

		WriteMeshToC(output, mesh, fmt.Sprintf("_level_test_geo_%d", material), writeColorVertex)
	}
}

func testCOut() {
	var face = buildFace("ply/flat_face.ply", LowerFloor)

	output, err := os.OpenFile("output/test.inc.c", os.O_CREATE|os.O_WRONLY|os.O_TRUNC, 0664)

	if err != nil {
		log.Fatal(err)
	}

	defer output.Close()

	var mesh = TransformMesh(face.Mesh, RoundToN64)
	mesh = RemoveDuplicates(mesh)
	mesh = FlipZ(mesh)
	mesh = Triangulate(mesh)

	WriteMeshToC(output, mesh, "_level_geo_test_%d", writeColorVertex)
}

func main() {
	testLevelCombine()
}
