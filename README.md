# TCP-IP-client-server-code-Implemented-in-Docker
client server application code implementing docker containers and volumes.
Steps to build receiver code using dockerfile and volumes
  docker build -t receiver_ubuntu -f Dockerfile_receiver.txt .
  docker volume create my-data-volume
  docker run -it --name receiver_container -v my-data-volume:/app/data -p 5055:5055 -p 5056:5056 receiver_ubuntu /bin/bash
  
Steps to build sender code using dockerfile and volumes
docker build -t sender_ubuntu -f Dockerfile_sender.txt .
docker run -it --name sender_container sender_ubuntu /bin/bash
