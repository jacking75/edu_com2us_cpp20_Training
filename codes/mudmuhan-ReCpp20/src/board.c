/* 게시판 */

#include <time.h>
#include <unistd.h>
#include <string.h>
#include "mstruct.h"
#include "mextern.h"
#include "board.h"

struct BOARD_INDEX {
    int num;
    char upload[16];
    int year;
    int month;
    int day;
    int hour;
    int min;
    int sec;
    int line;
    int readnum;
    char title[40];
    int extra[41];   /* total 64 * 4 = 256 byte */
};

struct BOARD_DIR {
    int num;
    char *dir;
} board_dir[]={
    {   0, "" },
    { 100, MUDHOME"/board/info" },
    { 101, MUDHOME"/board/family1" },
    { 102, MUDHOME"/board/family2" },
    { 103, MUDHOME"/board/family3" },
    { 104, MUDHOME"/board/family4" },
    { 105, MUDHOME"/board/family5" },
    { 106, MUDHOME"/board/family6" },
    { 107, MUDHOME"/board/family7" },
    { 108, MUDHOME"/board/family8" },
    { 109, MUDHOME"/board/family9" },
    { 110, MUDHOME"/board/family10" },
    { 111, MUDHOME"/board/family11" },
    { 112, MUDHOME"/board/family12" },
    { 113, MUDHOME"/board/family13" },
    { 114, MUDHOME"/board/family14" },
    { 115, MUDHOME"/board/family15" },
    { 116, MUDHOME"/board/family" },
    { 120, MUDHOME"/board/user" },
    {  -1, "" },
};

object *board_obj[PMAX];
int board_handle[PMAX];
int board_handle2[PMAX];
int board_num[PMAX];
long board_pos[PMAX];
char board_title[PMAX][44];

int look_board(ply_ptr,cmnd)
creature *ply_ptr;
cmd *cmnd;
{
        room    *rom_ptr;
        object  *obj_ptr;
        int     fd;

        fd = ply_ptr->fd;
        rom_ptr = ply_ptr->parent_rom;

        obj_ptr = find_obj(ply_ptr, rom_ptr->first_obj,
                   "게시판", 1);

        if(!obj_ptr) {
            print(fd,"이곳에는 게시판이 없습니다.");
            return 0;
        }

        F_SET(ply_ptr, PNOBRD);
        board_obj[fd]=obj_ptr;
        list_board(fd, 0, "");
        F_CLR(ply_ptr, PNOBRD);

        return DOPROMPT;
}

