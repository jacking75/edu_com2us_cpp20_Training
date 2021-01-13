/*
 * COMMAND11.C:
 *
 *
 *   Additional user routines
 *
 */

#include "mstruct.h"
#include "mextern.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <stdio.h>

/******************************************************************/
/*                              emote                             */
/******************************************************************/

/* This command allows a player to echo a message unaccompanied by */
/* any message format, except for the players name at the beginning */

int emote(ply_ptr, cmnd)
creature        *ply_ptr;
cmd             *cmnd;
{
        room            *rom_ptr;
        int             index = -1, i, fd;
        int             len;

        fd = ply_ptr->fd;
        rom_ptr = ply_ptr->parent_rom;

        index=0;
        len=cut_command(cmnd->fullstr);
        /*
        for(i=0; i<strlen(cmnd->fullstr) && i < 256; i++) {
                if(cmnd->fullstr[i] == ' ') {
                        index = i + 1;
                        break;
                }
        }
        */
        cmnd->fullstr[255] = 0;

        if(len==0) {
                print(fd, "무슨말을 표현하시려구요?\n");
                return(0);
        }
        if(F_ISSET(ply_ptr, PSILNC)){
                print(fd, "당신은 지금당장 그것을 할 수 없습니다.\n");
                return(0);
        }
        F_CLR(ply_ptr, PHIDDN);
        if(F_ISSET(ply_ptr, PLECHO)){
                ANSI(fd, CYAN);
                print(fd, ":%M이 %s.", ply_ptr, &cmnd->fullstr[index]);
                ANSI(fd, NORMAL);
                ANSI(fd, WHITE);
        }
        else
                print(fd, "예. 좋습니다.\n");

        broadcast_rom(fd, rom_ptr->rom_num, "\n:%M이 %s.",
                        ply_ptr, &cmnd->fullstr[index]);
        paste_command(cmnd->fullstr,len);

        return(0);
}
/*==============================================================*/
/*                          passwd                              */
/*==============================================================*/
/* The passwd function callls the necessary function to allow *
 * a player to change their password. */

int passwd (ply_ptr, cmnd)
creature    *ply_ptr;
cmd     *cmnd;
{
int     fd;
extern void chpasswd();
        fd = ply_ptr->fd;

                /* do not flash output until player hits return */
        F_SET(Ply[fd].ply, PREADI);
                chpasswd(fd,0,"");
        return(DOPROMPT);

}
/*==============================================================*/
/*                       chpasswd                               */
/*==============================================================*/
/* The chpasswd command handles  the procedure involved in       *
 * changing a player's password.  A player first must enter the  *
 * correct current password, then the new password, and re enter *
 * the new password to comfirm it. If the player enters the      *
 * wrong password  or an invalid password (too short or long),   *
 * the password will not be changed and the procedure is aborted. */

