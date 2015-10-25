#include	"../unp.h"

/*
 * Get file fingerprint from client
 * see if the file is duplicated
 * reply with yes/no  
 */
void recv_fp(int sockfd) {
	ssize_t		n;
	char		buf[MAXLINE];
	char *res = "no";

	n = read(sockfd, buf, MAXLINE);

	// look up this fp for dedu

	// printf("%s\n", buf);

	Write(sockfd, res, strlen(res));
}

void
recv_chunk(int sockfd)
{
	ssize_t		n;
	char		buf[MAXLINE];
	char *res = "data recv done";

	// recv the data chunk and store 
	while ( (n = read(sockfd, buf, MAXLINE)) > 0){
		if(strcmp(buf, "exit") == 0){
			res = "exit";
			Writen(sockfd, buf, strlen(res));
			printf("%s %s\n", "while:", "recv_chunk end");
			break;
		}else{
			// store this chunk and index 
			printf("%s\n", "chunk recevied and store...");
			// if need reply to the client ?
			res = "chunk stored";
			Writen(sockfd, res, strlen(res));
		}
		memset(buf, 0, sizeof(buf));
	}
}

void
str_echo2(int sockfd)
{
	ssize_t		n;
	char		buf[MAXLINE];

again:
	while ( (n = read(sockfd, buf, MAXLINE)) > 0)
		Writen(sockfd, buf, n);

	if (n < 0 && errno == EINTR)
		goto again;
	else if (n < 0)
		err_sys("str_echo: read error");
}


int
main(int argc, char **argv)
{
	int					listenfd, listenfd2, connfd, udpfd, nready, maxfdp1;
	char				mesg[MAXLINE];
	pid_t				childpid;
	fd_set				rset;
	ssize_t				n;
	socklen_t			len;
	const int			on = 1;
	struct sockaddr_in	cliaddr, servaddr;
	void				sig_chld(int);

	/* create listening TCP socket : fingerprint packet recv */
	listenfd = Socket(AF_INET, SOCK_STREAM, 0);

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family      = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port        = htons(SERV_PORT+1);

	Setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
	Bind(listenfd, (SA *) &servaddr, sizeof(servaddr));

	Listen(listenfd, LISTENQ);

	/* create listening TCP socket : other packet */
	listenfd2 = Socket(AF_INET, SOCK_STREAM, 0);

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family      = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port        = htons(SERV_PORT);

	Setsockopt(listenfd2, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
	Bind(listenfd2, (SA *) &servaddr, sizeof(servaddr));

	Listen(listenfd2, LISTENQ);

	Signal(SIGCHLD, sig_chld);	/* must call waitpid() */

	FD_ZERO(&rset);
	maxfdp1 = max(listenfd, listenfd2) + 1;
	for ( ; ; ) {
		FD_SET(listenfd, &rset);
		FD_SET(listenfd2, &rset);
		if ( (nready = select(maxfdp1, &rset, NULL, NULL, NULL)) < 0) {
			if (errno == EINTR)
				continue;		/* back to for() */
			else
				err_sys("select error");
		}

		if (FD_ISSET(listenfd, &rset)) {
			len = sizeof(cliaddr);
			connfd = Accept(listenfd, (SA *) &cliaddr, &len);
	
			if ( (childpid = Fork()) == 0) {	/* child process */
				Close(listenfd);	/* close listening socket */
				recv_fp(connfd);	/* process the request : fp packet */
				exit(0);
			}
			
			Close(connfd);			/* parent closes connected socket */
		}

		if (FD_ISSET(listenfd2, &rset)) {
			len = sizeof(cliaddr);
			connfd = Accept(listenfd2, (SA *) &cliaddr, &len);
	
			if ( (childpid = Fork()) == 0) {	/* child process */
				Close(listenfd2);	/* close listening socket */
				recv_chunk(connfd);	/* process the request */
				exit(0);
			}
			
			Close(connfd);			/* parent closes connected socket */
		}
	}
}
