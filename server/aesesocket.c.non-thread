/*
** server.c -- a stream socket server demo
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <stdbool.h>
#include <syslog.h>
#include <fcntl.h>

#define LISTEN_PORT "9000"  
#define SAVE_FILE "/var/tmp/aesdsocketdata"
#define MAX_BUF 1024
#define MAX_PACKET_BUF 65000

#define BACKLOG 10	 // how many pending connections queue will hold

int fw = -1, fr = -1;

/* Signal Handlers */

static void exit_signal_handler (int signo) 
{
	syslog(LOG_DEBUG, "Caught signal, exiting");

	// clean up
	if (fr != -1) close(fr);
	if (fw != -1) close(fw);
	remove(SAVE_FILE); 

	//
	if (signo == SIGINT) 
		printf("Exit by SIGINT\n");
	else if (signo == SIGTERM)
		printf("Exit by SIGTERM\n");

	exit (EXIT_SUCCESS);
}

void sigchld_handler(int s)
{
	(void)s; // quiet unused variable warning

	// waitpid() might overwrite errno, so we save and restore it:
	int saved_errno = errno;

	while(waitpid(-1, NULL, WNOHANG) > 0);

	errno = saved_errno;
}

void catch_signals() 
{
	struct sigaction sa;

	// SIGINT, SIGTERM handler
	if (signal(SIGINT, exit_signal_handler) == SIG_ERR) {
		fprintf(stderr, "Cannot handle SIGINIT!\n");
		exit(-1);
	}

	if (signal(SIGTERM, exit_signal_handler) == SIG_ERR) {
		fprintf(stderr, "Cannot handle SIGTERM!\n");
		exit(-1);
	}

	// SIGCHLD handler
	sa.sa_handler = sigchld_handler; // reap all dead processes 
	sigemptyset(&sa.sa_mask); 
	sa.sa_flags = SA_RESTART; 

	if (sigaction(SIGCHLD, &sa, NULL) == -1) { 
		perror("sigaction"); 
		exit(-1);
	} 

}

/* Socket Utilities */

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}

	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

bool get_address_infos (struct addrinfo **infos)
{
	struct addrinfo hints;
	int rv;

	// reset hints
	memset(&hints, 0, sizeof(struct addrinfo));

	// get addresses for binding a socket to and accept connections
	//   node == NULL, hits.ai_flags == AI_PASSIVE
    // if node != NULL, AI_PASSIVE is ignored

	hints.ai_family   = AF_UNSPEC;   // any of IF_INET, AF_INET6, etc
	hints.ai_socktype = SOCK_STREAM; // TCP
	hints.ai_flags    = AI_PASSIVE;  // use my ip

    // lookup and make binary address structure   
    rv = getaddrinfo(NULL, LISTEN_PORT, &hints, infos);

	if (rv != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return false;
	} else {
		return true;
	}
}

bool bind_socket(int* fdp) {

	struct addrinfo *infos, *info;
	int yes = 1;
	int fd = -1;

    // get address infos
    if(!get_address_infos(&infos)) return false;

	// loop through all the results and bind to the first we can

	for(info = infos; info != NULL; info = info->ai_next) {

		// open socket 
		fd = socket(info->ai_family, info->ai_socktype, info->ai_protocol);

		if (fd == -1) {
			perror("server: socket");
			continue;
		}

		// set socket behaviour
		if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
			perror("setsockopt");
			exit(-1);
		} 

		// bind
		if (bind(fd, info->ai_addr, info->ai_addrlen) == -1) { 
			close(fd); 
			perror("server: bind"); 
			continue; 
		} 

		(*fdp) = fd;

		break; 
	}

	freeaddrinfo(infos); // all done with this structure 

	if (info == NULL) 
		return false;
	else 
		return true;
}


bool save_to_file(char* packet, int size) {

	fw = open(SAVE_FILE, O_CREAT | O_WRONLY | O_APPEND, S_IRWXU | S_IRWXG | S_IRWXO);

	if (fw == -1) {
		perror("open");
	}

	ssize_t nr = write(fw, packet, size);

	close(fw);

	fw = -1; // reset -- for signal handler

	if (nr == -1) {
		perror("write");
		return false;
	} else {
		return true;
	}
}

