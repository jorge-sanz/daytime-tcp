// Practica tema 6, Sanz Pérez Jorge

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>

#define BUFSIZE 1024   /* messages buffer size */
#define LISTEN_QUEUE 8 /* maximum number of client connections */

int s;                /* socket for new connections */
int connection;       /* socket for client */
pid_t childpid;       /* process identificator */
char buffer[BUFSIZE]; /* messages buffer */

/* wrapper for perror */
void error(char *msg)
{
    perror(msg);
    exit(1);
}

/* close sockets when a Ctlr+C is received */
void signal_handler(int signal)
{
    int result;

    if (signal == SIGINT)
    {
        if (shutdown(s, SHUT_RDWR) < 0)
        {
            error("ERROR in shutdown\n");
        }

        if ((result = recv(connection, buffer, BUFSIZE, 0)) == -1)
        {
            error("ERROR in recv\n");
        }

        /* close client socket */
        close(s);
        exit(EXIT_SUCCESS);
    }
}

int main(int argc, char **argv)
{
    int port;                          /* port to listen on */
    struct servent *application_name;  /* application (daytime) */
    struct sockaddr_in server_address; /* server structure */
    struct sockaddr_in client_address; /* client structure */
    socklen_t client_length;           /* client structure length */
    char hostname[128];                /* server hostname */
    FILE *file;                        /* temporary file for getting date */
    char response[BUFSIZE];            /* response to client */

    /* check if there is at least one program argument */
    if (argc < 1)
    {
        fprintf(stderr, "usage: %s [-p port-number]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    /* check if usage of port number argument is correct */
    if (argc == 3 && strncmp(argv[1], "-p", 2) != 0)
    {
        fprintf(stderr, "usage: %s [-p port-number]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    /* get port number from arguments */
    if (argc == 3)
    {
        port = htons(atoi(argv[2]));
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
    server_address.sin_family = AF_INET;         /* Internet Domain */
    server_address.sin_port = port;              /* Server Port */
    server_address.sin_addr.s_addr = INADDR_ANY; /* Server's Address   */

    /* bind socket to server structure */
    if (bind(s, (struct sockaddr *)&server_address, sizeof(server_address)) < 0)
    {
        error("ERROR in bind");
    }

    /* listen to new connections. second argument of listen means number of input connections buffer size. */
    if ((listen(s, LISTEN_QUEUE)) < 0)
    {
        error("ERROR in listen");
    }

    printf("Server running... Waiting for connections.\n");

    /* infinite loop. ends with Ctrl+C signal */
    while (1)
    {
        /* stop signal detection */
        if (signal(SIGINT, signal_handler) == SIG_ERR)
        {
            error("ERROR in signal");
        }

        /* accept new connection */
        client_length = sizeof(client_address);
        if ((connection = accept(s, (struct sockaddr *)&client_address, &client_length)) < 0)
        {
            error("ERROR in accept");
        }

        /* child created for dealing with client request */
        if ((childpid = fork()) == 0)
        {
            printf("Child created for dealing with client request.\n");

            /* get server hostname */
            gethostname(hostname, sizeof hostname);
            printf("My hostname: %s\n", hostname);

            /* get and print current daytime */
            system("date > /tmp/tt.txt");
            bzero(buffer, BUFSIZE);
            file = fopen("/tmp/tt.txt", "r");
            if (fgets(buffer, BUFSIZE, file) == NULL)
            {
                error("ERROR in system(), in fopen(), or in fgets()");
            }
            printf("Date: %s", buffer);

            /* concat server name + date */
            strcpy(response, hostname);
            strcat(response, ": ");
            strcat(response, buffer);
            printf("%s\n", response);

            /* send response */
            if (send(connection, response, BUFSIZE, 0) < 0)
            {
                error("ERROR in send");
            }

            /* shutdown and close child socket */
            if (shutdown(connection, SHUT_RDWR) < 0)
            {
                error("ERROR in child shutdown");
            }
            close(connection);
            /* kill child process */
            exit(0);
        }
    }
}