void chpasswd(fd,param,str)
int     fd;
int     param;
char *str;
{
        creature        *ply_ptr;


        ply_ptr = Ply[fd].ply;
        output_buf();
        switch (param) {
        case 0:
                        print(fd,"현재 암호를 입력하십시요: ");
                        output_buf();
            		Ply[fd].io->intrpt &= ~1;
                        RETURN(fd,chpasswd,1);
        break;
        case 1:
                if (!strcmp(ply_ptr->password,str)){
                        print(fd,"\n새 암호를 입력하십시요: ");
                     /* output_buf();
            		Ply[fd].io->intrpt &= ~1;
                     */ RETURN(fd,chpasswd,2);
                }
                else {
                        print(fd, "%c%c%c\n\r", 255, 252, 1);
                        print(fd,"암호가 틀렸습니다.\n");
                        print(fd,"암호가 변경되지 않았습니다.\n");
            		F_CLR(Ply[fd].ply, PREADI);
                        RETURN(fd, command, 1);
                }
        case 2:
                        if (strlen(str) <3){
                        	print(fd, "%c%c%c\n\r", 255, 252, 1);
                        	print(fd,"암호가 너무 짧습니다.\n");
                		print(fd,"암호가 변경되지 않았습니다.\n");
                		F_CLR(Ply[fd].ply, PREADI);
                                RETURN(fd, command, 1);
                        }
                        else if (strlen(str) > 14){
                        	print(fd, "%c%c%c\n\r", 255, 252, 1);
                                print(fd,"암호가 너무 깁니다.\n");
                                print(fd,"암호가 변경되지 않았습니다..\n");
                		F_CLR(Ply[fd].ply, PREADI);
                                RETURN(fd, command, 1);
                        }
                        else{
                                strcpy(Ply[fd].extr->tempstr[1], str);
                                print(fd,"\n새 암호를 다시 넣으십시요: ");
                             /* output_buf();
                		Ply[fd].io->intrpt &= ~1;
                             */ RETURN(fd,chpasswd,3);
                        }
        break;
        case 3:
                if(!strcmp(Ply[fd].extr->tempstr[1],str)){
                        strcpy(ply_ptr->password,str);
                        print(fd, "%c%c%c\n\r", 255, 252, 1);
                        print(fd,"암호가 변경되었습니다.\n");
                	F_CLR(Ply[fd].ply, PREADI);
                        save_ply(ply_ptr->name,ply_ptr);
                        RETURN(fd, command, 1);
                }
                else{
                        print(fd, "%c%c%c\n\r", 255, 252, 1);
                        print(fd,"암호가 서로 틀립니다.\n");
                        print(fd,"암호가 변경되지 않았습니다.\n");
                	F_CLR(Ply[fd].ply, PREADI);
            		RETURN(fd, command, 1);
                }
        break;

        }

}
/*==============================================================*/
/*                   vote                                       */
/*==============================================================*/
int vote (ply_ptr, cmnd)
creature    *ply_ptr;
cmd     *cmnd;
{
        int             fd;
        int     i,n, number;
        int             match = 0;
        char    str[80], tmp[256];
        FILE    *fp;
extern void vote_cmnd();

    fd = ply_ptr->fd;
        str[0] = 0;
        if(((18 + ply_ptr->lasttime[LT_HOURS].interval/86400L) < 21) && 
			ply_ptr->class < INVINCIBLE) {
                print(fd, "당신은 투표할 나이가 아닙니다.\n");
                return(0);
        }
        if (!F_ISSET(ply_ptr->parent_rom,RELECT)){
                print(fd,"투표소가 아닙니다.\n");
                return(0);
        }

        sprintf(Ply[fd].extr->tempstr[0],"%s/ISSUE",POSTPATH);
        fp = fopen(Ply[fd].extr->tempstr[0],"r");
        if(!fp){
                print(fd,"투표할 안건이 없네요.\n");
                return(0);
        }
        
        fgets(tmp, 256,fp);
        if (feof(fp)) {
                print(fd,"현재 투표할 안건이 없습니다.\n");
                return(0);
        }
        fclose(fp);

        n =  sscanf(tmp,"%d %s",&number,str);

        lowercize(str,1);
/*
        if (n <1){
                print(fd,"투표시간이 아닙니다.\n");
                return(0);
        } else if (n == 2 && str){
                        for (i=1;i<SUB_DM; i++)
                                if(!strcmp(str,class_str[i])){
                                        if(i == ply_ptr->class)
                                                match = 1;
                                        break;
                                }

                if(!match){
                        print(fd,"당신은 투표할수 없습니다.\n");
                        return(0);
                }
        }
*/
        Ply[fd].extr->tempstr[1][0] = MIN(79,number);
        Ply[fd].extr->tempstr[1][1] = 0;
        
        
        F_SET(Ply[fd].ply, PREADI); 
    vote_cmnd(fd,0,"");
    return(DOPROMPT);
                         
}
/*==============================================================*/
void vote_cmnd(fd,param,str)
int     fd;
int     param;
char *str;
{
    creature    *ply_ptr;
                int             i, n =0;
                char    tmp[1024], c;
                FILE    *fp;
        
        ply_ptr = Ply[fd].ply;

        output_buf();
        switch (param) {
        case 0:
                        sprintf(tmp,"%s/vote/%s_v",PLAYERPATH,ply_ptr->name);
                        fp = fopen(tmp,"r");
                        if (!fp){
                                fp = fopen(Ply[fd].extr->tempstr[0],"r");
                                if(!fp) {
                                        F_CLR(Ply[fd].ply, PREADI);
                                        RETURN(fd, command, 1);
                                }

                                fgets(tmp,1024,fp);
                                fgets(tmp,1024,fp);
                                tmp[1023] = 0;
                                fclose(fp);
                                print(fd,"\n%s",tmp);
                                print(fd,"당신의 선택은? : ");
                                output_buf();

                                Ply[fd].extr->tempstr[1][1] = 1;
                Ply[fd].io->intrpt &= ~1; 
                                RETURN(fd,vote_cmnd,2);
                        }
                        else {
                                fclose(fp);
                                print(fd,"당신은 이미 투표를 했습니다.\n");
                                print(fd,"당신의 선택을 바꾸시겠습니까? (y/n): ");
                                output_buf();
                Ply[fd].io->intrpt &= ~1; 
                                RETURN(fd,vote_cmnd,1);
                        }
        break;
        case 1:
                        if (str[0] == 'y' || str[0] == 'Y'){
                                sprintf(tmp,"%s/vote/%s_v",PLAYERPATH,ply_ptr->name);
                                unlink(tmp);

                                fp = fopen(Ply[fd].extr->tempstr[0],"r");
                                if(!fp) {
                                        F_CLR(Ply[fd].ply, PREADI);
                                        RETURN(fd, command, 1);
                                }

                                fgets(tmp,1024,fp);
                                fgets(tmp,1024,fp);
                                tmp[1023] = 0;
                                fclose(fp);
                                print(fd,"\n%s",tmp);
                                print(fd,"당신의 선택은? : ");
                                output_buf();

                                Ply[fd].extr->tempstr[1][1] = 1;
                Ply[fd].io->intrpt &= ~1; 
                                RETURN(fd,vote_cmnd,2);
                        }
            print(fd,"중단합니다.\n");
            F_CLR(Ply[fd].ply, PREADI);
            RETURN(fd, command, 1);

                break;
                case 2:
                        c = low(str[0]);
                        if (c != 'a' && c != 'b' && c != 'c' && c != 'd' && c != 'e' && c != 'f' && c != 'g'){
                                print(fd,"잘못된 선택입니다. 중단합니다.\n");
                                F_CLR(Ply[fd].ply, PREADI);
                                RETURN(fd, command, 1);
                        }
                        n = MAX(0,Ply[fd].extr->tempstr[1][1] -1);
                        Ply[fd].extr->tempstr[2][n] = up(c);
                        if (Ply[fd].extr->tempstr[1][1] >= Ply[fd].extr->tempstr[1][0]){
                                vote_cmnd(fd,3,"");
                                RETURN(fd,command,1);
                        }
                        else{
                                fp = fopen(Ply[fd].extr->tempstr[0],"r");
                                if(!fp) {
                                        F_CLR(Ply[fd].ply, PREADI);
                                        RETURN(fd, command, 1);
                                }

                                fgets(tmp,1024,fp);
                                if (feof(fp)) {
                                        F_CLR(Ply[fd].ply, PREADI);
                                        RETURN(fd, command, 1);
                                }
        
                                n = 0;
                                while(!feof(fp)){
                                        fgets(tmp,1024,fp);
                                        tmp[1023] = 0;
                                        n++;
                                        if ( n  == Ply[fd].extr->tempstr[1][1]+1)
                                                break;
                                }
                                print(fd,"\n%s",tmp);
                                print(fd,"당신의 선택은? : ");
                                output_buf();

                                Ply[fd].extr->tempstr[1][1] = n;
                                fgets(tmp,1024,fp);
                                if (feof(fp))
                                        Ply[fd].extr->tempstr[1][0] = Ply[fd].extr->tempstr[1][1];
                                fclose(fp);
        
                        Ply[fd].io->intrpt &= ~1; 
                                RETURN(fd,vote_cmnd,2);
                        }
                break;

                case 3:
                        n = Ply[fd].extr->tempstr[1][0] ;
                        sprintf(tmp,"%s/vote/%s_v",PLAYERPATH,ply_ptr->name);
                        Ply[fd].extr->tempstr[2][n] = 0;
                        i = open(tmp, O_CREAT | O_RDWR, ACC);
                        if(i < 0)
                merror("vote_cmnd", FATAL);
                                          
                        write(i,Ply[fd].extr->tempstr[2],n);
                        close(i);
                        print(fd,"투표를 하였습니다.\n");
                        F_CLR(Ply[fd].ply, PREADI);
            RETURN(fd, command, 1);
                break;
        }
}

