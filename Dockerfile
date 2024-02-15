
FROM arm64v8/ubuntu:22.04 
#FROM amd64/ubuntu:22.04 

RUN export DEBIAN_FRONTEND="noninteractive" && apt-get update && apt-get -y install default-jre default-jdk  

RUN apt-get update && apt-get -y install python3 python3-dev python3-pip git bison flex wget cmake build-essential pkg-config clang lldb
		
RUN python3 -m pip install -U pip

COPY . /miniCompEval

RUN cd /miniCompEval && cmake -B build && cmake --build build

CMD ["/bin/bash"]
