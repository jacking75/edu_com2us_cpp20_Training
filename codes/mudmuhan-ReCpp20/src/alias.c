/* 줄임말 , 칭호 */

#include <string.h>
#include <ctype.h>
#include "mstruct.h"
#include "mextern.h"

char *ply_titles[PMAX];

typedef struct ALIAS_COMMAND {
    char alias[16];
    char process[256];
} Alias_Cmd;

#define MAX_ALIAS 50	/* 최대 저장 줄임말 수 */

Alias_Cmd *ply_alias[PMAX][MAX_ALIAS];
char ply_alias_num[PMAX];
char init_alias_index[PMAX];

char alias_buf[PMAX][MAX_ALIAS_BUF][256];
int  alias_buf_pos[PMAX];
int  alias_buf_num[PMAX];

extern void save_alias();
extern void command_separate();

char title_cut_index[PMAX];

void init_alias(ply_ptr)
creature *ply_ptr;
{
    FILE *fp;
    char buf1[16],buf2[256],path[256];
    int len,fd,index=0;

    fd=ply_ptr->fd;

    ply_titles[fd]=NULL;
    ply_alias_num[fd]=
    alias_buf_pos[fd]=
    alias_buf_num[fd]=0;

    sprintf(path, "%s/alias/%s", PLAYERPATH, ply_ptr->name);
    fp = fopen(path, "r");
    if(!fp) return;

    while(!feof(fp) && ply_alias_num[fd]<MAX_ALIAS) {
        fgets(buf1,15,fp);
        len=strlen(buf1);
        if(!len || buf1[0]==0x0a) goto end_file;
        if(buf1[len-1]=='\n') buf1[len-1] = 0;
        if(!strcmp(buf1,"~!")) {
            index=1;
            break;
        }

        if(feof(fp)) goto end_file;
        fgets(buf2,255,fp);
        len=strlen(buf2);
        if(!len) break;
        buf2[len-1] = 0;
        if(!strcmp(buf2,"~!")) {
            index=1;
            break;
        }

        if(add_alias(fd,buf1,buf2)==-1) break;
    }
    if(index==1) {
        fgets(buf2,80,fp);
        len=strlen(buf2);
        buf2[len-1]=0;
        if(!strcmp(buf2,"~!")) goto end_file;
        ply_titles[fd]=(char *)malloc(len);
        if(ply_titles[fd]==NULL) goto end_file;
        strcpy(ply_titles[fd],buf2);
    }
end_file:
    fclose(fp);

    init_alias_index[fd]=77;
}

void close_alias(fd)
int fd;
{
    if(init_alias_index[fd]!=77) return;
    init_alias_index[fd]=0;
    while(ply_alias_num[fd]>0) {
        free(ply_alias[fd][ply_alias_num[fd]-1]);
        ply_alias_num[fd]--;
    }
    if(ply_titles[fd]!=NULL) free(ply_titles[fd]);
}

int alias_cmd(ply_ptr, cmnd)
creature *ply_ptr;
cmd *cmnd;
{
    int fd,i,index=4000,buf_num;
    char *cur, *next;
    char full_command[4096];

    fd=ply_ptr->fd;
    for(i=0;i<ply_alias_num[fd];i++) {
        if(!strcmp(ply_alias[fd][i]->alias,cmnd->str[0])) {
            command_separate(full_command,ply_alias[fd][i]->process,
                             cmnd->fullstr);
            cur=full_command;
            buf_num=0;
            for(;;) {
                if(*cur==0) break;
                next=strchr(cur,';');
                if(next!=NULL) *next=0;
                strcpy(alias_buf[fd][buf_num++],cur);
                if(buf_num==MAX_ALIAS_BUF) break;
                if(next==NULL) break;
                else {
                    *next=';';
                    cur=next+1;
                }
            }
            alias_buf_num[fd]=buf_num;
            alias_buf_pos[fd]=0;
            index=0;
            break;
        }
    }
    return (index);
}

int add_alias(fd,buf1,buf2)
int fd;
char *buf1;
char *buf2;
{
    if(ply_alias_num[fd]==MAX_ALIAS) {
        print(fd,"더이상 줄임말을 설정할 수 없습니다.");
        return -1;
    }
    ply_alias[fd][ply_alias_num[fd]]=(Alias_Cmd *)malloc(sizeof(Alias_Cmd));
    if(!ply_alias[fd][ply_alias_num[fd]]) {
        merror("load_crt_tlk", FATAL);
        return -1;
    }
    strcpy(ply_alias[fd][ply_alias_num[fd]]->alias,buf1);
    strcpy(ply_alias[fd][ply_alias_num[fd]]->process,buf2);
    ply_alias_num[fd]++;
    return 0;
}

