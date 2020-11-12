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
	var face = mesh.faces[faceIndex]
	var a = mesh.vertices[face.indices[0]].GetPos()
	var b = mesh.vertices[face.indices[1]].GetPos()
	var c = mesh.vertices[face.indices[2]].GetPos()

	var dirA = Vector3{
		b.X - a.X,
		b.Y - a.Y,
		b.Z - a.Z,
	}

	var dirB = Vector3{
		c.X - a.X,
		c.Y - a.Y,
		c.Z - a.Z,
	}

	var result = Cross3f(dirA, dirB)
	Normalize3f(&result.X, &result.Y, &result.Z)
	return result
}
