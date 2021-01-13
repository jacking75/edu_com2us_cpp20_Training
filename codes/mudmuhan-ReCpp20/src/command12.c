/*
*   command12.c
*/

#include "mstruct.h"
#include "mextern.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>

/******************************************************************
                     비교
******************************************************************/

int obj_compare(ply_ptr, cmnd)
creature *ply_ptr;
cmd *cmnd;
{
	int fd;
	object *obj_ptr;
	int check_ac, check_dmg;

	fd = ply_ptr->fd;

	if(cmnd->num < 2) {
		print(fd, "무엇을 비교하시려고요?");
		return(0);
	}

	obj_ptr = find_obj(ply_ptr, ply_ptr->first_obj,
			cmnd->str[1], cmnd->val[1]);
	if(!obj_ptr) {
		print(fd, "당신은 그런 것을 갖고 있지 않습니다.");
		return(0);
	}
	if(obj_ptr->type == ARMOR) {
		if(obj_ptr->wearflag == BODY)
			check_ac = obj_ptr->armor * 2;
		else check_ac = obj_ptr->armor * 5;

		if(check_ac > 29) {
			print(fd, "%I%j %d 레벨부터 입을 수 있습니다.", obj_ptr,
				"0", check_ac);
		}
		else print(fd, "%I%j 누구나 입을 수 있습니다.", obj_ptr, "0");
		return(0);
	}
	else if(obj_ptr->type < ARMOR) {
		check_dmg = obj_ptr->ndice * obj_ptr->sdice + obj_ptr->pdice;
		if(ply_ptr->class == FIGHTER) check_dmg -= 7;
		if(ply_ptr->class == ASSASSIN || ply_ptr->class == THIEF)
			check_dmg -= 3;
		if(ply_ptr->class == PALADIN || ply_ptr->class == RANGER)
			check_dmg -= 2;

		if(ply_ptr->class >= INVINCIBLE) {
			if(check_dmg > 15) {
			print(fd, "%I%j 검사는 %d레벨, 자객 도둑은 %d레벨, 무사 포졸은 %d레벨,\n",
				obj_ptr, "0", (check_dmg-7)*3, (check_dmg-3)*3, (check_dmg-2)*3);
			print(fd, "권법가 불제자 도술사는 %d레벨부터 사용할 수 있습니다.", check_dmg*3);
			}
			else print(fd, "%I%j 누구나 무장할 수 있습니다.", obj_ptr, "0");
			return(0);
		}
		if(check_dmg > 15) {
			print(fd, "%I%j %d 레벨부터 무장할 수 있습니다.",
				obj_ptr, "0", check_dmg*3);
		}
		else print(fd, "%I%j 누구나 무장할 수 있습니다.", obj_ptr, "0");
		return(0);
	}
	else {
		print(fd, "무기나 방어구만 가능합니다.");
		return(0);
	}
}


int memo(ply_ptr, cmnd)
creature *ply_ptr;
cmd *cmnd;
{ 
	char file[80], file2[80];
	char str[50], str2[71], str3[71];
	int fd, fp, ff;
	int len, space=0, i;
	long t;

	fd = ply_ptr->fd;

	if(cmnd->num < 3) {
		print(fd, "누구에게 어떤 메모를 남기시려고요?\n");
		return(0);
	}

	cmnd->str[1][0] = up(cmnd->str[1][0]);
	sprintf(file, "%s/%s/%s", PLAYERPATH, first_han(cmnd->str[1]), cmnd->str[1]);
	ff = open(file, O_RDONLY, 0);
	if(ff < 0) {
		print(fd, "그런 사용자는 없습니다.\n");
		return(0);
	}
	close(ff);

	len = cut_command(cmnd->fullstr);
	if(len > 80) {
		print(fd, "메모의 내용이 너무 깁니다.");
		return(0);
	}

	sprintf(file2, "%s/fal/%s", PLAYERPATH, cmnd->str[1]);
	fp = open(file2, O_RDWR, 0);
	if(fp < 0) {
		fp = open(file2, O_RDWR | O_CREAT, ACC);
		if(fp < 0) return(0);
	}
	lseek(fp, 0L, 2);
	for(i =0; i< len && i<256; i++) {
		if(cmnd->fullstr[i] == ' ' && cmnd->fullstr[i+1] != ' ') {
		space++;
		break;
		}
	}
	cmnd->fullstr[255] = 0;
	if(space < 1 || strlen(&cmnd->fullstr[i+1]) < 1) {
		print(fd, "무슨 말을 남기시려고요?");
		return(0);
	}

	t = time(0);
	strcpy(str, (char *)ctime(&t));
	str[strlen(str)-1] = 0;

	write(fp, str, strlen(str));
	sprintf(str2, " 에 [%s] 님이 남기신 메모 : \n", ply_ptr->name);
	write(fp, str2, strlen(str2));
	write(fp, ">>>>> ", strlen(">>>>> "));
	write(fp, &cmnd->fullstr[i+1], strlen(&cmnd->fullstr[i+1]));
	strcpy(str3, "\n");
	write(fp, str3, strlen(str3));
	close(fp);
	print(fd, "메모를 남겼습니다.");
	return(0);
}

