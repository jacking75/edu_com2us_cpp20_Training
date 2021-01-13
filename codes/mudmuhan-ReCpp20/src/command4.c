/*
 * COMMAND4.C:
 *
 *      Additional user routines
 *
 *      Copyright (C) 1991, 1992, 1993 Brett J. Vickers
 *
 */

#include "mstruct.h"
#include "mextern.h"
#include <sys/time.h>
#include <string.h>

/**********************************************************************/
/*                              health                                */
/**********************************************************************/

/* This function shows a player his current hit points, magic points,  */
/* experience, gold and level.                                         */

int health(ply_ptr, cmnd)
creature        *ply_ptr;
cmd             *cmnd;
{
        int     fd;
	char 	str[64];
        long    expneeded;

        fd = ply_ptr->fd;

        if(F_ISSET(ply_ptr, PBLIND)){
                ANSI(fd, RED);
                ANSI(fd, BOLD);
                ANSI(fd, BLINK);
                print(fd, "����� ���� �־� �ֽ��ϴ�!");
                ANSI(fd, NORMAL);
                ANSI(fd, WHITE);
                return(0);
        }
                ANSI(fd, WHITE);
        print(fd, "%s : %s (���� %d)", ply_ptr->name,
                title_ply(ply_ptr,ply_ptr), ply_ptr->level);
        if(F_ISSET(ply_ptr, PHIDDN)) {
                ANSI(fd, CYAN);
                print(fd, " *����* ");
        }
        if(F_ISSET(ply_ptr, PPOISN)) {
                ANSI(fd, BLINK);
                ANSI(fd, GREEN);
                print(fd, " *�ߵ�* ");
                ANSI(fd, NORMAL);
        }
        if(F_ISSET(ply_ptr, PCHARM)) {
                ANSI(fd, BOLD);
                ANSI(fd, CYAN);
                print(fd, " *�ָ�* ");
                ANSI(fd, NORMAL);
        }
        if(F_ISSET(ply_ptr, PSILNC)) {
                ANSI(fd, BLINK);
                ANSI(fd, MAGENTA);
                print(fd, " *���* ");
                ANSI(fd, NORMAL);
        }
        if(F_ISSET(ply_ptr, PDISEA)) {
                ANSI(fd, BLINK);
                ANSI(fd, RED);
                print(fd," *����* ");
                ANSI(fd, NORMAL);
        }
        if(ply_ptr->level < MAXALVL) 
                expneeded = needed_exp[ply_ptr->level-1];
        else
                expneeded = needed_exp[MAXALVL-2]+(ply_ptr->level-MAXALVL+1)*5000000;

        ANSI(fd, GREEN);
	sprintf(str,"%d/%d",ply_ptr->hpcur, ply_ptr->hpmax);
        print(fd, "\n [ü  ��] %-16s",str);
	sprintf(str,"%d/%d",ply_ptr->mpcur, ply_ptr->mpmax);
	print(fd, " [��  ��] %-16s",str);

        ANSI(fd, RED);
        print(fd, "[����]: %d\n", (100-ply_ptr->armor));

        ANSI(fd, YELLOW);
        if (ply_ptr->class == 10) {
            print(fd, " [���ġ] %-16lu [  ��  ] %ld��",
                    MAX(0,ply_ptr->experience-100000000), ply_ptr->gold);
        }            
        else {
            print(fd, " [��ǥġ] %-16lu [  ��  ] %ld��",
                    MAX(0,expneeded-ply_ptr->experience), ply_ptr->gold);
        }            

        ANSI(fd, NORMAL);
        ANSI(fd, WHITE);
		print(fd, "\n ����� %s�� �ֽ��ϴ�.", ply_ptr->description);
        return(0);
}

/**********************************************************************/
/*                              help                                  */
/**********************************************************************/

/* This function allows a player to get help in general, or help for a */
/* specific command.  If help is typed by itself, a list of commands   */
/* is produced.  Otherwise, help is supplied for the command specified */

