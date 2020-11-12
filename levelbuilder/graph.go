package main

import "log"

type GraphVertex struct {
	Id               int
	AdjacentVertices []*GraphVertex
	AdjacentFaces    []*GraphFace
}

type GraphFace struct {
	Id               int
	AdjacentVertices []*GraphVertex
	AdjacentEdges    []*GraphEdge
}

type GraphEdge struct {
	Id               int
	AdjacentFaces    []*GraphFace
	AdjacentVertices [2]*GraphVertex
}

type GraphMesh struct {
	Vertices []*GraphVertex
	Faces    []*GraphFace
	Edges    []*GraphEdge
}

type GraphDrawCommand struct {
	VertexStart       int
	VertexBufferStart int
	VertexCount       int
	Triangles         [][3]int
}

type GraphDrawOrder struct {
	Vertices     []*GraphVertex
	DrawCommands []*GraphDrawCommand
}

func markAdjacentVertices(vertex *GraphVertex, into map[int]int, with int) {
	_, alreadyHas := into[vertex.Id]

	if !alreadyHas {
		into[vertex.Id] = with

		for _, adj := range vertex.AdjacentVertices {
			markAdjacentVertices(adj, into, with)
		}
	}
}

func findMeshGroups(graph *GraphMesh) map[int]int {
	var idx = 0

	var result = make(map[int]int, len(graph.Vertices))

	for _, vertex := range graph.Vertices {
		_, alreadyHas := result[vertex.Id]

		if !alreadyHas {
			markAdjacentVertices(vertex, result, idx)
			idx = idx + 1
		}
	}

	return result
}

const VERTEX_BUFFER_SIZE = 16

func (vertex *GraphVertex) FindFurthestVertex() *GraphVertex {
	var currentGroup = make(map[*GraphVertex]bool)
	var distance = make(map[*GraphVertex]int)

	distance[vertex] = 0
	currentGroup[vertex] = true

	for len(currentGroup) > 0 {
		var nextGroup = make(map[*GraphVertex]bool)

		for vertexCheck, _ := range currentGroup {
			currDistance, _ := distance[vertexCheck]

			for _, adjVertex := range vertexCheck.AdjacentVertices {
				_, alreadyChecked := distance[adjVertex]

				if !alreadyChecked {
					nextGroup[adjVertex] = true
					distance[adjVertex] = currDistance + 1
				}
			}
		}

		currentGroup = nextGroup
	}

	var result *GraphVertex = nil
	var currDistnace = 0
	var vertexId = 0

	for check, checkDistance := range distance {
		if checkDistance > currDistnace || checkDistance == currDistnace && check.Id < vertexId {
			currDistnace = checkDistance
			result = check
			vertexId = check.Id
		}
	}

	return result
}

func (graph *GraphMesh) FindStartingVertices() []*GraphVertex {
	var grouping = findMeshGroups(graph)

	var hitCount = make(map[*GraphVertex]int)
	var groupCount = 0

	for _, vertex := range graph.Vertices {
		hit := vertex.FindFurthestVertex()

		if hit != nil {
			currHitCount, _ := hitCount[hit]
			hitCount[hit] = currHitCount + 1

			groupId, _ := grouping[vertex.Id]

			if groupId > groupCount {
				groupCount = groupId
			}
		}
	}

	var result = make([]*GraphVertex, groupCount+1)
	var currHitCount = make([]int, groupCount+1)
	var checkId = make([]int, groupCount+1)

	for check, checkHit := range hitCount {
		groupId, _ := grouping[check.Id]

		if checkHit > currHitCount[groupId] || checkHit == currHitCount[groupId] && check.Id < checkId[groupId] {
			currHitCount[groupId] = checkHit
			checkId[groupId] = check.Id
			result[groupId] = check
		}
	}

	return result
}

func calculateFaceDrawOrder(current *GraphFace, checked map[*GraphFace]bool, result []*GraphFace) []*GraphFace {
	_, has := checked[current]

	if !has {
		result = append(result, current)
		checked[current] = true

		for _, vertex := range current.AdjacentVertices {
			for _, adjFace := range vertex.AdjacentFaces {
				result = calculateFaceDrawOrder(adjFace, checked, result)
			}
		}
	}

	return result
}

