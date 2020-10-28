package main

import "math"

type GraphVertex struct {
	Id               int
	AdjacentVertices []*GraphVertex
	AdjacentFaces    []*GraphFace
}

type GraphFace struct {
	Id               int
	AdjacentVertices []*GraphVertex
}

type GraphMesh struct {
	Vertices []GraphVertex
	Faces    []GraphFace
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

	for check, checkDistance := range distance {
		if checkDistance > currDistnace {
			currDistnace = checkDistance
			result = check
		}
	}

	return result
}

func (graph *GraphMesh) FindStartingVertex(sampleCount int) *GraphVertex {
	var hitCount = make(map[*GraphVertex]int)

	if sampleCount < len(graph.Vertices) {
		sampleCount = len(graph.Vertices)
	}

	var step = float64(len(graph.Vertices)) / float64(sampleCount)
	var currIndex float64 = 0

	for i := 0; i < sampleCount; i = i + 1 {
		var currIndexAsInt = int(math.Floor(currIndex))

		hit := graph.Vertices[currIndexAsInt].FindFurthestVertex()

		currHitCount, _ := hitCount[hit]
		hitCount[hit] = currHitCount + 1

		currIndex = currIndex + step
	}

	var result *GraphVertex = nil
	var currHitCount = 0

	for check, checkHit := range hitCount {
		if checkHit > currHitCount {
			currHitCount = checkHit
			result = check
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

func verticesForFaces(faces []*GraphFace) map[*GraphVertex]bool {
	var result = make(map[*GraphVertex]bool)

	for _, face := range faces {
		for _, vertex := range face.AdjacentVertices {
			result[vertex] = true
		}
	}

	return result
}

func subtractVertices(from map[*GraphVertex]bool, diff map[*GraphVertex]bool) map[*GraphVertex]bool {
	var result = make(map[*GraphVertex]bool)

	for vertex, _ := range from {
		_, hasDiff := diff[vertex]

		if !hasDiff {
			result[vertex] = true
		}

	}

	return result
}

func intersectVertices(from map[*GraphVertex]bool, inter map[*GraphVertex]bool) map[*GraphVertex]bool {
	var result = make(map[*GraphVertex]bool)

	for vertex, _ := range from {
		_, hasDiff := inter[vertex]

		if hasDiff {
			result[vertex] = true
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
			}
		}
	}

	return result
}

func generateDrawOrder(chunks [][]*GraphFace) *GraphDrawOrder {
	var reuseFront bool = true
	var reusedVertices map[*GraphVertex]bool = nil

	var currentSlots [VERTEX_BUFFER_SIZE]*GraphVertex

	var result GraphDrawOrder

	for index, chunk := range chunks {
		var reuseNext map[*GraphVertex]bool = nil
		var unique map[*GraphVertex]bool = verticesForFaces(chunk)

		if index+1 < len(chunks) {
			var next = verticesForFaces(chunks[index+1])

			reuseNext = intersectVertices(next, unique)
			unique = subtractVertices(unique, reuseNext)
			unique = subtractVertices(unique, reusedVertices)
		}

		unique = subtractVertices(unique, reusedVertices)

		var drawCommand GraphDrawCommand

		drawCommand.VertexStart = len(result.Vertices)
		drawCommand.VertexCount = len(reuseNext) + len(unique)

		if reuseFront {
			drawCommand.VertexBufferStart = 0
			var vertexIndex = drawCommand.VertexBufferStart

			for vertex, _ := range reuseNext {
				currentSlots[vertexIndex] = vertex
				result.Vertices = append(result.Vertices, vertex)
				vertexIndex = vertexIndex + 1
			}

			for vertex, _ := range unique {
				currentSlots[vertexIndex] = vertex
				result.Vertices = append(result.Vertices, vertex)
				vertexIndex = vertexIndex + 1
			}
		} else {
			drawCommand.VertexBufferStart = VERTEX_BUFFER_SIZE - drawCommand.VertexCount
			var vertexIndex = drawCommand.VertexBufferStart

			for vertex, _ := range unique {
				currentSlots[vertexIndex] = vertex
				result.Vertices = append(result.Vertices, vertex)
				vertexIndex = vertexIndex + 1
			}

			for vertex, _ := range reuseNext {
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
	start := graph.FindStartingVertex(10)
	var drawOrder = calculateFaceDrawOrder(start.AdjacentFaces[0], make(map[*GraphFace]bool), nil)
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

func GraphFromMesh(graph *Mesh) *GraphMesh {
	var result GraphMesh

	for index, _ := range graph.vertices {
		result.Vertices = append(result.Vertices, GraphVertex{
			index,
			nil,
			nil,
		})
	}

	for index, face := range graph.faces {

		result.Faces = append(result.Faces, GraphFace{
			index,
			nil,
		})

		var graphFace = &result.Faces[index]

		for _, vertexIndex := range face.indices {
			graphFace.AdjacentVertices = append(graphFace.AdjacentVertices, &result.Vertices[vertexIndex])
			result.Vertices[vertexIndex].AdjacentFaces = append(result.Vertices[vertexIndex].AdjacentFaces, graphFace)

			for _, otherVertexIndex := range face.indices {
				if vertexIndex != otherVertexIndex {
					addAdjacentVertex(&result.Vertices[vertexIndex], &result.Vertices[otherVertexIndex])
					addAdjacentVertex(&result.Vertices[otherVertexIndex], &result.Vertices[vertexIndex])
				}
			}
		}

	}

	return &result
}