int help(ply_ptr, cmnd)
creature        *ply_ptr;
cmd             *cmnd;
{
        char    file[80];
        int     fd, c=0, match=0, num=0;
        int current_deep,match_deep=0;

        fd = ply_ptr->fd;
        strcpy(file, DOCPATH);

        if(cmnd->num < 2) {
                strcat(file, "/helpfile");
                view_file(fd, 1, file);
                return(DOPROMPT);
        }
        if(!strcmp(cmnd->str[1], "�ּ�")) {
                strcat(file, "/spellfile");
                view_file(fd, 1, file);
                return(DOPROMPT);
        }

        if(!strcmp(cmnd->str[1], "��å")) {
                strcat(file, "/policy");
                view_file(fd, 1, file);
                return(DOPROMPT);
        }

        do {
                if(!strcmp(cmnd->str[1], cmdlist[c].cmdstr)) {
                        match = 1;
                        num = c;
                        break;
                }
		else if((current_deep=str_compare(cmnd->str[1], cmdlist[c].cmdstr))!=0) {
			match = 1;
			if(match_deep==0 || match_deep>current_deep) {
				num = c;
				match_deep = current_deep;
			}
		}
                c++;
        } while(cmdlist[c].cmdno);

        if(match == 1) {
                sprintf(file, "%s/help.%d", DOCPATH, cmdlist[num].cmdno);
                view_file(fd, 1, file);
                return(DOPROMPT);
        }
        else if(match > 1) {
                print(fd, "����� �� �𸣰ڳ׿�.");
                return(0);
        }

        c = num = match_deep=0;
        do {
                if(!strcmp(cmnd->str[1], spllist[c].splstr)) {
                        match = 1;
                        num = c;
                        break;
                }
		else if((current_deep=str_compare(cmnd->str[1], spllist[c].splstr))!=0) {
			match = 1;
			if(match_deep==0 || match_deep>current_deep) {
				num=c;
				match_deep = current_deep;
			}
		}
                c++;
        } while(spllist[c].splno != -1);

        if(match == 0) {
                print(fd, "�� ��ɾ ���� ������ �����ϴ�.");
                return(0);
        }
/*
        else if(match > 1) {
                print(fd, "�ּ� �̸��� �߸𸣰ڳ׿�.");
                return(0);
        }
*/

        sprintf(file, "%s/spell.%d", DOCPATH, spllist[num].splno);
        view_file(fd, 1, file);
        return(DOPROMPT);
}

/**********************************************************************/
/*                              welcome                               */
/**********************************************************************/

/* Outputs welcome file to user, giving him/her info on how to play   */
/* the game                                                           */

int welcome(ply_ptr, cmnd)
creature        *ply_ptr;
cmd             *cmnd;
{
        char    file[80];
        int     fd;

        fd = ply_ptr->fd;

        sprintf(file, "%s/welcome", DOCPATH);

        view_file(fd, 1, file);
        return(DOPROMPT);
}

/**********************************************************************/
/*                              info                                  */
/**********************************************************************/

/* This function displays a player's entire list of information, including */
/* player stats, proficiencies, level and class.                           */

int info(ply_ptr, cmnd)
creature        *ply_ptr;
cmd             *cmnd;
{
        char    alstr[16];
        int     fd, cnt;
        long    expneeded, lv;

        fd = ply_ptr->fd;

        /*update_ply(ply_ptr);*/

        if(ply_ptr->level < MAXALVL)
                expneeded = needed_exp[ply_ptr->level -1];
        else
                expneeded = needed_exp[MAXALVL-2]+(ply_ptr->level-MAXALVL+1)*5000000;

        if(ply_ptr->alignment < -100)
                strcpy(alstr, " (���մϴ�)");
        else if(ply_ptr->alignment < 101)
                strcpy(alstr, " (����մϴ�)");
        else
                strcpy(alstr, " (���մϴ�) ");

        for(lv=0,cnt=0; lv<MAXWEAR; lv++)
                if(ply_ptr->ready[lv]) cnt++;
        cnt += count_inv(ply_ptr, -1);

        print(fd, "\n[��  ��] %s        [Ī  ȣ] %s\n\n", ply_ptr->name, title_ply(ply_ptr,ply_ptr));

        print(fd, "[��  ��] %-20d       [��  ��] %s\n",
                ply_ptr->level, race_str[ply_ptr->race]);
        print(fd, "[��  ��] %-20s       [��  ��] %s %s\n",
                class_str[ply_ptr->class],
                F_ISSET(ply_ptr, PCHAOS) ? "��":"��", alstr);

#define INTERVAL ply_ptr->lasttime[LT_HOURS].interval
        print(fd, "���ӽð� : ");
        if(INTERVAL > 86400L)
                print(fd, "%d�� ", INTERVAL/86400L);
        if(INTERVAL > 3600L)
                print(fd, "%d�ð� ", (INTERVAL % 86400L)/3600L);
        print(fd, "%d��\n\n", (INTERVAL % 3600L)/60L);

        print(fd, "[  ��  ] %-2d      [��  ø] %-2d      [��  ��] %-2d\n",
              ply_ptr->strength, ply_ptr->dexterity, ply_ptr->constitution);
        print(fd, "[��  ��] %-2d      [�žӽ�] %-2d\n\n",
              ply_ptr->intelligence, ply_ptr->piety);
        print(fd,
         "[ü  ��] %-5d/%-5d          [����ġ] %lu ( %lu�� ����ġ�� �ʿ��մϴ�. )\n",
         ply_ptr->hpcur, ply_ptr->hpmax, ply_ptr->experience,
         MAX(0, expneeded-ply_ptr->experience));
        print(fd, "[��  ��] %-5d/%-5d          [  ��  ] %-7lu\n",
              ply_ptr->mpcur, ply_ptr->mpmax, ply_ptr->gold);
        print(fd, "[����] %-5d                [����ǰ ����] %d �� (�� %d��).\n\n",
              (100-ply_ptr->armor), weight_ply(ply_ptr), cnt);

        print(fd, "## ������ɷ� ##\n");
        print(fd, "\n[ �� ] %2d%%         [ �� ] %2d%%         [ �� ] %2d%%\n",
              profic(ply_ptr, SHARP), profic(ply_ptr, THRUST),
              profic(ply_ptr, BLUNT));
        print(fd, "[ â ] %2d%%         [ �� ] %2d%%\n\n",
              profic(ply_ptr, POLE), profic(ply_ptr, MISSILE));

        print(fd, "## �� ��  �� �� ##:\n");
        print(fd,
          "[ �� ] %2d%%      [�ٶ�] %2d%%    [ �� ] %2d%%   [ �� ] %2d%%\n\n",
                mprofic(ply_ptr, EARTH), mprofic(ply_ptr, WIND),
                mprofic(ply_ptr, FIRE), mprofic(ply_ptr, WATER));

        F_SET(ply_ptr, PREADI);
        print(fd, "[����]�� ��������. �׸����÷��� [.]�� ġ����: ");
        output_buf();
        Ply[fd].io->intrpt &= ~1;
        Ply[fd].io->fn = info_2;
        Ply[fd].io->fnparam = 1;
        return(DOPROMPT);
}

