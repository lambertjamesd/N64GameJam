package main

type VertexTransform func(vertex *MeshVertex)

func TransformMesh(input *Mesh, transform VertexTransform) *Mesh {
	var result Mesh

	input.vertices = make([]MeshVertex, len(input.vertices))

	for index, vertex := range input.vertices {
		input.vertices[index] = vertex
		transform(&input.vertices[index])
	}

	result.faces = input.faces

	return &result
}

func RotateVertexToFront(input *MeshVertex) {
	input.y, input.z = input.z-1, -input.y-1
}

func RotateVertexCCW(input *MeshVertex) {
	input.x, input.z = -input.z, input.x
}

func RotateVertex180(input *MeshVertex) {
	input.x, input.z = -input.x, -input.z
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
