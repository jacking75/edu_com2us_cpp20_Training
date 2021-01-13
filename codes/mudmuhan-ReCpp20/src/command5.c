/*
 * COMMAND5.C:
 *
 *  Additional user routines.
 *
 *  Copyright (C) 1991, 1992, 1993 Brett J. Vickers
 *
 */

#include "mstruct.h"
#include "mextern.h"
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>

extern long all_broad_time;
extern long login_time[PMAX];
extern char title_cut_index[PMAX];

/**********************************************************************/
/*              attack                                                */
/**********************************************************************/

/* This function allows the player pointed to by the first parameter */
/* to attack a monster.                                              */

int attack(ply_ptr, cmnd)
creature    *ply_ptr;
cmd     *cmnd;
{
    creature    *crt_ptr;
    room        *rom_ptr;
    long        i, t;
    int     fd;

    fd = ply_ptr->fd;

    t = time(0);
    i = LT(ply_ptr, LT_ATTCK);

    if(ply_ptr->class==0) {
        print(fd,"당신은 전투가 금지된 직업을 갖고 있습니다.");
        return 0;
    }
    if(t < i) {
        please_wait(fd, i-t);
        return(0);
    }

    if(cmnd->num < 2 || F_ISSET(ply_ptr, PBLIND)) {
        print(fd, "누구를 공격하시려구요?");
        return(0);
    }

    rom_ptr = ply_ptr->parent_rom;

    if(ply_is_attacking(ply_ptr,cmnd)) {
        print(fd, "당신은 지금 싸우고 있잖아요!");
        return(0);
    }

    crt_ptr = find_crt(ply_ptr, rom_ptr->first_mon,
               cmnd->str[1], cmnd->val[1]);

    if(!crt_ptr) {
        cmnd->str[1][0] = up(cmnd->str[1][0]);
        crt_ptr = find_crt(ply_ptr, rom_ptr->first_ply,
                   cmnd->str[1], cmnd->val[1]);

        if(!crt_ptr || crt_ptr == ply_ptr || strlen(cmnd->str[1]) < 2) {
            print(fd, "그런건 여기 없습니다.");
            return(0);
        }

    }

    if(F_ISSET(ply_ptr,PDMINV)) F_CLR(ply_ptr,PDMINV);

    if(t-login_time[fd]<120) login_time[fd]-=120;
    attack_crt(ply_ptr, crt_ptr);

    return(0);

}

/**********************************************************************/
/*              attack_crt                */
/**********************************************************************/

/* This function does the actual attacking.  The first parameter contains */
/* a pointer to the attacker and the second contains a pointer to the     */
/* victim.  A 1 is returned if the attack restults in death.          */

