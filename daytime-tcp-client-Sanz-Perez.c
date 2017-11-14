// Practica tema 6, Sanz Pérez Jorge

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>

#define BUFSIZE 1024

/* wrapper for perror */
void error(char *msg)
{
    perror(msg);
    exit(EXIT_FAILURE);
}

int main(int argc, char **argv)
{
    int s;                            /* socket */
    int port;                         /* port number */
    struct sockaddr_in destination;   /* server structure */
    char buffer[BUFSIZE];             /* messages buffer */
    struct servent *application_name; /* application structure */

    /* check if there is at least one program argument */
    if (argc < 2)
    {
        fprintf(stderr, "usage: %s address.IP.server [-p port-number]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    /* check if usage of port number argument is correct */
    if (argc == 4 && strncmp(argv[2], "-p", 2) != 0)
    {
        fprintf(stderr, "usage: %s address.IP.server [-p port-number]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    /* get port number from arguments */
    if (argc == 4)
    {
        port = htons(atoi(argv[3]));
    }
    else
    { /* get the port by getservbyname when is not passed as argument */
        application_name = getservbyname("daytime", "tcp");

        if (!application_name)
        {
            printf("unknown application daytime\n");
            exit(EXIT_FAILURE);
        }
        else
        {
            port = application_name->s_port;
        }
    }

    /*
    * create a datagram socket in the internet domain and use the
    * TCP protocol
    */
    s = socket(AF_INET, SOCK_STREAM, 0);
    if (s < 0)
        error("ERROR opening socket");

    /* set up the server name */
    destination.sin_family = AF_INET;                 /* Internet Domain */
    destination.sin_port = port;                      /* Server Port */
    destination.sin_addr.s_addr = inet_addr(argv[1]); /* Server's Address   */

    /* connect to server */
    if (connect(s, (struct sockaddr *)&destination, sizeof(destination)) < 0)
    {
        error("ERROR in connect");
    }

    /* send datagram to server */
    if (send(s, buffer, BUFSIZE, 0) < 0)
    {
        error("ERROR in send");
    }

    /* receive datagram from server */
    if (recv(s, buffer, BUFSIZE, 0) < 0)
    {
        error("ERROR in recv");
    }

    /* print received message  */
    printf("%s\n", buffer);

    /* close socket */
    close(s);

    return 0;
}
