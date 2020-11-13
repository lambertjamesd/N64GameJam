package main

import (
	"errors"
	"math"
)

type Matrix3x3 [3][3]float32

func (matrix Matrix3x3) Inverse() (Matrix3x3, error) {
	var determanant = matrix[0][0]*matrix[1][1]*matrix[2][2] +
		matrix[0][1]*matrix[1][2]*matrix[2][0] +
		matrix[0][2]*matrix[1][0]*matrix[2][1] -
		matrix[0][2]*matrix[1][1]*matrix[2][0] -
		matrix[0][1]*matrix[1][0]*matrix[2][2] -
		matrix[0][0]*matrix[1][2]*matrix[2][1]

	if math.Abs(float64(determanant)) < 0.00001 {
		return [3][3]float32{}, errors.New("Cannot invert")
	}

	var inv = 1 / determanant

	return [3][3]float32{
		[3]float32{
			(matrix[1][1]*matrix[2][2] - matrix[1][2]*matrix[2][1]) * inv,
			-(matrix[0][1]*matrix[2][2] - matrix[0][2]*matrix[2][1]) * inv,
			(matrix[0][1]*matrix[1][2] - matrix[0][2]*matrix[1][1]) * inv,
		},
		[3]float32{
			-(matrix[1][0]*matrix[2][2] - matrix[1][2]*matrix[2][0]) * inv,
			(matrix[0][0]*matrix[2][2] - matrix[0][2]*matrix[2][0]) * inv,
			-(matrix[0][0]*matrix[1][2] - matrix[0][2]*matrix[1][0]) * inv,
		},
		[3]float32{
			(matrix[1][0]*matrix[2][1] - matrix[1][1]*matrix[2][0]) * inv,
			-(matrix[0][0]*matrix[2][1] - matrix[0][1]*matrix[2][0]) * inv,
			(matrix[0][0]*matrix[1][1] - matrix[0][1]*matrix[1][0]) * inv,
		},
	}, nil
}

func (matrix Matrix3x3) Mul3f(input Vector3) Vector3 {
	return Vector3{
		matrix[0][0]*input.X + matrix[0][1]*input.Y + matrix[0][2]*input.Z,
		matrix[1][0]*input.X + matrix[1][1]*input.Y + matrix[1][2]*input.Z,
		matrix[2][0]*input.X + matrix[2][1]*input.Y + matrix[2][2]*input.Z,
	}
}