int fm_out(ply_ptr, cmnd)
creature *ply_ptr;
cmd *cmnd;
{
	struct	stat	f_stat;
	creature	*player;
	int		fd;
	int 	ftotal;
	char	tmp[80];
	fd = ply_ptr->fd;

#ifdef CHECKFAMILY 
	ftotal = load_family();
	if(!strcmp(ply_ptr->name, fmboss_str[ply_ptr->daily[DL_EXPND].max])) {
		F_SET(ply_ptr, PFMBOS);
	}
#endif

	if(!F_ISSET(ply_ptr, PFMBOS)) {
		print(fd, "패거리의 두목만이 사용가능한 명령입니다.\n");
		return(0);
	}
	if(cmnd->num < 2) {
		print(fd, "누구를 패거리에서 쫓아내시려고요?\n");
		return(0);
	}
	cmnd->str[1][0] = up(cmnd->str[1][0]);
	player = find_who(cmnd->str[1]);
	if(ply_ptr == player) {
		print(fd, "자기 자신을 추방하려고요?");
		return(0);
	}
	if(!player) {
	if(load_ply(cmnd->str[1], &player) < 0) {
		print(fd, "그런 사용자는 없습니다.\n");
		return(0);
	}
	sprintf(tmp, "%s/%s/%s", PLAYERPATH, first_han(cmnd->str[1]), cmnd->str[1]);
	if(stat(tmp, &f_stat)) {
		print(fd, "그런 사용자는 없습니다.\n");
		return(0);
	}
	if(F_ISSET(player, PFAMIL)) {
		if(ply_ptr->daily[DL_EXPND].max == player->daily[DL_EXPND].max) {
			player->daily[DL_EXPND].max = 0;
			F_CLR(player, PFAMIL);
			print(fd, "%s님을 패거리에서 추방하였습니다.\n", player->name);
		edit_member(player->name, player->class, ply_ptr->daily[DL_EXPND].max, 2);
			save_ply(player->name, player);
			free_crt(player);
			return(0);
		}
		else {
			print(fd, "그 사람은 당신의 패거리원이 아닙니다.\n");
			free_crt(player);
			return(0);
		}
	}
	else {
		print(fd, "그 사람은 당신의 패거리원이 아닙니다.\n");
		free_crt(player);
		return(0);
	}
	}
	else {
	if(F_ISSET(player, PFAMIL)) {
		if(ply_ptr->daily[DL_EXPND].max == player->daily[DL_EXPND].max) {
			player->daily[DL_EXPND].max = 0;
			F_CLR(player, PFAMIL);
		edit_member(player->name, player->class, ply_ptr->daily[DL_EXPND].max, 2);
			print(fd, "%s님을 패거리에서 추방하였습니다.\n", player->name);
			print(player->fd, "\n당신은 패거리에서 추방되었습니다.\n");
			return(0);
		}
		else {
			print(fd, "그 사람은 당신의 패거리원이 아닙니다.\n");
			return(0);
		}
	}
	else {
		print(fd, "그 사람은 당신의 패거리원이 아닙니다.\n");
		return(0);
	}
	}
}

