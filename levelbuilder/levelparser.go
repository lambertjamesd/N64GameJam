package main

import (
	"encoding/binary"
	"io"
	"log"
	"os"
)

func readString(reader io.Reader) string {
	var len uint8
	binary.Read(reader, binary.LittleEndian, &len)
	var buffer = make([]byte, len)
	reader.Read(buffer)
	return string(buffer)
}

func ParseLevel(filename string, tileMap *LevelTileSet) *LevelGrid {
	var result LevelGrid

	file, err := os.Open(filename)

	if err != nil {
		log.Fatal(err)
	}

	var header = readString(file)

	if header != "LevelData" {
		log.Fatal("Not a level file")
	}

	var version uint32
	binary.Read(file, binary.LittleEndian, &version)

	var cols uint32
	binary.Read(file, binary.LittleEndian, &cols)

	for col := uint32(0); col < cols; col = col + 1 {
		var rows uint32
		binary.Read(file, binary.LittleEndian, &rows)

		var rowData = make([]LevelTileSlot, rows)

		for row := uint32(0); row < rows; row = row + 1 {
			tile, _ := tileMap.Tiles[readString(file)]

			var rotation uint8
			binary.Read(file, binary.LittleEndian, &rotation)

			rowData[row] = LevelTileSlot{
				tile,
				int(rotation),
			}
		}

		result.Tiles = append(result.Tiles, rowData)
	}

	return &result
}
