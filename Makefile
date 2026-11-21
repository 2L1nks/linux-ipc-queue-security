CC = gcc
CFLAGS = -std=c11 -O2 -Wall -Wextra -Wpedantic
LDLIBS = -lrt
PROGRAMS = sysv_demo posix_demo sysv_limits posix_limits permission_probe namespace_probe resource_probe queue_count_probe
TARGETS = $(addprefix bin/,$(PROGRAMS))

.PHONY: all clean test
all: $(TARGETS)

bin:
	mkdir -p bin

bin/%: src/%.c | bin
	$(CC) $(CFLAGS) $< -o $@ $(LDLIBS)

test: all
	bash scripts/verify_project.sh

clean:
	rm -f $(TARGETS)
