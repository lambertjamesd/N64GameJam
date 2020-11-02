package main

import (
	"fmt"
	"io/ioutil"
	"log"
	"math"
	"os"
)

const (
	COLLISION_TYPE_TRI  = 0
	COLLISION_TYPE_QUAD = 1
)

type CollisionFace struct {
	planeA, planeB, planeC, planeD float32
	origin                         Vector3
	d1                             Vector3
	d2                             Vector3
	faceType                       int
}

type CollisionEdge struct {
	origin Vector3
	dir    Vector3
	length float32
}

type CollisionPoint struct {
	origin Vector3
}

type CollisionMesh struct {
	faces  []CollisionFace
	edges  []CollisionEdge
	points []CollisionPoint
}

func VecFromVert(vertex *MeshVertex) Vector3 {
	return Vector3{
		vertex.x,
		vertex.y,
		vertex.z,
	}
}

func ConvertFaceToCollisionFace(face MeshFace, verts []MeshVertex) CollisionFace {
	var result CollisionFace

	result.origin = VecFromVert(&verts[face.indices[0]])
	var a = VecFromVert(&verts[face.indices[1]])
	var b = VecFromVert(&verts[face.indices[len(face.indices)-1]])

	var edge0 = Sub3f(a, result.origin)
	var edge1 = Sub3f(b, result.origin)

	var edgeDot = Dot3f(edge0, edge1)
	var edgeLength0 = Dot3f(edge0, edge0)
	var edgeLength1 = Dot3f(edge1, edge1)

	var scalar = 1 / (edgeLength0*edgeLength1 - edgeDot*edgeDot)

	result.d1 = Scale3F(Sub3f(Scale3F(edge0, edgeLength1), Scale3F(edge1, edgeDot)), scalar)
	result.d2 = Scale3F(Sub3f(Scale3F(edge1, edgeLength0), Scale3F(edge0, edgeDot)), scalar)

	if len(face.indices) == 3 {
		result.faceType = COLLISION_TYPE_TRI
	} else {
		result.faceType = COLLISION_TYPE_QUAD
	}

	var normal = Cross3f(edge0, edge1)
	Normalize3f(&normal.X, &normal.Y, &normal.Z)

	result.planeA = normal.X
	result.planeB = normal.Y
	result.planeC = normal.Z
	result.planeD = -Dot3f(normal, result.origin)

	return result
}

func EdgeFromPoints(a Vector3, b Vector3) CollisionEdge {
	var result CollisionEdge

	result.origin = a
	result.dir = Sub3f(b, a)
	result.length = float32(math.Sqrt(float64(Dot3f(result.dir, result.dir))))
	result.dir = Scale3F(result.dir, 1/result.length)

	return result
}

func CollisionMeshFromMesh(mesh *Mesh) CollisionMesh {
	var result CollisionMesh

	var edged = make(map[uint32]bool)

	for _, face := range mesh.faces {
		result.faces = append(result.faces, ConvertFaceToCollisionFace(face, mesh.vertices))

		for faceIndex, vertIndex := range face.indices {
			var nextIndex = face.indices[(faceIndex+1)%len(face.indices)]

			var key uint32

			if vertIndex < nextIndex {
				key = ((vertIndex & 0xFFFF) << 16) | (nextIndex & 0xFFFF)
			} else {
				key = ((nextIndex & 0xFFFF) << 16) | (vertIndex & 0xFFFF)
			}

			_, hasEdge := edged[key]

			if !hasEdge {
				edged[key] = true

				result.edges = append(result.edges, EdgeFromPoints(
					VecFromVert(&mesh.vertices[vertIndex]),
					VecFromVert(&mesh.vertices[nextIndex]),
				))
			}
		}
	}

	for _, vert := range mesh.vertices {
		result.points = append(result.points, CollisionPoint{
			VecFromVert(&vert),
		})
	}

	return result
}

func WriteOutCollisionMesh(file *os.File, namePrefix string, mesh *Mesh) error {
	var collisionMesh = CollisionMeshFromMesh(mesh)

	file.WriteString(fmt.Sprintf("\nstruct CollisionFace _%s_faces[] = {\n", namePrefix))

	for _, face := range collisionMesh.faces {
		file.WriteString(fmt.Sprintf(
			`    {
        {%.6f, %.6f, %.6f, %.6f},
        {%.6f, %.6f, %.6f},
        {%.6f, %.6f, %.6f},
        {%.6f, %.6f, %.6f},
        %d,
    },
`, face.planeA, face.planeB, face.planeC, face.planeD,
			face.origin.X, face.origin.Y, face.origin.Z,
			face.d1.X, face.d1.Y, face.d1.Z,
			face.d2.X, face.d2.Y, face.d2.Z,
			face.faceType,
		))
	}

	file.WriteString("};\n")

	file.WriteString(fmt.Sprintf("\nstruct CollisionEdge _%s_edges[] = {\n", namePrefix))

	for _, edge := range collisionMesh.edges {
		file.WriteString(fmt.Sprintf(
			`    {
        {%.6f, %.6f, %.6f},
        {%.6f, %.6f, %.6f},
        %.6f,
    },
`, edge.origin.X, edge.origin.Y, edge.origin.Z,
			edge.dir.X, edge.dir.Y, edge.dir.Z,
			edge.length,
		))
	}

	file.WriteString("};\n")

	file.WriteString(fmt.Sprintf("\nstruct CollisionPoint _%s_points[] = {\n", namePrefix))

	for _, point := range collisionMesh.points {
		file.WriteString(fmt.Sprintf(
			`    {
        {%.6f, %.6f, %.6f},
    },
`, point.origin.X, point.origin.Y, point.origin.Z,
		))
	}

	file.WriteString("};\n")

	file.WriteString(fmt.Sprintf(`
struct CollisionCollider _%s_collider = {
	ColliderTypeMesh,
	0,
	.mesh = {
		_%s_faces, %d,
		_%s_edges, %d,
		_%s_points, %d,
	},
};
`, namePrefix,
		namePrefix, len(collisionMesh.faces),
		namePrefix, len(collisionMesh.edges),
		namePrefix, len(collisionMesh.points),
	))

	return nil
}

func ConvertCollisionMesh(prefix string, inFile string, outFile string) {
	content, err := ioutil.ReadFile(inFile)

	if err != nil {
		log.Fatal(err)
	}

	mesh, err := ParsePly(string(content))

	mesh = TransformMesh(mesh, func(vertex *MeshVertex) {
		vertex.nx = 0
		vertex.ny = 0
		vertex.nz = 0
		vertex.s = 0
		vertex.t = 0
		vertex.r = 0
		vertex.g = 0
		vertex.b = 0
		vertex.a = 0
	})

	mesh = RemoveDuplicates(mesh)

	output, err := os.OpenFile(outFile, os.O_CREATE|os.O_WRONLY|os.O_TRUNC, 0664)

	if err != nil {
		log.Fatal(err)
	}

	defer output.Close()

	WriteOutCollisionMesh(output, prefix, mesh)
}
