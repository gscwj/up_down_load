#include "dirent.h"
#include "stdlib.h"
#include "string.h"
#include "sys/stat.h"
#include "strings.h"
#define N 128


int CloseDir(DIR *src);
void mode_to_letter(int mode,char *str);


int rewindir(const char *srv){
        DIR *dp_server;
        struct dirent *dir;
        struct stat myst;
        char server[128]={0};
        char fname[128]={0};
        char mode[20]={0};

        strcpy(server,srv);

start:
        if((dp_server=opendir(server))==NULL)
        {
                printf("open server %s failure!\n",server);
                goto start;
        }
        printf("open server %s successful!\n",server);

        while(1)
        {
                if((dir=readdir(dp_server))!=NULL)
                {

                        strcat(strcat(server,"/"),dir->d_name);
                        strcpy(fname,server);
                        if(lstat(fname,&myst)!=0){
                                perror("lstat");
                                return -1;
                        }
                        mode_to_letter(myst.st_mode,mode);
                        printf("i_node:%ld,\ttype&mode: %s\tsize:%ldB\tname:%s\n",dir->d_ino,mode,myst.st_size,dir->d_name);
                        strcpy(server,srv);
                }
                else
                        break;
        }
        CloseDir(dp_server);

}


int download(const char *srv,const char *fn){
        FILE *src_f,*dst_f;
        char fname[128]={0},server[128]={0};
        int ret,readnum;
        char readbuf[N]={0};
        strcpy(server,srv);
        strcpy(fname,fn);
        printf("srv=%s\tfname=%s\n",server,fname);//***
        if((src_f=fopen(strcat(strcat(server,"/"),fname),"r"))==NULL)
        {
                printf("open file %s is failure!\n",fname);
                return -1;
        }
        printf("open file %s is sucessful!\n",fname);

        if((dst_f=fopen(fname,"w"))==NULL)
        {
                printf("create file %s is failure!\n",fname);
                return -1;
        }
        printf("create file %s is sucessful!",fname);
        while(1)
        {
                readnum=fread(readbuf,1,N,src_f);
                if(readnum<128)
                {
                        printf("read is end!\n");
                        break;

                }
                fwrite(readbuf,1,N,dst_f);
        }
        fwrite(readbuf,1,readnum,dst_f);
        fclose(src_f);
        fclose(dst_f);
}


int CloseDir(DIR *src){
        int i=3;
        while(i--){
                if(closedir(src)<0)
                {
                        printf("close dir is failure!\n");
                        continue;
                }
                else{
                        i=0;
                        printf("close dir is sucessful!\n");
                }
        }

}

int upload(const char *srv,const char *clt,const char *fn){
        char client [128]={0},server[128]={0},fname[128]={0};
        FILE *src_f,*dst_f;
        int ret,readnum;
        char readbuf[N]={0};

        strcpy(server,srv);
        strcpy(client,clt);
        strcpy(fname,fn);

        src_f=fopen(strcat(strcat(client,"/"),fname),"r");
        if(src_f==NULL)
        {
                printf("open file %s is faifure!\n",fname);
                return -1;
        }
        printf("open file %s is sucessful!\n",fname);
        dst_f=fopen(strcat(strcat(server,"/"),fname),"w");
        printf("%p\n",dst_f);
        if(dst_f==NULL)
        {
                printf("open file %s is failfure!\n",fname);
                return -1;
        }
        printf("open file %s is sucessful!\n",fname);
        while(1)
        {
                readnum=fread(readbuf,1,N,src_f);
                if(readnum<N)
                {
                        printf("read is end!\n");
                        break;
                }
                fwrite(readbuf,1,N,dst_f);
        }
        fwrite(readbuf,1,readnum,dst_f);
        fclose(src_f);
        fclose(dst_f);
        return 0;
}
int main(int argc,char *argv[])
{
        char server[128]={0},fname[128]={0},client[128]={0};
        int select;

        printf("Please input server PATH and Directory name:\n");
        scanf("%s",server);

start1:
        rewindir(server);       //遍历目录
        printf("Please select the option below:\n");
        printf("\t1:download files\n");
        printf("\t2:upload   files\n");
        printf("\t0:quit!!!\n");
        scanf("%d",&select);
        switch(select)
        {
          case 1:
                printf("Please input download file:\n");
                scanf("%s",fname);
                download(server,fname);
                break;
          case 2:
                printf("Please input upload PATH:\n");
                scanf("%s",client);

                rewindir(client);
                printf("Please input upload file:\n");
                scanf("%s",fname);
                upload(server,client,fname);
                break;
         case 0:
                return 0;
        }
        goto start1;
}



void mode_to_letter(int mode,char *str)
{
/*-------这个函数用来把模式值转化为字符串------*/
str[0]='-'; /*------这里的S_*****都是宏定义，用来判断模式属性-*/

if(S_ISDIR(mode)) str[0]='d';/*-文件夹-*/
if(S_ISCHR(mode)) str[0]='c';/*-字符设备-*/
if(S_ISBLK(mode)) str[0]='b';/*-块设备-*/

if(mode & S_IRUSR) str[1]='r';/*--用户的三个属性-*/
else str[1]='-';
if(mode & S_IWUSR) str[2]='w';
else str[2]='-';
if(mode & S_IXUSR) str[3]='x';
else str[3]='-';

if(mode & S_IRGRP) str[4]='r';/*--组的三个属性-*/
else str[4]='-';
if(mode & S_IWGRP) str[5]='w';
else str[5]='-';
if(mode & S_IXGRP) str[6]='x';
else str[6]='-';

if(mode & S_IROTH) str[7]='r';/*-其他人的三个属性-*/
else str[7]='-';
if(mode & S_IWOTH) str[8]='w';
else str[8]='-';
if(mode & S_IXOTH) str[9]='x';
else str[9]='-';

str[10]='\0';
}
/*
int main(int argc,char *argv[])
{
struct stat sb;*- 定义stat结构--*
char str[12];
if(argc!=2){
fprintf(stderr,"Usage: %s <pathname>\n",argv[0]);
exit(EXIT_FAILURE);
}

if(stat(argv[1],&sb)==-1){*-stat函数，详情请 man 2 stat 查看 -*
perror("stat");
exit(EXIT_FAILURE);
}

printf("Mode:%lo(octal)\n",(unsigned long)sb.st_mode);
mode_to_letter(sb.st_mode,str);
printf("Mode:%s\n",str);
return 0;
}
*/
~
~
~
~
~
~
~
~
~
~
