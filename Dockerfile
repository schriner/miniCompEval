
FROM arm32v7/ubuntu 

RUN export DEBIAN_FRONTEND="noninteractive" && \
		apt-get update && \
		apt-get install -y \
				gpg wget && \ 
		apt-get update && \
		apt-get -y install python3 python3-dev python3-pip git wget cmake build-essential  pkg-config
		
RUN python3 -m pip install -U pip	

COPY test/Compiler /Compiler
COPY test/CompilerOut /CompilerOut

CMD ["/bin/bash"]