/************************************************************************/
/*                              info_2                                  */
/************************************************************************/

/* This function is the second half of info which outputs spells        */

void info_2(fd, param, instr)
int     fd, param;
char    *instr;
{
        char            str[2048];
        char            spl[128][20];
        int             i, j;
        creature        *ply_ptr;

        ply_ptr = Ply[fd].ply;

        if(instr[0]=='.') {
                print(fd, "�ߴܵǾ����ϴ�.\n");
                F_CLR(ply_ptr, PREADI);
                RETURN(fd, command, 1);
        }

        strcpy(str, "\n�ֹ�: ");
        for(i=0,j=0; i<56; i++)
                if(S_ISSET(ply_ptr, i))
                        strcpy(spl[j++], spllist[i].splstr);

        if(!j)
                strcat(str, "����.");
        else {
                qsort((void *)spl, j, 20, strcmp);
                for(i=0; i<j; i++) {
                        strcat(str, spl[i]);
                        strcat(str, ", ");
                }
                str[strlen(str)-2] = '.';
                str[strlen(str)-1] = 0;
        }
        print(fd, "%s\n", str);

        strcpy(str, "����� ���ֹ�: ");
        if(F_ISSET(ply_ptr, PBLESS)) strcat(str, "������, ");
        if(F_ISSET(ply_ptr, PLIGHT)) strcat(str, "�߱�, ");
        if(F_ISSET(ply_ptr, PPROTE)) strcat(str, "��ȣ��, ");
        if(F_ISSET(ply_ptr, PINVIS)) strcat(str, "���й�, ");
        if(F_ISSET(ply_ptr, PDINVI)) strcat(str, "���а���, ");
        if(F_ISSET(ply_ptr, PDMAGI)) strcat(str, "�ֹ�����, ");
        if(F_ISSET(ply_ptr, PLEVIT)) strcat(str, "�ξ��, ");
        if(F_ISSET(ply_ptr, PRFIRE)) strcat(str, "�濭��, ");
        if(F_ISSET(ply_ptr, PFLYSP)) strcat(str, "����, ");
        if(F_ISSET(ply_ptr, PRMAGI)) strcat(str, "������, ");
        if(F_ISSET(ply_ptr, PKNOWA)) strcat(str, "���ǰ���, ");
        if(F_ISSET(ply_ptr, PRCOLD)) strcat(str, "������, ");
        if(F_ISSET(ply_ptr, PBRWAT)) strcat(str, "������, ");
        if(F_ISSET(ply_ptr, PSSHLD)) strcat(str, "����ȣ, ");
        if(strlen(str) == 15)
                strcat(str, "����.");
        else {
                str[strlen(str)-2] = '.';
                str[strlen(str)-1] = 0;
        }
        print(fd, "%s\n", str);

        i=0;
        while(Q_ISSET(ply_ptr,i)) i++;
        if(i==0) print(fd,"����� ���� �޼��� �ӹ��� �����ϴ�.");
        else print(fd,"����� ���� �ӹ� %d���� �޼��Ͽ����ϴ�.",i);

        F_CLR(Ply[fd].ply, PREADI);
        RETURN(fd, command, 1);
}

