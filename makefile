# File author is Ítalo Lima Marconato Matias
#
# Created on May 11 of 2018, at 13:14 BRT
# Last edited on November 17 of 2018, at 13:05 BRT

ARCH ?= x86
VERBOSE ?= false
DEBUG ?= false

PATH := $(shell dirname $(realpath $(lastword $(MAKEFILE_LIST))))/../toolchain/$(ARCH)/bin:$(PATH)
SHELL := env PATH=$(PATH) /bin/bash

ifeq ($(ARCH),x86)
	TARGET ?= i686-chicago
	ARCH_CFLAGS := -DCHEXEC_ARCH=CHEXEC_HEADER_FLAGS_ARCH_X86
	
	ifneq ($(SUBARCH),)
		UNSUPPORTED_ARCH := true
	endif
	
	ARCH_OBJECTS := start.s.o
	ARCH_OBJECTS += arch.c.o
	ARCH_OBJECTS += io/debug.c.o io/ide.c.o io/keyboard.c.o io/mouse.c.o
	ARCH_OBJECTS += sys/gdt.c.o sys/idt.c.o sys/panic.c.o sys/pit.c.o
	ARCH_OBJECTS += sys/process.c.o sys/sc.c.o
	ARCH_OBJECTS += mm/pmm.c.o mm/vmm.c.o
	
	OBJCOPY_FORMAT := elf32-i386
	OBJCOPY_ARCH := i386
	
	LINKER_SCRIPT := link.ld
else
	UNSUPPORTED_ARCH := true
endif

OBJECTS := main.c.o
OBJECTS += ds/list.c.o ds/queue.c.o ds/stack.c.o
OBJECTS += exec/chexec.c.o exec/exec.c.o exec/lib.c.o
OBJECTS += io/console.c.o io/device.c.o io/debug.c.o io/display.c.o
OBJECTS += io/file.c.o
OBJECTS += io/dev/framebuffer.c.o io/dev/rawkeyboard.c.o io/dev/rawmouse.c.o io/dev/null.c.o
OBJECTS += io/dev/zero.c.o
OBJECTS += io/fs/chfs.c.o io/fs/devfs.c.o io/fs/iso9660.c.o
OBJECTS += mm/alloc.c.o mm/heap.c.o mm/pmm.c.o mm/ualloc.c.o
OBJECTS += mm/virt.c.o
OBJECTS += sys/ipc.c.o sys/panic.c.o sys/process.c.o sys/rand.c.o
OBJECTS += sys/sc.c.o sys/string.c.o

OTHER_OBJECTS := splash.bmp

ARCH_OBJECTS := $(addprefix build/arch/$(ARCH)/,$(ARCH_OBJECTS))
OBJECTS := $(addprefix build/,$(OBJECTS))
OTHER_OBJECTS := $(addsuffix .oo, $(addprefix build/,$(OTHER_OBJECTS)))
LINKER_SCRIPT := arch/$(ARCH)/$(LINKER_SCRIPT)

ifeq ($(SUBARCH),)
	KERNEL := build/chkrnl-$(ARCH)
else
	KERNEL := build/chkrnl-$(ARCH)_$(SUBARCH)
endif

ifneq ($(VERBOSE),true)
NOECHO := @
endif

all: $(KERNEL)

clean:
ifeq ($(UNSUPPORTED_ARCH),true)
	$(error Unsupported architecture $(ARCH))
endif
	$(NOECHO)rm -f $(ARCH_OBJECTS) $(OBJECTS) $(OTHER_OBJECTS) $(KERNEL)

clean-all:
ifeq ($(UNSUPPORTED_ARCH),true)
	$(error Unsupported architecture $(ARCH))
endif
	$(NOECHO)rm -rf build

remake: clean all
ifeq ($(UNSUPPORTED_ARCH),true)
	$(error Unsupported architecture $(ARCH))
endif

