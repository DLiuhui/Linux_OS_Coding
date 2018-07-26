#include "header.h"

int main()
{
	char *wday[] = {"Sunday", "Monday", "Tuesday", 
					"Wednesday", "Thursday", "Friday", "Saturday"};
	time_t timep;
	struct tm *pTm;
	time(&timep);
	pTm = gmtime(&timep);
	printf("%04d-%02d-%02d\t", 
			(1900 + pTm->tm_year), 
			(1 + pTm->tm_mon),
			(pTm->tm_mday)
		  );
	printf("%02d:%02d:%02d\t", 
			(8 + pTm->tm_hour), 
			(pTm->tm_min),
			(pTm->tm_sec)
		  );
	printf("%s\n", wday[pTm->tm_wday]);
	return 0;
}