/*************************************************************************/
/*                              pfinger                                  */
/*************************************************************************/
int pfinger(ply_ptr, cmnd) 
creature        *ply_ptr;
cmd             *cmnd;
{

    struct stat f_stat;
    creature    *player;
        char            tmp[80];
        int                     fd;

        fd = ply_ptr->fd; 
    if(cmnd->num < 2) {
        print(fd, "누구의 정보를 보고 싶으세요?\n");
        return(0);
    }
        cmnd->str[1][0] = up(cmnd->str[1][0]);
        player = find_who(cmnd->str[1]);

        if (!player){

        if(load_ply(cmnd->str[1], &player) < 0){
                print(fd,"그런 사용자는 없습니다.\n");
                return (0);
        }             

          if (ply_ptr->class < DM && player->class == DM) {
                        print(fd,"당신은 그 사용자의 정보를 볼 수 없습니다.\n");
                        return (0);
                }

                sprintf(tmp,"%s/%s/%s",PLAYERPATH,first_han(cmnd->str[1]), cmnd->str[1]);
            if (stat(tmp,&f_stat)){
                print(fd,"그런 사용자는 없습니다.\n");
                return (0);
        }
 
            print(fd,"%s %+25s %s\n",player->name, 
                 race_str[player->race],class_str[player->class] );
        print(fd,"마지막 접속시간: %s",ctime(&f_stat.st_ctime));
		if(F_ISSET(player, SUICD)) {
			print(fd, "그 사용자는 자살신청한 사용자입니다.\n");
		}
        free_crt(player);
        } 
        else{
            if(ply_ptr->class < SUB_DM && F_ISSET(player, PDMINV) && (player->class == SUB_DM)) {
                print(fd,"%s %+25s %s\n",player->name, 
                     race_str[player->race],class_str[player->class] );
                print(fd,"마지막 접속시간: %s",ctime(&f_stat.st_ctime));
                return(0);
            }
                if (F_ISSET(player, PDMINV) && 
                  (ply_ptr->class < SUB_DM || (ply_ptr->class == SUB_DM &&
          player->class == DM))) {
                        print(fd,"당신은 그 사용자의 정보를 볼 수 없습니다.\n");
                        return (0);
                }

            print(fd,"%s %+25s %s\n",player->name, 
                 race_str[player->race], class_str[player->class]);
        print(fd,"현재 접속 중 입니다.\n");
        }

    sprintf(tmp,"%s/%s",POSTPATH,cmnd->str[1]);
    if (stat(tmp,&f_stat)){
       print(fd,"받은 편지가 없습니다.\n");
       return (0);
    }

        if (f_stat.st_atime > f_stat.st_ctime)
                print(fd,"읽지 않은 편지가 도착한 날짜: %s",ctime(&f_stat.st_atime));
        else
                print(fd,"새 편지가 도착한 날짜: %s",ctime(&f_stat.st_ctime));

}

/******************************************************************************
*  			패거리 
******************************************************************************/

int load_family()
{
	FILE *fp;
	int fnum;
	char file[80];

	sprintf(file, "%s/family/family_list", PLAYERPATH);
	fp = fopen(file, "r");
	
	fnum =0 ;
	do {
		fscanf(fp, "%d %s %s %d", &family_num[fnum],family_str[fnum], 
			fmboss_str[fnum], &family_gold[fnum]);
		if(family_num[fnum] == 16) break;
		fnum ++;
	}while(fnum < MAXFAMILY);
	fclose(fp);
	return(fnum);
}

int family(ply_ptr,cmnd)
creature 	*ply_ptr;
cmd		*cmnd;
{
	int	fd;
extern void add_family();

	fd = ply_ptr->fd;

	F_SET(ply_ptr, PREADI);

	if(F_ISSET(ply_ptr, PFAMIL)) {
		print(fd,"당신은 이미 패거리에 가입이 되어있습니다.\n");
		F_CLR(ply_ptr, PREADI);
		RETURN(fd, command, 1);
	}
	
	if(F_ISSET(ply_ptr, PRDFML)) {
		print(fd,"당신은 이미 가입신청을 해두고 있습니다.\n");
		F_CLR(ply_ptr, PREADI);
		RETURN(fd, command, 1);
	}

	add_family(fd, 0, "");
	return(DOPROMPT);
}

void add_family(fd, param, str)
int fd;
int param;
char *str;
{
	int i, num;
	int ftotal=16;
	creature *ply_ptr;
	creature *crt_ptr;
	ply_ptr = Ply[fd].ply;

#ifdef CHECKFAMILY 
	ftotal = load_family();
#endif
	switch(param) {
	case 0 :
		print(fd, "다음과 같은 패거리가 있습니다.\n");
		print(fd, "\n%-20s  %-20s\n\n","패거리이름","두목 이름");
		for(num = 1; num < ftotal; num++) {
			print(fd,"%-20s  %-20s\n",family_str[num], fmboss_str[num]); 
		}
		print(fd,"\n당신은 어떤 패거리에 가입을 원하십니까? ");
		print(fd, "\n패거리의 이름을 입력해 주십시요.  ");
		RETURN(fd,add_family,1);
	case 1 :	
		for(num = 0; num < ftotal; num++){	
			if(!strcmp(str, family_str[num])) {
				if(!strcmp(fmboss_str[num], "*해체*")) {
					print(fd, "해체된 패거리입니다.");
					F_CLR(ply_ptr, PREADI);
					RETURN(fd, command, 1);
				}
				crt_ptr = find_who(fmboss_str[num]); 
				if(!crt_ptr) {
					print(fd, "패거리의 두목인 %s님이 현재 이용중이 아닙니다.",fmboss_str[num]); 
					F_CLR(ply_ptr, PREADI);
					RETURN(fd, command, 1);
				}
				ply_ptr->daily[DL_EXPND].max = family_num[num];
				print(fd, "%s에 가입을 하시겠습니까? (예/아니오) ",family_str[num]);
				RETURN(fd, add_family, 2);
			} 
		}
		print(fd,"\n잘못된 선택입니다.\n");
		F_CLR(ply_ptr, PREADI);
		RETURN(fd, command, 1);
	case 2 :
		if(strcmp(str, "예")) {
			print(fd,"\n가입 신청을 취소합니다.");
			ply_ptr->daily[DL_EXPND].max = 0;
			F_CLR(ply_ptr, PREADI);
			RETURN(fd, command, 1);
		}

		crt_ptr = find_who(fmboss_str[(ply_ptr->daily[DL_EXPND].max)] ) ;
		print(crt_ptr->fd, "\n>>> %s님이 당신의 패거리에 가입하기를 원합니다.", ply_ptr->name );
		F_SET(ply_ptr, PRDFML);
		print(fd,"\n가입 신청을 하였습니다. \n");
		print(fd,"패거리 두목의 허가를 기다리십시요."); 
		F_CLR(ply_ptr, PREADI);
		RETURN(fd, command, 1);
	}
}