void accept_loop(int fd_server)
{
    int fd_client;
	struct sockaddr_storage client_addr; 
	socklen_t sin_size = sizeof(struct sockaddr_storage);;
	char s[INET6_ADDRSTRLEN];
	char packet_buf[MAX_PACKET_BUF], recv_buf[MAX_BUF];
	int n_packet = 0, n_recv;
	int i, pos_newline, rc, n_read;

	printf("server: waiting for connections...\n");

	while(1) {  
		fd_client = accept(fd_server, (struct sockaddr *)&client_addr, &sin_size); 

		printf("new connection: %d\n", fd_client);

		if (fd_client == -1) { 
			perror("accept"); 
			continue; 
		} 

		// log
		inet_ntop(client_addr.ss_family, get_in_addr((struct sockaddr *)&client_addr), s, sizeof s); 
		printf("server: got connection from %s\n", s); 
		syslog(LOG_DEBUG, "Accepted connection from %s", s);

		/* 
		if (!fork()) { // this is the child process 
			// Start of Child Process 

			close(fd_server); // child doesn't need the listener 

			if (send(fd_client, "Hello, world!", 13, 0) == -1) 
				perror("send"); 

			close(fd_client); 
			exit(0); 

			//
		} 
		*/

		while(1) {
			n_recv = recv(fd_client, recv_buf, MAX_BUF, 0);

			// socket closed
			if (n_recv == 0) {
				printf("server: closed connection from %s\n", s); 
				syslog(LOG_DEBUG, "Closed connection from %s", s);
				break;
			}

			// check buffer full
			if (n_packet + n_recv > MAX_PACKET_BUF) {
				printf("Buffer full. Packet is discarded\n");
				n_packet = 0;
				continue;
			} 

			// copy to master buffer
			memcpy(packet_buf + n_packet, recv_buf, n_recv);
			n_packet += n_recv;

			printf("%d characters received, total = %d\n", n_recv, n_packet);

			// check packet ending
			pos_newline = -1;

			for(i = 0; i < n_packet; i++) {

			    //printf("[%d]%c", i, packet_buf[i]);

				if (packet_buf[i] == '\n') {
					pos_newline = i;
					break;
				}
			}

			// packet completed
			if(pos_newline >= 0) {
				// save
				save_to_file(packet_buf, pos_newline+1); // include newline

				if (n_packet > pos_newline+1) {
					memcpy(packet_buf, packet_buf + pos_newline+1, n_packet - pos_newline - 1);
					n_packet = n_packet - pos_newline - 1;
				} else {
					n_packet = 0;
				}

				//if (send(fd_client, "Hello, world!", 13, 0) == -1) 
				//	perror("send"); 
				// send the whole content

				fr = open(SAVE_FILE, O_RDONLY);

				if (fr == -1) {
				    perror("open - rd");
					exit(-1);
				}
				
				while(1) {
					n_read = read(fr, recv_buf, MAX_BUF);

					if (n_read == -1) {
						perror("read");
						break;
					}

					if (n_read > 0) {
						rc = send(fd_client, recv_buf, n_read, 0);
						if (rc == -1) {
							perror("send");
							break;
						}
					} else {
						break;
					}
				}
				close(fr);

				fr = -1; // for signal handler
			}
		}

		close(fd_client);  // parent doesn't need this 
	} 
}

int main(int argc, char *argv[])
{
	int fd;
	pid_t pid;

	// syslog
	openlog("Assignment5", LOG_NDELAY, LOG_USER);

	// bind socket
	if(!bind_socket(&fd)) {
		fprintf(stderr, "server: failed to bind\n"); 
		exit(-1); 
	}

	// make it a daemon
	if (argc == 2 && strcmp(argv[1], "-d") == 0) {

		printf("Daemon mode\n");

		pid = fork();

		if (pid < 0) {
			// parent exit with failure
			perror("fork");
			exit(EXIT_FAILURE);
		}
		if (pid > 0) {
			// kill the parent
			exit(EXIT_SUCCESS);
		}
		// pid == 0, child process continues to run
	}

	// listen
	if (listen(fd, BACKLOG) == -1) { 
		perror("listen"); 
		exit(-1); 
	} 

	//
	catch_signals();

	// accept loop
	accept_loop(fd);

	return 0;
}
