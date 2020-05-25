# --------------------------------------------------------------------------------------------------
# Docker container with the 'webserver' novus example.
# --------------------------------------------------------------------------------------------------

FROM  bastianblokland/novus:latest as build
LABEL description="Novus Webserver Example"

# Copy sources into the container.
COPY *    /webserver/
COPY site /webserver/site
WORKDIR   /webserver

# Compile the webserver into a novus assembly file (.nova).
RUN novc main.nov --out server.nova

# Open port '80'
EXPOSE 80

# At entry start the webserver on port '80'.
ENTRYPOINT ["novrt", "server.nova", "--port", "80"]
