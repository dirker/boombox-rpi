package mpg123

import (
	"bufio"
	"errors"
	"fmt"
	"io"
	"os/exec"
	"strconv"
	"strings"
)

// Errors returned when interacting with mpg123
var (
	ErrBadFile = errors.New("mpg123: bad file")
)

type State int

const (
	StateStopped State = iota
	StatePaused
	StatePlaying
)

// Handle references a running mpg123 process
type Handle struct {
	cmd       *exec.Cmd
	stdin     io.WriteCloser
	stdout    io.ReadCloser
	stderr    io.ReadCloser
	volume    uint
	frame     uint
	state     State
	notifiers map[byte]chan bool
}

// Spawn spawns a new mpg123 process to be remote controlled
func Spawn() (h *Handle, err error) {
	h = &Handle{
		cmd: exec.Command("mpg123", "--remote"),
	}

	h.stdin, err = h.cmd.StdinPipe()
	if err != nil {
		return nil, err
	}

	h.stdout, err = h.cmd.StdoutPipe()
	if err != nil {
		return nil, err
	}

	h.stderr, err = h.cmd.StderrPipe()
	if err != nil {
		return nil, err
	}

	h.notifiers = make(map[byte]chan bool)

	h.cmd.Start()
	go h.processInput()
	return
}

func (h *Handle) processInput() {
	scanner := bufio.NewScanner(h.stdout)
	for scanner.Scan() {
		line := scanner.Text()
		if !strings.HasPrefix(line, "@") {
			continue
		}

		args := strings.Fields(line)
		event := args[0][1]
		fmt.Printf("<%c>:<%q>\n", event, args[1:])

		switch event {
		case 'R': /* release info (on startup) */
			continue
		case 'F': /* frame status */
			/*
					@F <a> <b> <c> <d>
				  			Status message during playing (frame info)
				    			a = framecount (int)
									b = frames left this song (int)
									c = seconds (float)
									d = seconds left (float)
			*/
			frame, _ := strconv.Atoi(args[1])
			h.frame = uint(frame)
		case 'I': /* info after loading */
			// nothing to do
		case 'P': /* state */
			state, _ := strconv.Atoi(args[1])
			h.state = State(state)
		case 'V': /* volume */
			volume, _ := strconv.ParseFloat(args[1], 32)
			h.volume = uint(volume * 100)
		default:
			panic("mpg123: unhandled event")
		}

		/* process notifiers */
		if notifier, ok := h.notifiers[event]; ok {
			close(notifier)
			h.notifiers[event] = nil
		}
	}
}

func (h *Handle) send(cmd string) {
	h.stdin.Write([]byte(cmd + "\n"))
}

// Kill kills the referenced mpg123 process
func (h *Handle) Kill() error {
	h.send("q")
	return h.cmd.Wait()
}

// Expect creates a notifier for an expected event
func (h *Handle) Expect(event byte) (notifier chan bool) {
	notifier = make(chan bool)
	v, ok := h.notifiers[event]
	if ok && v != nil {
		panic("Tried to overwrite notifier")
	}
	h.notifiers[event] = notifier
	return
}

// Load loads fname and starts playing
func (h *Handle) Load(fname string) error {
	p := h.Expect('P')
	h.send("l " + fname)
	<-p

	if h.state == StateStopped {
		// error
	}

	return nil
}

// LoadPaused loads fname and goes to paused state
func (h *Handle) LoadPaused(fname string) {
	i := h.Expect('I')
	p := h.Expect('P')
	h.send("lp " + fname)
	<-i
	<-p

	if h.state == StateStopped {
		// FIXME: handle error
	}
}

// SetVolume sets the volume to a value between 0 and 100 (percentage)
func (h *Handle) SetVolume(vol uint) {
	if vol > 100 {
		vol = 100
	}

	v := h.Expect('V')
	h.send("v " + strconv.FormatUint(uint64(vol), 10))
	<-v
}
