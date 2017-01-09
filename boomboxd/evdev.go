package main

import "boomboxd/evdev"

func evdevToInput(ev *evdev.Event) inputEvent {
	switch ev.Type {
	case evdev.EvKey:
		if ev.Code == evdev.KeyNextsong {
			return inputEvent{inputNext}
		}
	case evdev.EvRel:
		if ev.Code == evdev.RelX {
			if ev.Value > 0 {
				return inputEvent{inputVolumeUp}
			}

			return inputEvent{inputVolumeDown}
		}
	}

	return inputEvent{}
}

func evdevReader(fname string, ch chan<- inputEvent) {
	dev, err := evdev.Open(fname)
	if err != nil {
		return
	}

	go func() {
		for {
			ev, err := dev.ReadOne()
			if err != nil {
				break
			}

			input := evdevToInput(ev)
			if input.code == inputUnknown {
				continue
			}

			ch <- input
		}
	}()
}
