# Dockerfile for https://index.docker.io/u/crosbymichael/python/ 
FROM gcc:4.9

RUN apt-get update && apt-get install -y \
    cmake

