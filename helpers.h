int valid_user(char * username, char * password){
	int fd = open("login.bin", O_RDWR);
	if(fd < 0)
		return -1;
	int size = sizeof(struct login_struct);
	struct login_struct data;
	int index = 0;
	struct flock lock;
	get_lock(fd,0,0,0,0,&lock);
	while(read(fd, &data, sizeof(data))){
		if(!cmp(username, data.username) && !cmp(password,data.password)){
			user = data;
			free_lock(fd, &lock);
			return index;
		}
		index++;
	}
	free_lock(fd, &lock);
	return -1;
}

int login(int client_id)
{
	char username[100] = {0};
	char password[100] = {0};
	char buffer[1024] = {0};
	print(client_id, "please enter your username : ");
	read(client_id, username, sizeof(username));
	print(client_id, "please enter your password : ");
	read(client_id, password, sizeof(password));
	int index = valid_user(username, password);
	if(index == -1){
		print(client_id, "INVALID USER ID/PASSWORD\n\nSELECT ONE OPTION\n1 FOR LOGIN\n2 FOR SIGN UP\n3 FOR EXIT\n");
		return -1;
	}
	else if(user.approved == 0)
	{
		print(client_id, "YOU HAVEN'T BEEN APPROVED TO LOGIN!\n\nSELECT ONE OPTION\n1 FOR LOGIN\n2 FOR SIGN UP\n3 FOR EXIT\n");
		return -1;	
	}
	return index;
}

void create_user(int client_id, struct login_struct * data)
{
	char * username = data->username;
	char * password = data->password;
	int index = valid_user(username,password);
	if(index != -1)
	{
		print(client_id, "USER ALREADY EXIST, Enter diffirent credentials\nSELECT ONE OPTION\n1 FOR LOGIN\n2 FOR SIGN UP\n3 FOR EXIT\n");
		return;
	}
	int fd = open("login.bin", O_RDWR|O_CREAT, 0777);
	if(fd < 0){
		print(client_id, "cannot create user, please try again later!\nSELECT ONE OPTION\n1 FOR LOGIN\n2 FOR SIGN UP\n3 FOR EXIT\n");
		return;
	}
	struct flock lock;
	get_lock(fd,0,0,0,1,&lock);
	lseek(fd, 0, SEEK_END);
	write(fd, data, sizeof(*data));
	free_lock(fd, &lock);
	print(client_id, "SELECT ONE OPTION\n1 FOR LOGIN\n2 FOR SIGN UP\n3 FOR EXIT\n");
}
void signup(int client_id, int admin, int agent)
{
	char username[100] = {0};
	char password[100] = {0};
	char buffer[1024] = {0};
	int user_id = random_number(1, 100000);
	struct login_struct data;
	print(client_id, "please enter your username : ");
	read(client_id, data.username, sizeof(data.username));
	print(client_id, "please enter your password : ");
	read(client_id, data.password, sizeof(data.password));
	data.admin = admin;
	data.agent = agent;
	data.user_id = user_id;
	data.approved = 0;
	data.logged_in = 0;
	create_user(client_id, &data);
	return;	
}
void handle(int client_id)
{
    char buffer[1024] = {0};
    print(client_id,"\t\t\tWELCOME TO GOLDEN CHARIOT\n\nSELECT ONE OPTION\n1 FOR LOGIN\n2 FOR SIGN UP\n3 FOR EXIT\n");
    int index = -1;
    while(1){
    	    char option[10];
	    read(client_id, option, sizeof(option));
    	    int type = atoi(option);
	    switch(type)
	    {
	    	case 1:
	    		index = login(client_id);
	    		if(index != -1){
	    			user_index = index;
	    			
	    			if((user.admin == 0) && (user.agent == 0) && (user.logged_in != 0))
	    			{
	    				print(client_id, "PLEASE LOG OUT FROM OUR PREVIOUS SESSION AND TRY AGAIN\n1 FOR LOGIN\n2 FOR SIGN UP\n3 FOR EXIT\n");
	    			}
	    			else
	    			{
		    			int fd = open("login.bin", O_RDWR);
					struct flock lock;
					lseek(fd, user_index * sizeof(user), SEEK_SET);
					user.logged_in = 1;
					write(fd, &user, sizeof(user));
		    			handle_session(client_id);
		    			print(client_id, "SELECT ONE OPTION\n1 FOR LOGIN\n2 FOR SIGN UP\n3 FOR EXIT\n");
	    			}
	    		}		
	    	break;
	    	case 2:
	    		print(client_id,"Sign UP as ?\nPRESS 1 FOR ADMIN\nPRESS 2 FOR AGENT \nPRESS 3 FOR USER\n");
	    		int a = 0;
	    		char buff[10];
	    		read(client_id, buff, sizeof(buff));
	    		a = atoi(buff);
	 		switch(a){
	 			case 1:
	 				signup(client_id,1,0);	
	 			break;
	 			case 2:
	 				signup(client_id,0,1);	
	 			break;
	 			case 3:
	 				signup(client_id,0,0);
	 			break;
	 			default:
	 				print(client_id, "SELECT ONE OPTION\n1 FOR LOGIN\n2 FOR SIGN UP\n3 FOR EXIT\n");
	 			break;
	 		}
	    	break;
	    	case 3:
	    		print(client_id,"shutdown");
	    	break;
	    	default:
	    		print(client_id,"please enter a valid request\nSELECT ONE OPTION\n1 FOR LOGIN\n2 FOR SIGN UP\n3 FOR EXIT\n");
	    }
    }
}