void edit_member(name, class, num, param)
char *name;
int class;
int num;
int param;
{
	FILE *fp;
	char file[80], str[256][15];
	int cnum[256], i, count;

	sprintf(file, "%s/family/family_member_%d", PLAYERPATH, num);
	fp = fopen(file, "r");
	for(i=0; i<256; i++) {
		fscanf(fp, "%d %s", &cnum[i], str[i]);
		if(cnum[i] == 0) {
			count = i;
			break;
		}
	}
	fclose(fp);
	
	switch(param) {
	case 1:  /* 가입 */
		cnum[count] = class;
		strcpy(str[count], name);
		cnum[count+1] = 0;
		strcpy(str[count+1], family_str[num]);
		fp = fopen(file, "w");
		for(i=0; i<count+2; i++) {
			fprintf(fp, "%d %s\n", cnum[i], str[i]);
		}
		fclose(fp);
		break;
	case 2:  /*  탈퇴 & 추방 */
		for(i=0; i< count+1; i++) {
			if(!strcmp(name, str[i])) {
				cnum[i] = -1;
				break;
			}
		}
		fp = fopen(file, "w");
		for(i=0; i<count+1; i++) {
			if(cnum[i] == -1) continue;
			fprintf(fp, "%d %s\n", cnum[i], str[i]);
		}
		fclose(fp);
		break;
	case 3:  /* 수정  */
		for(i=0; i< count+1; i++) {
			if(!strcmp(name, str[i])) {
				cnum[i] = class;
				break;
			}
		}
		fp = fopen(file, "w");
		for(i=0; i<count+1; i++) {
			fprintf(fp, "%d %s\n", cnum[i], str[i]);
		}
		fclose(fp);
		break;
	}
	return;
}

int family_member(ply_ptr, cmnd)
creature *ply_ptr;
cmd *cmnd;
{
	FILE *fp;
	char file[80], str[256][15];
	int cnum[256], i, count, num, total;
	int fd;
	fd = ply_ptr->fd;
	
	if(!F_ISSET(ply_ptr, PFAMIL)) {
		print(fd, "당신은 패거리에 가입되어 있지 않습니다.");
		return(0);
	}
	num = ply_ptr->daily[DL_EXPND].max;
	count = 0;
	sprintf(file, "%s/family/family_member_%d", PLAYERPATH, num);
	fp = fopen(file, "r");
	for(i =0; i<256; i++) {
		fscanf(fp, "%d %s", &cnum[i], str[i]);
		if(cnum[i] == 0) {
			total = i;
			break;
		}
	}
	print(fd, "당신은 [%s] 패거리에 가입되어 있습니다.\n", family_str[num]);
	for(i=0; i<total; i++) { 
		print(fd, "[%-4.4s]  %-15s  ", class_str[cnum[i]], str[i]);
		count++;
		if(count%3 == 0) print(fd, "\n");
	}
	if(count%3 != 0) print(fd, "\n");
	print(fd, "총 %d명의 사람들이 가입되어 있습니다.\n", total);
	return(0);
}

int invite(ply_ptr, cmnd)
creature *ply_ptr;
cmd *cmnd;
{
	FILE *fp;
	int fd, i, count, num, match=0;
	char file[80], str[10][15];
	fd = ply_ptr->fd;
	num = ply_ptr->daily[DL_MARRI].max;
	sprintf(file, "%s/invite/invite_%d", PLAYERPATH, num);

	if(num == 0) {
		print(fd, "당신은 사용할 권한이 없습니다.");
		return(0);
	}
	if(!F_ISSET(ply_ptr->parent_rom, RONMAR)) {
		print(fd, "당신의 집에서만 가능합니다.");
		return(0);
	}

	if(cmnd->num != 2) {
		if(!file_exists(file)) {
			print(fd, "초대한 사람이 없습니다.");
			return(0);
		}
		fp = fopen(file, "r");
		print(fd, "당신이 초대한 사람들 : \n");
		for(i=0; i<10; i++) {
			fscanf(fp, "%s", str[i]);
			if(str[i][0] == '0') break;
			print(fd, "%s\n", str[i]);
		}
		fclose(fp);
		return(0);
	}

	if(!ishan(cmnd->str[1])) {
		print(fd, "사람의 이름은 한글로 적어야 합니다.");
		return(0);
	}

	if(!file_exists(file)) {
		fp = fopen(file, "w");
		fprintf(fp, "%s\n", cmnd->str[1]);
			fprintf(fp, "0");
		fclose(fp);
			print(fd, "초대 대상에 추가했습니다.\n");
		return(0);
	}

	fp = fopen(file, "r");
	for(i=0; i<10; i++) {
		fscanf(fp, "%s", str[i]);
		if(str[i][0] == '0') break;
		if(!strcmp(cmnd->str[1], str[i])) {
			match =1;
			count = i;
		}
	}
	fclose(fp);

	if(match) {
		str[count][0] = -1;
		fp = fopen(file, "w");
		count =0;
		for(i=0; i<10; i++) {
			if(str[i][0] == -1) continue;
			if(str[i][0] == 0) continue;
			if(!str[i][0]) continue;
			fprintf(fp, "%s\n", str[i]);
			count++;
		}
		fprintf(fp, "0");
		print(fd, "초대 대상에서 삭제하였습니다.\n");
		if(count < 1) 
			unlink(file);
		fclose(fp);
	}
	else {
		fp = fopen(file, "w");
		for(i=0; i<10; i++) {
			if(str[i][0] == '0') break;
			if(!str[i][0]) continue;
			fprintf(fp, "%s\n", str[i]);
		}
		fprintf(fp, "%s\n", cmnd->str[1]);
		fprintf(fp, "0");
		print(fd, "초대 대상에 추가하였습니다.\n");
		fclose(fp);
	}
	return(0);
}

