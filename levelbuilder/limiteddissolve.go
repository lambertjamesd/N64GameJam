package main

func findEdgeIndicesForFace(face *GraphFace, edge *GraphEdge) (int, int) {
	for index, edgeCheck := range face.AdjacentEdges {
		if edge == edgeCheck {
			return index, (index + 1) % len(face.AdjacentEdges)
		}
	}

	return -1, -1
}

func dissolveEdge(mesh *GraphMesh, index int) {
	if index < len(mesh.Edges) {
		var edge = mesh.Edges[index]

		var faceA = edge.AdjacentFaces[0]
		var faceB = edge.AdjacentFaces[1]

		firstFaceStart, firstFaceEnd := findEdgeIndicesForFace(faceA, edge)
		secondFaceStart, secondFaceEnd := findEdgeIndicesForFace(faceB, edge)

		var newVertices = make([]*GraphVertex, len(faceA.AdjacentVertices)+len(faceB.AdjacentVertices)-2)
		var newEdges = make([]*GraphEdge, len(faceA.AdjacentEdges)+len(faceB.AdjacentEdges)-2)

		var outIndex = 0

		for i := 0; i < firstFaceStart; i++ {
			newVertices[outIndex] = faceA.AdjacentVertices[i]
			newEdges[outIndex] = faceA.AdjacentEdges[i]
			outIndex++
		}

		if secondFaceEnd == 0 {
			for i := 0; i < len(faceB.AdjacentVertices)-1; i++ {
				newVertices[outIndex] = faceB.AdjacentVertices[i]
				newEdges[outIndex] = faceB.AdjacentEdges[i]
				outIndex++
			}
		} else {
			for i := secondFaceEnd; i < len(faceB.AdjacentVertices); i++ {
				newVertices[outIndex] = faceB.AdjacentVertices[i]
				newEdges[outIndex] = faceB.AdjacentEdges[i]
				outIndex++
			}

			for i := 0; i < secondFaceStart; i++ {
				newVertices[outIndex] = faceB.AdjacentVertices[i]
				newEdges[outIndex] = faceB.AdjacentEdges[i]
				outIndex++
			}
		}

		if firstFaceEnd != 0 {
			for i := firstFaceEnd; i < len(faceA.AdjacentVertices); i++ {
				newVertices[outIndex] = faceA.AdjacentVertices[i]
				newEdges[outIndex] = faceA.AdjacentEdges[i]
				outIndex++
			}
		}

		// update references from edge to face
		for _, faceBedge := range faceB.AdjacentEdges {
			for index, _ := range faceBedge.AdjacentFaces {
				if faceBedge.AdjacentFaces[index] == faceB {
					faceBedge.AdjacentFaces[index] = faceA
				}
			}
		}

		// update references from vertex to face
		for _, faceBVertex := range faceB.AdjacentVertices {
			for index, _ := range faceBVertex.AdjacentFaces {
				if faceBVertex.AdjacentFaces[index] == faceB {
					faceBVertex.AdjacentFaces[index] = faceA
				}
			}
		}

		var edgeToVertexIndex []int = nil

		for index, edge := range newEdges {
			if edge.AdjacentVertices[0] == newVertices[index] {
				edgeToVertexIndex = append(edgeToVertexIndex, 0)
			} else {
				edgeToVertexIndex = append(edgeToVertexIndex, 1)
			}
		}

		var removedEdges []*GraphEdge = nil
		var filteredEdges []*GraphEdge = nil
		var filteredVertices []*GraphVertex = nil

		var hasVertex = make(map[int]bool)

		for index, edge := range newEdges {
			if len(edge.AdjacentFaces) == 2 && edge.AdjacentFaces[0] == faceA && edge.AdjacentFaces[1] == faceA {
				removedEdges = append(removedEdges, edge)
			} else {
				filteredVertices = append(filteredVertices, edge.AdjacentVertices[edgeToVertexIndex[index]])
				filteredEdges = append(filteredEdges, edge)
				hasVertex[edge.AdjacentVertices[0].Id] = true
				hasVertex[edge.AdjacentVertices[1].Id] = true
			}
		}

		// combined face
		faceA.AdjacentVertices = filteredVertices
		faceA.AdjacentEdges = filteredEdges

		for _, removedEdge := range removedEdges {
			removedEdge.AdjacentFaces = nil
		}

		faceB.AdjacentEdges = nil
		faceB.AdjacentVertices = nil

		edge.AdjacentFaces = nil
	}
}

func shouldDissolveEdge(edge *GraphEdge, normals map[int]Vector3) bool {
	if len(edge.AdjacentFaces) != 2 {
		return false
	}

	if edge.AdjacentFaces[0] == edge.AdjacentFaces[1] {
		return false
	}

	nA, ok := normals[edge.AdjacentFaces[0].Id]

	if !ok {
		return false
	}

	nB, ok := normals[edge.AdjacentFaces[1].Id]

	if !ok {
		return false
	}

	return Dot3f(nA, nB) > 0.999
}

func dissolveEdges(mesh *GraphMesh, normals map[int]Vector3) {
	for i := 0; i < len(mesh.Edges); i++ {
		if shouldDissolveEdge(mesh.Edges[i], normals) {
			dissolveEdge(mesh, i)
		}
	}
}

func dissolveVertex(face *GraphFace, index int) {
	var next = make([]*GraphVertex, len(face.AdjacentVertices)-1)

	for i := 0; i < index; i++ {
		next[i] = face.AdjacentVertices[i]
	}

	for i := index + 1; i < len(face.AdjacentVertices); i++ {
		next[i-1] = face.AdjacentVertices[i]
	}

	face.AdjacentVertices = next
}

func shouldDissolveVertex(face *GraphFace, index int, origMesh *Mesh) bool {
	var curr = origMesh.vertices[face.AdjacentVertices[index].Id]
	var next = origMesh.vertices[face.AdjacentVertices[(index+1)%len(face.AdjacentVertices)].Id]
	var prev = origMesh.vertices[face.AdjacentVertices[(index-1+len(face.AdjacentVertices))%len(face.AdjacentVertices)].Id]

	var dirBack Vector3 = Sub3f(curr.GetPos(), prev.GetPos())
	var dirForward Vector3 = Sub3f(next.GetPos(), curr.GetPos())

	var check = Cross3f(dirBack, dirForward)

	return Dot3f(check, check) < 0.000001
}

func dissolveVertices(mesh *GraphMesh, origMesh *Mesh) {
	for _, face := range mesh.Faces {
		for index := 0; index < len(face.AdjacentVertices); {
			if shouldDissolveVertex(face, index, origMesh) {
				dissolveVertex(face, index)
			} else {
				index++
			}
		}
	}
}

func LimitedDissolve(mesh *Mesh) *Mesh {
	var graph = GraphFromMesh(mesh)
	var normals = make(map[int]Vector3)

	for index, _ := range mesh.faces {
		normals[index] = CalculateNormal(mesh, index)
	}

	dissolveEdges(graph, normals)
	dissolveVertices(graph, mesh)

	return MeshFromGraph(graph, mesh)
}
