package main

func RemoveDuplicates(mesh *Mesh) *Mesh {
	var result Mesh

	var vertexIndexMapping = make(map[int]int)
	var existingVertices = make(map[MeshVertex]int)

	for index, vertex := range mesh.vertices {
		existingIndex, hasExisting := existingVertices[vertex]

		if hasExisting {
			vertexIndexMapping[index] = existingIndex
		} else {
			vertexIndexMapping[index] = len(result.vertices)
			existingVertices[vertex] = len(result.vertices)
			result.vertices = append(result.vertices, vertex)
		}
	}

	for _, face := range mesh.faces {
		var indices []uint32 = nil

		for _, index := range face.indices {
			mappedIndex, _ := vertexIndexMapping[int(index)]
			indices = append(indices, uint32(mappedIndex))
		}

		result.faces = append(result.faces, MeshFace{indices})
	}

	return &result
}
