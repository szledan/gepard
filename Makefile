
all: build_dir
	cd build/ && make

gepard:
	cd build/ && make gepard

build_dir:
	mkdir -p build/
	cd build/ && cmake ../

clean:
	cd build && make clean

distclean:
	rm -rf build/