int boss_family(ply_ptr, cmnd)
creature *ply_ptr;
cmd *cmnd;
{
	int fd;
	int ftotal;
	int num;
	creature *crt_ptr;
	fd = ply_ptr->fd;
#ifdef CHECKFAMILY 
	ftotal = load_family();

	if(!strcmp(ply_ptr->name, fmboss_str[ply_ptr->daily[DL_EXPND].max])) {
		F_SET(ply_ptr, PFMBOS);
	}
#endif

	if(!F_ISSET(ply_ptr, PFMBOS)) {
		print(fd, "패거리의 두목만이 가능합니다.");
		RETURN(fd, command, 1);
	}
	if(cmnd->num < 2) {
		print(fd, "누구의 가입을 허가 하시겠습니까?");
		RETURN(fd, command,1 );
	}

	F_SET(ply_ptr, PREADI);
	lowercize(cmnd->str[1], 1);
	crt_ptr = find_who(cmnd->str[1]);
	
	if(!crt_ptr || F_ISSET(crt_ptr, PDMINV) || F_ISSET(ply_ptr, PBLIND) ||
		(F_ISSET(crt_ptr, PINVIS) && !F_ISSET(ply_ptr, PDINVI))) {
		print(fd,"현재 이용중이 아닙니다.");
		F_CLR(ply_ptr, PREADI);
		RETURN(fd, command, 1);
	}
	if(!F_ISSET(crt_ptr, PRDFML) || (ply_ptr->daily[DL_EXPND].max !=
		crt_ptr->daily[DL_EXPND].max) || F_ISSET(crt_ptr, PFAMIL)) {
		print(fd,"당신의 패거리에 가입신청을 한 사람이 아닙니다.");
		F_CLR(ply_ptr, PREADI);
		RETURN(fd, command, 1);
	}
	
	num = ply_ptr->daily[DL_EXPND].max ;
	if(ply_ptr->gold < family_gold[num]*10000) {
		print(fd, "당신의 자금사정으로는 패거리원을 받을수 없습니다.");
		F_CLR(ply_ptr, PREADI);
		RETURN(fd, command, 1);
	}

	F_CLR(crt_ptr, PRDFML);
	F_SET(crt_ptr, PFAMIL);
	edit_member(crt_ptr->name, crt_ptr->class, ply_ptr->daily[DL_EXPND].max, 1);
	crt_ptr->gold += family_gold[num] * 10000;
	ply_ptr->gold -= family_gold[num] * 10000;
	print(fd,"%s님의 패거리 가입을 허가하였습니다.\n",crt_ptr->name);
	print(fd,"%s님에게 가입축하금을 지급하였습니다.",crt_ptr->name);
	broadcast_all("\n### %s님이 %s에 가입을 하였습니다.",crt_ptr->name,
			family_str[ply_ptr->daily[DL_EXPND].max ]);
	print(crt_ptr->fd, "\n당신은 당신의 패거리로부터 가입축하금을 받았습니다.");
	print(crt_ptr->fd, "\n당신은 이제 %ld냥을 갖고 있습니다.",crt_ptr->gold);
	F_CLR(ply_ptr,PREADI);
	return(0);
}

int out_family(ply_ptr, cmnd)
creature *ply_ptr;
cmd *cmnd;
{
	int fd;
extern void exit_family();
	fd = ply_ptr->fd;
	
	F_SET(ply_ptr, PREADI);
	exit_family(fd, 0, "");
	return(DOPROMPT);
}

void exit_family(fd, param, str)
int fd;
int param;
char *str;
{
	int ftotal;
	creature *ply_ptr;
	ply_ptr = Ply[fd].ply;

#ifdef CHECKFAMILY 
	ftotal = load_family();
	if(!strcmp(ply_ptr->name, fmboss_str[ply_ptr->daily[DL_EXPND].max])) {
		F_SET(ply_ptr, PFMBOS);
	}
	else F_CLR(ply_ptr, PFMBOS);
#endif

	if(F_ISSET(ply_ptr, PFMBOS)) {
		print(fd, "패거리의 두목은 탈퇴를 할수 없습니다.");
		F_CLR(ply_ptr, PREADI);
		RETURN(fd, command, 1);
	}
	if(F_ISSET(ply_ptr, PRDFML)) {
		print(fd, "패거리 가입신청을 취소합니다.");
		F_CLR(ply_ptr, PRDFML);
		ply_ptr->daily[DL_EXPND].max = 0;
		F_CLR(ply_ptr, PREADI);
		RETURN(fd, command, 1);
	}
	if(!F_ISSET(ply_ptr, PFAMIL) && !F_ISSET(ply_ptr, PRDFML)) {
		print(fd, "당신은 어떤 패거리에도 가입이 되어 있지 않습니다.");
		F_CLR(ply_ptr, PREADI);
		RETURN(fd, command, 1);
	} 

	switch(param) {
	case 0 :
		print(fd,"당신은 지금 현재의 패거리를 탈퇴하실 생각입니까? (예/아니오) ");
		RETURN(fd, exit_family, 1);
	case 1 :
		if(strcmp(str, "예")) {
			print(fd, "패거리를 탈퇴하지 않았습니다.");
			F_CLR(ply_ptr, PREADI);
			RETURN(fd, command, 1);
		}
		else {
			if(ply_ptr->gold < family_gold[ply_ptr->daily[DL_EXPND].max ] * 20000 ) {
				print(fd, "당신이 가진 돈으로는 패거리탈퇴비를 낼수 없습니다.\n");
				print(fd, "패거리를 탈퇴하지 않았습니다.");
				F_CLR(ply_ptr, PREADI);
				RETURN(fd, command, 1);
			}
			print(fd, "당신은 패거리에서 탈퇴를 하였습니다.\n");
			broadcast_all("\n### %s님이 %s에서 탈퇴를 하였습니다.",ply_ptr->name,
				family_str[ply_ptr->daily[DL_EXPND].max]);
			ply_ptr->gold -= family_gold[ply_ptr->daily[DL_EXPND].max] * 20000 ;
			print(fd, "\n당신은 이제 %ld냥을 갖고 있습니다.",ply_ptr->gold);
		edit_member(ply_ptr->name, ply_ptr->class, ply_ptr->daily[DL_EXPND].max, 2);
			F_CLR(ply_ptr, PFAMIL);
			ply_ptr->daily[DL_EXPND].max = 0;
			F_CLR(ply_ptr, PREADI);
			RETURN(fd, command, 1);
		}
	}
}