/**********************************************************************/
/*                              send                                  */
/**********************************************************************/

/* This function allows a player to send a message to another player.  If */
/* the other player is logged in, the message is sent successfully.       */

int send(ply_ptr, cmnd)
creature        *ply_ptr;
cmd             *cmnd;
{
        creature        *crt_ptr = 0;
        etag            *ign;
        int             spaces=0, i, fd;
        int             len;

        fd = ply_ptr->fd;

        if(cmnd->num < 2) {
                print(fd, "�������� ���� ���Ͻ÷�����?");
                return 0;
        }

        cmnd->str[1][0] = up(cmnd->str[1][0]);
        for(i=0; i<Tablesize; i++) {
                if(!Ply[i].ply) continue;
                if(Ply[i].ply->fd == -1) continue;
                if(F_ISSET(Ply[i].ply, PDMINV) && ply_ptr->class < DM)
                        continue;
                if(!strncmp(Ply[i].ply->name, cmnd->str[1],
                   strlen(cmnd->str[1])))
                        crt_ptr = Ply[i].ply;
                if(!strcmp(Ply[i].ply->name, cmnd->str[1]))
                        break;
        }
        if(!crt_ptr) {
                print(fd, "�������� ���� ���Ͻ÷�����?");
                return(0);
        }
        if(ply_ptr->class < DM && (F_ISSET(crt_ptr, PINVIS) && !F_ISSET(ply_ptr, PDINVI))) {
                print(fd, "�������� ���� ���Ͻ÷�����?");
                return(0);
        }
        if(F_ISSET(crt_ptr, PIGNOR) && (ply_ptr->class < DM )) {
                print(fd, "%s���� �̾߱� ��� �ź� �����Դϴ�.", crt_ptr->name);
                return(0);
        }

        ign = Ply[crt_ptr->fd].extr->first_ignore;
        while(ign) {
                if(!strcmp(ign->enemy, ply_ptr->name)) {
                        print(fd, "%s is ignoring you.", crt_ptr->name);
                        return(0);
                }
                ign = ign->next_tag;
        }
        
        len = cut_command(cmnd->fullstr);
        for(i=0; i< len && i<256; i++) {
                if(cmnd->fullstr[i] == ' ' && cmnd->fullstr[i+1] != ' ') {
                        spaces++;
                        break;
                }
        }
        cmnd->fullstr[255] = 0;

        if(spaces < 1 || strlen(&cmnd->fullstr[i+1]) < 1) {
                print(fd, "���� ���� ���Ͻ÷�����?");
                return(0);
        }
        if(F_ISSET(ply_ptr, PSILNC)) {
                print(fd, "����� ���� �� �� �����ϴ�.");
                return(0);
        }
        if(F_ISSET(ply_ptr, PLECHO)){
                print(fd, "����� %M���� \"%s\"��� �̾߱��մϴ�.", crt_ptr,&cmnd->fullstr[i+1]);
        }
        else
                print(fd, "%s�Կ��� ���� �����Ͽ����ϴ�.", crt_ptr->name);
        
        print(crt_ptr->fd, "\n%C%M%j ��ſ��� \"%s\"��� �̾߱��մϴ�.%D", "31",ply_ptr,"1",
              &cmnd->fullstr[i+1],"37");

        strcpy(Ply[crt_ptr->fd].extr->talksend, ply_ptr->name);

        if(ply_ptr->class > CARETAKER || crt_ptr->class > CARETAKER)
                return(0);

        broadcast_eaves("\n--- %s���� %s�Կ��� \"%s\"��� �̾߱��մϴ�.", ply_ptr->name,
                         crt_ptr->name, &cmnd->fullstr[i+1]);

        paste_command(cmnd->fullstr,len);
        return(0);

}

/**********************************************************************/
/*                              broadsend                             */
/**********************************************************************/

/* This function is used by players to broadcast a message to every   */
/* player in the game.  Broadcasts by players are of course limited,  */
/* so the number used that day is checked before the broadcast is     */
/* allowed.                                                           */
long broad_time[PMAX];
long all_broad_time=0;

