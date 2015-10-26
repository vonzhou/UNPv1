#include	"global.h"

/* maybe return the list of fp indicating the chunk needed to transfer to server TODO
 * here just a whole file ,so i return 0/1 
 * bug 1: Use read() to get reply from serer NOT readline() ...
 * bug 2: buffer should be memset to 0
 *
 * client 02
 * add file fp
 * transfer file by fixed chunk
 * 
 */

#define CHUNK_SIZE 1400

// use for file chunk and sha1
FileInfo *fi;

int isExistsFile(int sockfd, char *fp_filename) {
	char	recvline[MAXLINE];
	
	Write(sockfd, fp_filename, strlen(fp_filename));

	if (Read(sockfd, recvline, MAXLINE) == 0)
		err_quit("client: server terminated prematurely");

	if(strcmp(recvline, "yes") == 0){
		printf("**File duplicated, so stop transfer**\n");
		return 1;
	}else{
		printf("**File new, so need transfer**\n");
		return 0;
	}
}

void
transfer_file(int sockfd, char *fp_filename, char *filename){
	char	recvline[MAXLINE];
	char buf[CHUNK_SIZE];
	char *sendline = "file chunk data";
	int fd, res;
	

	int chunk_count = 4, i = 0;
	FILE *fs = fopen(filename, "r");

	// 1. transfer fp+file name
	// printf("%s\n", fp_filename);
	Write(sockfd, fp_filename, strlen(fp_filename));
	// fp+file name should be a separate packet
	if (Read(sockfd, recvline, MAXLINE) == 0)
			err_quit("transfer_file: server terminated prematurely");

	// 2. then transfer the whole file chunkly
	while((res = fread(buf, sizeof(char), CHUNK_SIZE, fs)) > 0){
		Write(sockfd, buf, res);
		// we DO NOT need reply
		memset(buf, 0, CHUNK_SIZE);
	}

	// 3. eof it
	sendline = "exit";
	Write(sockfd, sendline, strlen(sendline));
	
	fclose(fs);
}

int
main(int argc, char **argv)
{
	int					sockfd, sockfd2;
	struct sockaddr_in	servaddr;
	// For simple, I use 40B hex string instead
	char hash[41];
	int res = 0;
	char fp_filename[MAXLINE];

	if (argc != 3)
		err_quit("usage: client <IPaddress> <file>");

	// 1. inquiry the server if the file is duplicated by fingerprint (a RTT)
	fi = file_new();
	strcpy(fi->file_path, argv[2]);
	// file's sha1 
	res = SHA1File(fi->file_path, fi->file_hash);
	if(res){
		printf("Get file sha1 hash failed.\n");
		exit(-1);
	}
	digestToHash(fi->file_hash, hash); // rabin.h 
	fp_filename[0] = '\0';
	strcat(fp_filename, hash);
	strcat(fp_filename, argv[2]);
	
	sockfd = Socket(AF_INET, SOCK_STREAM, 0);

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(SERV_PORT+1); //
	Inet_pton(AF_INET, argv[1], &servaddr.sin_addr);

	Connect(sockfd, (SA *) &servaddr, sizeof(servaddr));

	res = isExistsFile(sockfd, fp_filename);		/* fp match with server */

	close(sockfd); // yes close sockfd

	if(res){ // file existed , stop
		exit(0);
	}
		

	// 2. transfer not duplicated chunks to server 
	sockfd2 = Socket(AF_INET, SOCK_STREAM, 0);

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(SERV_PORT); //
	Inet_pton(AF_INET, argv[1], &servaddr.sin_addr);

	Connect(sockfd2, (SA *) &servaddr, sizeof(servaddr));

	transfer_file(sockfd2, fp_filename, argv[2]);

	close(sockfd2);

	exit(0);
}