int attack_crt(ply_ptr, crt_ptr)
creature    *ply_ptr;
creature    *crt_ptr;
{
    long    i, t;
    int fd, m, n, p, lev, addprof;
    int j, count=1;
    float st=0;
    int tes=0;

    fd = ply_ptr->fd;

    t = time(0);
    i = LT(ply_ptr, LT_ATTCK); 


    if(t < i)
        return(0); 
        if (crt_ptr->type != MONSTER) {
         if(is_charm_crt(ply_ptr->name, crt_ptr) && F_ISSET(ply_ptr, PCHARM)) {
                print(fd, "당신은 %S%j 너무 좋아해서 그럴수 없습니다.", crt_ptr->name,"3");
                return(0);
         }
        else
                del_charm_crt(ply_ptr->name, crt_ptr);

    }
    F_CLR(ply_ptr, PHIDDN);
    if(F_ISSET(ply_ptr, PINVIS)) {
        F_CLR(ply_ptr, PINVIS);
        print(fd, "\n당신의 모습이 서서히 드러났습니다.");
        broadcast_rom(fd, ply_ptr->rom_num, "\n%M의 모습이 서서히 드러났습니다.",
                  ply_ptr);
    }

    ply_ptr->lasttime[LT_ATTCK].ltime = t;
    if(F_ISSET(ply_ptr, PHASTE))
        ply_ptr->lasttime[LT_ATTCK].interval = 1;
    else
        ply_ptr->lasttime[LT_ATTCK].interval = 1;
        if(F_ISSET(ply_ptr, PBLIND)){
                ply_ptr->lasttime[LT_ATTCK].interval = 6;
    }
    if(crt_ptr->type == MONSTER) {
        if(F_ISSET(crt_ptr, MUNKIL)) {
            print(fd, "당신은 %s를 해칠 수 없습니다.\n",
                F_ISSET(crt_ptr, MMALES) ? "그":"그녀");
            return(0);
        }

        if(add_enm_crt(ply_ptr->name, crt_ptr) < 0) {
            print(fd, "당신은 %M%j 공격합니다.", crt_ptr,"3");
            broadcast_rom(fd, ply_ptr->rom_num, "\n%M%j %M%j 공격합니다.",
                      ply_ptr, "1",crt_ptr,"3");
        }


        if(F_ISSET(crt_ptr, MMGONL)) {
            print(fd, "\n당신의 무기는 %M에게 아무 소용이 없는듯 합니다.",
                crt_ptr);
            return(0);
        }
        if(F_ISSET(crt_ptr, MENONL) && ply_ptr->class < CARETAKER) {
            if(!ply_ptr->ready[WIELD-1] ||
                ply_ptr->ready[WIELD-1]->adjustment < 1) {
                print(fd, "\n당신의 무기는 %M에게 아무 소용이 없는듯 합니다.",
                    crt_ptr);
                return(0);
            }
        }
    }
    else {
        if(F_ISSET(ply_ptr->parent_rom, RNOKIL)) {
            print(fd, "이 곳에서는 싸울 수 없습니다.");
            return(0);
        }

		if(!F_ISSET(ply_ptr, PFAMIL) || !F_ISSET(crt_ptr, PFAMIL)) {
                if(!F_ISSET(ply_ptr, PCHAOS) && ply_ptr->level < 128 && !F_ISSET(ply_ptr->parent_rom, RSUVIV)) {
                    print(fd, "당신은 선하다는걸 아세요.");
                    return (0);
                }
                if(!F_ISSET(crt_ptr, PCHAOS) && ply_ptr->level <128 && !F_ISSET(ply_ptr->parent_rom, RSUVIV)) {
                    print(fd, "그 사용자는 선해서 공격할 수 없습니다.");
                    return (0);
                }
		}
		else if(check_war(ply_ptr->daily[DL_EXPND].max, crt_ptr->daily[DL_EXPND].max)) {
                if(!F_ISSET(ply_ptr, PCHAOS) && ply_ptr->level < 128 && !F_ISSET(ply_ptr->parent_rom, RSUVIV)) {
                    print(fd, "당신은 선하다는걸 아세요.");
                    return (0);
                }
                if(!F_ISSET(crt_ptr, PCHAOS) && ply_ptr->level <128 && !F_ISSET(ply_ptr->parent_rom, RSUVIV)) {
                    print(fd, "그 사용자는 선해서 공격할 수 없습니다.");
                    return (0);
                }
		}

        ply_ptr->lasttime[LT_ATTCK].interval += 3;
        print(crt_ptr->fd, "\n%M%j 당신을 공격합니다!", ply_ptr,"1");
        broadcast_rom2(fd, crt_ptr->fd, ply_ptr->rom_num,
                   "\n%M%j %M%j 공격합니다!", ply_ptr, "1",crt_ptr,"3");
    }

 count = 1;
 if(F_ISSET(ply_ptr, PUPDMG)) {
	if((ply_ptr->class == INVINCIBLE && ply_ptr->level > 100) ||
		(ply_ptr->class > INVINCIBLE)) {
		if((ply_ptr->level-97)/10 + mrand(0,3) > 2)
			count++;
	}
	if(ply_ptr->class > INVINCIBLE && mrand(1, 4) == 1)
			count++;
 }
 for(j=0;j<count;j++) {

    if(ply_ptr->ready[WIELD-1]) {
        if(ply_ptr->ready[WIELD-1]->shotscur < 1) {
            print(fd, "\n%S%j 부서져 버렸습니다.",
                  ply_ptr->ready[WIELD-1]->name,"1");
            broadcast_rom(fd, ply_ptr->rom_num, "\n%M의 %S%j 부서져 버렸습니다.",
                ply_ptr, ply_ptr->ready[WIELD-1]->name,"1");
            add_obj_crt(ply_ptr->ready[WIELD-1], ply_ptr);
            ply_ptr->ready[WIELD-1] = 0;
            return(0);
        }
    }

    n = ply_ptr->thaco - crt_ptr->armor/8;

    if (F_ISSET(ply_ptr, PFEARS)) n += 2;
    if (F_ISSET(ply_ptr, PBLIND)) n += 5;

    if(mrand(1,30) >= n) { /* 원래값 20 */
        if(ply_ptr->ready[WIELD-1]) {
            n = mdice(ply_ptr->ready[WIELD-1]) +
                bonus[(int)ply_ptr->strength] +
                profic(ply_ptr, ply_ptr->ready[WIELD-1]->type)/10;
			if(ply_ptr->ready[HELD-1] && ply_ptr->ready[HELD-1]->type < ARMOR)
				n += mdice(ply_ptr->ready[HELD-1])/10;
        }
        else if(ply_ptr->class == BARBARIAN || ply_ptr->class >INVINCIBLE) {
            n = mdice(ply_ptr) +
                bonus[(int)ply_ptr->strength] +
                ((ply_ptr->level+3)/4);
        }
        else {
            n = mdice(ply_ptr) +
                bonus[(int)ply_ptr->strength];
        }

        if(ply_ptr->class == MAGE || ply_ptr->class == CLERIC) {
             if(ply_ptr->ready[WIELD-1]) {
                n = mdice(ply_ptr->ready[WIELD-1]) +
                    bonus[(int)ply_ptr->strength];
             }
             else {
                n = mdice(ply_ptr) + bonus[(int)ply_ptr->strength]; 
             }
        }

        if(crt_ptr->class >= DM) n = 0;

        n = MAX(1,n);

        if(ply_ptr->class == PALADIN) {
            if(ply_ptr->alignment < 0) {
                n /= 2;
                print(fd, "\n당신의 악행이 양심을 괴롭힙니다.");
            }
            else if(ply_ptr->alignment > 250) {
                n += mrand(1,3);
                print(fd, "\n당신의 선행이 능력을 배가시킵니다.");
            }
        }

        p = mod_profic(ply_ptr);
        if(mrand(1,100) <= p ||
          (ply_ptr->ready[WIELD-1] &&
          F_ISSET(ply_ptr->ready[WIELD-1],OALCRT))) {
            ANSI(fd, GREEN);
            print(fd, "\n당신은 %M에게 치명타를 날렸습니다.",crt_ptr);
            ANSI(fd, WHITE);
            broadcast_rom(fd, ply_ptr->rom_num,
                "\n%M이 치명타를 날렸습니다.", ply_ptr);
            n *= mrand(3,6);
            if(ply_ptr->ready[WIELD-1] && (!F_ISSET(ply_ptr->ready[WIELD-1], ONSHAT)) && ((mrand(1,100) < 3) || (F_ISSET(ply_ptr->ready[WIELD-1],OALCRT)))) {
            if(!F_ISSET(ply_ptr->ready[WIELD-1], ONEWEV)) {
                print(fd, "\n%S%j 산산히 부서집니다.",
                    ply_ptr->ready[WIELD-1]->name,"1");
                broadcast_rom(fd, ply_ptr->rom_num,
                    "\n%s의 %S%j 산산히 부서집니다.",
                    F_ISSET(ply_ptr, PMALES) ? "그":"그녀",
                    ply_ptr->ready[WIELD-1]->name,"1");
                free_obj(ply_ptr->ready[WIELD-1]);
                ply_ptr->ready[WIELD-1] = 0;
            }
            }
        }
        else if(mrand(1,100) <= (5-p) && ply_ptr->ready[WIELD-1] &&
            !F_ISSET(ply_ptr->ready[WIELD-1], OCURSE)) {
            ANSI(fd, GREEN);
            print(fd, "\n당신은 무기를 떨어뜨렸습니다.");
            ANSI(fd, WHITE);
            broadcast_rom(fd, ply_ptr->rom_num,
                "\n%M이 무기를 떨어뜨렸습니다.", ply_ptr);
            n = 0;
            add_obj_crt(ply_ptr->ready[WIELD-1], ply_ptr);
            ply_ptr->ready[WIELD-1] = 0;
            compute_thaco(ply_ptr);
        }

        print(fd, "\n당신은 %M에게 %d 만큼의 피해를 주었습니다.", crt_ptr, n);
        broadcast_rom(fd, ply_ptr->rom_num, "\n%M%j %M%j %d만큼의 피해를 입힙니다.",ply_ptr, "1",crt_ptr,"3",n); 
        print(crt_ptr->fd, "\n당신은 %M에게서 %d 만큼의 피해를 입었습니다.",
              ply_ptr, n);

        if(ply_ptr->ready[WIELD-1] && !mrand(0,3))
            ply_ptr->ready[WIELD-1]->shotscur--;

        m = MIN(crt_ptr->hpcur, n);
        crt_ptr->hpcur -= n;

        print(fd, "\n");
        display_status(fd, crt_ptr);
        print(fd, "\n");

        if(crt_ptr->type != PLAYER) {
            add_enm_dmg(ply_ptr->name, crt_ptr, m);
            if(ply_ptr->ready[WIELD-1]) {
                p = MIN(ply_ptr->ready[WIELD-1]->type, 4);
                addprof = (m * crt_ptr->experience) /
                    MAX(crt_ptr->hpmax, 1);
                addprof = MIN(addprof, crt_ptr->experience);
                ply_ptr->proficiency[p] += addprof;
            }
        }

        if(crt_ptr->hpcur < 1) {
            print(fd, "\n당신은 %M%j 죽였습니다.", crt_ptr,"3");
            broadcast_rom2(fd, crt_ptr->fd, ply_ptr->rom_num,
                      "\n%M%j %M%j 죽였습니다.", ply_ptr,"1", crt_ptr,"3");
            die(crt_ptr, ply_ptr);
            return(1);
        }
        else
            check_for_flee(ply_ptr, crt_ptr);
 }
    else {
        print(fd, "\n허공을 쳤습니다.");
        print(crt_ptr->fd, "\n%M%j 허공을 쳤습니다.", ply_ptr,"1");
    }
  }
    return(0);
}

