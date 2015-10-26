#include	"../unp.h"

/* maybe return the list of fp indicating the chunk needed to transfer to server TODO
 * here just a whole file ,so i return 0/1 
 * bug 1: Use read() to get reply from serer NOT readline() ...
 * bug 2: buffer should be memset to 0
 */
void inquiry_by_fp(int sockfd, char *fp) {
	char	recvline[MAXLINE];
	char *sendline = "aaf4c61ddcc5e8a2dabede0f3b482cd9aea9434d Test";
	
	Write(sockfd, sendline, strlen(sendline));

	if (Read(sockfd, recvline, MAXLINE) == 0)
		err_quit("inquiry_by_fp: server terminated prematurely");

	if(strcmp(recvline, "yes") == 0){
		printf("%s\n", "File duplicated, so stop transfer");
	}else{
		printf("%s\n", "File new, so need transfer");
	}
}

void
transfer_file(int sockfd, int filefd)
{
	char	recvline[MAXLINE];
	char *sendline = "file chunk data";

	int chunk_count = 4, i = 0;

	for(i = 0;i < chunk_count; i++){
		
		Write(sockfd, sendline, strlen(sendline));
		if (Read(sockfd, recvline, MAXLINE) == 0)
			err_quit("transfer_file: server terminated prematurely");

		printf("%s\n", recvline);

		memset(recvline, 0, sizeof(recvline));
	}

	// eof 
	sendline = "exit";
	Writen(sockfd, sendline, strlen(sendline));
	if (Read(sockfd, recvline, MAXLINE) == 0)
		err_quit("transfer_file: server terminated prematurely");

	printf("%s\n", recvline);
}

int
main(int argc, char **argv)
{
	int					sockfd, sockfd2;
	struct sockaddr_in	servaddr;

	if (argc != 2)
		err_quit("usage: tcpcli <IPaddress>");

	// 1. inquiry the server if the file is duplicated by fingerprint (a RTT)

	sockfd = Socket(AF_INET, SOCK_STREAM, 0);

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(SERV_PORT+1); //
	Inet_pton(AF_INET, argv[1], &servaddr.sin_addr);

	Connect(sockfd, (SA *) &servaddr, sizeof(servaddr));

	inquiry_by_fp(sockfd, "not_used");		/* do it all */

	close(sockfd); // yes close sockfd

	// 2. transfer not duplicated chunks to server 
	sockfd2 = Socket(AF_INET, SOCK_STREAM, 0);

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(SERV_PORT); //
	Inet_pton(AF_INET, argv[1], &servaddr.sin_addr);

	Connect(sockfd2, (SA *) &servaddr, sizeof(servaddr));

	transfer_file(sockfd2, 0);

	close(sockfd2);

	exit(0);
}
