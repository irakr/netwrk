#include <stdio.h>
#include "netwrk/ftp.h"

int main()
{
	ftp_user_info_t user_info;

	printf("################ Simple FTP client ################\n");
	
	printf("Username: ");
	scanf("%s", user_info.username);
	printf("Password: ");
	scanf("%s", user_info.password);
	
	return 0;
}
