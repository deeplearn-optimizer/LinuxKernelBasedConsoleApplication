#include "tools.h"
#include "client.h"

int main(){
	sleep(1);
	int id = server_connect();
	start_transfer(id);
	return 0;
}
