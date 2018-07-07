FROM ubuntu:18.04

ENV DEBIAN_FRONTEND noninteractive

RUN apt-get update && apt-get install -y --no-install-recommends apt-utils
RUN apt-get upgrade -y
RUN apt-get install -y build-essential \
                       wget \
		       ninja-build \
		       libgl1-mesa-dev \
		       g++-8 \
		       git-core

RUN cd /usr/local && \
    wget https://cmake.org/files/v3.11/cmake-3.11.4-Linux-x86_64.tar.gz && \
    tar --strip-components=1 -xf cmake-3.11.4-Linux-x86_64.tar.gz

CMD update-alternatives --install /usr/bin/gcc gcc $(which gcc-8) 0
CMD update-alternatives --install /usr/bin/g++ g++ $(which g++-8) 0
