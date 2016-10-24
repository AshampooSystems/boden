# Dockerfile for https://index.docker.io/u/crosbymichael/python/ 
FROM gcc:4.9

RUN apt-get update && apt-get install -y --no-install-recommends \
    cmake \
    xvfb \
    xauth \
    libgtk-3-dev \
    iceweasel

# add a non-root user that we run as
RUN adduser --disabled-password --gecos '' normusr

USER normusr

ADD . boden/

WORKDIR boden


CMD xvfb-run ./prepareBuildTest linux make


