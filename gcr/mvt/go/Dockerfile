# Use the offical golang image to create a binary.
# This is based on Debian and sets the GOPATH to /go.
# https://hub.docker.com/_/golang
FROM golang:1.16-buster as builder

# Create and change to the app directory.
WORKDIR /builder

# RUN apt-get update
# RUN apt-get install -y gccgo
# RUN go env
# RUN apt-get update && apt-get install -y --no-install-recommends \
#     gccgo-go
# Retrieve application dependencies.
# This allows the container build to reuse cached dependencies.
# Expecting to copy go.mod and if present go.sum.
# COPY go.* ./builder
# RUN go mod download

# Copy local code to the container image.
ADD . /builder

# Get Assembly
# RUN script -q -c "go build -mod=vendor -gcflags -S main.go" /dev/null > mvt.asm

# Build the binary.
# RUN CGO_ENABLED=0 go build -compiler=gccgo main.go
RUN CGO_ENABLED=0 go build -mod=vendor -v -o mvt

# Run the Program
# CMD ["sh", "-c", "./mvt ${INPUT} ${THREADS}"]
# Use the official Debian slim image for a lean production container.
# https://hub.docker.com/_/debian
# https://docs.docker.com/develop/develop-images/multistage-build/#use-multi-stage-builds
FROM debian:buster-slim
RUN set -x && apt-get update && DEBIAN_FRONTEND=noninteractive apt-get install -y \
    ca-certificates && \
    rm -rf /var/lib/apt/lists/*

WORKDIR /r

# Copy the program from the previously created stage and make it the entry point.
COPY --from=builder /builder/mvt /r
# COPY --from=builder /builder/mvt.asm /r

CMD ["sh", "-c", "./mvt"]


