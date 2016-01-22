ifeq ($(V),1)
  Q =
else
  Q = @
endif

src := $(wildcard *.c)
obj := $(src:%.c=%.o)

CFLAGS += -g

CFLAGS += $(shell sdl2-config --cflags)
LDFLAGS += $(shell sdl2-config --libs)

CFLAGS += $(shell pkg-config libmpg123 --cflags)
LDFLAGS += $(shell pkg-config libmpg123 --libs)

depfile = .$@.d
c_flags = -MD -MF $(depfile) -MT $@ $(CFLAGS)

boomplay: $(obj) Makefile
	@echo 'LD '$@
	$(Q)gcc $(obj) $(LDFLAGS) -o $@

%.o: %.c Makefile
	@echo 'CC '$@
	$(Q)gcc $(c_flags) -c -o $@ $<

clean:
	rm -f boomplay *.o

-include $(wildcard .*.d)
