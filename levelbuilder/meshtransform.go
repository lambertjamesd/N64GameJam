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
	input.x = float32(math.Round(float64(input.x)*WORLD_SCALE) / WORLD_SCALE)
	input.y = float32(math.Round(float64(input.y)*WORLD_SCALE) / WORLD_SCALE)
	input.z = float32(math.Round(float64(input.z)*WORLD_SCALE) / WORLD_SCALE)

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

		input.s = input.s - float32(s)
		input.t = input.t - float32(layer)
	}
}

func FullRightTileTransform(tileX int, tileY int, s int, t int, layer int) VertexTransform {
	return func(input *MeshVertex) {
		RotateVertexToRight(input)

		input.x = input.x + float32(tileX)*2
		input.y = input.y + float32(layer)*2
		input.z = input.z + float32(tileY)*2

		input.s = input.s - float32(t)
		input.t = input.t - float32(layer)
	}
}

func FullBackTileTransform(tileX int, tileY int, s int, t int, layer int) VertexTransform {
	return func(input *MeshVertex) {
		RotateVertexToBack(input)

		input.x = input.x + float32(tileX)*2
		input.y = input.y + float32(layer)*2
		input.z = input.z + float32(tileY)*2

		input.s = input.s + float32(s)
		input.t = input.t - float32(layer)
	}
}

func FullLeftTileTransform(tileX int, tileY int, s int, t int, layer int) VertexTransform {
	return func(input *MeshVertex) {
		RotateVertexToLeft(input)

		input.x = input.x + float32(tileX)*2
		input.y = input.y + float32(layer)*2
		input.z = input.z + float32(tileY)*2

		input.s = input.s + float32(t)
		input.t = input.t - float32(layer)
	}
}

func DoLinesCross(a0 *Vector3, a1 *Vector3, b0 *Vector3, b1 *Vector3) bool {
	var aDir = Sub3f(*a1, *a0)
	var bDir = Sub3f(*b1, *b0)

	var normal = Cross3f(aDir, bDir)

	if math.Abs(float64(Dot3f(normal, normal))) < 0.000001 {
		return false
	}

	var matrix Matrix3x3 = [3][3]float32{
		[3]float32{-aDir.X, bDir.X, normal.X},
		[3]float32{-aDir.Y, bDir.Y, normal.Y},
		[3]float32{-aDir.Z, bDir.Z, normal.Z},
	}

	inverse, _ := matrix.Inverse()

	var lerpValues = inverse.Mul3f(Sub3f(*a0, *b0))

	return lerpValues.X >= -0.00001 && lerpValues.X <= 1.000001 && lerpValues.Y >= -0.0000001 && lerpValues.Y <= 1.0000001
}

func CanRemoveTriangleAtVertex(mesh *Mesh, face *MeshFace, vertexIndex int, normal *Vector3) bool {
	var nextIndex = (vertexIndex + 1) % len(face.indices)
	var prevIndex = (vertexIndex + len(face.indices) - 1) % len(face.indices)

	var startPos = mesh.vertices[face.indices[vertexIndex]].GetPos()

	var nextVertex = mesh.vertices[face.indices[nextIndex]].GetPos()
	var prevVertex = mesh.vertices[face.indices[prevIndex]].GetPos()

	var subFaceNormal = Cross3f(Sub3f(nextVertex, startPos), Sub3f(prevVertex, startPos))

	if Dot3f(subFaceNormal, *normal) < 0 {
		return false
	}

	var newFace = MeshFace{[]uint32{
		face.indices[prevIndex],
		face.indices[vertexIndex],
		face.indices[nextIndex],
	}}

	var collisionFace = ConvertFaceToCollisionFace(newFace, mesh.vertices)

	for i := nextIndex; i != prevIndex; i = (i + 1) % len(face.indices) {
		var nextCheck = (i + 1) % len(face.indices)
		var curr = mesh.vertices[face.indices[i]].GetPos()
		var baryCoord = collisionFace.ToBaryCoord(curr)

		if baryCoord.X > 0 && baryCoord.Y > 0 && baryCoord.Z > 0 {
			return false
		}

		if i != nextIndex && i != prevIndex && nextCheck != nextIndex && nextCheck != prevIndex {

			var next = mesh.vertices[face.indices[nextCheck]].GetPos()

			if DoLinesCross(&prevVertex, &nextVertex, &curr, &next) {
				return false
			}
		}
	}

	return true
}

func RemoveTriangle(mesh *Mesh, face *MeshFace, index int, newFaces []MeshFace) []MeshFace {
	var nextIndex = (index + 1) % len(face.indices)
	var prevIndex = (index + len(face.indices) - 1) % len(face.indices)

	var startPos = mesh.vertices[face.indices[index]].GetPos()

	var nextVertex = mesh.vertices[face.indices[nextIndex]].GetPos()
	var prevVertex = mesh.vertices[face.indices[prevIndex]].GetPos()

	var subFaceNormal = Cross3f(Sub3f(nextVertex, startPos), Sub3f(prevVertex, startPos))

	if Dot3f(subFaceNormal, subFaceNormal) > 0.000001 {
		newFaces = append(newFaces, MeshFace{
			[]uint32{face.indices[prevIndex], face.indices[index], face.indices[nextIndex]},
		})
	}

	var nextFaces = make([]uint32, index)

	for i := 0; i < index; i++ {
		nextFaces[i] = face.indices[i]
	}

	nextFaces = append(nextFaces, face.indices[index+1:len(face.indices)]...)

	face.indices = nextFaces

	return newFaces
}

func TriangulateFace(mesh *Mesh, face *MeshFace, newFaces []MeshFace, normal *Vector3) []MeshFace {
	var currentIndex = 0

	for len(face.indices) > 3 {
		if CanRemoveTriangleAtVertex(mesh, face, currentIndex, normal) {
			newFaces = RemoveTriangle(mesh, face, currentIndex, newFaces)
			currentIndex = (currentIndex + 1) % len(face.indices)
		} else {
			currentIndex = (currentIndex + 1) % len(face.indices)
		}
	}

	newFaces = append(newFaces, *face)

	return newFaces
}

func Triangulate(mesh *Mesh) *Mesh {
	var result Mesh

	result.vertices = mesh.vertices

	for faceIndex, face := range mesh.faces {
		var normal = CalculateNormal(mesh, faceIndex)
		result.faces = TriangulateFace(mesh, &face, result.faces, &normal)
	}

	return &result
}
