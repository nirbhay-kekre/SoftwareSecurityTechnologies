FROM ubuntu:latest
RUN apt-get update && apt-get install -y gcc && apt-get install -y vim && apt-get install -y iputils-ping && apt-get install -y git && apt-get install -y sudo && useradd -ms /bin/bash testuser && usermod -aG sudo testuser
RUN echo 'testuser:password' | chpasswd
USER testuser
RUN mkdir ~/work
WORKDIR /home/testuser/work
CMD ["/bin/bash"]
