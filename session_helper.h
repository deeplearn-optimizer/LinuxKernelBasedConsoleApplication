void logout(int client)
{
	int fd = open("login.bin", O_RDWR);
	struct flock lock;
	lseek(fd, user_index * sizeof(user), SEEK_SET);
	user.logged_in = 0;
	write(fd, &user, sizeof(user));	
	close(fd);
	return;
}
void print_this(int client_id,char * message)
{
	char buffer[1000];
	if(!user.admin)
		sprintf(buffer,"%s\nWHAT WOULD YOU LIKE TO PROCEED WITH?\n1 FOR BOOKING\n2 FOR UPDATE/CANCEL YOUR BOOKING INFO\n3 FOR VIEW PREVIOUS BOOKING\nPRESS 4 FOR LOG OUT\n",message);
	else
		sprintf(buffer,"%s\nWHAT WOULD YOU LIKE TO PROCEED WITH?\nPRESS 4 ADD NEW ROUTES\nPRESS 5 UPDATE/DELETE NEW ROUTES\nPRESS 6 SEE ALL USERS\nPRESS 7 VIEW BOOKINGS\nPRESS 8 VIEW ALL ROUTES\nPRESS 9 FOR APPROVE/DISAPPROVE USER\nPRESS 10 FOR LOG OUT\n",message);
	print(client_id, buffer);
}

void print_dis(int client_id,char * message)
{
	char buffer[1000];
	if(!user.admin)
		sprintf(buffer,"%s\nWHAT WOULD YOU LIKE TO PROCEED WITH?\n1 FOR BOOKING\n2 FOR UPDATE/CANCEL YOUR BOOKING INFO\n3 FOR VIEW PREVIOUS BOOKING\nPRESS 4 FOR LOG OUT\n",message);
	else
		sprintf(buffer,"%s\nWHAT WOULD YOU LIKE TO PROCEED WITH?\nPRESS 4 ADD NEW ROUTES\nPRESS 5 UPDATE/DELETE NEW ROUTES\nPRESS 6 SEE ALL USERS\nPRESS 7 VIEW BOOKINGS\nPRESS 8 VIEW ALL ROUTES\nPRESS 9 FOR APPROVE/DISAPPROVE USER\nPRESS 10 FOR LOG OUT\n",message);
	print(client_id, buffer);
}

void add_route_seats(int train_no, int seats)
{
	int fd = open("routes.bin",O_RDWR);
	struct flock lock;
	get_lock(fd,0,0,0,1,&lock);
	struct route * arr = (struct route *)malloc(sizeof(struct route)*100);
	int index = 0;
	while(read(fd, arr, sizeof(struct route)) > 0){
		if(arr->train_no == train_no)
		{
			lseek(fd, index * sizeof(struct route), SEEK_SET);
			arr->num_seats_left += seats;
			struct route data = *arr;
			write(fd, &data, sizeof(data));
			break;
		}
		arr++;
		index++;
	}
	free_lock(fd, &lock);	
}

