
FROM arm32v7/ubuntu:22.04 

RUN export DEBIAN_FRONTEND="noninteractive" && apt-get update && apt-get -y install default-jre default-jdk  

RUN apt-get update && apt-get -y install python3 python3-dev python3-pip git wget cmake build-essential pkg-config
		
RUN python3 -m pip install -U pip	

COPY CodeGen /Compiler
COPY ARMv7Out /CompilerOut
COPY comp.sh /comp.sh

CMD ["/bin/bash"]