/**********************************************************************/
/*              who                   */
/**********************************************************************/

/* This function outputs a list of all the players who are currently */
/* logged into the game.  It includes their titles and the last      */
/* command they typed.                           */

int who(ply_ptr, cmnd)
creature    *ply_ptr;
cmd     *cmnd;
{
    char    str[15];
    int fd, i, j;
    int total=0;
    int sum = 0;

    fd = ply_ptr->fd;

         if(F_ISSET(ply_ptr, PBLIND)){
                ANSI(fd, RED);
                print(fd, "당신은 눈이 멀어 있습니다!\n");
                ANSI(fd, WHITE);
                return(0);
        }

   for(i=0; i<Tablesize; i++) { 
   	if(!Ply[i].ply) continue;
   	sum ++;
   }

  if((sum > 30) && !((cmnd->num == 2) && !strcmp(cmnd->str[1], "l"))) { 
   print(fd, "%-14s    [레벨 직업]", "사용자");
   print(fd, "  %-14s    [레벨 직업]\n", "사용자");
   print(fd, "-------------------------------");
   print(fd, "-----------------------------\n");
   for(i=0; i<Tablesize; i++) {
      if(!Ply[i].ply) continue;
      if(Ply[i].ply->fd == -1) continue;
      if(F_ISSET(Ply[i].ply, PDMINV) && Ply[i].ply->class == DM && ply_ptr->class < SUB_DM)
      		continue;
      if(F_ISSET(Ply[i].ply, PDMINV) && ply_ptr->class < SUB_DM)
      		continue;
	if(F_ISSET(Ply[i].ply, PINVIS) && !F_ISSET(ply_ptr, PDINVI) &&
	  ply_ptr->class < SUB_DM)
	  if(Ply[i].ply!=ply_ptr) continue;
	total++;
	print(fd, "%-14s%s ", Ply[i].ply->name, (F_ISSET(Ply[i].ply, PDMINV) ||
		F_ISSET(Ply[i].ply, PINVIS)) ? "(*)" : "   ");
	print(fd, "[%s%02d ", (Ply[i].ply->level >= 100) ? "" : " ",
		Ply[i].ply->level);
	print(fd, " %-4.4s]  ", class_str[(int)Ply[i].ply->class]);
	if((total%2 == 0) || i == (Tablesize-1))
		print(fd, "\n");
   }
  if(total%2 != 0) print(fd, "\n"); 
}
   else {
#ifdef LASTCOMMAND
    PRINt(fd, "%-23s  %-20s     %-20s\n", "사용자", "Title", "마지막 명령");
#else
    ANSI (fd, BOLD);
    ANSI (fd, BLUE);
    print(fd, "%-13s     레벨  %-4s %-8s %-6s %-32s\n", "사용자", "직업","종족","패거리","칭호");
#endif
    print(fd, "----------------------------------------------------------------------\n");
    ANSI(fd, NORMAL);
    ANSI(fd, WHITE);
    for(i=0; i<Tablesize; i++) {
        if(!Ply[i].ply) continue;
        if(Ply[i].ply->fd == -1) continue;
        if(F_ISSET(Ply[i].ply, PDMINV) && Ply[i].ply->class == DM &&
           ply_ptr->class < SUB_DM)
            continue;
        if(F_ISSET(Ply[i].ply, PDMINV) && ply_ptr->class < SUB_DM)
            continue;
        if(F_ISSET(Ply[i].ply, PINVIS) && !F_ISSET(ply_ptr, PDINVI) &&
           ply_ptr->class < SUB_DM)
            if(Ply[i].ply!=ply_ptr) continue; /* 투명되어도 자신은 나오도록... */
        total++;
        ANSI(fd, WHITE);
        print(fd, "%-13s%s ", Ply[i].ply->name,
              (F_ISSET(Ply[i].ply, PDMINV) ||
              F_ISSET(Ply[i].ply, PINVIS)) ? "(*)":"   ");
         ANSI(fd, GREEN);
        print(fd, "[%s%02d ] ", (Ply[i].ply->level>=100)? "" : " ",
                                Ply[i].ply->level);
 /* 불사이상 빨강색 */
/*        if(Ply[i].ply->class>=CARETAKER) {
            ANSI(fd,BOLD);
            ANSI(fd,RED);
        }
        else ANSI(fd,WHITE);  */
        ANSI(fd, WHITE);
        print(fd, "%-4.4s ",class_str[(int)Ply[i].ply->class]);
        ANSI(fd, WHITE);
#ifdef LASTCOMMAND
        strncpy(str, Ply[i].extr->lastcommand, 14);
        for(j=0; j<15; j++)
            if(str[j] == ' ') {
                str[j] = 0;
                break;
            }
        if(!str[0])
            print(fd, "Logged in\n");
        else
            print(fd, "%s\n", str);
#else
        ANSI(fd, CYAN);
        print(fd, "%-8s ", race_str[(int)Ply[i].ply->race]);
		ANSI(fd, WHITE);
		if(F_ISSET(Ply[i].ply, PFAMIL)) { 
			ANSI(fd, YELLOW);
			print(fd, "[%-4.4s] ", family_str[Ply[i].ply->daily[DL_EXPND].max]);
		}
		else {
			ANSI(fd, BLUE);
			print(fd, "[중립] ");
		}
		ANSI(fd, WHITE);
         title_cut_index[fd]=1;
         print(fd, "%s\n", title_ply(ply_ptr,Ply[i].ply));
         title_cut_index[fd]=0;

#endif
    }
 }
    if(total!=1) print(fd, "\n총 %d명의 사용자가 무한대전을 이용하고 있습니다.",total);
    else         print(fd, "\n당신 혼자서 외로이 무한대전을 이용하고 있습니다.");

	if(AT_WAR)
		print(fd, "\n%s 패거리와 %s 패거리가 전쟁중입니다.",
			family_str[AT_WAR/16], family_str[AT_WAR%16]);

    return(0);

}
 