int get_bookings(int user_id, struct book_data * arr, int * pos)
{
	int fd = open("books.bin",O_RDWR,0777);
	if(fd < 0)
		return -1;
	struct flock lock;
	get_lock(fd,0,0,0,0,&lock);
	struct book_data * temp = arr;
	int index = 0;
	int i = 0;
	while(read(fd,arr,sizeof(struct book_data))>0){
		if(arr->user_id == user_id){
			arr++;
			pos[i++] = index;
		}
		index++;
	}
	int len = arr-temp;
	free_lock(fd,&lock);
	return len;
}
void update_booking(int client_id, int user_id)
{
	struct book_data * arr = (struct book_data *)malloc(sizeof(struct book_data) * 100);
	int * pos = (int *)malloc(sizeof(int) * 100);
	int n = get_bookings(user_id,arr,pos);
	if(n <= 0){
		print_this(client_id,"NO BOOKINGS AVAILABLE FOR THIS USER\n\n");
		return;
	}
	char buffer[1024] = {0};
	print(client_id, "Listing");
	sleep(1);
	print(client_id, "\nINDEX\tBOOKING ID\tUSER\tTRAIN NO\tNUM_SEATS\tSTATUS\n");
	for(int i = 0;i<n;i++)
	{
		sleep(1);
		sprintf(buffer, "%-9d\t%-14d\t%-8d\t%-12d\t%-12d\t%-14s\n",(i+1),arr[i].booking_id,arr[i].user_id,arr[i].train_no,arr[i].seats, (arr[i].status == 0)?"Cancelled":"Confirm");
		print(client_id, buffer);
	}
	sleep(1);
	print(client_id, "PLEASE ENTER THE INDEX OF BOOKING YOU WANT TO CANCEL");
	
	sleep(1);
	print(client_id, "END");
	
	char buff[10] = {0};
	read(client_id, buff, sizeof(buff));
	int index = atoi(buff);
	if(!(index > 0 && index <= n)){
		print_this(client_id, "Please enter a valid index!\n\n");
		return;
	}
	index--;
	struct flock lock;
	int book_fd = open("books.bin",O_RDWR|O_CREAT,0777);
	get_lock(book_fd,0,0,0,1,&lock);	
	arr[index].status = 0;
	
	lseek(book_fd, pos[index]*sizeof(struct book_data), SEEK_SET);
	struct book_data data = arr[index];
	write(book_fd,&data, sizeof(data));
	add_route_seats(arr[index].train_no, data.seats);
	free_lock(book_fd,&lock);
	print_this(client_id, "Your booking is cancelled\n\n");
}
void display_bookings(int client_id,int user_id){
	struct book_data * arr = (struct book_data *)malloc(sizeof(struct book_data) * 100);
	int * pos = (int *)malloc(sizeof(int) * 100);
	int n = get_bookings(user_id,arr,pos);
	if(n <= 0){
		print_this(client_id,"NO BOOKINGS AVAILABLE FOR THIS USER\n\n");
		return;
	}
	char buffer[1024] = {0};
	print(client_id, "Listing");
	sleep(1);
	print(client_id, "\nINDEX\t\tBOOKING ID\tUSER\tTRAIN NO\tNUM_SEATS\tSTATUS\n");
	for(int i = 0;i<n;i++)
	{
		sleep(1);
		sprintf(buffer, "%-9d\t%-10d\t%-7d\t%-11d\t%-11d\t%-14s\n",(i+1),arr[i].booking_id,arr[i].user_id,arr[i].train_no,arr[i].seats, (arr[i].status == 0)?"Cancelled":"Confirm");
		print(client_id, buffer);
	}
	sleep(1);
	print_dis(client_id, "\n");
	sleep(1);
	print(client_id, "END");
}
void display_routes(int client_id, int n){
	char buffer[1024] = {0};
	print(client_id, "Listing");
	sleep(1);
	print(client_id, "\nINDEX\tTRAIN NO\tSOURCE\t\tDESTINATION\tNUM_SEATS\tSEATS LEFT\tSTATUS\n");
	for(int i = 0;i<n;i++)
	{
		sleep(1);
		sprintf(buffer, "%-5d\t%-8d\t%-10s\t%-14s\t%-12d\t%-12d\t%-14s\n",(i+1),routes[i].train_no,routes[i].source,routes[i].destination,routes[i].num_seats,routes[i].num_seats_left,(routes[i].status == 0)?"Cancelled":"Running");
		print(client_id, buffer);
	}
	sleep(1);
	print(client_id, "Enter the index of route with which you would like to proceed : ");
	sleep(1);
	print(client_id, "END");
}
int get_routes(){
	int fd = open("routes.bin",O_RDWR,0777);
	if(fd < 0)
		return -1;
	struct flock lock;
	get_lock(fd,0,0,0,0,&lock);
	routes = (struct route *)malloc(sizeof(struct route)*100);
	struct route * arr = routes;
	while(read(fd, arr, sizeof(struct route)) > 0)
		arr++;
	int num_elements = arr - routes;
	free_lock(fd, &lock);	
	return num_elements;
}
void handle_booking(int client_id)
{
	char buffer[1024] = {0};
	int n = get_routes();
	if(n <= 0) {
		print_this(client_id,"No Routes are avaible please try again later!\n\n");
		return;
	}
	display_routes(client_id,n);
	char buff2[10] = {0};
	read(client_id,buff2,sizeof(buff2));
	int index = atoi(buff2);
	if((index > 0) && (index <= n))
		index--;
	else{
		print_this(client_id,"Enter a valid number!\n\n");
		return;
	}
	if(routes[index].status == 0){
		print_this(client_id, "That route is not functional!\n");
		return;
	}
	char seats[10] = {0};
	print(client_id, "enter the number of seats to book : ");
	read(client_id, seats, sizeof(seats));
	int num_seats = atoi(seats);
	n = get_routes();
	struct route data = routes[index];
	if(num_seats > data.num_seats_left){
		print_this(client_id,"Not Enough seats available/or may be that's booked while we were processing!\n\n");
		return;
	}
	else if(num_seats < 1){
		print_this(client_id,"\nNot a valid number of seats!\n\n");
		return;
	}
	int fd = open("routes.bin",O_RDWR,0777);
	struct flock lock;
	get_lock(fd,0,0,0,1,&lock);
	data.num_seats_left -= num_seats;
	lseek(fd,index*sizeof(data),SEEK_SET);
	write(fd,&data,sizeof(data));
	free_lock(fd, &lock);
	
	int book_fd = open("books.bin",O_RDWR|O_CREAT,0777);
	get_lock(book_fd,0,0,0,1,&lock);
	
	struct book_data booking;
	booking.train_no = data.train_no;
	booking.seats = num_seats;
	booking.user_id = user.user_id;
	booking.booking_id = random_number(10, 100000);
	booking.status = 1;
	lseek(book_fd, 0, SEEK_END);
	write(book_fd, &booking, sizeof(booking));
	free_lock(book_fd,&lock);
	print_this(client_id, "Congratulations!, Your booking is confirmed\n\n");
	return;
}

