package main

import "math"

type Vector3 struct {
	X, Y, Z float32
}

func Normalize3f(x, y, z *float32) {
	var mag = *x**x + *y**y + *z**z

	if mag != 0 {
		var scale = 1 / float32(math.Sqrt(float64(mag)))

		*x = *x * scale
		*y = *y * scale
		*z = *z * scale
	}
}

func Dot3f(a, b Vector3) float32 {
	return a.X*b.X + a.Y*b.Y + a.Z*b.Z
}

func Cross3f(a, b Vector3) Vector3 {
	return Vector3{
		a.Y*b.Z - a.Z*b.Y,
		a.Z*b.X - a.X*b.Z,
		a.X*b.Y - a.Y*b.X,
	}
}

func Sub3f(a, b Vector3) Vector3 {
	return Vector3{
		a.X - b.X,
		a.Y - b.Y,
		a.Z - b.Z,
	}
}

func Scale3F(a Vector3, scalar float32) Vector3 {
	return Vector3{
		a.X * scalar,
		a.Y * scalar,
		a.Z * scalar,
	}
}