/**********************************************************************/
/*                              whois                                 */
/**********************************************************************/
/* The whois function displays a selected player's name, class, level *
 * title, age and gender */
      
int whois(ply_ptr, cmnd)
creature    *ply_ptr;
cmd     *cmnd;
{
    creature    *crt_ptr;
    int     fd;
 
    fd = ply_ptr->fd;
 
    if(cmnd->num < 2) {
        print(fd, "누구를 검색하시려구요?");
        return(0);
    }
 
    lowercize(cmnd->str[1], 1);
    crt_ptr = find_who(cmnd->str[1]);
 
    if(!crt_ptr || F_ISSET(crt_ptr, PDMINV) || F_ISSET(ply_ptr, PBLIND) ||
       (F_ISSET(crt_ptr, PINVIS) && !F_ISSET(ply_ptr, PDINVI))) {
        print(fd, "현재 이용중이 아닙니다.");
        return(0);
    }
 
        ANSI(fd, YELLOW); 
        print(fd, "%-18s  %-4s [레벨] %-4s %-20s  %-4s  %-10s\n", "사용자", "성별", "직업", "칭호", "나이", "종족");
        print(fd, "----------------------------------------------------------------------------\n");
        print(fd, "%-18s  %-4s [ %02d ] %-4.4s %-20s  %-4d  ",
                crt_ptr->name,
                F_ISSET(crt_ptr, PMALES) ? " 남":" 여",  
                crt_ptr->level,
                class_str[(int)crt_ptr->class],
                title_ply(ply_ptr,crt_ptr), 
                18 + crt_ptr->lasttime[LT_HOURS].interval/86400L);
print(fd, "%-10s", race_str[(int)crt_ptr->race]);
        ANSI(fd, NORMAL);
        ANSI(fd, WHITE);
    return(0);
}

/**********************************************************************/
/*              search                    */
/**********************************************************************/

/* This function allows a player to search a room for hidden objects,  */
/* exits, monsters and players.                        */