func breakupGraphFaceChunks(faces []*GraphFace) [][]*GraphFace {
	var result [][]*GraphFace = [][]*GraphFace{nil}
	var currentChunkIndex int = 0
	var usedVertices map[*GraphVertex]bool = make(map[*GraphVertex]bool)

	for _, face := range faces {
		for _, vertex := range face.AdjacentVertices {
			usedVertices[vertex] = true
		}

		if len(usedVertices) > VERTEX_BUFFER_SIZE {
			currentChunkIndex = currentChunkIndex + 1
			result = append(result, nil)
		}

		var currentChunk = result[currentChunkIndex]

		if currentChunk == nil {
			usedVertices = make(map[*GraphVertex]bool)
			currentChunk = append(currentChunk, face)

			for _, vertex := range face.AdjacentVertices {
				usedVertices[vertex] = true
			}
		} else {
			currentChunk = append(currentChunk, face)
		}

		result[currentChunkIndex] = currentChunk
	}

	return result
}

func verticesForFaces(faces []*GraphFace) []*GraphVertex {
	var didAdd = make(map[*GraphVertex]bool)
	var result []*GraphVertex = nil

	for _, face := range faces {
		for _, vertex := range face.AdjacentVertices {
			_, added := didAdd[vertex]

			if !added {
				didAdd[vertex] = true
				result = append(result, vertex)
			}
		}
	}

	return result
}

func subtractVertices(from []*GraphVertex, diff []*GraphVertex) []*GraphVertex {
	var result []*GraphVertex = nil

	for _, vertex := range from {
		var otherHas = false

		for _, otherCheck := range diff {
			if otherCheck == vertex {
				otherHas = true
				break
			}
		}

		if !otherHas {
			result = append(result, vertex)
		}

	}

	return result
}

func intersectVertices(from []*GraphVertex, inter []*GraphVertex) []*GraphVertex {
	var result []*GraphVertex = nil

	for _, vertex := range from {
		var otherHas = false

		for _, otherCheck := range inter {
			if otherCheck == vertex {
				otherHas = true
				break
			}
		}

		if otherHas {
			result = append(result, vertex)
		}

	}

	return result
}

func extractFaceIndices(from *GraphFace, vertexSlots [VERTEX_BUFFER_SIZE]*GraphVertex) [3]int {
	var result [3]int

	for i := 0; i < 3; i = i + 1 {
		for vertexIndex := 0; vertexIndex < VERTEX_BUFFER_SIZE; vertexIndex = vertexIndex + 1 {
			if vertexSlots[vertexIndex] == from.AdjacentVertices[i] {
				result[i] = vertexIndex
				break
			} else if vertexIndex+1 == VERTEX_BUFFER_SIZE {
				log.Fatal("Couldn't find vertex in vertex buffer")
			}
		}
	}

	return result
}

func generateDrawOrder(chunks [][]*GraphFace) *GraphDrawOrder {
	var reuseFront bool = true
	var reusedVertices []*GraphVertex = nil

	var currentSlots [VERTEX_BUFFER_SIZE]*GraphVertex

	var result GraphDrawOrder

	for index, chunk := range chunks {
		var reuseNext []*GraphVertex = nil
		var unique []*GraphVertex = verticesForFaces(chunk)

		unique = subtractVertices(unique, reusedVertices)

		if index+1 < len(chunks) {
			var next = verticesForFaces(chunks[index+1])

			reuseNext = intersectVertices(next, unique)

			unique = subtractVertices(unique, reuseNext)
		}

		var drawCommand GraphDrawCommand

		drawCommand.VertexStart = len(result.Vertices)
		drawCommand.VertexCount = len(reuseNext) + len(unique)

		if reuseFront {
			drawCommand.VertexBufferStart = 0
			var vertexIndex = drawCommand.VertexBufferStart

			for _, vertex := range reuseNext {
				currentSlots[vertexIndex] = vertex
				result.Vertices = append(result.Vertices, vertex)
				vertexIndex = vertexIndex + 1
			}

			for _, vertex := range unique {
				currentSlots[vertexIndex] = vertex
				result.Vertices = append(result.Vertices, vertex)
				vertexIndex = vertexIndex + 1
			}
		} else {
			drawCommand.VertexBufferStart = VERTEX_BUFFER_SIZE - drawCommand.VertexCount
			var vertexIndex = drawCommand.VertexBufferStart

			for _, vertex := range unique {
				currentSlots[vertexIndex] = vertex
				result.Vertices = append(result.Vertices, vertex)
				vertexIndex = vertexIndex + 1
			}

			for _, vertex := range reuseNext {
				currentSlots[vertexIndex] = vertex
				result.Vertices = append(result.Vertices, vertex)
				vertexIndex = vertexIndex + 1
			}
		}

		for _, face := range chunk {
			drawCommand.Triangles = append(drawCommand.Triangles, extractFaceIndices(face, currentSlots))
		}

		result.DrawCommands = append(result.DrawCommands, &drawCommand)

		reuseFront = !reuseFront
		reusedVertices = reuseNext
	}

	return &result
}

