package main

import "math"

type VertexTransform func(vertex *MeshVertex)

func TransformMesh(input *Mesh, transform VertexTransform) *Mesh {
	var result Mesh

	result.vertices = make([]MeshVertex, len(input.vertices))

	for index, vertex := range input.vertices {
		result.vertices[index] = vertex
		transform(&result.vertices[index])
	}

	result.faces = input.faces

	return &result
}

func RoundToN64(input *MeshVertex) {
	input.x = float32(math.Round(float64(input.x)*256) / 256)
	input.y = float32(math.Round(float64(input.y)*256) / 256)
	input.z = float32(math.Round(float64(input.z)*256) / 256)

	input.nx = float32(math.Round(float64(input.nx)*128) / 128)
	input.ny = float32(math.Round(float64(input.ny)*128) / 128)
	input.nz = float32(math.Round(float64(input.nz)*128) / 128)

	input.s = float32(math.Round(float64(input.s)*1024) / 1024)
	input.t = float32(math.Round(float64(input.t)*1024) / 1024)
}

func FlipZ(input *Mesh) *Mesh {
	var result Mesh

	result.vertices = make([]MeshVertex, len(input.vertices))

	for index, vertex := range input.vertices {
		result.vertices[index] = vertex
		result.vertices[index].z = -result.vertices[index].z
	}

	for _, face := range input.faces {
		var indices = make([]uint32, len(face.indices))

		for i, index := range face.indices {
			indices[len(face.indices)-1-i] = index
		}

		result.faces = append(result.faces, MeshFace{indices})
	}

	return &result
}

func RotateVertexToFront(input *MeshVertex) {
	input.y, input.z = input.z-1, -input.y-1
	input.ny, input.nz = input.nz, -input.ny
}

func RotateVertexCCW(input *MeshVertex) {
	input.x, input.z = -input.z, input.x
	input.nx, input.nz = -input.nz, input.nx
}

func RotateVertexCW(input *MeshVertex) {
	input.x, input.z = input.z, -input.x
	input.nx, input.nz = input.nz, -input.nx
}

func RotateVertex180(input *MeshVertex) {
	input.x, input.z = -input.x, -input.z
	input.nx, input.nz = -input.nx, -input.nz
}

func RotateVertexToRight(input *MeshVertex) {
	RotateVertexToFront(input)
	RotateVertexCCW(input)
}

func RotateVertexToBack(input *MeshVertex) {
	RotateVertexToFront(input)
	RotateVertex180(input)
}

func RotateVertexToLeft(input *MeshVertex) {
	RotateVertexToFront(input)
	RotateVertex180(input)
	RotateVertexCCW(input)
}

func TranslateTextureCoords(ds float32, dt float32) VertexTransform {
	return func(input *MeshVertex) {
		input.s = input.s + ds
		input.t = input.t + dt
	}
}

func TranslateMesh(dx float32, dy float32, dz float32) VertexTransform {
	return func(input *MeshVertex) {
		input.x = input.x + dx
		input.y = input.y + dy
		input.z = input.z + dz
	}
}

func FullTopTileTransform(tileX int, tileY int, s int, t int, layer int, rotation int) VertexTransform {
	return func(input *MeshVertex) {
		for i := 0; i < rotation; i = i + 1 {
			RotateVertexCW(input)
		}

		input.x = input.x + float32(tileX)*2
		input.y = input.y + float32(layer)*2
		input.z = input.z + float32(tileY)*2

		input.s = input.s + float32(s)
		input.t = input.t + float32(t)
	}
}

func FullFrontTileTransform(tileX int, tileY int, s int, t int, layer int) VertexTransform {
	return func(input *MeshVertex) {
		RotateVertexToFront(input)

		input.x = input.x + float32(tileX)*2
		input.y = input.y + float32(layer)*2
		input.z = input.z + float32(tileY)*2

		input.s = input.s + float32(s)
		input.t = input.t + float32(layer)
	}
}

func FullRightTileTransform(tileX int, tileY int, s int, t int, layer int) VertexTransform {
	return func(input *MeshVertex) {
		RotateVertexToRight(input)

		input.x = input.x + float32(tileX)*2
		input.y = input.y + float32(layer)*2
		input.z = input.z + float32(tileY)*2

		input.s = input.s + float32(t)
		input.t = input.t + float32(layer)
	}
}

func FullBackTileTransform(tileX int, tileY int, s int, t int, layer int) VertexTransform {
	return func(input *MeshVertex) {
		RotateVertexToBack(input)

		input.x = input.x + float32(tileX)*2
		input.y = input.y + float32(layer)*2
		input.z = input.z + float32(tileY)*2

		input.s = input.s - float32(s)
		input.t = input.t + float32(layer)
	}
}

func FullLeftTileTransform(tileX int, tileY int, s int, t int, layer int) VertexTransform {
	return func(input *MeshVertex) {
		RotateVertexToLeft(input)

		input.x = input.x + float32(tileX)*2
		input.y = input.y + float32(layer)*2
		input.z = input.z + float32(tileY)*2

		input.s = input.s - float32(t)
		input.t = input.t + float32(layer)
	}
}

func Triangulate(mesh *Mesh) *Mesh {
	var result Mesh

	result.vertices = mesh.vertices

	for _, face := range mesh.faces {
		for i := 2; i < len(face.indices); i = i + 1 {
			result.faces = append(result.faces, MeshFace{[]uint32{
				face.indices[0],
				face.indices[i-1],
				face.indices[i],
			}})
		}
	}

	return &result
}
