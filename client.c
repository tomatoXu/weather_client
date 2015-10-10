#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>

#define MAXLINE 4096

#define SERV_PORT 4321
#define MAX_CITY 29
#define SEND_SIZE 23

void output(unsigned char *recvline)
{
	char city[MAX_CITY + 1], tianqi[] [7] = {"shower", "clear", "cloudy", "rain", "fog"};
	strncpy(city, recvline + 2, MAX_CITY);
	int i = 0;
	if (recvline[0] == 0x03){
		printf(" City: %s",city);
		printf(" Today is: %d/%02d/%02d ", recvline[22] * 0x100 +recvline[23], recvline[24], recvline[25]);
		printf("  Weather information is as follows:\n");
		if (recvline[1] == 0x41){
			if (recvline[26] == 0x01) printf(" Today's ");
			else if (recvline[26] >= 0x02 && recvline[26] <= 0x09) printf(" The %dth day's ", recvline[26]);
			else {printf("error\n"); return;}
			if (recvline[27] >= 0 && recvline[27] <=100) printf(" weather is :%s ;",tianqi[(int)recvline[27]]);
			else printf(" error\n");
			printf(" Temp: %02d \n", (int)recvline[28]);
		}
		else if (recvline[1] == 0x42){
			if ( recvline[26] != 0x03 ) printf ( " error \n " ) ;
                        printf("%s\n",tianqi[recvline[27]-'0']);
                        printf ( " The 1th day's weather is:%s ; Temp :%02d\n", tianqi[(int)recvline[27]] , (int)recvline[28]) ;
                        printf ( " The 2th day's weather is:%s ; Temp :%02d\n", tianqi[(int)recvline[29]] , (int)recvline[30]) ;
                        printf ( " The 3th day's weather is:%s ; Temp :%02d\n", tianqi[(int)recvline[31]] , (int)recvline[32]) ;

		}
		else printf(" error \n");
	}
	else if ( recvline[0] ==0x04 ) printf ( "Sorry , no given day's weather information for city %s!\n " , city ) ;
}

void send_recv ( char *sendline, char *recvline , int sockfd )
{
        send ( sockfd , sendline ,SEND_SIZE ,0) ;
        if ( recv( sockfd , recvline , MAXLINE , 0) == 0 )
        {
                perror ( " The server terminated prematurely " );
                exit (3) ;
        }
}

int main(int argc, char **argv){
	int sockfd;
	struct sockaddr_in servaddr;
	char sendline[MAXLINE], recvline[MAXLINE], buffer[MAXLINE];

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = inet_addr("114.212.191.33");
	servaddr.sin_port = htons(SERV_PORT);
	connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr));
	system("clear");

	while (1){
		memset(sendline, 0, MAXLINE);
		memset(recvline, 0, MAXLINE);
		printf("Welcome to NJUCS Weather Forecast Demo Program!\n");
		printf("Please input City Name in Chinese pinyin(e.g. nanjing or beijing)\n");
		printf("(c)cls,(#)exit\n");
		scanf("%s", buffer);
		if (buffer[0] == '#' && buffer[1] == '\0')
			exit(0);
		if (buffer[0] == 'c' && buffer[1] == '\0'){
			system("clear");
			continue;
		}
		sendline[0] = 0x01;
		sendline[1] = 0x00;

		strncpy(&sendline[2], buffer, MAX_CITY);
		send_recv(sendline, recvline, sockfd);
		if(recvline[0] == 0x02)
			printf(" Sorry, Server does not have weather information for city %s!\n", buffer);
		else{
			system("clear");
			printf(" Please enter the given number to query\n");
			printf(" 1.today\n 2.three days from today\n 3.custom day by yourself\n(r)back,(c)cls,(#)exit\n");
			printf("==============================================\n");
			while(1){
				scanf("%s", buffer);
				if (buffer[1] == '\0'){
					if (buffer[0] == '#') exit(0);
					else if (buffer[0] == 'c'){
						system("clear");
                        			printf(" Please enter the given number to query\n");
                        			printf(" 1.today\n 2.three days from today\n 3.custom day by yourself\n(r)back,(c)cls,(#)exit\n");
                        			printf("==============================================\n");
					}
					else if (buffer[0] == 'r') {system("clear"); break;}
					else if (buffer[0] == '1'){
						sendline[0] = 0x02;
						sendline[1] = 0x01;
						sendline[22] = 1;
						send_recv(sendline, recvline, sockfd);
						output(recvline);
					}
					else if (buffer[0] == '2'){
						sendline[0] = 0x02;
						sendline[1] = 0x02;
						sendline[22] = 3;
						send_recv(sendline, recvline, sockfd);
						output(recvline);
					}
					else if (buffer[0] == '3'){
						int flag = 0;
						while(1){
							printf(" Please enter the day number(below 10,e.g. 1 means today)");
							if (flag == 0) fgets(buffer, MAXLINE, stdin);
							fgets(buffer, MAXLINE, stdin);
							if (buffer[1] != '\n' || buffer[0] < '1' || buffer[0] > '9'){
								printf("input error\n");
								flag++;
							}
							else{
								sendline[0] = 0x02;
								sendline[1] = 0x01;
								sendline[22] = buffer[0] - '0';
								send_recv(sendline, recvline, sockfd);
								output(recvline);
								break;
							}
						}
					}
					else printf(" Input error\n");
				}
				else printf(" input error!\n");
			}
		}
	}
	exit(0);
}
