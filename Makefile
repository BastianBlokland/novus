default: build.debug

# --------------------------------------------------------------------------------------------------
# Convient development utlitities.
# --------------------------------------------------------------------------------------------------

.PHONY: build.debug
build.debug:
	cmake -B build -DCMAKE_BUILD_TYPE=Debug && \
	cd build && \
	make -j$(shell nproc)

.PHONY: build.release
build.release:
	cmake -B build -DCMAKE_BUILD_TYPE=Release && \
	cd build && \
	make -j$(shell nproc)

.PHONY: test
test:
	cmake -B build -DCMAKE_BUILD_TYPE=Debug && \
	cd build && \
	make tests -j$(shell nproc) && \
	ctest

.PHONY: clean
clean:
	rm -rf build