int search(ply_ptr, cmnd)
creature    *ply_ptr;
cmd     *cmnd;
{
    room    *rom_ptr;
    xtag    *xp;
    otag    *op;
    ctag    *cp;
    long    i, t;
    int fd, chance, found = 0;

    fd = ply_ptr->fd;
    rom_ptr = ply_ptr->parent_rom;

    chance = 15 + 5*bonus[(int)ply_ptr->piety] + ((ply_ptr->level+3)/4)*2;
    chance = MIN(chance, 90);
    if(ply_ptr->class == RANGER)
		chance = 100;
/*
        chance += ((ply_ptr->level+3)/4)*8;
*/
    if(F_ISSET(ply_ptr, PBLIND))
        chance = MIN(chance, 20);
    if(ply_ptr->class >= CARETAKER)
        chance = 100;

    t = time(0);
    i = LT(ply_ptr, LT_SERCH);

    if(t < i) {
        please_wait(fd, i-t);
        return(0);
    }

    F_CLR(ply_ptr, PHIDDN);

    ply_ptr->lasttime[LT_SERCH].ltime = t;
    if(ply_ptr->class == RANGER)
        ply_ptr->lasttime[LT_SERCH].interval = 3;
    else
        ply_ptr->lasttime[LT_SERCH].interval = 7;

    xp = rom_ptr->first_ext;
    while(xp) {
        if(F_ISSET(xp->ext, XSECRT) && mrand(1,100) <= chance)
           if((!F_ISSET(xp->ext, XINVIS) || F_ISSET(ply_ptr,PDINVI))
            && !F_ISSET(xp->ext, XNOSEE)){
            found++;
            print(fd, "\n출구를 찾았습니다: %s.", xp->ext->name);
        }
        xp = xp->next_tag;
    }

    op = rom_ptr->first_obj;
    while(op) {
        if(F_ISSET(op->obj, OHIDDN) && mrand(1,100) <= chance)
        if(!F_ISSET(op->obj, OINVIS) || F_ISSET(ply_ptr,PDINVI)) { 
            found++;
            print(fd, "\n당신은 %1i%j 찾았습니다.", op->obj,"3");
        }
        op = op->next_tag;
    }

    cp = rom_ptr->first_ply;
    while(cp) {
        if(F_ISSET(cp->crt, PHIDDN) && !F_ISSET(cp->crt, PDMINV) &&
           mrand(1,100) <= chance)
        if(!F_ISSET(cp->crt, PINVIS) || F_ISSET(ply_ptr,PDINVI)) {
            found++;
            print(fd, "\n당신은 숨어있는 %S%j 찾아내었습니다.", cp->crt->name,"3");
        }
        cp = cp->next_tag;
    }

    cp = rom_ptr->first_mon;
    while(cp) {
        if(F_ISSET(cp->crt, MHIDDN) && mrand(1,100) <= chance)
        if(!F_ISSET(cp->crt, MINVIS) || F_ISSET(ply_ptr,PDINVI)) {
            found++;
            print(fd, "\n당신은 숨어있는 %1M%j 찾아내었습니다.", cp->crt,"3");
        }
        cp = cp->next_tag;
    }

    broadcast_rom(fd, ply_ptr->rom_num, "\n%M이 주변을 샅샅이 뒤져봅니다.", ply_ptr);

    if(found)
        broadcast_rom(fd, ply_ptr->rom_num, "\n%s가 뭘 발견한것 같군요!",
                  F_ISSET(ply_ptr, MMALES) ? "그":"그녀");
    else
        print(fd, "당신은 아무것도 찾지 못했습니다.\n");

    return(0);

}

/**********************************************************************/
/*              ply_suicide               */
/**********************************************************************/

/* This function is called whenever the player explicitly asks to     */
/* commit suicide.  It then calls the suicide() function which takes  */
/* over that player's input.                          */

int ply_suicide(ply_ptr, cmnd)
creature    *ply_ptr;
cmd     *cmnd;
{
    suicide(ply_ptr->fd, 1, "");
    return(DOPROMPT);
}

/**********************************************************************/
/*              suicide                   */
/**********************************************************************/

/* This function allows a player to kill himself, thus erasing his entire */
/* player file.                               */

void suicide(fd, param, str)
int fd;
int param;
char    *str;
{
    char    file[120],file2[120], file3[120], temp[120],temp2[120], temp3[120];
    creature *ply_ptr;
    long t;
    char buf[256];

    ply_ptr = Ply[fd].ply;

    switch(param) {
        case 1:
            ANSI(fd, BOLD);
            ANSI(fd, RED);
            print(fd, "당신에 관한 데이터를 완전히 삭제합니다.\n");
            print(fd, "당신의 현재 암호를 넣어주십시요 : "); 
            ANSI(fd, NORMAL);
            ANSI(fd, WHITE);
            F_SET(Ply[fd].ply,PREADI);
            RETURN(fd, suicide, 2);
        case 2:
            F_CLR(Ply[fd].ply,PREADI);
            if(!strcmp(ply_ptr->password, str)) {
            	print(fd, "찐짜로? (찐짜로/뻥으로)");
            	F_SET(Ply[fd].ply,PREADI);
            	RETURN(fd, suicide, 3);
            }
            else {
            	print(fd, "암호가 틀립니다.\n삭제되지 않았습니다.");
            	RETURN(fd, command, 1);
            }
        case 3:
            F_CLR(Ply[fd].ply,PREADI);
            if(!strcmp(str,"찐짜로")) {
			if(F_ISSET(ply_ptr, PFAMIL)) {
		edit_member(ply_ptr->name, ply_ptr->class, ply_ptr->daily[DL_EXPND].max, 2);
		}
                broadcast_all("\n### %s님이 자살신청을 하였습니다.\n", Ply[fd].ply->name);
                all_broad_time=time(0);
                sprintf(file, "%s/%s/%s", PLAYERPATH,first_han(Ply[fd].ply->name), Ply[fd].ply->name);
                 sprintf(file2,"%s/alias/%s",PLAYERPATH,Ply[fd].ply->name);
                 sprintf(file3,"%s/bank/%s",PLAYERPATH,Ply[fd].ply->name);
			sprintf(temp, "mv %s %s/suic/%s", file, PLAYERPATH, Ply[fd].ply->name);
			sprintf(temp2, "mv %s %s/suic/%s.alias", file2, PLAYERPATH, Ply[fd].ply->name);
            sprintf(temp3, "mv %s %s/suic/%s.bank", file3, PLAYERPATH, Ply[fd].ply->name);
#ifdef SUICIDE
                t = time(0);
                strcpy(buf,ctime(&t));
                buf[strlen(buf)-1] = 0;
                logn("SUICIDE","%s : %s (%s)님이 자살신청을 하였습니다.\n", buf,Ply[fd].ply->name, Ply[fd].io->address);
#endif
                disconnect(fd);
			system(temp);
			system(temp2);
            system(temp3);         
                return;
            }
            else {
                print(fd, "삭제되지 않았습니다.");
                RETURN(fd, command, 1);
            }
    }
}

