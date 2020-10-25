package main

func offsetInices(indices []uint32, offset uint32) []uint32 {
	var result []uint32 = nil

	for i := 0; i < len(indices); i = i + 1 {
		result = append(result, indices[i]+offset)
	}

	return result
}

func JoinMesh(meshes []*Mesh) *Mesh {
	var result Mesh

	var indexOffset uint32 = 0

	for _, mesh := range meshes {
		result.vertices = append(result.vertices, mesh.vertices...)

		for _, face := range mesh.faces {
			result.faces = append(result.faces, MeshFace{offsetInices(face.indices, indexOffset)})
		}

		indexOffset = indexOffset + uint32(len(mesh.vertices))
	}

	return &result
}