int check_war(first_num, second_num)
int first_num;
int second_num;
{
	int war_family1, war_family2;

	war_family1 = AT_WAR/16;
	war_family2 = AT_WAR%16;
	
	if(first_num == 0 || second_num == 0) {
		return(1);
	}
	if(AT_WAR == 0) return(1);

	if(first_num == war_family1 && second_num == war_family2)
		return(0);
	else if(second_num == war_family1 && first_num == war_family2)
			return(0);
		else 	return(1);
}

int check_war_one(num)
int num;
{
	if(num == 0) return(1);
	if(AT_WAR == 0) return(1);
	if(num == (AT_WAR/16)) return(0);
	if(num == (AT_WAR%16)) return(0);

	return(1);

}

int description(ply_ptr, cmnd)
creature    *ply_ptr;
cmd         *cmnd;
{
    int     fd;
    int     index=-1, len, i=0;
    
    fd = ply_ptr->fd;

    len = strlen(cmnd->fullstr);
    for(i=len-1; i>=0; i--) {
        if(cmnd->fullstr[i] == ' ') {
            cmnd->fullstr[i]=0;
            index = i;
            break;
        }
    }

    if(!len || index == -1) {
            print(fd, "당신은 서 있습니다.");
        ply_ptr->description[0] = 0;
        return(0);
    }

    if(len > 31) {
        print(fd, "묘사가 너무 깁니다.");
        return(0);
    }

    memcpy(ply_ptr->description, &cmnd->fullstr[0], len);
    strcat(ply_ptr->description, " ");
    ply_ptr->description[len+1] = 0;
    paste_command(cmnd->fullstr, index);
        print(fd, "당신은 이제부터 %s서 있습니다.", ply_ptr->description);
	return(0);
}

int resend(ply_ptr, cmnd)
creature        *ply_ptr;
cmd             *cmnd;
{
        creature        *crt_ptr = 0;
        etag            *ign;
        int             fd;
        int             len;

        fd = ply_ptr->fd;

        crt_ptr = find_who(Ply[fd].extr->talksend);
        if(!crt_ptr) {
                print(fd, "누구에게 말을 전하시려구요?");
                return(0);
        }
        if(ply_ptr->class < DM && (F_ISSET(crt_ptr, PINVIS) && !F_ISSET(ply_ptr,PDINVI))) {
                print(fd, "누구에게 말을 전하시려구요?");
                return(0);
        }
        if(F_ISSET(crt_ptr, PIGNOR) && (ply_ptr->class < DM )) {
                print(fd, "%s님은 이야기 듣기 거부 상태입니다.", crt_ptr->name);
                return(0);
        }

        ign = Ply[crt_ptr->fd].extr->first_ignore;
        while(ign) {
                if(!strcmp(ign->enemy, ply_ptr->name)) {
                        print(fd, "%s님은 이야기 거부중입니다.", crt_ptr->name);
                        return(0);
                }
                ign = ign->next_tag;
        }
        
        len = cut_command(cmnd->fullstr);
        cmnd->fullstr[255] = 0;

        if(len == 0) {
                print(fd, "무슨 말을 전하시려구요?");
                return(0);
        }
        if(F_ISSET(ply_ptr, PSILNC)) {
                print(fd, "당신은 말을 할 수 없습니다.");
                return(0);
        }
        if(F_ISSET(ply_ptr, PLECHO)){
                print(fd, "당신은 %M에게 \"%s\"라고 이야기합니다.", 
                                        crt_ptr,&cmnd->fullstr[0]);
        }
        else
                print(fd, "%s님에게 말을 전하였습니다.", crt_ptr->name);
        
        print(crt_ptr->fd, "\n%C%M%j 당신에게 \"%s\"라고 이야기합니다.%D", "31",
                            ply_ptr, "1", &cmnd->fullstr[0],"37");
        strcpy(Ply[crt_ptr->fd].extr->talksend, ply_ptr->name);
        paste_command(cmnd->fullstr,len);
        return(0);

}