func CalculateGraphDrawOrder(graph *GraphMesh) *GraphDrawOrder {
	startingVertices := graph.FindStartingVertices()
	checked := make(map[*GraphFace]bool)

	var drawOrder []*GraphFace = nil

	for _, start := range startingVertices {
		drawOrder = calculateFaceDrawOrder(start.AdjacentFaces[0], checked, drawOrder)
	}

	var drawChunks = breakupGraphFaceChunks(drawOrder)
	return generateDrawOrder(drawChunks)
}

func addAdjacentVertex(into *GraphVertex, adj *GraphVertex) {
	for _, other := range into.AdjacentVertices {
		if other == adj {
			return
		}
	}

	into.AdjacentVertices = append(into.AdjacentVertices, adj)
}

func addEdge(mesh *GraphMesh, a *GraphVertex, b *GraphVertex, face *GraphFace) {
	if a.Id > b.Id {
		a, b = b, a
	}

	var edgeId = (a.Id << 16) | (b.Id & 0xffff)

	for _, edge := range mesh.Edges {
		if edge.Id == edgeId {
			edge.AdjacentFaces = append(edge.AdjacentFaces, face)
			face.AdjacentEdges = append(face.AdjacentEdges, edge)
			return
		}
	}

	var result = &GraphEdge{
		edgeId,
		[]*GraphFace{face},
		[2]*GraphVertex{a, b},
	}

	mesh.Edges = append(mesh.Edges, result)
	face.AdjacentEdges = append(face.AdjacentEdges, result)
}

func GraphFromMesh(graph *Mesh) *GraphMesh {
	var result GraphMesh

	for index, _ := range graph.vertices {
		result.Vertices = append(result.Vertices, &GraphVertex{
			index,
			nil,
			nil,
		})
	}

	for index, face := range graph.faces {
		result.Faces = append(result.Faces, &GraphFace{
			index,
			nil,
			nil,
		})

		var graphFace = result.Faces[index]

		for i, vertexIndex := range face.indices {
			graphFace.AdjacentVertices = append(graphFace.AdjacentVertices, result.Vertices[vertexIndex])
			result.Vertices[vertexIndex].AdjacentFaces = append(result.Vertices[vertexIndex].AdjacentFaces, graphFace)

			var nextIndex = face.indices[(i+1)%len(face.indices)]

			addEdge(&result, result.Vertices[vertexIndex], result.Vertices[nextIndex], graphFace)
			addAdjacentVertex(result.Vertices[vertexIndex], result.Vertices[nextIndex])
			addAdjacentVertex(result.Vertices[nextIndex], result.Vertices[vertexIndex])
		}

	}

	return &result
}

func MeshFromGraph(graph *GraphMesh, originalMesh *Mesh) *Mesh {
	var usedVerts = make(map[int]bool)

	for _, face := range graph.Faces {
		if len(face.AdjacentVertices) > 0 {
			for _, vert := range face.AdjacentVertices {
				usedVerts[vert.Id] = true
			}
		}
	}

	var idMapping = make(map[int]int)
	var result Mesh

	for _, vertex := range graph.Vertices {
		_, used := usedVerts[vertex.Id]

		if used {
			idMapping[vertex.Id] = len(result.vertices)
			result.vertices = append(result.vertices, originalMesh.vertices[vertex.Id])
		}
	}

	for _, face := range graph.Faces {
		if len(face.AdjacentVertices) > 0 {
			var newFace MeshFace

			for _, vertex := range face.AdjacentVertices {
				newId, _ := idMapping[vertex.Id]

				newFace.indices = append(newFace.indices, uint32(newId))
			}

			result.faces = append(result.faces, newFace)
		}
	}

	return &result
}
