package main

import (
	"encoding/binary"
	"io"
	"log"
	"os"
	"strings"
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
			0, 0,
			0, 0,
			nil,
			nil,
			nil,
			nil,
			nil,
		}
	}

	maxX = maxX + 1
	maxY = maxY + 1

	var result LevelGrid

	result.Tiles = make([][]LevelTileSlot, maxX-minX+1)

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

	result.PlayerFinishPosX = level.PlayerFinishPosX - float32(minX)
	result.PlayerFinishPosY = level.PlayerFinishPosY - float32(minY)

	result.RobotFinishPosX = level.RobotFinishPosX - float32(minX)
	result.RobotFinishPosY = level.RobotFinishPosY - float32(minY)

	return &result
}

func splitTileName(tileName string) (string, map[string]string) {
	var parts = strings.Split(tileName, ";")
	var mapResult = make(map[string]string)

	for i := 1; i < len(parts); i = i + 1 {
		var valuePair = strings.Split(parts[i], "=")

		if len(valuePair) == 1 {
			mapResult[valuePair[0]] = ""
		} else {
			mapResult[valuePair[0]] = valuePair[1]
		}
	}

	return parts[0], mapResult
}

func findSwitches(level *LevelGrid) {
	for x, row := range level.Tiles {
		for y, cell := range row {
			if cell.Tile == nil {
				continue
			}

			switch cell.Tile.DynamicType {
			case DynamicTypeLargeSwitch:
				level.Switches = append(level.Switches, LevelSwitchDef{
					Vector3{float32(x * 2), 0, float32(-y * 2)},
					LevelSwitchTypeLarge,
					cell.ParamAsInt("color", 0),
				})
			case DynamicTypeSmallSwitch:
				level.Switches = append(level.Switches, LevelSwitchDef{
					Vector3{float32(x * 2), 0, float32(-y * 2)},
					LevelSwitchTypeSmall,
					cell.ParamAsInt("color", 0),
				})
			case DynamicTypePlatformSwitch:
				level.Switches = append(level.Switches, LevelSwitchDef{
					Vector3{float32(x * 2), 1, float32(-y * 2)},
					LevelSwitchTypeSmall,
					cell.ParamAsInt("color", 0),
				})
			case DynamicTypeDoor:
				level.Doors = append(level.Doors, LevelDoorDef{
					Vector3{float32(x * 2), 0, float32(-y * 2)},
					cell.ParamAsInt("color", 0),
				})
			case DynamicTypeBreakablePlatform:
				level.Breakables = append(level.Breakables, LevelBreakableDef{
					Vector3{float32(x * 2), 0, float32(-y * 2)},
					0,
				})
			case DynamicTypeBreakableBarrier:
				level.Breakables = append(level.Breakables, LevelBreakableDef{
					Vector3{float32(x * 2), 0, float32(-y * 2)},
					1,
				})
			case DynamicTypeMovingPlatform:
				level.Platforms = append(level.Platforms, LevelPlatformDef{
					Vector3{float32(x * 2), 0, float32(-y * 2)},
					cell.ParamAsInt("color", 0),
					len(level.PlatformSlots),
				})
				level.PlatformSlots = append(level.PlatformSlots, LevelPlatformSlotDef{
					Vector3{float32(x * 2), 0, float32(-y * 2)},
				})
			case DynamicTypePlatformSlot:
				level.PlatformSlots = append(level.PlatformSlots, LevelPlatformSlotDef{
					Vector3{float32(x * 2), 0, float32(-y * 2)},
				})
			}
		}
	}
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
			tileName, extraValues := splitTileName(readString(file))

			tile, _ := tileMap.Tiles[tileName]

			var rotation uint8
			binary.Read(file, binary.LittleEndian, &rotation)

			rowData[row] = LevelTileSlot{
				tile,
				int(rotation),
				extraValues,
			}
		}

		result.Tiles = append(result.Tiles, rowData)
	}

	binary.Read(file, binary.LittleEndian, &result.PlayerPosX)
	binary.Read(file, binary.LittleEndian, &result.PlayerPosY)

	binary.Read(file, binary.LittleEndian, &result.RobotPosX)
	binary.Read(file, binary.LittleEndian, &result.RobotPosY)

	var prefabCount uint32
	binary.Read(file, binary.LittleEndian, &prefabCount)

	binary.Read(file, binary.LittleEndian, &result.PlayerFinishPosX)
	binary.Read(file, binary.LittleEndian, &result.PlayerFinishPosY)

	binary.Read(file, binary.LittleEndian, &result.RobotFinishPosX)
	binary.Read(file, binary.LittleEndian, &result.RobotFinishPosY)

	var trimmed = TrimLevel(&result)

	findSwitches(trimmed)

	return trimmed
}
