default: build

# --------------------------------------------------------------------------------------------------
# Convenient development utlitities.
# --------------------------------------------------------------------------------------------------

.PHONY: configure.debug
configure.debug:
	./scripts/configure.sh --type Debug

.PHONY: configure.release
configure.release:
	./scripts/configure.sh --type Release

.PHONY: build
build:
	./scripts/build.sh

.PHONY: test
test: build
	./scripts/test.sh

.PHONY: clean
clean:
	rm -rf build

# Watch for changes to '.nov' files and automatically compile them.
# Requires 'inotify-tools' to be installed.
.PHONY: watch.nov
watch.nov:
	./scripts/watch_nov.sh . ./bin/novc

.SILENT:
