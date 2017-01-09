package main

import (
	"bufio"
	"bytes"
	"errors"
	"strconv"

	"github.com/tarm/serial"
)

// RFID reader sends data through serial, 9600 baud:
//
//  1 byte: 0x2 (STX)
// 10 byte: ASCII (data)
//  2 byte: ASCII (crc)
//  1 byte: '\r'
//  1 byte: '\n'
//  1 byte: 0x3 (ETX)

const (
	rfidSTX       = 0x2
	rfidETX       = 0x3
	rfidDataStart = 1
	rfidDataLen   = 10
	rfidDataEnd   = rfidDataStart + rfidDataLen
	rfidCRCStart  = rfidDataEnd
	rfidCRCLen    = 2
	rfidCRCEnd    = rfidCRCStart + rfidCRCLen
	rfidLen       = rfidDataLen + rfidCRCLen + 4
)

func rfidCRCOk(data, crc []byte) bool {
	ref, _ := strconv.ParseUint(string(crc), 16, 8)
	var chk byte

	for i := 0; i < len(data); i += 2 {
		b, _ := strconv.ParseUint(string(data[i:i+2]), 16, 8)
		chk ^= byte(b)
	}

	return chk == byte(ref)
}

func rfidSplitter(data []byte, atEOF bool) (advance int, token []byte, err error) {
	if atEOF {
		err = errors.New("rfid reader closed the line? wtf")
		return
	}

	for {
		// find start marker
		i := bytes.IndexByte(data[advance:], rfidSTX)

		// if no start marker found, fully advance
		if i < 0 {
			return len(data), nil, nil
		}

		// advance keeps all the bytes we already searched through
		i += advance

		// if start marker found, but not enough data yet, advance until start marker
		if len(data) < i+rfidLen {
			return i, nil, nil
		}

		tag := data[i : i+rfidLen]
		advance = i + rfidLen

		// if no end marker found, restart search
		if tag[rfidLen-1] != rfidETX {
			continue
		}

		// if crc not ok, restart search
		data := tag[rfidDataStart:rfidCRCEnd]
		crc := tag[rfidCRCStart:rfidCRCEnd]
		if !rfidCRCOk(data, crc) {
			continue
		}

		// good tag received, done
		return advance, data, nil
	}
}

func rfidReader(fname string, ch chan<- rfidEvent) {
	c := &serial.Config{Name: fname, Baud: 9600}
	s, err := serial.OpenPort(c)
	if err != nil {
		panic(err)
	}

	scanner := bufio.NewScanner(s)
	scanner.Split(rfidSplitter)

	for scanner.Scan() {
		ch <- rfidEvent{scanner.Text()}
	}

	if err := scanner.Err(); err != nil {
		panic(err)
	}
}
