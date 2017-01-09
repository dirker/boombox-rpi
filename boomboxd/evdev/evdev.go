package evdev

import (
	"io"
	"log"
	"os"
	"syscall"
	"unsafe"
)

// Event codes
const (
	EvKey = 0x01
	EvRel = 0x02
)

// Key codes
const (
	KeyNextsong = 163
)

// Rel codes
const (
	RelX = 0x00
)

// Device ...
type Device struct {
	r io.ReadCloser
}

// Event ...
type Event struct {
	Time  syscall.Timeval
	Type  uint16
	Code  uint16
	Value int32
}

var eventsize = int(unsafe.Sizeof(Event{}))

// Open ...
func Open(fname string) (dev *Device, err error) {
	dev = new(Device)
	dev.r, err = os.OpenFile(fname, os.O_RDWR, 0666)
	return
}

// From ...
func From(r io.ReadCloser) (dev *Device) {
	dev = new(Device)
	dev.r = r
	return
}

// ReadOne ...
func (dev *Device) ReadOne() (ev *Event, err error) {
	/*
		ev = &Event{}
		buffer := make([]byte, eventsize)

		_, err = dev.File.Read(buffer)
		if err != nil {
			return nil, err
		}

		b := bytes.NewBuffer(buffer)
		// FIXME: use native conversion using unsafe.Pointer
		err = binary.Read(b, binary.LittleEndian, ev)
		if err != nil {
			return nil, err
		}
	*/

	b := make([]byte, eventsize)
	_, err = dev.r.Read(b)
	if err != nil {
		return nil, err
	}

	log.Printf("%q\n", b)

	ev = (*Event)(unsafe.Pointer(&b[0]))
	log.Printf("%q\n", ev)
	return ev, nil
}

// Close ...
func (dev *Device) Close() {
	dev.r.Close()
}