int broadsend(ply_ptr, cmnd)
creature        *ply_ptr;
cmd             *cmnd;
{
        int     i, found=0, fd;
        int     hpdown, len;
        int discount=2;
        long t,tt;
        int dc_table[31]={
           60, 57, 54, 51, 48, 45, 42, 40, 38, 36,
           34, 32, 30, 28, 26, 24, 22, 20, 18, 16,
           14, 12, 10,  8,  7,  6,  5,  4,  3,  2,  2, };

        fd = ply_ptr->fd;
        len = cut_command(cmnd->fullstr);
        cmnd->fullstr[255] = 0;

        if(len==0) {
                print(fd, "���� ���� �Ͻ÷�����?");
                return(0);
        }

        tt=time(0);
        t=tt-all_broad_time;

        if(t>10 && F_ISSET(ply_ptr, PBRSND) && ply_ptr->class<SUB_DM) {
            if(!dec_daily(&ply_ptr->daily[DL_BROAD])) {
                print(fd,"����� ���� ����� �Ѱ踦 �Ѱ���ϴ�.");
                return(0);
            }
        }
      
	if(F_ISSET(ply_ptr, PSILNC) && ply_ptr->class<SUB_DM) {
                print(fd, "����� ��Ҹ��� �ʹ� �۾� ����� �� �� �����ϴ�.");
                return(0);
        }
        
        if(ply_ptr->level < 20 && ply_ptr->class < CARETAKER) {
        	print(fd, "����� �����δ� ����� �� �� �����ϴ�.");
        	return(0);
        }

        t=tt-broad_time[fd];

        if(t<0 || t>30) discount=2;
        else discount=dc_table[t];

		if(ply_ptr->class >= INVINCIBLE && ply_ptr->class < SUB_DM)
			discount += discount*(ply_ptr->level)/60; 

        if(ply_ptr->hpcur<=discount && ply_ptr->class<SUB_DM) {
            print(fd,"����� ����� ���·ο� ����� �� �� �����ϴ�.");
            return 0;
        }
        if(ply_ptr->class<SUB_DM) ply_ptr->hpcur-=discount;
        broad_time[fd]=tt;

        broadcast("\n%C%s> %s%D", "32",ply_ptr->name, &cmnd->fullstr[0],"37");

        return(0);

}

int broadsend2(ply_ptr, cmnd)
creature        *ply_ptr;
cmd             *cmnd;
{
        int     i, found=0, fd;
        int     len;
        int discount=2;
        long t,tt;
        int dc_table[31]={
           60, 57, 54, 51, 48, 45, 42, 40, 38, 36,
           34, 32, 30, 28, 26, 24, 22, 20, 18, 16,
           14, 12, 10,  8,  7,  6,  5,  4,  3,  2,  2, };

        fd = ply_ptr->fd;
        len = cut_command(cmnd->fullstr);
        cmnd->fullstr[255] = 0;

        if(len==0) {
                print(fd, "���� ���� �Ͻ÷�����?");
                return(0);
        }

        tt=time(0);
        t=tt-all_broad_time;

        if(t>10 && F_ISSET(ply_ptr, PBRSND) && ply_ptr->class<SUB_DM) {
            if(!dec_daily(&ply_ptr->daily[DL_BROAD])) {
                print(fd,"����� ���� ����� �Ѱ踦 �Ѱ���ϴ�.");
                return(0);
            }
        }
      
	if(F_ISSET(ply_ptr, PSILNC) && ply_ptr->class<SUB_DM) {
                print(fd, "����� ��Ҹ��� �ʹ� �۾� ȯȣ�� �� �� �����ϴ�.");
                return(0);
        }
        
        if(ply_ptr->level < 20 && ply_ptr->class < CARETAKER) {
        	print(fd, "����� �����δ� ȯȣ�� �� �� �����ϴ�.");
        	return(0);
        }

        t=tt-broad_time[fd];

        if(t<0 || t>30) discount=2;
        else discount=dc_table[t];

		if(ply_ptr->class >= INVINCIBLE && ply_ptr->class < SUB_DM)
			discount += discount*(ply_ptr->level)/60; 

        if(ply_ptr->hpcur<=discount && ply_ptr->class<SUB_DM) {
            print(fd,"����� ����� ���·ο� ȯȣ�� �� �� �����ϴ�.");
            return 0;
        }
        if(ply_ptr->class<SUB_DM) ply_ptr->hpcur-=discount;
        broad_time[fd]=tt;

        broadcast2("\n%C%s���� \"%s\"��� ȯȣ�� �մϴ�.%D", "32",ply_ptr->name, &cmnd->fullstr[0],"37");

        return(0);
}

/**********************************************************************/
/*                              follow                                */
/**********************************************************************/

