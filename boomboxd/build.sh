#!/bin/sh

srcdir="$(pwd)"

export GOPATH="${srcdir}/build"
mkdir -p "${GOPATH}/src"

ln -sfn "$srcdir" "${GOPATH}/src/boomboxd"
(cd "${GOPATH}/src/boomboxd" && go build)
