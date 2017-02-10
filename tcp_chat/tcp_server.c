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
	ret=bind(sfd,(struct sockaddr*)&ser,sizeof(ser));//绑定ip地址和端口号
	if(-1==ret)
	{
		perror("bind");
		return -1;
	}
	listen(sfd,10);
	struct sockaddr_in cli;
	memset(&cli,0,sizeof(cli));
	char buf[50]={0};
	fd_set rdset,tmpset;
	int new_fd=-1;
	FD_ZERO(&tmpset);
	FD_SET(STDIN_FILENO,&tmpset);
	FD_SET(sfd,&tmpset);
	while(1)
	{
		FD_ZERO(&rdset);//清空集合
		memcpy(&rdset,&tmpset,sizeof(fd_set));	
		ret=select(13,&rdset,NULL,NULL,NULL);
		if(ret>0)
		{
			if(FD_ISSET(sfd,&rdset))
			{
				int addrlen=sizeof(cli);
				new_fd=accept(sfd,(struct sockaddr*)&cli,&addrlen);
				if(-1==new_fd)
				{
					perror("accept");
					return -1;
				}
				printf("client ip=%s,client port=%d\n",inet_ntoa(cli.sin_addr),ntohs(cli.sin_port));
				FD_SET(new_fd,&tmpset);				
			}
			if(FD_ISSET(new_fd,&rdset))
			{
				memset(buf,0,sizeof(buf));
				ret=recv(new_fd,buf,sizeof(buf),0);
				if(-1==ret)
				{
					perror("recv");
					return 0;
				}else if(0==ret)
				{
					printf("byebye\n");
					close(new_fd);
					FD_CLR(new_fd,&tmpset);
					continue;	
				}
				printf("%s\n",buf);
			}
			if(FD_ISSET(STDIN_FILENO,&rdset))
			{
				memset(buf,0,sizeof(buf));
				ret=read(STDIN_FILENO,buf,sizeof(buf));
				if(ret<=0)
				{
					printf("byebye\n");
					close(new_fd);
					FD_CLR(new_fd,&tmpset);
					continue;
				}
				ret=send(new_fd,buf,strlen(buf)-1,0);
				if(-1==ret)
				{
					perror("send");
					return -1;
				}
			}
		}
	}
	close(sfd);
	return 0;
}
