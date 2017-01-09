package main

const (
	inputUnknown = iota
	inputVolumeUp
	inputVolumeDown
	inputNext
)

type inputEvent struct {
	code int
}

type rfidEvent struct {
	tag string
}
