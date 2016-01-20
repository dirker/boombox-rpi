src := $(wildcard *.c)
obj := $(src:%.c=%.o)

CFLAGS += -g

CFLAGS += $(shell sdl2-config --cflags)
LDFLAGS += $(shell sdl2-config --libs)

CFLAGS += $(shell pkg-config libmpg123 --cflags)
LDFLAGS += $(shell pkg-config libmpg123 --libs)

boomplay: $(obj) Makefile
	@echo 'LD '$@
	@gcc $(LDFLAGS) $(obj) -o $@

%.o: %.c Makefile
	@echo 'CC '$@
	@gcc $(CFLAGS) -c -o $@ $<
