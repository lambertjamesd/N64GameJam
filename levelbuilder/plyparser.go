package main

import (
	"errors"
	"fmt"
	"strconv"
	"strings"
)

type plyProperty struct {
	propertyType string
	name         string
}

type plyElement struct {
	name       string
	count      uint32
	properties []plyProperty
}

type plyParseData struct {
	elements        []*plyElement
	target          *Mesh
	lines           []string
	currentLine     int
	collisionLayers string
}

func parseElements(parseData *plyParseData) error {
	var currentElement *plyElement = nil
	var i = parseData.currentLine

	for ; i < len(parseData.lines); i = i + 1 {
		lineSplit := strings.Split(parseData.lines[i], " ")

		var name = strings.TrimSpace(lineSplit[0])

		if name == "element" {
			if len(lineSplit) != 3 {
				return errors.New(fmt.Sprintf("Incorrent number of parameters for element line: %d", i+1))
			}

			elementSize, err := strconv.ParseUint(lineSplit[2], 10, 32)

			if err != nil {
				return err
			}

			currentElement = &plyElement{
				strings.TrimSpace(lineSplit[1]),
				uint32(elementSize),
				nil,
			}

			parseData.elements = append(parseData.elements, currentElement)
		} else if name == "property" {
			if len(lineSplit) < 3 {
				return errors.New(fmt.Sprintf("Incorrent number of parameters for property line: %d", i+1))
			}

			if currentElement == nil {
				return errors.New(fmt.Sprintf("Trying to use property before element line: %d", i+1))
			}

			currentElement.properties = append(currentElement.properties, plyProperty{
				strings.Join(lineSplit[1:len(lineSplit)-1], " "),
				lineSplit[len(lineSplit)-1],
			})
		} else if name == "end_header" {
			i = i + 1
			break
		} else if name == "comment" && strings.TrimSpace(lineSplit[1]) == "CollisionLayers" {
			parseData.collisionLayers = strings.Join(lineSplit[2:len(lineSplit)], " ")
		}
	}

	parseData.currentLine = i

	return nil
}

func parseVertexElement(vertex *MeshVertex, propertyType string, value string) error {
	switch propertyType {
	case "x":
		val, err := strconv.ParseFloat(value, 32)
		vertex.x = float32(val)
		return err
	case "y":
		val, err := strconv.ParseFloat(value, 32)
		vertex.y = float32(val)
		return err
	case "z":
		val, err := strconv.ParseFloat(value, 32)
		vertex.z = float32(val)
		return err

	case "nx":
		val, err := strconv.ParseFloat(value, 32)
		vertex.nx = float32(val)
		return err
	case "ny":
		val, err := strconv.ParseFloat(value, 32)
		vertex.ny = float32(val)
		return err
	case "nz":
		val, err := strconv.ParseFloat(value, 32)
		vertex.nz = float32(val)
		return err

	case "s":
		val, err := strconv.ParseFloat(value, 32)
		vertex.s = float32(val)
		return err
	case "t":
		val, err := strconv.ParseFloat(value, 32)
		vertex.t = float32(val)
		return err

	case "red":
		val, err := strconv.ParseUint(value, 10, 8)
		vertex.r = uint8(val)
		return err
	case "green":
		val, err := strconv.ParseUint(value, 10, 8)
		vertex.g = uint8(val)
		return err
	case "blue":
		val, err := strconv.ParseUint(value, 10, 8)
		vertex.b = uint8(val)
		return err
	case "alpha":
		val, err := strconv.ParseUint(value, 10, 8)
		vertex.a = uint8(val)
		return err
	}

	return errors.New("Could not parse property of type " + propertyType)
}

func parseData(parseData *plyParseData) error {
	var i = parseData.currentLine
	var dataIndex uint32 = 0
	var elementIndex = 0
	var currentElement *plyElement = nil

	if len(parseData.elements) > 0 {
		currentElement = parseData.elements[0]
	} else {
		return nil
	}

	for ; i < len(parseData.lines); i = i + 1 {
		parts := strings.Split(parseData.lines[i], " ")

		if currentElement.name == "vertex" {
			var vertex MeshVertex

			vertex.r = 255
			vertex.g = 255
			vertex.b = 255
			vertex.a = 255

			for j := 0; j < len(currentElement.properties); j = j + 1 {
				parseVertexElement(&vertex, currentElement.properties[j].name, strings.TrimSpace(parts[j]))
			}

			parseData.target.vertices = append(parseData.target.vertices, vertex)

			dataIndex = dataIndex + 1
		} else if currentElement.name == "face" {
			count, err := strconv.ParseUint(parts[0], 10, 32)

			if err != nil {
				return nil
			}

			if count+1 != uint64(len(parts)) {
				return errors.New("Wrong number of face indices")
			}

			var face MeshFace
			face.indices = make([]uint32, count)

			for i := 0; uint64(i) < count; i = i + 1 {
				index, err := strconv.ParseUint(parts[i+1], 10, 32)

				if err != nil {
					return nil
				}

				face.indices[i] = uint32(index)
			}

			parseData.target.faces = append(parseData.target.faces, face)

			dataIndex = dataIndex + 1
		}

		if dataIndex == currentElement.count {
			dataIndex = 0
			elementIndex = elementIndex + 1

			if elementIndex < len(parseData.elements) {
				currentElement = parseData.elements[elementIndex]
			} else {
				return nil
			}
		}
	}

	return nil
}

func ParsePly(fileContent string) (*Mesh, *plyParseData, error) {
	var result Mesh
	var state plyParseData

	state.elements = nil
	state.target = &result
	state.lines = strings.Split(fileContent, "\n")

	err := parseElements(&state)

	if err != nil {
		return nil, nil, err
	}

	err = parseData(&state)

	if err != nil {
		return nil, nil, err
	}

	return &result, &state, nil
}
