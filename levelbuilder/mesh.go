package main

type MeshVertex struct {
	x, y, z    float32
	nx, ny, nz float32
	s, t       float32
	r, g, b, a uint8
}

type MeshFace struct {
	indices []uint32
}

type Mesh struct {
	vertices []MeshVertex
	faces    []MeshFace
}

func (vertex *MeshVertex) GetPos() Vector3 {
	return Vector3{vertex.x, vertex.y, vertex.z}
}

func CalculateNormal(mesh *Mesh, faceIndex int) Vector3 {
	var result = Vector3{}
	var face = mesh.faces[faceIndex]
	var origin = mesh.vertices[face.indices[0]].GetPos()

	for i := 1; i+1 < len(face.indices); i++ {
		var p1 = mesh.vertices[face.indices[i]].GetPos()
		var p2 = mesh.vertices[face.indices[i+1]].GetPos()

		var dirA = Vector3{
			p1.X - origin.X,
			p1.Y - origin.Y,
			p1.Z - origin.Z,
		}

		var dirB = Vector3{
			p2.X - origin.X,
			p2.Y - origin.Y,
			p2.Z - origin.Z,
		}

		var faceCross = Cross3f(dirA, dirB)

		result.X = result.X + faceCross.X
		result.Y = result.Y + faceCross.Y
		result.Z = result.Z + faceCross.Z
	}

	Normalize3f(&result.X, &result.Y, &result.Z)
	return result
}
