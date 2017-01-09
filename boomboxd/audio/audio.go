package audio

import "boomboxd/audio/mpg123"

// Foo something
func Foo() {
	mpg123.Spawn()
}

// Play plays a file
func Play(fname string) {
}

// SystemSound plays a system sound
func SystemSound(fname string) error {
	return nil
}

// AdjustVolume adjusts the volume
func AdjustVolume(val int) (volume int) {
	return 0
}

// Next skips to the next song
func Next() {
}
