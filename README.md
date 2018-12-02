# CHicago Kernel

## Building it

For building it from scratch, you need:

	A Unix-like environment
	CHicago Toolchain in ../toolchain (git clone https://github.com/CHOSTeam/CHicago-Toolchain.git ../toolchain)
	Make

To build it, go to the root directory and type:

	make

You can append ARCH=\<ARCH\> and SUBARCH=\<SUBARCH\> to change the system architecture, VERBOSE=true to enable verbose build (good for finding compilation errors) and DEBUG=yes to disable optimizations and make a debug build.
