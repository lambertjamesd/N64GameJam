package main

import (
	"fmt"
	"io/ioutil"
	"log"
)

func main() {
	content, err := ioutil.ReadFile("/home/james/Documents/GameJam/Test.ply")

	if err != nil {
		log.Fatal(err)
	}

	mesh, err := ParsePly(string(content))

	if err != nil {
		log.Fatal(err)
	}

	fmt.Printf("Vertices %d Faces %d", len(mesh.vertices), len(mesh.faces))
}
