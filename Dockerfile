FROM ubuntu:18.04

RUN apt update && apt upgrade -y
RUN apt install -y build-essential wget ninja-build libgl1-mesa-dev g++-8
RUN cd /usr/local && \
    wget https://cmake.org/files/v3.11/cmake-3.11.4-Linux-x86_64.tar.gz && \
    tar --strip-components=1 -xf cmake-3.11.4-Linux-x86_64.tar.gz
