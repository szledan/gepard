.PHONY: all
all: release

.PHONY: release
release: build_dir.release
	cd build/release/ && cmake -DCMAKE_BUILD_TYPE=Release $(OPTIONS) ../../ && make && make unit

.PHONY: debug
debug: build_dir.debug
	cd build/debug/ && cmake -DCMAKE_BUILD_TYPE=Debug $(OPTIONS) ../../ && make && make unit

.PHONY: gepard
gepard: build_dir.release
	cd build/release/ && cmake -DCMAKE_BUILD_TAPE=Release $(OPTIONS) ../../ && make gepard

.PHONY: gepard.debug
gepard.debug: build_dir.debug
	cd build/debug/ && cmake -DCMAKE_BUILD_TYPE=Debug $(OPTIONS) ../../ && make gepard

.PHONY: cppcheck
cppcheck:
	cppcheck src
	cppcheck examples

.PHONY: cppcheck-all
cppcheck-all:
	cppcheck --enable=all src
	cppcheck --enable=all examples

.PHONY: build_dir.release
build_dir.release:
	mkdir -p build/release/

.PHONY: build_dir.debug
build_dir.debug:
	mkdir -p build/debug/

.PHONY: run-test
run-test: release
	cd build/release/ && ./bin/unit

.PHONY: run-test.debug
run-test.debug: debug
	cd build/debug/ && ./bin/unit

.PHONY: clean
release.clean:
	cd build/release/ && make clean

.PHONY: clean.debug
debug.clean:
	cd build/debug/ && make clean

.PHONY: distclean
distclean:
	rm -rf build/