$(KERNEL): $(ARCH_OBJECTS) $(OBJECTS) $(OTHER_OBJECTS) $(LINKER_SCRIPT)
ifeq ($(UNSUPPORTED_ARCH),true)
	$(error Unsupported architecture $(ARCH))
endif
	$(NOECHO)echo Linking $@
	$(NOECHO)if [ ! -d $(dir $@) ]; then mkdir -p $(dir $@); fi
	$(NOECHO)$(TARGET)-gcc -T$(LINKER_SCRIPT) -fno-pie -no-pie -ffreestanding -nostdlib -o $@ $(ARCH_OBJECTS) $(OBJECTS) $(OTHER_OBJECTS) $(ARCH_LDFLAGS) -lgcc

build/%.oo: %
ifeq ($(UNSUPPORTED_ARCH),true)
	$(error Unsupported architecture $(ARCH))
endif
	$(NOECHO)echo Compiling $<
	$(NOECHO)if [ ! -d $(dir $@) ]; then mkdir -p $(dir $@); fi
	$(NOECHO)$(TARGET)-objcopy -Ibinary -O$(OBJCOPY_FORMAT) -B$(OBJCOPY_ARCH) $< $@
	$(NOECHO)$(TARGET)-objcopy --rename-section .data=.rodata,alloc,load,readonly,data,contents $@ $@

build/%.s.o: %.s
ifeq ($(UNSUPPORTED_ARCH),true)
	$(error Unsupported architecture $(ARCH))
endif
	$(NOECHO)echo Compiling $<
	$(NOECHO)if [ ! -d $(dir $@) ]; then mkdir -p $(dir $@); fi
	$(NOECHO)$(TARGET)-as $(ARCH_AFLAGS) $< -o $@

build/%.c.o: %.c
ifeq ($(UNSUPPORTED_ARCH),true)
	$(error Unsupported architecture $(ARCH))
endif
	$(NOECHO)echo Compiling $<
	$(NOECHO)if [ ! -d $(dir $@) ]; then mkdir -p $(dir $@); fi
ifeq ($(SUBARCH),)
ifeq ($(DEBUG),yes)
	$(NOECHO)$(TARGET)-gcc -DARCH=\"$(ARCH)\" -DDEBUG -g -std=c11 -Iinclude -Iarch/$(ARCH)/include -fno-pie -no-pie -ffreestanding -O0 -Wall -Wextra -Wno-implicit-fallthrough -Wno-pointer-to-int-cast -Wno-int-to-pointer-cast $(ARCH_CFLAGS) -c $< -o $@
else
	$(NOECHO)$(TARGET)-gcc -DARCH=\"$(ARCH)\" -std=c11 -Iinclude -Iarch/$(ARCH)/include -fno-pie -no-pie -ffreestanding -O3 -Wall -Wextra -Wno-implicit-fallthrough -Wno-pointer-to-int-cast -Wno-int-to-pointer-cast $(ARCH_CFLAGS) -c $< -o $@
endif
else
ifeq ($(DEBUG),yes)
	$(NOECHO)$(TARGET)-gcc -DARCH=\"$(ARCH)\" -DDEBUG -g -std=c11 -Iinclude -Iarch/$(ARCH)/include -I arch/$(ARCH)/subarch/$(SUBARCH)/include -fno-pie -no-pie -ffreestanding -O0 -Wall -Wextra -Wno-implicit-fallthrough -Wno-pointer-to-int-cast -Wno-int-to-pointer-cast $(ARCH_CFLAGS) -c $< -o $@
else
	$(NOECHO)$(TARGET)-gcc -DARCH=\"$(ARCH)\" -std=c11 -Iinclude -Iarch/$(ARCH)/include -I arch/$(ARCH)/subarch/$(SUBARCH)/include -fno-pie -no-pie -ffreestanding -O3 -Wall -Wextra -Wno-implicit-fallthrough -Wno-pointer-to-int-cast -Wno-int-to-pointer-cast $(ARCH_CFLAGS) -c $< -o $@
endif
endif
