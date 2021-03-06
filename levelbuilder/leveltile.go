package main

import "strconv"

type MaterialType int

const (
	LowerFloor MaterialType = 0
	UpperFloor MaterialType = 1
	BarrierTop MaterialType = 2
	Wall       MaterialType = 3
	Underhang  MaterialType = 4
	Lava       MaterialType = 5
	Track      MaterialType = 6
)

const (
	DynamicTypeNone              = 0
	DynamicTypeLargeSwitch       = 1
	DynamicTypeSmallSwitch       = 2
	DynamicTypePlatformSwitch    = 3
	DynamicTypeDoor              = 4
	DynamicTypeBreakablePlatform = 5
	DynamicTypeBreakableBarrier  = 6
	DynamicTypeMovingPlatform    = 7
	DynamicTypePlatformSlot      = 8
)

var AllMaterials = []MaterialType{
	LowerFloor,
	UpperFloor,
	BarrierTop,
	Wall,
	Underhang,
	Lava,
	Track,
}

type LevelBlockFace struct {
	Mesh     *Mesh
	Material MaterialType
}

type LevelBlock struct {
	IsSolid bool
	Sides   [4]*LevelBlockFace
	Top     *LevelBlockFace
}

type LevelTile struct {
	Blocks            [3]*LevelBlock
	CollisionTileName string
	DynamicType       int
}

type LevelTileSet struct {
	Tiles map[string]*LevelTile
}

type LevelTileSlot struct {
	Tile     *LevelTile
	Rotation int
	TileData map[string]string
}

const (
	LevelSwitchTypeLarge          = 0
	LevelSwitchTypeSmall          = 1
	LevelSwitchTypeLargePermanant = 2
	LevelSwitchTypeSmallPermanant = 3
)

type LevelSwitchDef struct {
	Pos   Vector3
	Type  int
	Color int
}

type LevelDoorDef struct {
	Pos      Vector3
	Color    int
	Inverted bool
}

type LevelBreakableDef struct {
	Pos  Vector3
	Type int
}

type LevelPlatformDef struct {
	Pos       Vector3
	Color     int
	SlotIndex int
}

type LevelPlatformSlotDef struct {
	Pos Vector3
}

type LevelGemDef struct {
	Pos Vector3
}

type LevelGrid struct {
	Tiles                              [][]LevelTileSlot
	PlayerPosX, PlayerPosY             float32
	RobotPosX, RobotPosY               float32
	PlayerFinishPosX, PlayerFinishPosY float32
	RobotFinishPosX, RobotFinishPosY   float32
	Switches                           []LevelSwitchDef
	Doors                              []LevelDoorDef
	Breakables                         []LevelBreakableDef
	Platforms                          []LevelPlatformDef
	PlatformSlots                      []LevelPlatformSlotDef
	Gems                               []LevelGemDef
	Name                               string
	MusicName                          string
}

func (level *LevelGrid) GetSize() (int, int) {
	var cols = 0

	for _, row := range level.Tiles {
		if len(row) > cols {
			cols = len(row)
		}
	}

	return len(level.Tiles), cols
}

func (tile *LevelTile) IsSolidAtHeight(height int) bool {
	return height >= 0 && height < 3 && tile.Blocks[height] != nil && tile.Blocks[height].IsSolid
}

func (tile *LevelTileSlot) ParamAsInt(name string, defaultVal int) int {
	asString, ok := tile.TileData[name]

	if !ok {
		return defaultVal
	}

	asInt, err := strconv.ParseInt(asString, 10, 32)

	if err != nil {
		return defaultVal
	}

	return int(asInt)
}

func (tile *LevelTileSlot) ParamAsBool(name string, defaultVal bool) bool {
	asString, ok := tile.TileData[name]

	if !ok {
		return defaultVal
	}

	return asString == "true"
}

func (level *LevelGrid) IsSolid(x, y, height int) bool {
	if x < 0 || y < 0 || x >= len(level.Tiles) {
		return false
	}

	var row = level.Tiles[x]

	if y >= len(row) {
		return false
	}

	var cell = row[y]

	if cell.Tile == nil {
		return false
	}

	return cell.Tile.IsSolidAtHeight(height)
}

// func (level *LevelGrid) Trim() *LevelGrid {

// }
