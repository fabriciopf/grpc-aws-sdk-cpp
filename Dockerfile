FROM ubuntu

RUN apt-get update
RUN apt install -y git
RUN apt install -y build-essential 
RUN apt install -y cmake 

# AWS SDK C++ 
RUN apt install -y libcurl4-openssl-dev
RUN apt install -y zlib1g-dev
RUN apt install -y libssl-dev
WORKDIR /
RUN git clone --progress https://github.com/aws/aws-sdk-cpp.git
RUN mkdir /aws-sdk-cpp/build
WORKDIR /aws-sdk-cpp/build
RUN cmake -DCMAKE_BUILD_TYPE=Release -DBUILD_ONLY="dynamodb;kms" -DENABLE_TESTING=Off ..
#RUN cmake -DBUILD_SHARED_LIBS=Off -DCMAKE_BUILD_TYPE=Release -DBUILD_ONLY="dynamodb;kms" -DENABLE_TESTING=Off ..
RUN make
RUN make install

# gRPC
RUN apt install -y libgflags-dev libgtest-dev clang libc++-dev golang autoconf libtool
WORKDIR /
RUN git clone --progress https://github.com/grpc/grpc.git
WORKDIR /grpc
RUN git submodule update --init
RUN make 
RUN make install
RUN mkdir build
WORKDIR /grpc/build
RUN cmake ..
RUN make
RUN make install

# KV Service
WORKDIR /
RUN apt install -y rpm
ADD src /kv-service
RUN mkdir /kv-service/build
WORKDIR /kv-service/build
RUN cmake /kv-service
RUN make package