/* 게시판 보기 */
void list_board(fd, param, str)
int fd;
int param;
char *str;
{
    struct BOARD_INDEX board_index;
    char name[128];
    int i,handle;

    switch(param) {
        case 0: /* 게시판 보기 시작 */
            broadcast_rom(fd,Ply[fd].ply->parent_rom->rom_num,
                          "\n%M이 게시판을 봅니다.",Ply[fd].ply);
            board_handle[fd] = 0;
            i = 1;
            while(board_dir[i].num>0) {
                if(board_dir[i].num==board_obj[fd]->type) goto exit_while;
                i++;
            }
        exit_while:
            if(board_dir[i].num<0) {
                print(fd,"잘못된 게시판입니다.");
                RETURN(fd, command, 1);
            }

            sprintf(name,"%s/board_index",board_dir[i].dir);
            if((handle=open(name,O_RDONLY))==-1) {
                print(fd,"인덱스화일을 읽을수 없습니다.");
                RETURN(fd, command, 1);
            }
            board_pos[fd]=lseek(handle,0,SEEK_END);
            if(board_pos[fd]<sizeof(struct BOARD_INDEX)) {
                print(fd,"등록된 게시물이 없습니다.");
                close(handle);
                RETURN(fd, command, 1);
            }
            board_handle[fd]=handle;
        case 1:
            handle=board_handle[fd];
            F_CLR(Ply[fd].ply,PREADI);

            if(*str=='.') {
                print(fd,"게시물을 그만 봅니다.");
                close(handle);
                RETURN(fd, command, 1);
            }

            print(fd, "\n 번호 올린이       날짜  줄수 조회 제목\n");
            print(fd, "-------------------------------------");
            print(fd, "--------------------------------------\n");

            i=0;
            while(i<20) {
                if(board_pos[fd]<sizeof(struct BOARD_INDEX)) {
                    close(handle);
                    RETURN(fd, command, 1);
                }
                board_pos[fd]-=sizeof(struct BOARD_INDEX);
                lseek(handle,board_pos[fd],SEEK_SET);
                read(handle,&board_index,sizeof(struct BOARD_INDEX));
                if(board_index.readnum<0 && Ply[fd].ply->class<DM) continue;

                print(fd," %4d %-12s ",board_index.num,board_index.upload);
                print(fd,"%02d/%02d ",board_index.month,board_index.day);
                print(fd,"%4d %4d ",board_index.line,board_index.readnum);
                print(fd,"%-40s\n",board_index.title);
                i++;
            }
            if(board_pos[fd]<=0) {
                close(handle);
                RETURN(fd, command, 1);
            }

            F_SET(Ply[fd].ply,PREADI);
            print(fd,"\n다음 게시물을 계속 보시려면 [Enter]를, 중단하시려면 [.]을 누르세요.");
            output_buf();
            Ply[fd].io->intrpt &= ~1;
            RETURN(fd, list_board, 1);
    }
    RETURN(fd, command, 1);
}

int writeboard(ply_ptr,cmnd)
creature *ply_ptr;
cmd *cmnd;
{
        room    *rom_ptr;
        object  *obj_ptr;
        int     fd;

        fd = ply_ptr->fd;
        rom_ptr = ply_ptr->parent_rom;

        obj_ptr = find_obj(ply_ptr, rom_ptr->first_obj,
                   "게시판", 1);

        if(!obj_ptr) {
            print(fd,"이곳에는 게시판이 없습니다.");
            return 0;
        }

        F_SET(ply_ptr, PNOBRD);    
        board_obj[fd]=obj_ptr;
        write_board(fd, 0, NULL);
        F_CLR(ply_ptr, PNOBRD);

        return(DOPROMPT);

}

