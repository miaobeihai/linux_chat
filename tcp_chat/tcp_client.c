#include "func.h"

int main(int argc,char* argv[])
{
	if(argc!=3)
	{
		printf("error args\n");
		return -1;
	}
	int sfd;
	sfd=socket(AF_INET,SOCK_STREAM,0);
	if(-1==sfd)
	{
		perror("socket");
		return -1;
	}
	struct sockaddr_in ser;
	memset(&ser,0,sizeof(ser));
	ser.sin_family=AF_INET;
	ser.sin_port=htons(atoi(argv[2]));//port转网络字节序
	ser.sin_addr.s_addr=inet_addr(argv[1]);//IP地址转网络字节序
	int ret;
	ret=connect(sfd,(struct sockaddr*)&ser,sizeof(ser));
	if(-1==ret)
	{
		perror("connect");
		return -1;
	}
	char buf[50]={0};
	fd_set rdset;
	while(1)
	{
		FD_ZERO(&rdset);//清空集合
		FD_SET(STDIN_FILENO,&rdset);
		FD_SET(sfd,&rdset);
		ret=select(sfd+1,&rdset,NULL,NULL,NULL);
		if(ret>0)
		{
			if(FD_ISSET(STDIN_FILENO,&rdset))
			{
				memset(buf,0,sizeof(buf));
				ret=read(0,buf,sizeof(buf));				
				if(ret<=0)
				{
					printf("byebye\n");
					break;
				}
				ret=send(sfd,buf,strlen(buf)-1,0);
				if(-1==ret)
				{
					perror("send");
					return -1;
				}
			}
			if(FD_ISSET(sfd,&rdset))
			{
				memset(buf,0,sizeof(buf));
				ret=recv(sfd,buf,sizeof(buf),0);
				if(-1==ret)
				{
					perror("recv");
					return -1;
				}else if(0==ret)
				{
					printf("byebye\n");
					break;
				}
				printf("%s\n",buf);
			}
		}
	}
	close(sfd);
	return 0;
}