int family_talk(ply_ptr, cmnd)
creature  *ply_ptr;
cmd	*cmnd;
{
	creature *crt_ptr;
	int i, fd;
	int len, num, ftotal;

	fd = ply_ptr->fd;
	num = ply_ptr->daily[DL_EXPND].max;
#ifdef CHECKFAMILY 
	ftotal = load_family();
#endif

	if(!F_ISSET(ply_ptr, PFAMIL)) {
		print(fd, "당신은 패거리에 속해있지 않습니다.");
		return(0);
	}
	if(F_ISSET(ply_ptr, PSILNC)) {
		print(fd, "입이 막혀 말이 나오질 않습니다.");
		return(0);
	}
	
	len = cut_command(cmnd->fullstr);
	cmnd->fullstr[255] = 0;

	if(len == 0) {
		print(fd, "패거리원들에게 무슨 말을 하시려고요?");
		return(0);
	}
	
	for(i = 0; i < Tablesize; i++) {
		if(!Ply[i].ply) continue;
		if(Ply[i].ply->fd == -1) continue;
		if((F_ISSET(Ply[i].ply, PFAMIL)) && (Ply[i].ply->daily[DL_EXPND].max == num)){ 
			print(Ply[i].ply->fd, "\n%C%s>>> %s%D", 
				"33", ply_ptr->name, &cmnd->fullstr[0], "37"); 
		}
	}
	broadcast_eaves("\n>>> %s님이 [%s]에게 \"%s\" 라고 이야기합니다.",
			ply_ptr->name, family_str[num], &cmnd->fullstr[0]);
	return(0);
}		

int family_who(ply_ptr, cmnd)
creature *ply_ptr;
cmd *cmnd;
{
	creature *crt_ptr;
	int i, fd;
	int num, ftotal, total;

	fd = ply_ptr->fd;
	num = ply_ptr->daily[DL_EXPND].max;
#ifdef CHECKFAMILY 
	ftotal = load_family();
#endif
	total = 0;

	if(F_ISSET(ply_ptr, PBLIND)) {
		print(fd, "당신은 눈이 멀어 있습니다!");
		return(0);
	}

	if(cmnd->num == 2) {
		lowercize(cmnd->str[1], 1);
		crt_ptr = find_who(cmnd->str[1]);
		if(!crt_ptr || F_ISSET(crt_ptr, PDMINV) || F_ISSET(ply_ptr, PBLIND) ||
			(F_ISSET(crt_ptr, PINVIS) && !F_ISSET(ply_ptr, PDINVI))) {
			print(fd,"현재 이용중이 아닙니다.");
			RETURN(fd, command, 1);
		}
		if(!F_ISSET(crt_ptr, PFAMIL) && !F_ISSET(crt_ptr, PRDFML)) {
			print(fd, "%s님은 어떤 패거리에도 소속되어 있지 않습니다.", crt_ptr->name);
			RETURN(fd, command, 1);
		}

		if(F_ISSET(crt_ptr, PRDFML)) {
			print(fd, "%s님은 [%s] 패거리에 가입을 신청중입니다.", crt_ptr->name, family_str[crt_ptr->daily[DL_EXPND].max]);
			RETURN(fd, command, 1);
		}
		print(fd, "%s님은 [%s] 패거리에 속해있습니다.",crt_ptr->name, family_str[crt_ptr->daily[DL_EXPND].max]);
		return(0);
	}

	if(!F_ISSET(ply_ptr, PFAMIL) && !F_ISSET(ply_ptr, PRDFML)) {
		print(fd, "당신은 어떤 패거리에도 소속되어 있지 않습니다.");
		return(0);
	}

	if(F_ISSET(ply_ptr, PRDFML)) {
		print(fd, "당신은 [%s] 패거리에 가입을 신청중입니다.\n\n",family_str[num]);
	}
	else {
		print(fd, "당신은 [%s] 패거리에 소속되어 있습니다.\n\n",family_str[num]);
	}

	for(i = 0; i < Tablesize; i++) {
		if(!Ply[i].ply) continue;
		if(Ply[i].ply->fd == -1) continue;
		if(!F_ISSET(Ply[i].ply, PFAMIL) && !F_ISSET(Ply[i].ply, PRDFML)) continue;
		if((F_ISSET(Ply[i].ply, PFAMIL)) && (Ply[i].ply->daily[DL_EXPND].max == num)) {
			print(fd, "   %-14s", Ply[i].ply->name);
			total ++;
		}
		else if((F_ISSET(Ply[i].ply, PRDFML)) && (Ply[i].ply->daily[DL_EXPND].max == num)) {
			print(fd, "(-)%-14s", Ply[i].ply->name);
			total ++;
		}
		else { continue; }
		if((total != 0 ) && (total % 3 == 0))
			print(fd, "\n");
	}
	if(total % 3 != 0) print(fd, "\n");
	print(fd, "\n총 %d명의 패거리원들이 이용중입니다.", total);
	return(0);
}