/**********************************************************************/
/*              hide                      */
/**********************************************************************/

/* This command allows a player to try and hide himself in the shadows */
/* or it can be used to hide an object in a room.              */

int hide(ply_ptr, cmnd)
creature    *ply_ptr;
cmd     *cmnd;
{
    room    *rom_ptr;
    object  *obj_ptr;
    long    i, t;
    int fd, chance;

    fd = ply_ptr->fd;
    rom_ptr = ply_ptr->parent_rom;

    i = LT(ply_ptr, LT_HIDES);
    t = time(0);

    if(i > t) {
        please_wait(fd, i-t);
        return(0);
    }

    ply_ptr->lasttime[LT_HIDES].ltime = t;
    ply_ptr->lasttime[LT_HIDES].interval = (ply_ptr->class == THIEF ||
        ply_ptr->class == ASSASSIN || ply_ptr->class == RANGER) ? 5:15;

    if(cmnd->num == 1) {

        if(ply_ptr->class == THIEF || ply_ptr->class == ASSASSIN || ply_ptr->class >= CARETAKER ) 
            chance = MIN(90, 5 + 6*((ply_ptr->level+3)/4) +
                3*bonus[(int)ply_ptr->dexterity]);
        else if(ply_ptr->class == RANGER)
            chance = 5 + 10*((ply_ptr->level+3)/4) +
                3*bonus[(int)ply_ptr->dexterity];
        else
            chance = MIN(90, 5 + 2*((ply_ptr->level+3)/4) +
                3*bonus[(int)ply_ptr->dexterity]);

        print(fd, "당신은 애써 숨어보려고 합니다.");

        if(F_ISSET(ply_ptr, PBLIND))
            chance = MIN(chance, 20);

        if(mrand(1,100) <= chance) {
            F_SET(ply_ptr, PHIDDN);
            print(fd, "\n당신은 성공적으로 숨었습니다.");
            broadcast_rom(fd, ply_ptr->rom_num,
                   "\n%M%j 그림자 사이로 숨었습니다.", ply_ptr,"1");
            }
        else {
            F_CLR(ply_ptr, PHIDDN);
            broadcast_rom(fd, ply_ptr->rom_num,
                  "\n%M%j 애써 숨어보려고 합니다.", ply_ptr,"1");
        }


        return(0);

    }

    obj_ptr = find_obj(ply_ptr, rom_ptr->first_obj,
               cmnd->str[1], cmnd->val[1]);

    if(!obj_ptr) {
        print(fd, "그런것은 여기 없어요.");
        return(0);
    }

    if(F_ISSET(obj_ptr,ONOTAK)){
        print(fd,"당신은 그것을 숨길 수 없습니다.");
        return (0);
   }
    if(ply_ptr->class == THIEF || ply_ptr->class == ASSASSIN)
        chance = MIN(90, 10 + 5*((ply_ptr->level+3)/4) +
            5*bonus[(int)ply_ptr->dexterity]);
    else if(ply_ptr->class == RANGER)
        chance = 5 + 9*((ply_ptr->level+3)/4) +
            3*bonus[(int)ply_ptr->dexterity];
    else
        chance = MIN(90, 5 + 3*((ply_ptr->level+3)/4) +
            3*bonus[(int)ply_ptr->dexterity]);

    print(fd, "당신은 그것을 숨겨보려고 합니다.");
    broadcast_rom(fd, ply_ptr->rom_num, "\n%M%j %1i%j 숨겨보려고 합니다.",
              ply_ptr, "1",obj_ptr,"3");

    if(mrand(1,100) <= chance) {
        F_SET(obj_ptr, OHIDDN);
        print(fd, "\n당신은 성공적으로 숨겼습니다.");
        broadcast_rom(fd, ply_ptr->rom_num, "\n%M%j %1i%j 어딘가 숨깁니다.",
              ply_ptr, "1", obj_ptr,"3");
        }
    else
        F_CLR(obj_ptr, OHIDDN);

    return(0);

}

/************************************************************************/
/************************************************************************/

/*  Display information on creature given to player given.              */

