default: build.debug

# --------------------------------------------------------------------------------------------------
# Convenient development utlitities.
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

# Evaluate nov files when they are written to.
# Requires 'inotify-tools' to be installed.
.PHONY: watch.nov
watch.nov:
	inotifywait --event close_write -qm --format '%w' **/*.nov \
		| xargs -I{} -r sh -c \
		"clear; echo "{}:"; ./bin/eval {}; echo "------""

.SILENT:
