module main

go 1.16

require (
	github.com/aws/aws-lambda-go v1.24.0
	kiener_ma/mandelbrot v0.0.0-00010101000000-000000000000
)

replace kiener_ma/mandelbrot => ../../../base-programs/mandelbrot/go/src
