struct login_struct{
	char username[100];
	char password[100];
	int admin;
	int user_id;
	int approved;
	int agent;
	int logged_in;
};

struct route{
	int train_no;
	char source[100];
	char destination[100];
	int num_seats;
	int num_seats_left;
	int status;
};

struct book_data{
	int booking_id;
	int train_no;
	int seats;
	int user_id;
	int status;
};
