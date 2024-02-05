
FROM arm32v7/ubuntu:22.04 

#RUN export DEBIAN_FRONTEND="noninteractive" && \
RUN	apt-get update && \
		apt-get --debug install -y gpg wget default-jre default-jdk && \ 

RUN apt-get update && \
		apt-get --debug -y install python3 python3-dev python3-pip git wget cmake build-essential  pkg-config
		
RUN python3 -m pip install -U pip	

COPY test/Compiler /Compiler
COPY test/CompilerOut /CompilerOut
COPY test/comp.sh /comp.sh

CMD ["/bin/bash"]