/* This command allows a player (or a monster) to follow another player. */
/* Follow loops are not allowed; i.e. you cannot follow someone who is   */
/* following you.  Also, you cannot follow yourself.                     */

int follow(ply_ptr, cmnd)
creature        *ply_ptr;
cmd             *cmnd;
{
        creature        *old_ptr, *new_ptr;
        room            *rom_ptr;
        ctag            *cp, *pp, *prev;
        int             fd;

        fd = ply_ptr->fd;
        rom_ptr = ply_ptr->parent_rom;

        /* ��� ������� */
/*      print(fd,"���װ� �ִ°���� ����� �����մϴ�.");
        return;
*/
        if(cmnd->num < 2) {
                print(fd, "������ ���� ���ð� ��������?");
                return(0);
        }

        F_CLR(ply_ptr, PHIDDN);
        if(!strcmp(cmnd->str[1],"��")) new_ptr=ply_ptr;
        else new_ptr = find_crt(ply_ptr, rom_ptr->first_ply,
                           cmnd->str[1], cmnd->val[1]);

        if(!new_ptr) {
                print(fd, "�׷� ����� ���� �����ϴ�.");
                return(0);
        }

        if(new_ptr == ply_ptr && !ply_ptr->following) {
                print(fd, "�ڱ��ڽ��� ���� ���� �����ϴ�.");
                return(0);
        }

        if(new_ptr->following == ply_ptr) {
                print(fd, "�̹� %s�� ����� ����ٴϰ� �ֽ��ϴ�.",
                      F_ISSET(new_ptr, PMALES) ? "��":"�׳�");
                return(0);
        }

        if(ply_ptr->following) { /* ���󰡰� �ִ� ����� �ִٸ�... */
                old_ptr = ply_ptr->following;
                cp = old_ptr->first_fol;
                if(cp->crt == ply_ptr) {
                        old_ptr->first_fol = cp->next_tag;
                        free(cp);
                }
                else while(cp) {
                        if(cp->crt == ply_ptr) {
                                prev->next_tag = cp->next_tag;
                                free(cp);
                                break;
                        }
                        prev = cp;
                        cp = cp->next_tag;
                }
                ply_ptr->following = 0;
                print(fd, "����� %s���� �׸� ���� �ٴϱ�� �Ͽ����ϴ�.\n", old_ptr->name);
                if(!F_ISSET(ply_ptr, PDMINV))
                        print(old_ptr->fd, "\n%M�� ���� ����� ����ٴ��� �ʽ��ϴ�.", 
                              ply_ptr);
        }

        if(ply_ptr == new_ptr)
                return(0);

        ply_ptr->following = new_ptr;

        pp = (ctag *)malloc(sizeof(ctag));
        if(!pp)
                merror("follow", FATAL);
        pp->crt = ply_ptr;
        pp->next_tag = 0;

        if(!new_ptr->first_fol)
                new_ptr->first_fol = pp;
        else {
                pp->next_tag = new_ptr->first_fol;
                new_ptr->first_fol = pp;
        }

        print(fd, "����� �������� %s���� ����ٴմϴ�.", new_ptr->name);
        if(!F_ISSET(ply_ptr, PDMINV)) {
                print(new_ptr->fd, "\n%M�� �������� ����� ����ٴմϴ�.", ply_ptr);
                broadcast_rom2(fd, new_ptr->fd, ply_ptr->rom_num,
                              "\n%M�� �������� %M�� ����ٴմϴ�.", ply_ptr, new_ptr);
        }

        return(0);

}

/**********************************************************************/
/*                              lose                                  */
/**********************************************************************/

/* This function allows a player to lose another player who might be  */
/* following him.  When successful, that player will no longer be     */
/* following.                                                         */

