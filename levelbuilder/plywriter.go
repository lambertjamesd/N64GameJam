package main

import (
	"fmt"
	"os"
)

func (mesh *Mesh) WritePly(output *os.File) error {
	_, err := output.WriteString(fmt.Sprintf(
		`ply
format ascii 1.0
element vertex %d
property float x
property float y
property float z
property float nx
property float ny
property float nz
property float s
property float t
property uchar red
property uchar green
property uchar blue
property uchar alpha
element face %d
property list uchar uint vertex_indices
end_header
`,
		len(mesh.vertices),
		len(mesh.faces),
	))

	if err != nil {
		return err
	}

	for _, vertex := range mesh.vertices {
		_, err = output.WriteString(fmt.Sprintf("%.6f %.6f %.6f %.6f %.6f %.6f %.6f %.6f %d %d %d %d\n",
			vertex.x, vertex.y, vertex.z,
			vertex.nx, vertex.ny, vertex.nz,
			vertex.s, vertex.t,
			vertex.r, vertex.g, vertex.b, vertex.a,
		))

		if err != nil {
			return err
		}
	}

	for _, face := range mesh.faces {
		output.WriteString(fmt.Sprintf("%d", len(face.indices)))

		for _, index := range face.indices {
			output.WriteString(fmt.Sprintf(" %d", index))
		}

		output.WriteString("\n")
	}

	return nil
}
