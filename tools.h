#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <ctype.h>
#include <arpa/inet.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include <time.h>

#define PORT 9002

#include "sturts.h"


struct login_struct user;
struct route * routes;
int user_index = 0;

void print(int client_id, char * data){
	send(client_id, data, strlen(data)+1, 0);
}

void get_lock(int fd, int whence, int start, int length, int write, struct flock * lock)
{
	lock->l_type = F_RDLCK;
	if(write)
		lock->l_type = F_WRLCK;
	lock->l_whence = whence;
	lock->l_start = start;
	lock->l_len = length;
	lock->l_pid = getpid();
	fcntl(fd, F_SETLKW, lock);
}

void free_lock(int fd, struct flock * lock){
	lock->l_type = F_UNLCK;
	fcntl(fd, F_SETLK, lock);
	close(fd);
}
int cmp(char*a, char*b)
{
    int flag=0,i=0;
    while(a[i]!='\0' &&b[i]!='\0')
    {  
       if(a[i]!=b[i])  
       {  
           flag=1;  
           break; 
       }  
       i++;  
    }  
    if(flag==0)  
    return 0;  
    else  
    return 1;  
}
int random_number(int min_num, int max_num)
        {
        	sleep(1);
            int result=0,low_num=0,hi_num=0;
            if(min_num<max_num)
            {
                low_num=min_num;
                hi_num=max_num+1; // this is done to include max_num in output.
            }else{
                low_num=max_num+1;// this is done to include max_num in output.
                hi_num=min_num;
            }
            srand(time(NULL));
            result = (rand()%(hi_num-low_num-low_num))+low_num;
            return result;
        }