void write_board(fd, param, str)
int fd;
int param;
unsigned char *str;
{
    struct BOARD_INDEX board_index;
    char name[128];
    int i,handle;
    long number;
    long t;
    struct tm *tt;

    switch(param) {
        case 0: /* 게시판 쓰기 준비 */
            broadcast_rom(fd,Ply[fd].ply->parent_rom->rom_num,
                          "\n%M이 게시판에 글을 씁니다.",Ply[fd].ply);
            board_handle[fd] = 0;
            i = 1;
            while(board_dir[i].num>0) {
                if(board_dir[i].num==board_obj[fd]->type) goto exit_while;
                i++;
            }
        exit_while:
            if(board_dir[i].num<0) {
                print(fd,"잘못된 게시판입니다.");
                RETURN(fd, command, 1);
            }

            sprintf(name,"%s/board_index",board_dir[i].dir);
            if((handle=open(name,O_RDWR))==-1) {
                print(fd,"인덱스화일을 읽을수 없습니다.");
                RETURN(fd, command, 1);
            }
            board_handle[fd]=handle;

            sprintf(name,"%s/%s",board_dir[i].dir,Ply[fd].ply->name);
            if((handle=creat(name,0644))==-1) {
                print(fd,"화일을 생성할 수 없습니다.");
                close(board_handle[fd]);
                RETURN(fd, command, 1);
            }
            board_num[fd]=i;
            board_handle2[fd]=handle;

            F_SET(Ply[fd].ply,PREADI);
            print(fd,"제목: ");
            output_buf();
            Ply[fd].io->intrpt &= ~1;
            RETURN(fd, write_board, 1);
        case 1:
            F_CLR(Ply[fd].ply,PREADI);
            if(strlen(str)==0) {
                close(board_handle[fd]);
                close(board_handle2[fd]);
                print(fd,"게시물 작성을 취소합니다.");
                RETURN(fd,command,1);
            }

            strncpy(board_title[fd],str,40);
            board_title[fd][40]=0;

            board_pos[fd]=1;
            print(fd,"게시물을 작성합니다. 끝내시려면 행의 처음에 [.]을 입력하십시요.\n");
            print(fd,"중간에 취소하시려면 행의 처음에 [!!]를 입력하십시요.\n\n");
            F_SET(Ply[fd].ply,PREADI);
            print(fd,"%3ld: ",board_pos[fd]);
            output_buf();
            Ply[fd].io->intrpt &= ~1;
            RETURN(fd, write_board, 2);
        case 2:
            F_CLR(Ply[fd].ply,PREADI);

            if(str[0]=='!' && str[1]=='!') {
                close(board_handle2[fd]);
                close(board_handle[fd]);
                sprintf(name,"%s/%s",board_dir[board_num[fd]].dir,Ply[fd].ply->name);
                unlink(name);
                print(fd,"게시물 작성을 취소합니다.");
                RETURN(fd,command,1);
            }
            if(str[0]=='.') {
                close(board_handle2[fd]);

                board_index.num=lseek(board_handle[fd],0,SEEK_END)
                                /sizeof(struct BOARD_INDEX)+1;
                sprintf(name,"mv -f %s/%s %s/board.%ld",
                    board_dir[board_num[fd]].dir,Ply[fd].ply->name,
                    board_dir[board_num[fd]].dir,board_index.num);
                system(name);
                strcpy(board_index.upload,Ply[fd].ply->name);
                t=time(0);
                tt=localtime(&t);
                board_index.year =tt->tm_year;
                board_index.month=tt->tm_mon+1;
                board_index.day  =tt->tm_mday;
                board_index.hour =tt->tm_hour;
                board_index.min  =tt->tm_min;
                board_index.sec  =tt->tm_sec;
                board_index.line =(int)board_pos[fd];
                board_index.readnum=0;
                strcpy(board_index.title,board_title[fd]);
                write(board_handle[fd], &board_index,
                      sizeof(struct BOARD_INDEX));
                close(board_handle[fd]);

                print(fd,"게시물이 등록되었습니다.");
                RETURN(fd, command, 1);
            }
            write(board_handle2[fd],str,strlen(str));
            write(board_handle2[fd],"\n",1);

            F_SET(Ply[fd].ply,PREADI);
            print(fd,"%3ld: ",++board_pos[fd]);
            output_buf();
            Ply[fd].io->intrpt &= ~1;
            RETURN(fd, write_board, 2);
    }
}

