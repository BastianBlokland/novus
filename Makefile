default: build.debug

# --------------------------------------------------------------------------------------------------
# Convenient development utlitities.
# --------------------------------------------------------------------------------------------------

nproc := $(shell ./scripts/nproc.sh)

.PHONY: build.debug
build.debug:
	cmake -B build -DCMAKE_BUILD_TYPE=Debug && \
	cd build && \
	make -j$(nproc)

.PHONY: build.release
build.release:
	cmake -B build -DCMAKE_BUILD_TYPE=Release && \
	cd build && \
	make -j$(nproc)

.PHONY: test
test:
	cmake -B build -DCMAKE_BUILD_TYPE=Debug && \
	cd build && \
	make tests -j$(nproc) && \
	make test

.PHONY: clean
clean:
	rm -rf build

# Watch for changes to '.nov' files and automatically evaluate them.
# Requires 'inotify-tools' to be installed.
.PHONY: watch.nov
watch.nov:
	./scripts/watch_nov.sh . ./bin/nove

.SILENT:
