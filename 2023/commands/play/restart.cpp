#include<stdio.h>
#include<stdlib.h>   // exit
#include<fcntl.h>    // O_WRONLY
#include<sys/stat.h>
#include<time.h>     // time
#include<unistd.h>
#include<string.h>

int main(int argc, char *argv[])
{
	int fd;
	int n, i;
	char buf[1024];

	printf("I am %d process.\n", getpid()); // 说明进程ID

	if((fd = open("fifo1", O_WRONLY)) < 0) // 以写打开一个FIFO 
	{
		perror("Open FIFO Failed");
		exit(1);
	}
    
    n=sprintf(buf,"Process %d, restart",getpid());
    printf("Send message: %s", buf);
    if(write(fd, buf, n+1) < 0)
    {
        perror("Write FIFO Failed");
        close(fd);
        exit(1);
    }
    // sleep(1);  // 休眠1秒
	close(fd);  // should be comment in formal version
	return 0;
}