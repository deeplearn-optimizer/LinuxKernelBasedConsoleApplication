#include<unistd.h>
#include<stdio.h>
#include<fcntl.h>
#include "sturts.h"
#include<string.h>


int main(){
	struct login_struct data;
	data.user_id = 1;
	strcpy(data.username, "admin");
	strcpy(data.password, "admin");
	data.approved = 1;
	data.admin = 1;
	data.agent = 0;
	data.logged_in = 0;
	int fd = open("login.bin", O_RDWR|O_CREAT, 0777);
	write(fd,&data, sizeof(data));
	close(fd);
	return 0;
}
