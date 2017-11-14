# Daytime TCP
DAYTIME protocol concurrent server and client using TCP and written in C. Project developed for learning purposes in Networks and Services Architecture subject of Software Engineering degree at University of Valladolid.

## Requirements
- [Docker](https://www.docker.com/) running on your machine.

## Development
First, set up the environment:
```bash
git clone https://gitlab.com/jorgesanz/daytime-tcp # Clone repository
cd daytime-tcp # Go to repository folder
docker run --name=ubuntu-daytime-tcp -it -v $(PWD):/home ubuntu # Run Ubuntu container sharing repository folder

# Inside the container. Get the C compiler tools
apt-get update
apt-get install build-essential
```

For the next times, having the container already on my machine, how to deploy local environment?
```bash
docker start ubuntu-daytime-tcp
docker exec -it ubuntu-daytime-tcp bash
```

Compile client and server:
```bash
# Inside the container
cd /home
gcc -Wall -o client.out daytime-tcp-client-Sanz-Perez.c
gcc -Wall -o server.out daytime-tcp-server-Sanz-Perez.c
```

Run server:
```bash
# Inside the container
./server.out [-p port-number] # port-number is optional
```

To run a client, you need to open another terminal in the container:
```bash
docker exec -it ubuntu-daytime-tcp bash

# Inside the container
cd /home
./client.out [server.IP.address] [-p port-number] # server.IP.address is mandatory, port-number is optional
```
