package main

import (
	"fmt"
	"math"
	"os"
)

const MIN_INT16 = -32768
const MAX_INT16 = 32767

const MIN_INT8 = -128
const MAX_INT8 = 127

func floatToVertexCoord(input float32) int16 {
	var scaled = math.Round(float64(input * 256))

	if scaled > MAX_INT16 {
		scaled = MAX_INT16
	} else if scaled < MIN_INT16 {
		scaled = MIN_INT16
	}

	return int16(scaled)
}

func floatToTextureCoord(input float32) int16 {
	var scaled = math.Round(float64(input * 32))

	if scaled > MAX_INT16 {
		scaled = MAX_INT16
	} else if scaled < MIN_INT16 {
		scaled = MIN_INT16
	}

	return int16(scaled)
}

func floatToNormalCoord(input float32) int8 {
	var scaled = math.Round(float64(input * 128))

	if scaled > MAX_INT8 {
		scaled = MAX_INT8
	} else if scaled < MIN_INT8 {
		scaled = MIN_INT8
	}

	return int8(scaled)
}

func writeNormalVertex(out *os.File, vertex *MeshVertex) {
	out.WriteString(fmt.Sprintf("    {{%d, %d, %d}, %d, {%d, %d}, {%d, %d, %d, %d}",
		floatToVertexCoord(vertex.x),
		floatToVertexCoord(vertex.y),
		floatToVertexCoord(vertex.z),
		0,
		floatToTextureCoord(vertex.s),
		floatToTextureCoord(vertex.t),

		floatToNormalCoord(vertex.nx),
		floatToNormalCoord(vertex.ny),
		floatToNormalCoord(vertex.nz),
		vertex.a,
	))
}

func writeColorVertex(out *os.File, vertex *MeshVertex) {
	out.WriteString(fmt.Sprintf("    {{%d, %d, %d}, %d, {%d, %d}, {%d, %d, %d, %d}",
		floatToVertexCoord(vertex.x),
		floatToVertexCoord(vertex.y),
		floatToVertexCoord(vertex.z),
		0,
		floatToTextureCoord(vertex.s),
		floatToTextureCoord(vertex.t),

		vertex.r,
		vertex.g,
		vertex.b,
		vertex.a,
	))
}

type VertexWriter func(out *os.File, vertex *MeshVertex)

func WriteMeshToC(out *os.File, mesh *Mesh) {

}