int read_board(ply_ptr,cmnd)
creature *ply_ptr;
cmd *cmnd;
{
    struct BOARD_INDEX board_index;
    char name[128];
    int i,handle;
    int fd;
    long number;

    fd=ply_ptr->fd;
    i = 1;
    while(board_dir[i].num>0) {
        if(board_dir[i].num==board_obj[fd]->type) break;
        i++;
    }
    if(board_dir[i].num<0) {
        print(fd,"잘못된 게시판입니다.");
        return 0;
    }

    sprintf(name,"%s/board_index",board_dir[i].dir);
    if((handle=open(name,O_RDWR))==-1) {
        print(fd,"인덱스화일을 읽을수 없습니다.");
        return 0;
    }

    number=lseek(handle,0,SEEK_END)/sizeof(struct BOARD_INDEX);
    if(cmnd->val[0]>number || cmnd->val[0]<1) {
        print(fd,"범위에 벗어나는 게시물입니다.");
        close(handle);
        return 0;
    }

    lseek(handle,(cmnd->val[0]-1)*sizeof(struct BOARD_INDEX),SEEK_SET);
    read(handle,&board_index,sizeof(struct BOARD_INDEX));
    if(board_index.readnum<0 && ply_ptr->class<DM) {
        print(fd,"삭제된 게시물입니다.");
        close(handle);
        return 0;
    }
    broadcast_rom(fd,ply_ptr->parent_rom->rom_num,
             "\n%M이 게시판의 글을 읽습니다.",ply_ptr);

    print(fd,"\n번호: %d  올린이: %s  제목: %s\n",
              board_index.num, board_index.upload, board_index.title);
    print(fd,"올린날: 19%d년 %d월 %d일 %d시 %d분  ",
              board_index.year,board_index.month, board_index.day,
              board_index.hour,board_index.min);
    print(fd,"총줄수: %d  읽은횟수: %d\n",
              board_index.line, board_index.readnum);
    print(fd,"---------------------------------------------------------------\n");

    if(strcmp(board_index.upload,Ply[fd].ply->name)) {
        if(board_index.readnum<0) board_index.readnum--;
        else                      board_index.readnum++;
    }

    lseek(handle,(cmnd->val[0]-1)*sizeof(struct BOARD_INDEX),SEEK_SET);
    write(handle,&board_index,sizeof(struct BOARD_INDEX));
    close(handle);

    sprintf(name,"%s/board.%ld",board_dir[i].dir,cmnd->val[0]);
    view_file(fd,1,name);
    return(DOPROMPT);
}

int del_board(ply_ptr,cmnd)
creature *ply_ptr;
cmd *cmnd;
{
    object *obj_ptr;
    char name[128];
    int i,fd;
    int handle;
    struct BOARD_INDEX board_index;

    fd=ply_ptr->fd;

    if(cmnd->num<2 || strcmp(cmnd->str[1],"게시판")
        || (cmnd->val[1]==1 && ply_ptr->class<DM)) {
        print(fd, "사용법: 게시판 <번호> 글삭제");
        return 0;
    }
        
    obj_ptr=find_obj(ply_ptr,ply_ptr->parent_rom->first_obj,
                    "게시판", 1);

    if(!obj_ptr || obj_ptr->special!=SP_BOARD) {
        print(fd, "이곳에는 게시판이 없습니다.");
        return 0;
    }

    i = 1;
    while(board_dir[i].num>0) {
        if(board_dir[i].num==obj_ptr->type) goto exit_while;
        i++;
    }
exit_while:
    if(board_dir[i].num<0) {
        print(fd,"잘못된 게시판입니다.");
        return 0;
    }

    sprintf(name,"%s/board_index",board_dir[i].dir);
    if((handle=open(name,O_RDWR))==-1) {
        print(fd,"인덱스화일을 읽을수 없습니다.");
        return 0;
    } 

    if(lseek(handle,0,SEEK_END)<cmnd->val[1]*sizeof(struct BOARD_INDEX)) {
        print(fd, "범위에 벗어나는 게시물입니다.");
        return 0;
    }
    lseek(handle,(cmnd->val[1]-1)*sizeof(struct BOARD_INDEX),SEEK_SET);
    read(handle,&board_index,sizeof(struct BOARD_INDEX));
    
    if(strcmp(board_index.upload,ply_ptr->name) && ply_ptr->class<DM) {
        print(fd,"당신에게는 삭제할 권한이 없습니다.");
        close(handle);
        return 0;
    }
    board_index.readnum=-board_index.readnum;
    if(board_index.readnum==0) board_index.readnum=-1;
    if(board_index.readnum<0) {
        print(fd,"게시물이 삭제되었습니다.");
    }
    else {
        print(fd,"삭제된 게시물을 복구하였습니다.");
    }
    lseek(handle,(cmnd->val[1]-1)*sizeof(struct BOARD_INDEX),SEEK_SET);
    write(handle,&board_index,sizeof(struct BOARD_INDEX)); 
    close(handle);
    return 0;
}

