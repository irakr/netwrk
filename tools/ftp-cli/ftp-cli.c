#include <stdio.h>

#define MAX_LEN_USERNAME	128
#define MAX_LEN_PASSWORD	256

int main()
{
	char username[MAX_LEN_USERNAME], password[MAX_LEN_PASSWORD];

	printf("################ Simple FTP client ################\n");
	
	printf("Username: ");
	scanf("%s", username);
	printf("Password: ");
	scanf("%s", password);
	
	return 0;
}