int list_family(ply_ptr, cmnd)
creature *ply_ptr;
cmd	*cmnd;
{
	int fd, i;
	int ftotal;

	fd = ply_ptr->fd;
	ftotal = load_family();

	F_SET(ply_ptr, PREADI);
	print(fd, "다음과 같은 패거리가 있습니다.\n");
	print(fd, "%-14s %-14s \n", "패거리이름", "두목이름"); 
	print(fd, "--------------------------------------------\n");
	for(i = 1; i < ftotal; i++) {
		print(fd, "%-14s %-14s \n", family_str[i], fmboss_str[i]);
	}
	print(fd, "\n총 %d 개의 패거리가 활동중에 있습니다.", ftotal-1 );
	F_CLR(ply_ptr, PREADI);
	return(0);
}

/***************************************************************************
*		경험치 전수..(초인 이상, 패거리만 가능)
**************************************************************************/
/* 일단은 루틴만 있고.. 사용은 하지 않는데.. 필요하면 호출해서 사용가능. */
/*************************************************************************/

int trans_exp(ply_ptr, cmnd)
creature *ply_ptr;
cmd	*cmnd;
{
	creature *crt_ptr;
	room *rom_ptr;
	int fd;
	long amt;

	fd = ply_ptr->fd;
	rom_ptr = ply_ptr->parent_rom;

	amt = atol(cmnd->str[1]);

	if(ply_ptr->class < CARETAKER) {
		print(fd, "당신의 능력으로는 사용할수 없습니다.");
		return(0);
	}

	if(!F_ISSET(ply_ptr, PFAMIL)) {
		print(fd, "패거리 가입자만 가능합니다.");
		return(0);
	}

	if(cmnd->num < 3) {
		print(fd, "누구에게 경험치를 전수하시려고요?");
		return(0);
	}

	if((amt < 1) || (amt > 100000)) {
		print(fd, "1에서 100000점 사이만 가능합니다.");
		return(0);
	}

	crt_ptr = find_crt(ply_ptr, rom_ptr->first_ply, cmnd->str[2], cmnd->val[2]);
	
	if(!crt_ptr || crt_ptr == ply_ptr) {
		print(fd, "그런 사람은 여기 없어요!");
		return(0);
	}

	if(!F_ISSET(crt_ptr, PFAMIL) || (ply_ptr->daily[DL_EXPND].max != crt_ptr->daily[DL_EXPND].max)) {
		print(fd, "당신의 패거리사람에게만 경험치전수가 가능합니다.");
		return(0);
	}

	if(crt_ptr->level > 50 || crt_ptr->class == INVINCIBLE ) {
		print(fd, "당신은 그사람에게 경험치를 줄 수 없습니다.");
		return(0);
	}

	if(ply_ptr->experience < 100000) {
		print(fd, "당신은 남에게 줄만큼의 경험치를 가지고 있지 않습니다.");
		return(0);
	}

	ply_ptr->experience -= amt;
	crt_ptr->experience += amt/20;
	print(fd, "당신은 %s님에게 자신의 경험치 %ld점을 나눠주었습니다.", crt_ptr->name, amt);
	print(crt_ptr->fd, "\n$$$ %s님이 당신에게 경험치 %ld점을 나눠주었습니다.\n", ply_ptr->name, amt/10);
	return(0);
}

/****************************************************************************
*                   초인의 체력 도력 구입                                   *
****************************************************************************/