int lose(ply_ptr, cmnd)
creature        *ply_ptr;
cmd             *cmnd;
{
        creature        *crt_ptr;
        ctag            *cp, *prev;
        int             fd;

        fd = ply_ptr->fd;

if(cmnd->num == 1) {
 
                if (ply_ptr->following == 0){
                        print(fd, "����� ������ ����ٴϰ� ���� �ʽ��ϴ�.");
                        return(0);
 
                }
      crt_ptr = ply_ptr->following;
        cp = crt_ptr->first_fol;
        if(cp->crt == ply_ptr) {
                crt_ptr->first_fol = cp->next_tag;
                free(cp);
        }
        else while(cp) {
                if(cp->crt == ply_ptr) {
                        prev->next_tag = cp->next_tag;
                        free(cp);
                        break;
                }
                prev = cp;
                cp = cp->next_tag;
        }
        ply_ptr->following = 0; 
                print(fd,"����� %M�� �׸� ����ٴϱ�� �Ͽ����ϴ�.\n",crt_ptr);
                if (!F_ISSET(ply_ptr,PDMINV))
                        print(crt_ptr->fd,"\n%M�� ���� ����� ���� �ٴ��� �ʽ��ϴ�.",ply_ptr);
                return(0);
        }                   

        F_CLR(ply_ptr, PHIDDN);

        lowercize(cmnd->str[1], 1);
        crt_ptr = find_crt(ply_ptr, ply_ptr->first_fol,
                           cmnd->str[1], cmnd->val[1]);

        if(!crt_ptr) {
                print(fd, "�� ����� ����� ����ٴϰ� ���� �ʽ��ϴ�.");
                return(0);
        }

        if(crt_ptr->following != ply_ptr) {
                print(fd, "�� ����� ����� ����ٴϰ� ���� �ʽ��ϴ�.");
                return(0);
        }

        cp = ply_ptr->first_fol;
        if(cp->crt == crt_ptr) {
                ply_ptr->first_fol = cp->next_tag;
                free(cp);
        }
        else while(cp) {
                if(cp->crt == crt_ptr) {
                        prev->next_tag = cp->next_tag;
                        free(cp);
                        break;
                }
                prev = cp;
                cp = cp->next_tag;
        }
        crt_ptr->following = 0;

        print(fd, "����� %M�� ����� ������ ������ �Ͽ����ϴ�.", crt_ptr);
        if(!F_ISSET(ply_ptr, PDMINV)) {
                print(crt_ptr->fd, "\n%M�� ����� ������ ������ �Ͽ����ϴ�.", ply_ptr);
                broadcast_rom2(fd, crt_ptr->fd, "\n%M�� %M�� ������ ������ �Ͽ����ϴ�.", 
                               ply_ptr, crt_ptr);
        }

        return(0);

}

/**********************************************************************/
/*                              group                                 */
/**********************************************************************/

/* This function allows you to see who is in a group or party of people */
/* who are following you.                                               */

int group(ply_ptr, cmnd)
creature        *ply_ptr;
cmd             *cmnd;
{
        ctag    *cp;
        char    str[2048];
        char    str2[256];
        int     fd, found = 0;
        creature *leader;

        fd = ply_ptr->fd;

        /* ��� ������� */
/*      print(fd,"���װ� �ִ� ����� ����� �����մϴ�.");
        return;
*/
        if(ply_ptr->following) {
            cp = ply_ptr->following->first_fol;
            leader=ply_ptr->following;
        }
        else {
            cp = ply_ptr->first_fol;
            leader=ply_ptr;
            if(!cp) {
                print(fd,"����� �׷쿡 ���� ���� �ʽ��ϴ�.");
                return(0);
            }
        }

        strcpy(str, "�׷��:\n");
        sprintf(str2, "  %14s  ü��:%4d ����:%4d (����)\n",
            crt_str(leader,0,2),leader->hpcur,leader->mpcur);
        strcat(str,str2);

        while(cp) {
                if(!F_ISSET(cp->crt, PDMINV)) {
                        sprintf(str2, "  %14s  ü��:%4d ����:%4d\n",
                           crt_str(cp->crt,0,2),cp->crt->hpcur,cp->crt->mpcur);
                        strcat(str, str2);
                        found = 1;
                }
                cp = cp->next_tag;
        }

        if(!found) {
                print(fd, "����� �׷쿡 ���� ���� �ʽ��ϴ�.");
                return(0);
        }

        str[strlen(str)-1] = 0;
        print(fd, "%s", str);

        return(0);

}

/**********************************************************************/
/*                              track                                 */
/**********************************************************************/

/* This function is the routine that allows rangers to search for tracks */
/* in a room.  If the ranger is successful, he will be told what dir-    */
/* ection the last person who was in the room left.                      */

