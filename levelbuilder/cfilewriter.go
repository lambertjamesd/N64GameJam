package main

import (
	"fmt"
	"log"
	"math"
	"os"
	"path/filepath"
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
	var scaled = math.Round(float64(input*1024)) - 16

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
	out.WriteString(fmt.Sprintf("    {{{%d, %d, %d}, %d, {%d, %d}, {%d, %d, %d, %d}}},\n",
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
	out.WriteString(fmt.Sprintf("    {{{%d, %d, %d}, %d, {%d, %d}, {%d, %d, %d, %d}}},\n",
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

func WriteMeshToC(out *os.File, mesh *Mesh, cName string, vertex VertexWriter) string {
	var graph = GraphFromMesh(mesh)
	var drawOrder = CalculateGraphDrawOrder(graph)

	out.WriteString("/*** Generated File ***/\n")
	out.WriteString(fmt.Sprintf("\nVtx %s_vtx[] = {\n", cName))

	for _, vtx := range drawOrder.Vertices {
		vertex(out, &mesh.vertices[vtx.Id])
	}

	out.WriteString("};\n")

	out.WriteString(fmt.Sprintf("\nGfx %s_tri[] = {\n", cName))

	for _, gfx := range drawOrder.DrawCommands {
		out.WriteString(fmt.Sprintf("    gsSPVertex(&%s_vtx[%d], %d, %d),\n", cName, gfx.VertexStart, gfx.VertexCount, gfx.VertexBufferStart))

		for i := 0; i < len(gfx.Triangles); i = i + 1 {
			var a = gfx.Triangles[i]
			out.WriteString(fmt.Sprintf("    gsSP1Triangle(%d, %d, %d, 0),\n", a[0], a[1], a[2]))
		}
	}

	out.WriteString("    gsSPEndDisplayList(),\n")

	out.WriteString("};\n")

	return cName + "_tri"
}

func WriteGeoFile(filename string, geoSources []string) {
	output, err := os.OpenFile(filename, os.O_CREATE|os.O_WRONLY|os.O_TRUNC, 0664)

	if err != nil {
		log.Fatal(err)
	}

	defer output.Close()

	output.WriteString("/*** Generated File ***/\n")
	output.WriteString("\n#include <ultra64.h>\n#include \"src/graphics/levelgraphics.h\"\n\n")

	for _, geoSource := range geoSources {
		output.WriteString(fmt.Sprintf("#include \"%s\"\n", geoSource))
	}
}

func WriteGeoHeader(filename string, geoNames []string) {
	output, err := os.OpenFile(filepath.Join(filepath.Dir(filename), "header.h"), os.O_CREATE|os.O_WRONLY|os.O_TRUNC, 0664)

	if err != nil {
		log.Fatal(err)
	}

	defer output.Close()

	output.WriteString("/*** Generated File ***/\n\n")

	for _, geoSource := range geoNames {
		output.WriteString(fmt.Sprintf("extern %s;\n", geoSource))
	}
}
