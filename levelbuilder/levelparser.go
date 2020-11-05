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

func TrimLevel(level *LevelGrid) *LevelGrid {
	maxX, maxY := 0, 0
	minX, minY := level.GetSize()

	for x, row := range level.Tiles {
		for y, tile := range row {
			if tile.Tile != nil {
				if x < minX {
					minX = x
				}
				if x > maxX {
					maxX = x
				}
				if y < minY {
					minY = y
				}
				if y > maxY {
					maxY = y
				}
			}
		}
	}

	if minX > maxX || minY > maxY {
		// there is no level data
		return &LevelGrid{
			nil,
			0, 0,
			0, 0,
		}
	}

	maxX = maxX + 1
	maxY = maxY + 1

	var result LevelGrid

	result.Tiles = make([][]LevelTileSlot, maxX-minX)

	for x, row := range level.Tiles {
		if maxY <= len(row) {
			result.Tiles[x-minX] = row[minY:maxY]
		} else if minY < len(row) {
			result.Tiles[x-minX] = row[minY:len(row)]
		} else {
			result.Tiles[x-minX] = nil
		}
	}

	result.PlayerPosX = level.PlayerPosX - float32(minX)
	result.PlayerPosY = level.PlayerPosY - float32(minY)

	result.RobotPosX = level.RobotPosX - float32(minX)
	result.RobotPosY = level.RobotPosY - float32(minY)

	return &result
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

	binary.Read(file, binary.LittleEndian, &result.PlayerPosX)
	binary.Read(file, binary.LittleEndian, &result.PlayerPosY)

	binary.Read(file, binary.LittleEndian, &result.RobotPosX)
	binary.Read(file, binary.LittleEndian, &result.RobotPosY)

	return TrimLevel(&result)
}