int buy_states(ply_ptr, cmnd)
creature *ply_ptr;
cmd *cmnd;
{
	int fd, amt;
	int i;
	int amt2;
	fd = ply_ptr->fd;

	if(ply_ptr->class != CARETAKER) {
		print(fd, "초인만이 가능합니다.");
		return(0);
	}
	
	if(cmnd->num < 2) {
		print(fd, "\"체력\" 과 \"도력\" 중 어느 것을 올리시려고요?");
		return(0);
	}
	
	amt = (ply_ptr->experience - 100000000L) / 1000000L;

	if(amt < 1) {
		print(fd, "당신의 경험치로는 능력치 향상을 할 수 없습니다.");
		return(0);
	} 
	
 	if(ply_ptr->gold < amt*1000000L) {
		print(fd, "당신이 가진 돈으로는 능력치 향상을 할 수 없습니다.");
		return(0);
	}
	
	if(!strcmp(cmnd->str[1], "체력")) {
		if(ply_ptr->hpmax >= 2000 && ply_ptr->level < 128) {
			print(fd, "더이상은 체력을 올릴수 없습니다.");
			return(0);
		}
		ply_ptr->experience -= amt*1000000L;
		ply_ptr->gold -= amt*1000000L;
		amt2 = amt;
		amt *= 2;
		for(i=0; i<amt2; i++) {
			amt += mrand(0,3);
			amt++;
		}
		amt--;
		ply_ptr->hpmax += amt;
//		ply_ptr->hpmax = MIN(2000, ply_ptr->hpmax);
		ply_ptr->hpcur = ply_ptr->hpmax;
	}
	
	else if(!strcmp(cmnd->str[1], "도력")) {
		if(ply_ptr->mpmax >= 2000 && ply_ptr->level < 128) {
			print(fd, "더이상은 도력을 올릴수 없습니다.");
			return(0);
		}
		ply_ptr->experience -= amt*1000000L;
		ply_ptr->gold -= amt*1000000L;
		amt2 = amt;
		amt *= 2;
		for(i=0; i<amt2; i++) {
			amt += mrand(0,3);
			amt++;
		}
		amt--;
		ply_ptr->mpmax += amt;
//		ply_ptr->mpmax = MIN(2000, ply_ptr->mpmax);
		ply_ptr->mpcur = ply_ptr->mpmax;
	}
	
	else if(!strcmp(cmnd->str[1], "힘")) {
		if(ply_ptr->experience - 100000000L < 2000000L) {
			print(fd, "당신의 경험치로는 능력치를 올릴수 없습니다.");
			return(0);
		}
		if(ply_ptr->strength > 44) {
			print(fd, "더이상 올릴수 없는 능력치입니다.");
			return(0);
		}
		ply_ptr->experience -= 2000000L;
	    ply_ptr->gold -= 1000000L;
		ply_ptr->strength++;
		if(mrand(0,1)) 
			ply_ptr->hpmax += 4;
		else ply_ptr->mpmax += 3;
	}
	
	else if(!strcmp(cmnd->str[1], "민첩")) {
		if(ply_ptr->experience - 100000000L < 2000000L) {
			print(fd, "당신의 경험치로는 능력치를 올릴수 없습니다.");
			return(0);
		}
		if(ply_ptr->dexterity > 44) {
			print(fd, "더이상 올릴수 없는 능력치입니다.");
			return(0);
		}
		ply_ptr->experience -= 2000000L;
	    ply_ptr->gold -= 1000000L;
		ply_ptr->dexterity++;
		if(mrand(0,1)) 
			ply_ptr->hpmax += 4;
		else ply_ptr->mpmax += 3;
	}
	
	else if(!strcmp(cmnd->str[1], "맷집")) {
		if(ply_ptr->experience - 100000000L < 2000000L) {
			print(fd, "당신의 경험치로는 능력치를 올릴수 없습니다.");
			return(0);
		}
		if(ply_ptr->constitution > 44) {
			print(fd, "더이상 올릴수 없는 능력치입니다.");
			return(0);
		}
		ply_ptr->experience -= 2000000L;
	    ply_ptr->gold -= 1000000L;
		ply_ptr->constitution++;
		if(mrand(0,1)) 
			ply_ptr->hpmax += 4;
		else ply_ptr->mpmax += 3;
	}
	
	else if(!strcmp(cmnd->str[1], "지식")) {
		if(ply_ptr->experience - 100000000L < 2000000L) {
			print(fd, "당신의 경험치로는 능력치를 올릴수 없습니다.");
			return(0);
		}
		if(ply_ptr->intelligence > 44) {
			print(fd, "더이상 올릴수 없는 능력치입니다.");
			return(0);
		}
		ply_ptr->experience -= 2000000L;
	    ply_ptr->gold -= 1000000L;
		ply_ptr->intelligence++;
		if(mrand(0,1)) 
			ply_ptr->hpmax += 4;
		else ply_ptr->mpmax += 3;
	}
	
	else if(!strcmp(cmnd->str[1], "신앙심")) {
		if(ply_ptr->experience - 100000000L < 2000000L) {
			print(fd, "당신의 경험치로는 능력치를 올릴수 없습니다.");
			return(0);
		}
		if(ply_ptr->piety > 44) {
			print(fd, "더이상 올릴수 없는 능력치입니다.");
			return(0);
		}
		ply_ptr->experience -= 2000000L;
	    ply_ptr->gold -= 1000000L;
		ply_ptr->piety++;
		if(mrand(0,1)) 
			ply_ptr->hpmax += 4;
		else ply_ptr->mpmax += 3;
	}
	
	else {
		print(fd, "어떤 능력치를 올리시려고요?");
		return(0);
	}
	ply_ptr->ndice = 4;
	ply_ptr->sdice = 4;
	if(!F_ISSET(ply_ptr, PUPDMG)) ply_ptr->pdice = 4;
	broadcast_all("\n### %s님의 능력치가 향상이 되었습니다!", ply_ptr->name);
	print(fd, "\n축하합니다! 당신의 능력치가 올랐습니다!");
	return(0);
}

/****************************************************************************
*                             결혼                                          *
****************************************************************************/

int marriage(ply_ptr, cmnd)
creature *ply_ptr;
cmd *cmnd;
{
	FILE *fp, *fp2, *fp3;
	int fd, mnum;
	char file[80];
	char file2[80];
	char file3[80];
	char str[15];
    char mstr[20];   
	room	*rom_ptr;
	creature *crt_ptr;

	fd = ply_ptr->fd;
	rom_ptr = ply_ptr->parent_rom;
	
	if(!F_ISSET(rom_ptr, RMARRI)) {
		print(fd, "이곳은 결혼식장이 아닙니다.");
		return(0);
	}

	if((18 + ply_ptr->lasttime[LT_HOURS].interval / 86400L) < 25) {
		print(fd, "당신은 결혼할 수 있는 나이가 아닙니다.");
		return(0);
	}
	if(F_ISSET(ply_ptr, PRDMAR)) {
		print(fd, "결혼 신청을 취소합니다.");
		F_CLR(ply_ptr, PRDMAR);
		return(0);
	}
	if(F_ISSET(ply_ptr, PMARRI)) {
		print(fd, "당신은 이미 결혼했잖아요!");
		return(0);
	}

	if(cmnd->num < 2) {
		print(fd, "누구와 결혼을 하시려고요?");
		return(0);
	}
	
	crt_ptr = find_who(cmnd->str[1]);
	if(!crt_ptr || F_ISSET(crt_ptr, PDMINV) || F_ISSET(ply_ptr, PBLIND) ||
		(F_ISSET(crt_ptr, PINVIS) && !F_ISSET(ply_ptr, PDINVI))) {
		print(fd, "그런 사람을 찾을 수가 없군요.");
		return(0);
	}

	if(F_ISSET(ply_ptr, PMALES) && F_ISSET(crt_ptr, PMALES)) {
		print(fd, "남자끼리 결혼하시려고요?");
		return(0);
	}
	if(!F_ISSET(ply_ptr, PMALES) && !F_ISSET(crt_ptr, PMALES)) {
		print(fd, "여자끼리 결혼하시려고요?");
		return(0);
	}
	
	if((18 + crt_ptr->lasttime[LT_HOURS].interval / 86400L) < 25) {
		print(fd, "그사람은 아직 결혼할 나이가 되지 않았습니다.");
		return(0);
	}
	if(F_ISSET(crt_ptr, PMARRI)) {
		print(fd, "그사람은 이미 결혼한 사람입니다.");
		return(0);
	}
		
	if(!F_ISSET(crt_ptr, PRDMAR)) {
		print(fd, "당신은 %s님에게 결혼을 신청하였습니다.", crt_ptr->name);
		F_SET(ply_ptr, PRDMAR);
  /*      
		sprintf(file, "%s/marriage/%s", PLAYERPATH, ply_ptr->name);	
		fp = fopen(file, "w+");
		fprintf(fp, "%s\n", crt_ptr->name);
  */      
            sprintf(mstr, "m%s", crt_ptr->name);
            strcpy(ply_ptr->key[2], mstr);         
		print(crt_ptr->fd, "\n%C%s님이 당신에게 결혼을 신청합니다.%D",
			"36", ply_ptr->name, "37");
//		fclose(fp);
		return(0);
	}
	else {
 /*   
		sprintf(file2, "%s/marriage/%s", PLAYERPATH, crt_ptr->name);
		fp2 = fopen(file2, "r");
		fseek(fp2, 0L, 1);
		fscanf(fp2, "%s", str);
  */      
  
        sprintf(str, "%s", &crt_ptr->key[2][1]);      

		if(!strcmp(ply_ptr->name, str)) {
			print(fd, "당신은 %s님의 결혼신청을 받아들입니다.\n", crt_ptr->name);
			print(crt_ptr->fd, "\n%s님이 당신의 결혼신청을 받아들였습니다.",
				ply_ptr->name);
			F_CLR(ply_ptr, PRDMAR);
			F_CLR(crt_ptr, PRDMAR);
			F_SET(ply_ptr, PMARRI);
			F_SET(crt_ptr, PMARRI);
//			sprintf(file, "%s/marriage/%s", PLAYERPATH, ply_ptr->name);	
//			fp = fopen(file, "w+");
//			fprintf(fp, "%s\n", crt_ptr->name);
			broadcast_all("\n### %s님과 %s님이 결혼을 하였습니다.", crt_ptr->name, ply_ptr->name);
 //           sprintf(mstr, "m%s", ply_ptr->name);
 //           strcpy(crt_ptr->key[2], mstr);
            sprintf(mstr, "m%s", crt_ptr->name);
            strcpy(ply_ptr->key[2], mstr);         
//			fclose(fp);
//			fclose(fp2);
		}
		else {
			print(fd, "그사람은 다른 사람과 결혼을 준비중입니다.");
//			fclose(fp2);
			return(0);
		}
	}
	return(0);
}

