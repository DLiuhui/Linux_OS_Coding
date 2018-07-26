#include "header.h"

int main()
{
	struct tm *now_time;
	time_t get_time;
	char buffer[25];
	int i = 0;
	while(i < 20)
	{
		time(&get_time);
		now_time = gmtime(&get_time);
		sprintf(buffer, "%04d-%02d-%02d %02d:%02d:%02d",
				(1900 + now_time->tm_year), (1 + now_time->tm_mon), (now_time->tm_mday),
				(8 + now_time->tm_hour), (now_time->tm_min), (now_time->tm_sec));
		printf("%s\n", buffer);
		sleep(2);
		i++;
	}
	return 0;
}