int ply_aliases(ply_ptr,cmnd)
creature *ply_ptr;
cmd *cmnd;
{
    char buf[256];
    int i,j,k=0,fd,len;

    fd=ply_ptr->fd;

    if(cmnd->num==1) {
        /* 현재 설정되어 있는 줄임말을 보여준다. */
        if(ply_alias_num[fd]==0) {
            print(fd,"설정된 줄임말이 없습니다.");
            return 0;
        }
        print(fd,"줄임말:\n");
        for(i=0;i<ply_alias_num[fd];i++) {
            print(fd,"  %-14s: %s\n",
                ply_alias[fd][i]->alias,ply_alias[fd][i]->process);
        }
        return 0;
    }
    if(!strncmp(cmnd->str[1],"숫자",4)) {
        if(strlen(cmnd->str[1]+4)>0) k=4;
    }
    if(cmnd->num==2) {
        /* 줄임말 삭제 */
        for(i=0;i<ply_alias_num[fd];i++) {
            if(!strcmp(ply_alias[fd][i]->alias,cmnd->str[1]+k)) {
                free(ply_alias[fd][i]);
                for(j=i+1;j<ply_alias_num[fd];j++)
                    ply_alias[fd][j-1]=ply_alias[fd][j];
                ply_alias_num[fd]--;
                save_alias(ply_ptr);
                print(fd,"줄임말이 삭제되었습니다.");
                return 0;
            }
        }
        print(fd,"그 이름으로 설정된 줄임말이 없습니다.");
        return 0;
    }

    /* 줄임말 추가 */
    if(!strcmp(cmnd->str[1]+k,"~!")) {
        print(fd,"그 코드로는 줄임말로 설정할 수 없습니다.");
        return 0;
    }
    if(strlen(cmnd->str[1]+k)>14) {
        print(fd,"줄임말의 길이가 너무 깁니다.");
        return 0;
    }
    for(i=0;i<ply_alias_num[fd];i++) {
        if(!strcmp(ply_alias[fd][i]->alias,cmnd->str[1]+k)) {
            print(fd,"그 줄임말은 이미 설정되어 있습니다.");
            return 0;
        }
    }

    len=cut_command(cmnd->fullstr);
    if(len > 70) {
    	print(fd, "줄임말의 전체적 길이가 너무 깁니다.");
    	return(0);
    }

    strcpy(buf,strchr(cmnd->fullstr,' ')+1);
    paste_command(cmnd->fullstr,len);

    for(i=strlen(buf)-1;i>=0;i--) {
        if(buf[i]!=' ') break;
    }
    buf[i+1]=0;
    if(add_alias(fd,cmnd->str[1]+k,buf)!=-1) 
        print(fd,"줄임말이 설정되었습니다.");
    save_alias(ply_ptr);
    return 0;
}

void save_alias(ply_ptr)
creature *ply_ptr;
{
    char path[256];
    int i,fd,handle;

    fd=ply_ptr->fd;
    sprintf(path, "%s/alias/%s", PLAYERPATH, ply_ptr->name);
    handle = creat(path, 0660);
    if(handle==-1) {
        print(fd,"화일로 저장하는데 실패하였습니다");
        return;
    }
    for(i=0;i<ply_alias_num[fd];i++) {
        write(handle,ply_alias[fd][i]->alias,strlen(ply_alias[fd][i]->alias));
        write(handle,"\n",1);
        write(handle,ply_alias[fd][i]->process,strlen(ply_alias[fd][i]->process));
        write(handle,"\n",1);
    }
    write(handle,"~!\n",3);
    if(ply_titles[fd]!=NULL) {
        write(handle,ply_titles[fd],strlen(ply_titles[fd]));
        write(handle,"\n",1);
    }
    write(handle,"~!\n",3);
    close(handle);

}

void nstrcat(char *dest, char *src,int len)
{
	int i;
	i=strlen(dest);
	while(i<len) {
		dest[i++] = *src++;
		dest[i]=0;
	}
}