/*****************************************************************************
			m_send(결혼한 사람끼리의 대화)
*****************************************************************************/

int m_send(ply_ptr, cmnd)
creature *ply_ptr;
cmd *cmnd;
{
	FILE *fp;
	creature *crt_ptr;
	int len, fd;
	char file[80];
	char str[15];
	fd = ply_ptr->fd;

	if(!F_ISSET(ply_ptr, PMARRI)) {
		print(fd, "당신은 결혼하지 않았습니다.");
		return(0);
	}

    sprintf(str, "%s", &ply_ptr->key[2][1]);    
	crt_ptr = find_who(str);
	if(!crt_ptr) {
		print(fd, "당신의 배우자는 지금 이용중이 아닙니다.");
		return(0);
	}

	len = cut_command(cmnd->fullstr);
	cmnd->fullstr[255] = 0;
	if(len == 0) {
		print(fd, "무슨 말을 전하시려고요?");
		return(0);
	}
	if(F_ISSET(ply_ptr, PLECHO)){
		print(fd, "당신은 %M에게 \"%s\"라고 이야기합니다.", crt_ptr,
			&cmnd->fullstr[0]);
	}
	else
		print(fd, "%s님에게 말을 전달하였습니다.", crt_ptr->name);
	
	print(crt_ptr->fd, "\n%C%M%j 당신에게 \"%s\"라고 이야기합니다.%D",
		"34", ply_ptr, "1", &cmnd->fullstr[0], "37");
	return(0);
}


/*      이혼        */

int divorce(ply_ptr, cmnd)
creature    *ply_ptr;
cmd         *cmnd;
{
    int     fd;
    char    str[20];
    creature    *crt_ptr;

    fd = ply_ptr->fd;

    if(F_ISSET(ply_ptr, PRDMAR) && !F_ISSET(ply_ptr, PMARRI)) {
        print(fd, "결혼 신청을 취소합니다.");
        F_CLR(ply_ptr, PRDMAR);
        return(0);
    }
    if(!F_ISSET(ply_ptr, PMARRI)) {
        print(fd, "당신은 결혼하지 않았습니다.");
        return(0);
    }
    if(F_ISSET(ply_ptr, PRDDIV)) {
        print(fd, "이혼 신청을 취소합니다.");
        F_CLR(ply_ptr, PRDDIV);
        return(0);
    } 
    
    sprintf(str, "%s", &ply_ptr->key[2][1]);
    crt_ptr = find_who(str);

    if(!crt_ptr) {
        if(load_ply(str, &crt_ptr) < 0) {
            print(fd, "당신의 배우자는 존재하지 않습니다.");
            F_CLR(ply_ptr, PMARRI);
            ply_ptr->key[2][0] = 0;
            return(0);
        }
        print(fd, "당신의 배우자가 현재 접속중이지 않습니다.");
        return(0);
    }    
    
    if(!F_ISSET(crt_ptr, PRDDIV)) {
        print(fd, "당신은 당신의 배우자에게 이혼신청을 하였습니다.");
        F_SET(ply_ptr, PRDDIV);
        print(crt_ptr->fd, "\n%C%s님이 당신에게 이혼신청을 하였습니다.%D",
                "36", ply_ptr->name, "37");
        return(0);
    }
    else {
        print(fd, "당신은 %s님의 이혼신청을 받아들입니다.", crt_ptr->name);
        print(crt_ptr->fd, "\n%s님이 당신의 이혼신청을 받아들였습니다.",
                    ply_ptr->name);
        F_CLR(ply_ptr, PRDMAR);
        F_CLR(ply_ptr, PMARRI);
        F_CLR(ply_ptr, PRDDIV);
        F_CLR(crt_ptr, PRDMAR);
        F_CLR(crt_ptr, PMARRI);
        F_CLR(crt_ptr, PRDDIV);
        broadcast("\n### %s님과 %s님이 이혼을 하였습니다.",
                ply_ptr->name, crt_ptr->name);
        return(0);
    }
}
        


































