package main

import (
	"fmt"
	"log"
	"os"
	"path"
	"path/filepath"

	"boomboxd/audio"
)

// RFIDTag is a RFID tag as read from the RFID reader
type RFIDTag string

const (
	tagdir = "tags"
)

func handleInput(code int) {
	switch code {
	case inputNext:
		//audio.Next()
	case inputVolumeUp:
		vol := audio.AdjustVolume(2)
		log.Printf("Volume %d\n", vol)
	case inputVolumeDown:
		vol := audio.AdjustVolume(-2)
		log.Printf("Volume %d\n", vol)
	}
}

func handleTag(tag string) {
	fmt.Printf("rfid %q\n", tag)

	fname, err := os.Readlink(path.Join(tagdir, string(tag)))
	if err != nil {
		return
	}

	audio.Play(fname)
}

func main() {
	log.Println("Starting up ...")

	/* setup evdev inputs */
	inputChan := make(chan inputEvent)
	evdevReader("evdev.pipe", inputChan)
	{
		/* all event devices */
		evdevNames, _ := filepath.Glob("/dev/input/event*")
		for _, fname := range evdevNames {
			evdevReader(fname, inputChan)
		}
	}

	rfidChan := make(chan rfidEvent)
	// {
	// 	/* for debugging */
	// 	go func() {
	// 		_, _ = os.OpenFile("rfid.pipe", os.O_WRONLY, 0666)
	// 	}()
	// 	go rfidReader("rfid.pipe", rfidChan)
	// }
	//go rfidReader("/dev/ttyS0", rfidChan)

	log.Println("Entering event loop ...")

	for {
		select {
		case ev := <-inputChan:
			handleInput(ev.code)
		case rfid := <-rfidChan:
			handleTag(rfid.tag)
		}
	}
}