int track(ply_ptr, cmnd)
creature        *ply_ptr;
cmd             *cmnd;
{
        long    i, t;
        int     fd, chance;

        fd = ply_ptr->fd;

        if(ply_ptr->class != RANGER && ply_ptr->class < INVINCIBLE) {
                print(fd, "������ ���� �ִ� ����Դϴ�.");
                return(0);
        }

        F_CLR(ply_ptr, PHIDDN);

        t = time(0);
        i = LT(ply_ptr, LT_TRACK);

        if(t < i) {
                please_wait(fd, i-t);
                return(0);
        }

        ply_ptr->lasttime[LT_TRACK].ltime = t;
        ply_ptr->lasttime[LT_TRACK].interval = 5 - bonus[ply_ptr->dexterity];
        
        if(F_ISSET(ply_ptr, PBLIND)) {
                print(fd, "����� ���� �־� �ֽ��ϴ�. ������ ������ �� �� �����ϴ�.");
                return(0);
        }
        chance = 25 + (bonus[ply_ptr->dexterity] + ((ply_ptr->level+3)/4))*5;

        if(mrand(1,100) > chance) {
                print(fd, "���� ����!");
                return(0);
        }

        if(!ply_ptr->parent_rom->track[0]) {
                print(fd, "�ƹ��� ������ �������� �ʽ��ϴ�.");
                return(0);
        }

        print(fd, "%s������ ������ �� �ֽ��ϴ�.",
              ply_ptr->parent_rom->track);
        broadcast_rom(fd, ply_ptr->rom_num, "\n%M�� ���� ������ ������ ã�ҽ��ϴ�.", ply_ptr);

        return(0);

}

/**********************************************************************/
/*                              peek                                  */
/**********************************************************************/

/* This function allows a thief or assassin to peek at the inventory of */
/* another player.  If successful, they will be able to see it and      */
/* another roll is made to see if they get caught.                      */

int peek(ply_ptr, cmnd)
creature        *ply_ptr;
cmd             *cmnd;
{
        creature        *crt_ptr;
        room            *rom_ptr;
        char            str[2048];
        long            i, t;
        int             fd, n, chance;

        fd = ply_ptr->fd;
        rom_ptr = ply_ptr->parent_rom;
        str[0] = 0;

        if(cmnd->num < 2) {
                print(fd, "������ ����ǰ�� ��������?");
                return(0);
        }

        if(ply_ptr->class != THIEF && ply_ptr->class < INVINCIBLE) {
                print(fd, "��� �������δ� �ٸ������ ����ǰ�� �� �� �����ϴ�.");
                return(0);
        }

        if(F_ISSET(ply_ptr, PBLIND)){
                ANSI(fd, RED);
                print(fd, "����� ���� �־� �ֽ��ϴ�!");
                ANSI(fd, WHITE);
                return(0);
        }

        crt_ptr = find_crt(ply_ptr, rom_ptr->first_mon,
                           cmnd->str[1], cmnd->val[1]);

        if(!crt_ptr) {
                lowercize(cmnd->str[1], 1);
                crt_ptr = find_crt(ply_ptr, rom_ptr->first_ply,
                                   cmnd->str[1], cmnd->val[1]);

                if(!crt_ptr) {
                        print(fd, "�׷� ��� �����!");
                        return(0);
                }
        }

        i = LT(ply_ptr, LT_PEEKS);
        t = time(0);

        if(i > t) {
                please_wait(fd, i-t);
                return(0);
        }

        ply_ptr->lasttime[LT_PEEKS].ltime = t;
        ply_ptr->lasttime[LT_PEEKS].interval = 5;

        if((F_ISSET(crt_ptr, MUNSTL) || F_ISSET(crt_ptr, MTRADE) || F_ISSET(crt_ptr, MPURIT)) && ply_ptr->class < DM) {
                print(fd, "����� �ٸ������ ����ǰ�� �� �� �����ϴ�.\n�ٸ������ ��ź��� �����̶�� ������ ���Դϴ�.");
                return(0);
        }

        chance = (25 + ((ply_ptr->level+3)/4)*10)-(((crt_ptr->level+3)/4)*5);
        if (chance<0) chance=0;
        if (ply_ptr->class >= CARETAKER) chance=100;
        if(mrand(1,100) > chance) {
                print(fd, "�����Ͽ����ϴ�!");
                return(0);
        }

        chance = MIN(90, 15 + ((ply_ptr->level+3)/4)*5);

        if(mrand(1,100) > chance && ply_ptr->class < CARETAKER) {
                print(crt_ptr->fd, "%s���� ����� ����ǰ�� ��½ �����ϴ�.", ply_ptr);
                broadcast_rom2(fd, crt_ptr->fd, ply_ptr->rom_num,
                               "\n%M�� %M�� ����ǰ�� ��½ �����ϴ�.",
                               ply_ptr, crt_ptr);
        }

        sprintf(str, "%s�� ����ǰ: ", F_ISSET(crt_ptr, PMALES) ? "��":"�׳�");
        n = strlen(str);
        if(list_obj(&str[n], ply_ptr, crt_ptr->first_obj) > 0)
                print(fd, "%s", str);
        else
                print(fd, "%s�� �ƹ��͵� ��� ���� �ʽ��ϴ�.",
                      F_ISSET(crt_ptr, PMALES) ? "��":"�׳�");

        return(0);

}