int add_routes(int client_id){
	int fd = open("routes.bin",O_RDWR|O_CREAT,0777);
	if(fd < 0)
		return -1;
	struct flock lock;
	get_lock(fd,0,0,0,1,&lock);
	
	struct route data;
	char train_no[10];
	char num_seats[10];
	
	print(client_id, "Train No : ");
	read(client_id, train_no, sizeof(train_no));
	
	print(client_id, "Number of seats : ");
	read(client_id, num_seats, sizeof(num_seats));
	
	print(client_id, "Source : ");
	read(client_id, data.source, sizeof(data.source));
	
	print(client_id, "Destination : ");
	read(client_id, data.destination, sizeof(data.destination));
	
	data.train_no = atoi(train_no);
	data.num_seats = atoi(num_seats);
	data.num_seats_left = atoi(num_seats);
	data.status = 1;
	
	lseek(fd, 0, SEEK_END);
	write(fd, &data, sizeof(data));
	free_lock(fd, &lock);	
}
void change_source(int client_id, int train_no){	

	int fd = open("routes.bin",O_RDWR,0777);
	struct route data;
	print(client_id,"Please enter the source : ");
	read(client_id, data.source, sizeof(data.source));
	struct flock lock;
	get_lock(fd,0,0,0,1,&lock);
	routes = (struct route *)malloc(sizeof(struct route)*100);
	struct route * arr = routes;
	int index = 0;
	while(read(fd, arr, sizeof(struct route)) > 0){
		if(arr->train_no == train_no)
		{
			strcpy(data.destination, arr->destination);
			data.train_no = train_no;
			data.num_seats = arr->num_seats;
			data.num_seats_left = arr->num_seats_left;
			data.status = arr->status;
			lseek(fd, index*sizeof(struct route), SEEK_SET);
			write(fd, &data, sizeof(data));
			break;
		}
		arr++;
		index++;
	}
	free_lock(fd, &lock);	
	sleep(1);
}
void change_destination(int client_id, int train_no){	

	int fd = open("routes.bin",O_RDWR,0777);
	struct route data;
	print(client_id,"Please enter the destination : ");
	read(client_id, data.destination, sizeof(data.destination));
	struct flock lock;
	get_lock(fd,0,0,0,1,&lock);
	routes = (struct route *)malloc(sizeof(struct route)*100);
	struct route * arr = routes;
	int index = 0;
	while(read(fd, arr, sizeof(struct route)) > 0){
		if(arr->train_no == train_no)
		{
			strcpy(data.source, arr->source);
			data.train_no = train_no;
			data.num_seats = arr->num_seats;
			data.num_seats_left = arr->num_seats_left;
			data.status = arr->status;
			lseek(fd, index*sizeof(struct route), SEEK_SET);
			write(fd, &data, sizeof(data));
			break;
		}
		arr++;
		index++;
	}
	free_lock(fd, &lock);	
}
void cancel_tickets(int client_id, int train_no)
{
	int fd = open("books.bin",O_RDWR);
	struct flock lock;
	get_lock(fd,0,0,0,1,&lock);
	struct book_data * arr = (struct book_data *)malloc(sizeof(struct book_data));
	int index = 0;
	while(read(fd,arr,sizeof(struct book_data))>0){
		if(arr->train_no == train_no){
			arr->status = 0;
			lseek(fd, index * sizeof(struct book_data),SEEK_SET);
			write(fd, arr, sizeof(*arr));
		}
		index++;
	}
	free_lock(fd,&lock);
}
void change_seats(int client_id, int train_no){

	int fd = open("routes.bin",O_RDWR,0777);
	char buff[10];
	print(client_id,"Please enter the number of seats ( WILL AUTOMETICALLY SET TO PREVIOUS VALUE IF ENTERED LOWER VALUE ) : ");
	read(client_id, buff, sizeof(buff));
	int seats = atoi(buff);
	
	struct flock lock;
	get_lock(fd,0,0,0,1,&lock);
	routes = (struct route *)malloc(sizeof(struct route)*100);
	struct route * arr = routes;
	int index = 0;
	while(read(fd, arr, sizeof(struct route)) > 0){
		if(arr->train_no == train_no)
		{
			
			if(arr->num_seats < seats){
				arr->num_seats_left += (seats - arr->num_seats);
				arr->num_seats = seats;
				lseek(fd, index*sizeof(struct route), SEEK_SET);
				write(fd, arr, sizeof(*arr));
			}
			break;
		}
		arr++;
		index++;
	}
	free_lock(fd, &lock);	
	
}
void change_status(int client_id, int train_no)
{
	int fd = open("routes.bin",O_RDWR);
	char buff[10];
	print(client_id,"Please enter the status 1 for running or 2 for cancelling the route : ");
	read(client_id, buff, sizeof(buff));
	int status = atoi(buff);
	if((status > 2) || (status < 1))
	{
		print_this(client_id, "Please enter the correct choice\n\n");
		return;
	}
	status = (status == 2)?0:1;
	struct route * arr = (struct route *)malloc(sizeof(struct route));
	int index = 0;
	while(read(fd, arr, sizeof(struct route)) > 0){
		if(arr->train_no == train_no)
		{
			if((arr->status == 0) && (status == 1))
				arr->num_seats_left = arr->num_seats;
			arr->status = status;
			lseek(fd, index*sizeof(struct route), SEEK_SET);
			write(fd, arr, sizeof(*arr));
			break;
		}
		index++;
	}
	cancel_tickets(client_id, train_no);
}
void display_routes_admin(int client_id, int n){
	char buffer[1024] = {0};
	print(client_id, "Listing");
	sleep(1);
	print(client_id, "\nINDEX\tTRAIN NO\tSOURCE\t\tDESTINATION\tNUM_SEATS\tSEATS LEFT\tSTATUS\n");
	for(int i = 0;i<n;i++)
	{
		sleep(1);
		sprintf(buffer, "%-5d\t%-8d\t%-10s\t%-14s\t%-12d\t%-12d\t%-14s\n",(i+1),routes[i].train_no,routes[i].source,routes[i].destination,routes[i].num_seats,routes[i].num_seats_left,(routes[i].status == 0)?"Cancelled":"Running");
		print(client_id, buffer);
	}
	sleep(1);
	print_dis(client_id, "\n");
	sleep(1);
	print(client_id, "END");
}
void view_routes(int client_id)
{
	int n = get_routes();
	if(n <= 0) {
		print_this(client_id,"No Routes are avaible please try again later!\n\n");
		return;
	}
	display_routes_admin(client_id,n);
}
void view_bookings(int client_id){
	char buff[10];
	print(client_id, "Enter the user id to display bookings : ");
	read(client_id, buff, sizeof(buff));
	int user_id = atoi(buff);
	display_bookings(client_id, user_id);	
}
void update_routes(int client_id){
	char buffer[1024];
	int n = get_routes();
	if(n <= 0) {
		print_this(client_id,"No Routes are avaible please try again later!\n\n");
		return;
	}
	display_routes(client_id,n);
	char buff2[10] = {0};
	read(client_id,buff2,sizeof(buff2));
	int index = atoi(buff2);
	if((index > 0) && (index <= n))
		index--;
	else{
		print_this(client_id,"Enter a valid number!\n\n");
		return;
	}
	print(client_id,"What you want to change\n1 for source\n2 for Destination\n3 for updating seats (ONLY INCREASE)\n4 Update route status\n : ");
	read(client_id,buffer,sizeof(buffer));
	int choice = atoi(buffer);
	switch(choice)
	{
		case 1:
			change_source(client_id, routes[index].train_no);
		break;
		case 2:
			change_destination(client_id, routes[index].train_no);
		break;
		case 3:
			change_seats(client_id,routes[index].train_no);
		break;
		case 4:
			change_status(client_id, routes[index].train_no);
		break;
		default:
			print_this(client_id, "Enter a valid choice\n\n");
			return;
	}
}
int get_users(struct login_struct * arr){
	int fd = open("login.bin",O_RDWR);
	if(fd < 0)
		return -1;
	struct flock lock;
	get_lock(fd,0,0,0,0,&lock);
	int index = 0;
	while(read(fd, arr, sizeof(struct login_struct)) > 0){
		arr++;
		index++;
	}
	free_lock(fd, &lock);	
	return index;
}
void display_all_users(int client_id){
	struct login_struct * arr = (struct login_struct *)malloc(sizeof(struct login_struct) *100);
	int n  = get_users(arr);
	char buffer[1024] = {0};
	print(client_id, "Listing");
	sleep(1);
	print(client_id, "\nINDEX\tUSER ID\t\tUSERNAME\tADMIN\tAGENT\tAPPROVED\n");
	for(int i = 0;i<n;i++){
		sleep(1);
		sprintf(buffer, "%-5d\t%-9d\t%-10s\t%-5s\t%-5s\t%-5s\n",(i+1),arr[i].user_id,arr[i].username,(arr[i].admin == 1)?"YES":"NO",(arr[i].agent == 1)?"YES":"NO",(arr[i].approved == 1)?"YES":"NO");
		print(client_id, buffer);
	}
	sleep(1);
	print_dis(client_id, "\n");
	sleep(1);
	print(client_id, "END");
}
void approve_user(int client_id,int user_id)
{
	int fd = open("login.bin",O_RDWR);
	if(fd < 0)
		return;
	struct flock lock;
	get_lock(fd,0,0,0,1,&lock);
	int index = 0;
	struct login_struct * arr = (struct login_struct *)malloc(sizeof(struct login_struct));
	while(read(fd, arr, sizeof(struct login_struct)) > 0){
		if(arr->user_id == user_id)
		{
			print(client_id, "1 for Disapprove and 2 for Approve : ");
			char buff[10];
			read(client_id, buff, sizeof(buff));
			int a = atoi(buff);
			if(!(a == 1 || a == 2)){
				print_this(client_id, "Please enter a valid choice\n\n");
				return;
			}
			a--;
			arr->approved = a;
			lseek(fd,index*sizeof(struct login_struct), SEEK_SET);
			write(fd,arr,sizeof(*arr));
			break;
		}		
		index++;
	}
	free_lock(fd, &lock);	
	print_this(client_id,"\n\nDONE\n\n");
	return;
}
void update_users(int client_id){
	struct login_struct * arr = (struct login_struct *)malloc(sizeof(struct login_struct) *100);
	int n  = get_users(arr);
	char buffer[1024] = {0};
	print(client_id, "Listing");
	sleep(1);
	print(client_id, "\nINDEX\tUSER ID\t\tUSERNAME\tADMIN\tAGENT\tAPPROVED\n");
	for(int i = 0;i<n;i++){
		sleep(1);
		sprintf(buffer, "%-5d\t%-9d\t%-10s\t%-5s\t%-5s\t%-5s\n",(i+1),arr[i].user_id,arr[i].username,(arr[i].admin == 1)?"YES":"NO",(arr[i].agent == 1)?"YES":"NO",(arr[i].approved == 1)?"YES":"NO");
		print(client_id, buffer);
	}
	sleep(1);
	print(client_id, "Enter the index you want to continue\n");
	sleep(1);
	print(client_id, "END");
	char buff[10];
	read(client_id, buff, sizeof(buff));
	int index = atoi(buff);
	if(!((index > 0) && (index <= n))){
		print_this(client_id, "Please enter a valid index\n\n");
		return;
	}
	index--;
	approve_user(client_id, arr[index].user_id);
}

