
all: build_dir
	cd build/ && make

gepard:
	cd build/ && make gepard

cppcheck:
	cppcheck src
	cppcheck examples

build_dir:
	mkdir -p build/
	cd build/ && cmake ../

clean:
	cd build && make clean

distclean:
	rm -rf build/
