FROM ubuntu:18.04

RUN apt-get update && apt-get install -y \
    ca-certificates

ADD https://github.com/aktau/github-release/releases/download/v0.7.2/linux-amd64-github-release.tar.bz2 /tmp/github-release.tar.bz2

RUN tar -xjf /tmp/github-release.tar.bz2 \
    && cp ./bin/linux/amd64/* /bin


