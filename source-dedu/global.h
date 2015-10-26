#ifndef GLOBAL_H_
#define GLOBAL_H_

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>
#include <dirent.h>
#include <syslog.h>
#include <signal.h>
#include <alloca.h>
#include <errno.h>
#include <ctype.h>
#include <time.h>
#include <netdb.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <pthread.h>
#include <semaphore.h>
#include <arpa/inet.h>
#include <stdarg.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/time.h>
#include <math.h>
#include <linux/sem.h>
#include <netinet/tcp.h> 
#include <openssl/sha.h>
#include <mcheck.h>
#include <sys/sysinfo.h>
#include <sys/select.h>
#include "recipe.h"
#include "client.h"
#include "sha1.h"
#include "rabin.h"
#include "backup.h"

#define	MAXLINE		4096	/* max text line length */
#define	BUFFSIZE	8192	/* buffer size for reads and writes */
#define FILE_MODE (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH) //创建的文件默认访问权限
typedef	void	Sigfunc(int);	/* for signal handlers */

#define	min(a,b)	((a) < (b) ? (a) : (b))
#define	max(a,b)	((a) > (b) ? (a) : (b))

#define	SERV_PORT		 9877			/* TCP and UDP */
#define	SERV_PORT_STR	"9877"			/* TCP and UDP */

/* Following shortens all the typecasts of pointer arguments: */
#define	SA	struct sockaddr
/* Following could be derived from SOMAXCONN in <sys/socket.h>, but many
   kernels still #define it as 5, while actually supporting many more */
#define	LISTENQ		1024	/* 2nd argument to listen() */

//Error processing func prototype;
void err_sys(const char *fmt,...);
void err_quit(const char *fmt,...);

// socket process; implemented in wrapsock.c;

int Accept(int sockfd,struct sockaddr *sa,socklen_t *slenp);
void Bind(int sockfd,const struct sockaddr *sa,socklen_t len);
void Connect(int sockfd,const struct sockaddr *sa,socklen_t len);
void Getpeername(int fd,struct sockaddr *sa,socklen_t *salenp);
void Getsockname(int fd,struct sockaddr *sa,socklen_t *salenp);
void Getsockopt(int fd,int level,int optname,void *optval,socklen_t *optlenp);
void Listen(int fd,int backlog);
ssize_t Recv(int fd,void *ptr,size_t nbytes,int flags);
int Socket(int family,int type,int proto);
void Socketpair(int family,int type,int proto,int *fd);

// IO process;
void  Write(int fd,void *buf,size_t  n);
int Read(int fd,void *buf,size_t n);
ssize_t Readline(int fd,void *,size_t);

Sigfunc *Signal(int, Sigfunc *);

// hiredis usage
void redisInit();
int isExistsFP(char *fp);

#endif