void handle_session(int client_id){
	char buffer[1024] = {0};
	int action = 0;
	sprintf(buffer,"WELCOME %s, ", user.username);
	print_this(client_id, buffer);
	int log = 1;
	while(log){
		char option[10];
	    	read(client_id, option, sizeof(option));
    	    	action = atoi(option);
		switch(action)
		{
			case 1:
				if(user.admin)
					print_this(client_id,"\nPlease enter a valid option\n\n");
				else
					handle_booking(client_id);
			break;
			case 2:
				if(user.admin)
					print_this(client_id,"\nPlease enter a valid option\n\n");
				else
					update_booking(client_id, user.user_id);
			break;
			case 3:
				if(user.admin)
					print_this(client_id,"\nPlease enter a valid option\n\n");
				else
					display_bookings(client_id,user.user_id);
			break;
			case 4:
				if(!user.admin)
				{
					log = 0;
					logout(client_id);
					system("clear");
				}
				else{
					add_routes(client_id);	
					print_this(client_id,"\nDONE\n\n");	
					
				}				
			break;
			case 5:
				if(!user.admin)
					print_this(client_id,"\nPlease enter a valid option\n\n");
				else{
					update_routes(client_id);	
					print_this(client_id,"\nDONE\n\n");
				}
			break;
			case 6:
				if(!user.admin)
					print_this(client_id,"\nPlease enter a valid option\n\n");
				else{
					display_all_users(client_id);		
					
				}
			break;
			case 7:
				if(!user.admin)
					print_this(client_id,"\nPlease enter a valid option\n\n");
				else{
					view_bookings(client_id);	
					print_this(client_id,"\nDONE\n\n");		
				}
			break;
			case 8:
				if(!user.admin)
					print_this(client_id,"\nPlease enter a valid option\n\n");
				else{
					view_routes(client_id);	
				}
			break;
			case 9:
				if(!user.admin)
					print_this(client_id,"\nPlease enter a valid option\n\n");
				else{
					update_users(client_id);
				}
			break;
			case 10:
				if(user.admin){
					log = 0;
					logout(client_id);
					system("clear");
				}
				else{
					print_this(client_id,"\nPlease enter a valid option\n\n");
				}
			break;
			default:
				print_this(client_id, "Please enter a valid option\n\n");
			break;
		}
	}
}
