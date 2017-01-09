package mpg123

import "testing"

func TestSpawnKill(t *testing.T) {
	h, err := Spawn()
	if err != nil {
		t.Error("Failed to spawn")
	}

	err = h.Kill()
	if err != nil {
		t.Error("Failed to kill:", err)
	}
}

func TestTypical(t *testing.T) {
	h, err := Spawn()
	if err != nil {
		t.Error("Failed to spawn")
	}

	h.LoadPaused("test-01.mp3")
	if h.state != StatePaused {
		t.Error("Not in paused state")
	}

	h.SetVolume(70)

	_ = h.Kill()
}
