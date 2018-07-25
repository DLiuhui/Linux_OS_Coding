#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <shadow.h>
#include <errno.h>
#include <crypt.h>
#include <time.h>
#include <ctype.h>


void get_rand_str(char *buffer, int num)	//生成随即字符串
{
	//定义随机生成字符串表
	char *str = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz,./;\"'<>?";
	int i,lstr;
	char ss;
	lstr = strlen(str);//计算字符串长度
	srand((unsigned int)time((time_t *)NULL));//使用系统时间来初始化随机数发生器
	for(i = 0; i < num; i++)
	{//按指定大小返回相应的字符串
		ss = str[(rand()%lstr)];//rand()%lstr 可随机返回0-71之间的整数, str[0-71]可随机得到其中的字符
		buffer[i] = ss;//将随机生成的字符串连接到指定数组后面
	}
}


void getsalt(char *salt, char* passwd)
{
	int i, j;
	for(i = 0, j = 0; passwd[i] && j != 3; i++)
	{
		if(passwd[i] == '$')
		{
			j++;
		}
	}
	strncpy(salt, passwd, i-1);
}

int main(int argc, char *argv[])
{
	char passwd[100] = {0};
	char *p_code;
	/*
	struct spwd *sp;
	char salt[512] = {0};
	if(argc!=2)
	{
		printf("error args\n");
		return -1;
	}
	passwd = getpass("输入密码：");
	sp = getspnam(argv[1]);
	if(NULL == sp)
	{
		perror("getspnam");
		return -1;
	}
	printf("passwd:%s\n", passwd);
	printf("%-100s\n", sp->sp_pwdp);
	getsalt(salt, sp->sp_pwdp);
	puts(salt);
	char *p_code;
	if(!strcmp(sp->sp_pwdp, crypt(passwd, salt)))
	{
		printf("login succ\n");
	}else{
		printf("login fai\n");
	}
	*/
	//利用crypt函数对密码进行加密，第二个参数为盐值
	char buf[8] = {0};
	char buffer[11];
	get_rand_str(buf, 8);	//生成8位盐值码
	sprintf(buffer, "%s%s", "$6$" , buf);
	p_code = crypt(passwd, buffer);
	printf("buffer:%s\n", buffer);
	printf("%s\n", p_code);
	printf("%ld %ld\n", strlen(p_code), strlen(p_code) - 11);
	return 0;
}
