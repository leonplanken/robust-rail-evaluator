FROM tensorflow/tensorflow:2.1.2

# docker build -t tors-base .
# docker run --network="host" --rm -it tors-base /bin/bash

RUN apt-get clean && apt-get update && apt-get install -y locales
RUN echo "en_US.UTF-8 UTF-8" > /etc/locale.gen && \
    locale-gen
ENV LC_ALL en_US.UTF-8
ENV LANG en_US.UTF-8
ENV LANGUAGE en_US.UTF-8
ENV SHELL /bin/bash
ENV DEBIAN_FRONTEND noninteractive

# switch to bash within the container so ROS sourcing is easy in build commands
SHELL ["/bin/bash", "-c"]

RUN apt-get update && \
    apt-get install -y git curl make && \
    apt-get clean

ADD https://cmake.org/files/v3.16/cmake-3.16.3-Linux-x86_64.sh /cmake-3.16.3-Linux-x86_64.sh
RUN mkdir /opt/cmake
RUN sh /cmake-3.16.3-Linux-x86_64.sh --prefix=/opt/cmake --skip-license
RUN ln -s /opt/cmake/bin/cmake /usr/local/bin/cmake
RUN cmake --version

RUN python -m pip install --upgrade pip
RUN python -m pip install gym stable_baselines

# Update GCC to v9
RUN    apt-get update \
	&& DEBIAN_FRONTEND=noninteractive apt-get install -y --no-install-recommends software-properties-common \
	&& add-apt-repository ppa:ubuntu-toolchain-r/test \
	&& DEBIAN_FRONTEND=noninteractive apt-get install -y --no-install-recommends \
		build-essential \
		gcc-9 \
		g++-9 \
		gcc-9-multilib \
		g++-9-multilib \
		xutils-dev \
		patch \
		git \
		python3 \
		python3-pip \
		libpulse-dev \
	&& apt-get clean \
	&& rm -rf /var/lib/apt/lists/*

RUN    update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-7 10 \
	&& update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-9 20 \
	&& update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-7 10 \
	&& update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-9 20
	
RUN mkdir /ctors
COPY . /ctors
WORKDIR /ctors
RUN mkdir build
RUN python setup.py build
RUN python setup.py install
RUN mkdir agents


