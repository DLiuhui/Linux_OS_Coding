#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, char **argv)
{
	char c;
	char name[20] = {0};
	char code[20] = {0};
	int i;
	printf("Welcome!\n");
	while(1)
	{
		printf("choose 1 to login, 2 to regist:\n");
		while((c = getchar()) == '\n' || c == EOF)
		{
			;
		}
		if(c == '1')
		{
			while((c = getchar()) != '\n' && c != EOF)
			{
				;
			}
			i = 0;
			printf("printf name:");
			while((c = getchar()) != '\n' && c != EOF && i < 20)
			{
				name[i++] = c;
			}
			i = 0;
			printf("printf code:");
			while((c = getchar()) != '\n' && c != EOF && i < 20)
			{
				code[i++] = c;
			}
			if('\0' == name[0] || '\0' == code[0])
			{
				printf("name or code couldn't be empty\n");
				continue;
			}
			break;
		}
		else if(c == '2')
		{
			while((c = getchar()) != '\n' && c != EOF)
			{
				;
			}
			i = 0;
			printf("printf new name:");
			while((c = getchar()) != '\n' && c != EOF && i < 20)
			{
				name[i++] = c;
			}
			i = 0;
			printf("printf new code:");
			while((c = getchar()) != '\n' && c != EOF && i < 20)
			{
				code[i++] = c;
			}
			break;
		}
		else
		{
			while((c = getchar()) != '\n' && c != EOF)
				            {
								                ;
												            }
			printf("select error\n");
			continue;
		}
	}
	printf("%s\n%s\n", name, code);
	return 0;
}
