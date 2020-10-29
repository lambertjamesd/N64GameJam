package main

func ApplyLighting(ambient, dx, dy, dz float32) VertexTransform {
	return func(vertex *MeshVertex) {
		var dot = -(dx*vertex.nx + dy*vertex.ny + dz*vertex.nz)

		if dot < 0 {
			dot = 0
		}

		var lightValue = ambient*(1-dot) + dot

		vertex.r = uint8(lightValue * float32(vertex.r))
		vertex.g = uint8(lightValue * float32(vertex.g))
		vertex.b = uint8(lightValue * float32(vertex.b))
	}
}
