# --------------------------------------------------------------------------------------------------
# Docker container with the 'webserver' novus example.
# --------------------------------------------------------------------------------------------------

FROM  bastianblokland/novus:latest as build
LABEL description="Novus Webserver Example"

# Copy sources into the container.
COPY *    /webserver/
COPY site /webserver/site
WORKDIR   /webserver

# Compile the webserver into a novus excutable file (.nx).
RUN novc main.nov --out server.nx

# Open port '80'
EXPOSE 80

# At entry start the webserver on port '80'.
ENTRYPOINT ["novrt", "server.nx", "--port", "80"]
