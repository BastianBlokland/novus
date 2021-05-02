
# --------------------------------------------------------------------------------------------------
# Docker container with the compiler, runtime and examples.
# --------------------------------------------------------------------------------------------------

# -- Build container.
FROM alpine:latest as build

LABEL description="Novus - build container"

# Update the package manager.
RUN apk update

# Install build deps: gcc, make, cmake, libc-dev, bash, git and other basic build dependencies.
RUN apk add --no-cache build-base cmake bash git

# Copy sources into the build container.
COPY apps           /novus/apps
COPY include        /novus/include
COPY std            /novus/std
COPY scripts        /novus/scripts
COPY src            /novus/src
COPY utilities      /novus/utilities
COPY CMakeLists.txt /novus/CMakeLists.txt
WORKDIR /novus

# Configure.
RUN bash ./scripts/configure.sh

# Build.
RUN bash ./scripts/build.sh

# -- Run container.
FROM alpine:latest as runtime

LABEL description="Novus - run container"

# Update package manager.
RUN apk update

# Install libstdc++ runtime library.
RUN apk add --no-cache libstdc++

# Copy the binaries from the build container.
RUN mkdir /novus
COPY --from=build /novus/bin/. /novus

# Add the novus directory to the path.
ENV PATH="/novus:${PATH}"

# Copy the examples into the run container.
COPY examples /examples
