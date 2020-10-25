package main

import (
	"fmt"
	"os"
)

func main() {
	var tileSet = BuildTileSet()
	var tileMap = ParseLevel(os.Args[1], tileSet)

	fmt.Printf("Rows %d", len(tileMap.Tiles))
}