int flag_list(ply_ptr, cmnd)
creature        *ply_ptr;
cmd                     *cmnd;
{
    int         fd;

    fd = ply_ptr->fd;

    print(fd,"현재 설정 상태:\n");
    print(fd,"  이야기듣기: %s\n", F_ISSET(ply_ptr, PIGNOR) ? "미설정" : " 설정 ");
    print(fd,"  잡담듣기  : %s\n", F_ISSET(ply_ptr, PNOBRD) ? "미설정" : " 설정 ");
	print(fd,"  환호듣기  : %s\n", F_ISSET(ply_ptr, PNOBR2) ? "미설정" : " 설정 ");
	print(fd,"  묘사보기  : %s\n", F_ISSET(ply_ptr, PDSCRP) ? " 설정 " : "미설정");
    print(fd,"  소환      : %s\n", F_ISSET(ply_ptr, PNOSUM) ? " 불가 " : " 가능 ");
    print(fd,"  도망수치  : ");
        if(F_ISSET(ply_ptr, PWIMPY)) print(fd, "%-6d\n", ply_ptr->WIMPYVALUE);
        else                         print(fd, "미설정\n");
    print(fd,"  행삽입    : %s\n", F_ISSET(ply_ptr, PNOCMP) ? " 설정 " : "미설정");
    print(fd,"  상태      : %s\n", F_ISSET(ply_ptr, PPROMP) ? " 출력 " : "미설정");
    print(fd,"  반향      : %s\n", F_ISSET(ply_ptr, PLECHO) ? " 설정 " : "미설정");
    print(fd,"  색        : %s\n", F_ISSET(ply_ptr, PANSIC) ? " 사용 " : "미사용");
    print(fd,"  밝은색    : %s\n", F_ISSET(ply_ptr, PBRIGH) ? " 사용 " : "미사용");
    print(fd,"  방이름    : %s\n", F_ISSET(ply_ptr, PNORNM) ? "미설정" : " 출력 ");
    print(fd,"  짧은설명  : %s\n", F_ISSET(ply_ptr, PNOSDS) ? "미설정" : " 출력 ");
    print(fd,"  긴설명    : %s\n", F_ISSET(ply_ptr, PNOLDS) ? "미설정" : " 출력 ");
    print(fd,"  출구      : %s\n", F_ISSET(ply_ptr, PNOEXT) ? "그래프" : "텍스트");
    print(fd,"  패거리귀환: %s\n", F_ISSET(ply_ptr, PFRTUN) ? " 설정 " : "미설정");

/*
    if(F_ISSET(ply_ptr, PNOAAT)) strcat(str, "수동공격으로 전환합니다.\n");
            if(F_ISSET(ply_ptr, PHASTE)) strcat(str, "활보법으로 기를 운행하고 있습니다\n");
            if(F_ISSET(ply_ptr, PPRAYD)) strcat(str, "신의 보호를 받고있습니다\n");
            if(F_ISSET(ply_ptr, PPOWER)) strcat(str, "기공으로 힘을 모으고 있습니다\n");
            if(F_ISSET(ply_ptr, PSLAYE)) strcat(str, "당신의 무기에 살기가 감돕니다\n");
            if(F_ISSET(ply_ptr, PMEDIT)) strcat(str, "당신은 참선으로 사물을 꿰뚤어봅니다\n");

            print(fd,"%s\n",str);
*/
    print(fd, "[설정 도움말]이라고 치시면 자세한 설정사항을 볼 수 있습니다.\n");
    return(0);
}
/**********************************************************************/
/*              set                   */
/**********************************************************************/

/* This function allows a player to set certain one-bit flags.  The flags */
/* are settings for options that include brief and verbose display.  The  */
/* clear function handles the turning off of these flags.         */

int set(ply_ptr, cmnd)
creature    *ply_ptr;
cmd     *cmnd;
{
    int fd,i;
    struct {
        char *str;
        int flags;
    } setkeys[]={
        { "이야기듣기", PIGNOR },
        { "잡담듣기",   PNOBRD },
		{ "환호듣기", 	PNOBR2 },
		{ "묘사보기",	PDSCRP },
        { "소환",       PNOSUM },
        { "행삽입",     PNOCMP },
        { "상태",       PPROMP },
        { "반향",       PLECHO },
        { "색",         PANSIC },
        { "밝은색",     PBRIGH },
        { "방이름",     PNORNM },
        { "짧은설명",   PNOSDS },
        { "긴설명",     PNOLDS },
        { "출구",       PNOEXT },
        { NULL,         -1 },
    };

    fd =  ply_ptr->fd;

    if(cmnd->num == 1) {
                flag_list(ply_ptr,cmnd);
        return(0);
    }

    i=0;
    while(setkeys[i].str!=NULL) {
        if(!strcmp(cmnd->str[1], setkeys[i].str)) {
            if(F_ISSET(ply_ptr, setkeys[i].flags))
                F_CLR(ply_ptr, setkeys[i].flags );
            else
                F_SET(ply_ptr, setkeys[i].flags );
            break;
        }
        i++;
    }

    if(!strcmp(cmnd->str[1], "도망수치")) {
        F_SET(ply_ptr, PWIMPY);
        ply_ptr->WIMPYVALUE = cmnd->val[1] == 1L ? 10 : cmnd->val[1];
        ply_ptr->WIMPYVALUE = MAX(ply_ptr->WIMPYVALUE, 2);
        if(ply_ptr->WIMPYVALUE==0) F_CLR(ply_ptr,PWIMPY);
    }
    else if(!strcmp(cmnd->str[1], "패거리귀환")) {
    	if(F_ISSET(ply_ptr, PFAMIL)) {
    		F_SET(ply_ptr, PFRTUN);
    		print(fd, "패거리 존으로 귀환을 합니다.\n");
    	}
    	else print(fd, "당신은 패거리에 가입되어 있지 않습니다.\n");
    }
    else if(!strcmp(cmnd->str[1], "hexline")) {
        F_SET(ply_ptr, PHEXLN);
        print(fd, "Hexline enabled.\n");
    }
    else if(!strcmp(cmnd->str[1], "eavesdropper")) {
        F_SET(ply_ptr, PEAVES);
        print(fd, "Eavesdropper mode enabled.\n");
    }
    else if(!strcmp(cmnd->str[1], "~robot~")) {
        F_SET(ply_ptr, PROBOT);
        print(fd, "Robot mode on.\n");
    }
    else if(!strcmp(cmnd->str[1], "수동공격")) {
        F_SET(ply_ptr, PNOAAT);
        print(fd, "수동으로 공격합니다.\n");
    }

    flag_list(ply_ptr,cmnd);

    return(0);

}

