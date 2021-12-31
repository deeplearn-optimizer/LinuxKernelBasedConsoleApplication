
void start_transfer(int socket_id){
	char buffer[1024];
	while(read(socket_id,buffer,1024)>0){
		int pass = 0;
		if(cmp(buffer, "Listing") == 0){
			while(read(socket_id,buffer,1024)>0)
			{
				if(!cmp(buffer, "END"))
					break;
				printf("%s", buffer);
			}
		}
		else if(cmp("shutdown",buffer) == 0){
			close(socket_id);
			exit(0);
		}
		else{
			if(strstr(buffer, "password") != NULL)
				pass = 1;
			if(!pass)
				printf("%s", buffer);
		}
		if(!pass){
			Label:
			scanf("%s", buffer);
			if(strlen(buffer) == 0){
				printf("Input cannot be left empty!");
				goto Label;
			}
			send(socket_id,buffer,strlen(buffer)+1,0);
		}
		else{
			char * password = getpass(buffer);
			send(socket_id,password,strlen(password)+1,0);
		}
	}
}
int server_connect()
{
    int sock = 0, valread;
    struct sockaddr_in serv_addr;
    
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        printf("\n Socket creation error \n");
        exit(0);
    }
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
       
    if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0){
        printf("\nInvalid address/ Address not supported \n");
        exit(0);
    }
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0){
        printf("\nConnection Failed \n");
	exit(0);
    }
    return sock;
}