void command_separate(buf,process,fullstr)
char *buf;
char *process;
char *fullstr;
{
    int i,j,len;
    char full_str[256];
    char *sep[16];
    int num;

    len=cut_command(fullstr);
    strcpy(full_str,fullstr);

    i=0;
    for(num=0;num<16;num++) {
        while(full_str[i]==' ' && full_str[i]!=0) {
            full_str[i]=0;
            i++;
        }
        sep[num]=full_str+i;
        while(full_str[i]!=' ' && full_str[i]!=0) i++;
    }

    j=0;
    
    buf[0]=0;
    for(i=0;i<strlen(process) && j<255 ;i++) {
        if(process[i]=='$') {
            i++;
            if(process[i]=='*') {
                nstrcat(buf,fullstr, 255);
                j=strlen(buf);
                continue;
            }
            num=0;
            while(isdigit(process[i])) {
                num=num*10+process[i]-'0';
                i++;
            }
            i--;
            if(num<=0 || num>16) continue;
            nstrcat(buf,sep[num-1], 255);
            j=strlen(buf);
        }
        else {
            buf[j++]=process[i];
            buf[j]=0;
        }
    }

    paste_command(fullstr,len);
}

int set_title(ply_ptr,cmnd)
creature *ply_ptr;
cmd *cmnd;
{
    int len,fd;
    
    fd=ply_ptr->fd;
    if(cmnd->num<2) {
        title_cut_index[fd] = 1;
        print(fd,"당신은 %s %s입니다.",title_ply(ply_ptr,ply_ptr),ply_ptr->name);
        title_cut_index[fd] = 0;
        return 0;
    }
    len=cut_command(cmnd->fullstr);
    if(len>78) 
        print(fd,"칭호가 너무 깁니다.");
    else {
        ply_titles[fd]=(char *)malloc(len+1);
        if(ply_titles[fd]!=NULL) {
            strcpy(ply_titles[fd],cmnd->fullstr);
            save_alias(ply_ptr);
            title_cut_index[fd] = 1;
            print(fd,"당신은 이제부터 %s %s입니다.",title_ply(ply_ptr,ply_ptr),ply_ptr->name);
            title_cut_index[fd] = 0;
        }
    }

    paste_command(cmnd->fullstr,len);
    return 0;
}

int clear_title(ply_ptr,cmnd)
creature *ply_ptr;
cmd *cmnd;
{
    int fd;

    fd=ply_ptr->fd;
    if(ply_titles[fd]==NULL) {
        print(fd,"칭호가 설정되어 있지 않습니다.");
        return 0;

    }
    free(ply_titles[fd]);
    ply_titles[fd]=NULL;
    save_alias(ply_ptr);
    print(fd,"당신은 이제부터 %s %s입니다.",title_ply(ply_ptr,ply_ptr),ply_ptr->name);
    return 0;
}

int color_cmp(str)
char *str;
{
    char buf[3];
    char *col[7]={"빨","노","파","하","보","녹","흰"};
    char code[7]={31,33,34,36,35,32,37};
    int i;

    buf[0]=*str++;
    buf[1]=*str;
    buf[2]=0;

    for(i=0;i<7;i++) {
        if(!strcmp(buf,col[i])) return code[i];
    }
    return 0;
}

    
char *ansi_title(ply_ptr,str)
creature *ply_ptr;
char *str;
{
    static char buf[PMAX][512];
    int i,j=0,len;
    int c;
    int fd;
    int k=0;

    fd=ply_ptr->fd;

    len=strlen(str);
    buf[fd][0]=0;
    for(i=0;i<len;) {
        if(str[i]=='{') {
            if((c=color_cmp(str+i+1))!=0) {
                if(F_ISSET(ply_ptr,PANSIC)) 
                sprintf(buf[fd]+j,"%c[%d;%dm",27,F_ISSET(ply_ptr,PBRIGH)?1:0,c);
                j=strlen(buf[fd]);
                i+=3;
            }
            else {
               buf[fd][j++]='{';
               buf[fd][j]=0;
               i++;
            }
        }
        else if(str[i]=='}') {
            if(F_ISSET(ply_ptr,PANSIC))
            sprintf(buf[fd]+j,"%c[0;37m",27);
            j=strlen(buf[fd]);
            i++;
        }
        else {
            if(title_cut_index[fd]) {
                if(k>=35) break;
                if(k==34 && (str[i]&0x80)) break;
            }
            buf[fd][j++]=str[i++];
            k++;
            if((str[i-1]&0x80) && str[i]) {
                buf[fd][j++]=str[i++];
                k++;
            }
            buf[fd][j]=0;
        }
    }
    if(F_ISSET(ply_ptr,PANSIC))
    sprintf(buf[fd]+j,"%c[0;37m",27);

    return buf[fd];
}