/**********************************************************************/
/*              clear                     */
/**********************************************************************/

/* Like set, this function allows a player to alter the value of a part- */
/* icular player flag.                           */

int clear(ply_ptr, cmnd)
creature    *ply_ptr;
cmd     *cmnd;
{
    int fd;

    fd = ply_ptr->fd;

    if(cmnd->num == 1) {
        print(fd, "[해제 도움말]이라고 치시면 모든 설정사항들을 볼 수 있습니다.\n");
        return(0);
    }

    if(!strcmp(cmnd->str[1], "잡담듣기거부")) {
        F_CLR(ply_ptr, PNOBRD);
        print(fd, "이제부터 잡담을 듣습니다.\n");
    }
    else if(!strcmp(cmnd->str[1], "행삽입")) { /* compact */
        F_CLR(ply_ptr, PNOCMP);
        print(fd, "이제부터 메세지를 출력할때 한행을 삽입하지 않습니다.\n");
    }
    else if(!strcmp(cmnd->str[1], "반향")) {
        F_CLR(ply_ptr, PLECHO);
        print(fd, "당신의 메세지가 반향되지 않습니다.\n");
    }
    else if(!strcmp(cmnd->str[1], "방이름")) {
        F_SET(ply_ptr, PNORNM);
        print(fd, "이제부터 방이름을 출력하지 않습니다.\n");
    }
    else if(!strcmp(cmnd->str[1], "간단")) {
        F_SET(ply_ptr, PNOSDS);
        print(fd, "방의 간단한 설명을 보지 않습니다.\n");
    }
    else if(!strcmp(cmnd->str[1], "일반")) {
        F_SET(ply_ptr, PNOLDS);
        print(fd, "방의 자세한 설명을 보지 않습니다.\n");
    }
    else if(!strcmp(cmnd->str[1], "hexline")) {
        F_CLR(ply_ptr, PHEXLN);
        print(fd, "Hex line disabled.\n");
    }
    else if(!strcmp(cmnd->str[1], "도망수치")) {
        F_CLR(ply_ptr, PWIMPY);
        print(fd, "도망수치 설정이 해제되었습니다.\n");
    }
    else if(!strcmp(cmnd->str[1], "eavesdropper")) {
        F_CLR(ply_ptr, PEAVES);
        print(fd, "Eavesdropper mode disabled.\n");
    }
    else if(!strcmp(cmnd->str[1], "상태")) {
        F_CLR(ply_ptr, PPROMP);
        print(fd, "당신의 상태를 보여주지 않습니다.\n");
    }
    else if(!strcmp(cmnd->str[1], "~robot~")) {
        F_CLR(ply_ptr, PROBOT);
        print(fd, "Robot mode off.\n");
    }
    else if(!strcmp(cmnd->str[1], "색")) {
        F_CLR(ply_ptr, PANSIC);
        print(fd, "이제부터 메세지가 모두 흑백으로 출력됩니다.\n");
    }
    else if(!strcmp(cmnd->str[1], "소환거부")) {
        F_CLR(ply_ptr, PNOSUM);
        print(fd, "이제부터 다른사람이 당신을 소환할 수 있습니다.\n");
    }
    else if(!strcmp(cmnd->str[1], "이야기듣기거부")) {
        F_CLR(ply_ptr, PIGNOR);
        print(fd, "이제부터 개인적인 이야기를 듣습니다.\n");
    }
    else if(!strcmp(cmnd->str[1], "수동공격")) {
        F_CLR(ply_ptr, PNOAAT);
        print(fd, "이제부터 자동으로 공격합니다.\n");
    }
    else if(!strcmp(cmnd->str[1], "밝은색")) {
        F_CLR(ply_ptr,PBRIGH);
        print(fd,"어두운 색으로 출력합니다.\n");
    }
    else if(!strcmp(cmnd->str[1], "패거리귀환")) {
    	F_CLR(ply_ptr, PFRTUN);
    	print(fd,"광장으로 귀환합니다.\n");
    }
    else
        print(fd, "잘못 지정되었습니다.\n");

    return(0);

}

/**********************************************************************/
/*              quit                      */
/**********************************************************************/

/* This function checks to see if a player is allowed to quit yet.  It  */
/* checks to make sure the player isn't in the middle of combat, and if */
/* so, the player is not allowed to quit (and 0 is returned).       */

int quit(ply_ptr, cmnd)
creature    *ply_ptr;
cmd     *cmnd;
{
    long    i, t;
    int fd;

    fd = ply_ptr->fd;

    t = time(0);
    i = LT(ply_ptr, LT_ATTCK) + 20;

    if(ply_is_attacking(ply_ptr,cmnd) && t < i) {
        please_wait(fd, i-t);
        return(0);
    }

    update_ply(ply_ptr);
    return(DISCONNECT);
}

