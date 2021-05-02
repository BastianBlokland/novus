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

.PHONY: test.lex
test.lex: build
	./scripts/test.sh --filter "^\[lex\]"

.PHONY: test.parse
test.parse: build
	./scripts/test.sh --filter "^\[parse\]"

.PHONY: test.frontend
test.frontend: build
	./scripts/test.sh --filter "^\[frontend\]"

.PHONY: test.backend
test.backend: build
	./scripts/test.sh --filter "^\[backend\]"

.PHONY: test.opt
test.opt: build
	./scripts/test.sh --filter "^\[opt\]"

.PHONY: test.vm
test.vm: build
	./scripts/test.sh --filter "^\[vm\]"

.PHONY: test.std
test.std: build
	./scripts/test.sh --filter "^\[std\]"

.PHONY: clean
clean:
	rm -rf build

.PHONY: watch.validate
watch.validate:
	nov-watch.nx validate

.PHONY: watch.compile
watch.compile:
	nov-watch.nx compile

.PHONY: watch.evaluate
watch.evaluate:
	nov-watch.nx evaluate

.PHONY: docker.run
docker.run:
	sudo docker build -t novus . && sudo docker run --rm -it novus

.SILENT:
