package main

import "math"

func Normalize3f(x, y, z *float32) {
	var mag = *x**x + *y**y + *z**z

	if mag != 0 {
		var scale = 1 / float32(math.Sqrt(float64(mag)))

		*x = *x * scale
		*y = *y * scale
		*z = *z * scale
	}
}
