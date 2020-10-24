package main

type MeshVertex struct {
	x, y, z    float32
	nx, ny, nz float32
	s, t       float32
	r, g, b, a uint8
}

type MeshFace struct {
	indices []uint16
}

type Mesh struct {
	vertices []MeshVertex
	faces    []MeshFace
}
