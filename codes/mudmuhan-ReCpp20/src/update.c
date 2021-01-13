/*
 * UPDATE.C:
 *
 *  Routines to handle non-player game updates.
 *
 *  Copyright (C) 1991, 1992, 1993 Brett J. Vickers
 *
 */

#include "mstruct.h"
#include "mextern.h"
#include "update.h"

static long last_update;
static long last_user_update;
static long last_random_update;
static long last_active_update;
static long last_time_update;
static long last_shutdown_update;
static long last_moonstone_update;
static long last_monster_update;
static long last_monster_two_update;

static long last_allcmd;

long        last_exit_update;
long        TX_interval = 3600;
short       Random_update_interval = 20;

static ctag *first_active;
extern void update_allcmd();

/**********************************************************************/
/*              update_game               */
/**********************************************************************/

/* This function handles all the updates that occur while players are */
/* typing.                                */

void update_game()
{
    long    t;

    t = time(0);
    if(t == last_update)
        return;

    last_update = t;
    if(t - last_user_update >= 20)
        update_users(t);
    if(t - last_random_update >= Random_update_interval)
        update_random(t);
    if(t != last_active_update)
        update_active(t);
    if(t - last_time_update >= 150)
        update_time(t);
    if(t - last_moonstone_update >= 20000)
        update_moonstone(t);
    if(t - last_monster_update >= 4000)
        update_monster(t);
    if(t - last_monster_two_update >= 5000)
        update_monster_two(t);
    if(t - last_exit_update >= TX_interval)
    	update_exit(t);
    if(Shutdown.ltime && t - last_shutdown_update >= 30)
        if(Shutdown.ltime + Shutdown.interval <= t+500)
            update_shutdown(t);

#ifdef RECORD_ALL
    if(t- last_allcmd >= 300)
 	update_allcmd(t);
#endif RECORD_ALL
}

/**********************************************************************/
/*              update_users                  */
/**********************************************************************/

/* This function controls user updates.  Every 20 seconds it checks a */
/* user's time-out flags (such as invisibility, etc.) and it also     */
/* checks for excessive idle times.  If a user has been idle for over */
/* 30 mins, he is disconnected.                       */

void update_users(t)
long    t;
{
    int i;
    last_user_update = t;

    for(i=0; i<Tablesize; i++) {
        if(!Ply[i].io) continue;
        if(Ply[i].ply && Ply[i].ply->class == DM) continue;
        if(t - Ply[i].io->ltime > 300 && Ply[i].io->fn != waiting) {
            write(i, "\r\n입력없이 5분이상 유지하면 접속이 끊어집니다.\r\n", 48);
            disconnect(i);
            continue;
        }

        if(!Ply[i].ply) continue;
        if(Ply[i].ply->fd < 0) continue;
        update_ply(Ply[i].ply); 
    }
}

/**********************************************************************/
/*              update_random                 */
/**********************************************************************/

/* This function checks all player-occupied rooms to see if random monsters */
/* have entered them.  If it is determined that random monster should enter */
/* a room, it is loaded and items it is carrying will be loaded with it.    */

void update_random(t)
long    t;
{
    creature    *crt_ptr;
    object      *obj_ptr;
    room        *rom_ptr;
    int     check[PMAX];
    int     num, m, n, i, j, k, l, total = 0;

    last_random_update = t;
    for(i=0; i<Tablesize; i++) {
        if(!Ply[i].ply || !Ply[i].io) continue;
        if(Ply[i].ply->fd < 0) continue;

        rom_ptr = Ply[i].ply->parent_rom;

        for(j=0; j<total; j++)
            if(check[j] == rom_ptr->rom_num) break;
        if(j < total) continue;

        check[total++] = rom_ptr->rom_num;

        if(mrand(1,100) > rom_ptr->traffic) continue;

        n = mrand(0,9);
        if(!rom_ptr->random[n]) continue;
        m = load_crt(rom_ptr->random[n], &crt_ptr);
        if(m < 0) continue;

        if(F_ISSET(rom_ptr, RPLWAN))
            num = mrand(1, count_ply(rom_ptr));
        else if(crt_ptr->numwander > 1)
            num = mrand(1, crt_ptr->numwander);
        else
            num = 1;

        for(l=0; l<num; l++) {
            crt_ptr->lasttime[LT_ATTCK].ltime = 
            crt_ptr->lasttime[LT_MSCAV].ltime =
            crt_ptr->lasttime[LT_MWAND].ltime = t;

            if(crt_ptr->dexterity < 20)
                crt_ptr->lasttime[LT_ATTCK].interval = 3;
            else
                crt_ptr->lasttime[LT_ATTCK].interval = 2;

            j = mrand(1,100);
            if(j<90) j=1;
            else if(j<96) j=2;
            else j=3;
            for(k=0; k<j; k++) {
				if(F_ISSET(crt_ptr, MNRGLD)) {
					m = mrand(0, 49);
					if(m > 9) continue;
				}
                else  m = mrand(0,9);
                if(crt_ptr->carry[m]) {
                    m=load_obj(crt_ptr->carry[m],
                        &obj_ptr);
                    if(m > -1) {
                        if(F_ISSET(obj_ptr, ORENCH))
                            rand_enchant(obj_ptr);
                        obj_ptr->value =
                        mrand((obj_ptr->value*9)/10,
                              (obj_ptr->value*11)/10);
                        add_obj_crt(obj_ptr, crt_ptr);
                    }
                }
            }

            if(!F_ISSET(crt_ptr, MNRGLD) && crt_ptr->gold)
                crt_ptr->gold =
                mrand(crt_ptr->gold/10, crt_ptr->gold);

            if(!l) 
                add_crt_rom(crt_ptr, rom_ptr, num);
            else
                add_crt_rom(crt_ptr, rom_ptr, 0);

            add_active(crt_ptr);
            if(l != num-1)
                load_crt(rom_ptr->random[n], &crt_ptr);
        }
    }
}


/************************************************************************/
/*              도망_끝 버그 패치               */
/************************************************************************/
void clear_enm_crt(ply_ptr)
creature    *ply_ptr;
{
        ctag        *cp, *ctemp;

        if(!(cp = first_active)) return;
        while(cp) {
                if(!cp) break;
                ctemp = cp->next_tag;

                if(find_enm_crt(ply_ptr->name, cp->crt) > -1) {
                        del_enm_crt(ply_ptr->name, cp->crt);
                }
                cp = ctemp;
         }
}
/************************************************************************/
/*              update_active               */
/************************************************************************/

/* This function updates the activities of all monsters who are currently */
/* active (ie. monsters on the active list).  Usually this is reserved    */
/* for monsters in rooms that are occupied by players.            */
long login_time[PMAX];

void update_active(t)
long    t;
{
    creature    *crt_ptr, *att_ptr,*e_ptr;
    object      *obj_ptr;
    room        *rom_ptr;
    ctag        *cp;
    etag	*etmp;
    char        *enemy;
    long        i,z;
    long 	atime;
    int     rtn = 0, n, fd, p=0, tes;
    float   st=0;
    char talkbuf[32][256];
    char namebuf[1024];
    FILE *fp;
    int maxtalk=0,len;

    last_active_update = t;
    if(!(cp = first_active)) return;
    while(cp) {
        if(!cp) break;

        crt_ptr = cp->crt;
        rom_ptr = crt_ptr->parent_rom;
		if(!rom_ptr->first_ply) {
			del_active(crt_ptr);
			continue;
		}

        i = LT(crt_ptr, LT_BEFUD);
        if(i < t) {
            F_CLR(crt_ptr, MBEFUD);
        }

        i = LT(crt_ptr, LT_HEALS);
        while(i <= t && (crt_ptr->hpcur < crt_ptr->hpmax ||
              crt_ptr->mpcur < crt_ptr->mpmax)) {
            crt_ptr->hpcur += MAX(1,crt_ptr->hpmax/10);
            if(crt_ptr->hpcur > crt_ptr->hpmax)
                crt_ptr->hpcur = crt_ptr->hpmax;
            crt_ptr->mpcur += MAX(1,crt_ptr->mpmax/6);
            if(crt_ptr->mpcur > crt_ptr->mpmax)
                crt_ptr->mpcur = crt_ptr->mpmax;
            i += 60L;
            crt_ptr->lasttime[LT_HEALS].ltime = t;
            crt_ptr->lasttime[LT_HEALS].interval = 60L;
        }
	if (t > LT(crt_ptr, LT_CHRMD)&&F_ISSET(crt_ptr, MCHARM))
		F_CLR(crt_ptr, MCHARM);

        atime = LT(crt_ptr, LT_ATTCK);
		if(atime > t) {
			cp = cp->next_tag;
			continue;
		}
        crt_ptr->lasttime[LT_ATTCK].ltime = t;
        if(crt_ptr->dexterity < 20)
            crt_ptr->lasttime[LT_ATTCK].interval = 3;
        else
            crt_ptr->lasttime[LT_ATTCK].interval = 2;

        if(F_ISSET(crt_ptr, MSCAVE)) {
            i = crt_ptr->lasttime[LT_MSCAV].ltime;
            if(t-i > 20 && mrand(1,100) <= 15 && 
               rom_ptr->first_obj && 
               !F_ISSET(rom_ptr->first_obj->obj, ONOTAK) &&
               !F_ISSET(rom_ptr->first_obj->obj, OSCENE) &&
               !F_ISSET(rom_ptr->first_obj->obj, OHIDDN) &&
               !F_ISSET(rom_ptr->first_obj->obj, OPERM2) &&
               !F_ISSET(rom_ptr->first_obj->obj, OPERMT)) {
                obj_ptr = rom_ptr->first_obj->obj;
                del_obj_rom(obj_ptr, rom_ptr);
                add_obj_crt(obj_ptr, crt_ptr);
                F_SET(crt_ptr, MHASSC);
                broadcast_rom(-1, crt_ptr->rom_num,
                          "%M%j %1i%j 줍습니다.",
                          crt_ptr, "1",obj_ptr,"3");
            }
            if(t-i > 20)
                crt_ptr->lasttime[LT_MSCAV].ltime = t;
        }

        if(!F_ISSET(crt_ptr, MHASSC) && !F_ISSET(crt_ptr, MPERMT) && !F_ISSET(crt_ptr, MDMFOL)) {
            i = crt_ptr->lasttime[LT_MWAND].ltime;
            if(t-i > 20 
               && mrand(1,100) <= crt_ptr->parent_rom->traffic
               && !crt_ptr->first_enm) {
                broadcast_rom(-1, crt_ptr->rom_num,
                    "%1M%j 당신 주위를 방황하고 있습니다.",
                    crt_ptr,"1");
                del_crt_rom(crt_ptr, rom_ptr);
                del_active(crt_ptr);
                free_crt(crt_ptr);
                cp = first_active;
                continue;
            }
            if(t-i > 20)
                crt_ptr->lasttime[LT_MWAND].ltime = t;
        }
	if(!crt_ptr->first_enm && (!F_ISSET(crt_ptr, MAGGRE) && 
            !F_ISSET(crt_ptr, MGAGGR) && !F_ISSET(crt_ptr, MEAGGR))) {
            cp = cp->next_tag;
            continue;
        } 
        if(crt_ptr->first_enm) {
            enemy = crt_ptr->first_enm->enemy;
            att_ptr = find_crt(crt_ptr, rom_ptr->first_ply, enemy, 1);
            if(!att_ptr) {
                if(!find_who(enemy))
                    del_enm_crt(enemy, crt_ptr);
                else
                    end_enm_crt(enemy, crt_ptr);
            }
            else {
				p = 0;	
	        if(is_charm_crt(crt_ptr->name, att_ptr)&&
                   F_ISSET(crt_ptr, MCHARM))
	            p = 1;
                crt_ptr->NUMHITS++;
                n=20;
                if(F_ISSET(crt_ptr, MMAGIO)) n=crt_ptr->proficiency[0];
	         
	        if(F_ISSET(crt_ptr, MMAGIC) && mrand(1, 100) <= n && !p) {
                    rtn = crt_spell(crt_ptr, att_ptr);
					if(!rom_ptr->first_ply) {
						del_active(crt_ptr);
						cp = first_active;
						continue;
					}
            enemy = crt_ptr->first_enm->enemy;
            att_ptr = find_crt(crt_ptr, rom_ptr->first_ply, enemy, 1);
            if(!att_ptr) {
                    end_enm_crt(enemy, crt_ptr);
					cp = first_active;
					continue;
            }
                    if(rtn == 2) {
                        cp = first_active;
                        continue;
                    }
                    else if(rtn == 1)
                        n = 21;
                    else { 
			n = crt_ptr->thaco - att_ptr->armor/8;
			n = MAX(n, 1);
		    }
                }

                else {
                    n = crt_ptr->thaco - att_ptr->armor/8;
                    n = MAX(n, 1);
                }

                if(mrand(1,20) >= n && !p) {
                    fd = att_ptr->fd;
                    ANSI(fd, RED);

if(F_ISSET(crt_ptr, MBRETH) && mrand(1,30)<5) 
    if (F_ISSET(crt_ptr, MBRWP1) && !F_ISSET(crt_ptr, MBRWP2))
    {
    print(fd, "\n%M%j 당신에게 퉤! 침을 뱉습니다!", crt_ptr,"1");
    broadcast_rom(fd, att_ptr->rom_num,
       "\n%M%j %M에게 퉤! 침을 뱉습니다!", crt_ptr, "1",att_ptr);
        n = dice(((crt_ptr->level+3)/4), 3, 0);
    }
    else if (F_ISSET(crt_ptr, MBRWP1) && F_ISSET(crt_ptr, MBRWP2))
    {
    print(fd, "\n%M%j 당신에게 고약한 냄새가 나는 입김을 뿜어댑니다!", crt_ptr,"1");
    broadcast_rom(fd, att_ptr->rom_num,
       "\n%M%j %M에게 고약한 냄새가 나는 입김을 뿜어댑니다!", crt_ptr, "1",att_ptr);
        n = dice(((crt_ptr->level+3)/4), 2, 1);
        print(fd, "\n%M%j 당신을 중독시켰습니다.", crt_ptr,"1");
        F_SET(att_ptr, PPOISN);
    }
    else if (!F_ISSET(crt_ptr, MBRWP1) && F_ISSET(crt_ptr, MBRWP2))
    {
    print(fd, "\n%M%j 당신에게 냉기를 내뿜습니다!", crt_ptr,"1");
    broadcast_rom(fd, att_ptr->rom_num,
     "\n%M%j %M에게 냉기를 내뿜습니다!", crt_ptr, "1",att_ptr);
    if(F_ISSET(att_ptr, PRCOLD))
        n = dice(((crt_ptr->level+3)/4), 2, 0);
    else
        n = dice(((crt_ptr->level+3)/4), 4, 0);

    }
    else{
        print(fd, "\n%M%j 당신에게 불을 뿜습니다!", crt_ptr,"1");
        broadcast_rom(fd, att_ptr->rom_num,
           "\n%M%j %M에게 불을 뿜습니다!", crt_ptr,"1", att_ptr);
            if(F_ISSET(att_ptr, PRFIRE))
                n = dice(((crt_ptr->level+3)/4), 2, 0);
            else
                n = dice(((crt_ptr->level+3)/4), 4, 0);
    }
else
{
    if(F_ISSET(crt_ptr, MENEDR) && mrand(1,100)< 10) 
    {
        n = dice(((crt_ptr->level+3)/4), 5, (((crt_ptr->level+3)/4))*5);
        n = MAX(0,MIN(n,att_ptr->experience));
        print(fd, "\n%M%j 당신의 경험치를 갉아먹습니다!", crt_ptr,"1");
        broadcast_rom(fd, att_ptr->rom_num,
        "\n%M%j %M의 경험치를 갉아먹습니다!", crt_ptr, "1",att_ptr);
        att_ptr->experience -= n;
        lower_prof(att_ptr,n);
        print(fd, "\n%M%j 당신의 경험치 %d 를 줄었습니다.",crt_ptr,"1",n);
    }


n = mdice(crt_ptr) - ((70-att_ptr->armor)/5);
if(n<1) n = 1;
}
                if(F_ISSET(crt_ptr, MBEFUD)) {
                          n = n/3;
                          print(fd, "\n%M%j 혼비백산합니다.",crt_ptr, "1");
                          broadcast_rom(fd, att_ptr->rom_num,
                            "\n%M%j 혼비백산합니다.",crt_ptr, "1");
                }
                att_ptr->hpcur -= n;
                print(fd, "\n%M%j 당신에게 %d만큼의 상처를 입혔습니다.",
                      crt_ptr, "1", n);
                broadcast_rom(fd, att_ptr->rom_num, "\n%M%j %M%j %d만큼의 피해를 입힙니다.",crt_ptr,"1",att_ptr,"3",n);

                if(F_ISSET(crt_ptr, MPOISS) && 
                   mrand(1,100) <= 15) {
                    print(fd, "\n%M%j 당신을 중독시켰습니다.",
                          crt_ptr,"1");
                    F_SET(att_ptr, PPOISN);
                }

                if(F_ISSET(crt_ptr, MDISEA) && 
                   mrand(1,100) <= 10) {
                    print(fd, "\n%M%j 당신에게 감염시켰습니다.",
                          crt_ptr,"1");
                    F_SET(att_ptr, PDISEA);
                }

		
		if(F_ISSET(crt_ptr, MBLNDR) &&
                   mrand(1,100) <= 10) {
                    print(fd, "\n%M%j 당신을 실명시켰습니다.",
                          crt_ptr,"1");
                    F_SET(att_ptr, PBLIND);
		}
  
                if(F_ISSET(crt_ptr, MDISIT) && 
                   mrand(1,100) <= 15) 
                dissolve_item(att_ptr,crt_ptr);
                


                ANSI(fd, WHITE);

                n = choose_item(att_ptr);

                if(n) {
                    if(--att_ptr->ready[n-1]->shotscur<1) {
                        print(fd, 
                             "\n%S%j 산산히 부서져 버렸습니다.",
                             att_ptr->ready[n-1]->name,"1");
                        broadcast_rom(fd, 
                             att_ptr->rom_num,
                             "\n%M의 %S%j 산산히 부서져 버렸습니다.",
                             att_ptr,
                             att_ptr->ready[n-1]->name,"1");
                        add_obj_crt(att_ptr->ready[n-1],
                                att_ptr);
                        att_ptr->ready[n-1] = 0;
                        compute_ac(att_ptr);
                    }
                }
                
                if(crt_ptr->NUMHITS != 0 && /* !F_ISSET(att_ptr,PNOAAT) &&*/ !p) {
                                            /* 플레이어 자동공격 */
                    etmp=crt_ptr->first_enm->next_tag;
                    while(etmp) {
                        e_ptr = find_crt(crt_ptr, rom_ptr->first_ply, etmp->enemy, 1);
                        if(!e_ptr) {
                            if(!find_who(etmp->enemy))
                                del_enm_crt(etmp->enemy, crt_ptr);
                        }
                        else {
                            rtn=attack_crt(e_ptr,crt_ptr) ;
                            if(rtn) goto crt_died;
                        }
                        etmp=etmp->next_tag;
                    }
                    rtn = attack_crt(att_ptr, crt_ptr);
          crt_died:
                    if(rtn) {
                        cp = first_active;
                        continue;
                    }
                }

                if(att_ptr->hpcur < 1) {
                    ANSI(fd, MAGENTA);
                    print(fd, "\n\n애석하게도 당신은 %M에게 죽었습니다.", crt_ptr);
                    ANSI(fd, WHITE);
                    die(att_ptr, crt_ptr);
                    cp = first_active;
                    continue;
                }
    
                else if(F_ISSET(att_ptr, PWIMPY)) {
                    if(att_ptr->hpcur <= att_ptr->WIMPYVALUE) {
                        flee(att_ptr);
						cp = first_active;
                        continue;
                    }
                }
            
                else if(F_ISSET(att_ptr, PFEARS)) {
					int ff;
					ff = 40 + (1- (att_ptr->hpcur/att_ptr->hpmax))*40 +
						bonus[att_ptr->constitution]*3 + 
						(att_ptr->class == PALADIN) ? -10 : 0;
                    if(ff < mrand(1,100)) {
                        flee(att_ptr);
						cp = first_active;
                        continue;
                    }
                }
            }

            else {
                ANSI(att_ptr->fd, CYAN);
                print(att_ptr->fd, "\n당신은 %M의 공격을 피했습니다.", crt_ptr);
                ANSI(att_ptr->fd, WHITE);
                /*if(t - Ply[att_ptr->fd].io->ltime > 5L &&*/
                if(crt_ptr->NUMHITS != 0) {
                    etmp=crt_ptr->first_enm->next_tag;
                    while(etmp) {
                        e_ptr = find_crt(crt_ptr, rom_ptr->first_ply, etmp->enemy, 1);
                        if(!e_ptr) {
                            if(!find_who(etmp->enemy))
                                del_enm_crt(etmp->enemy, crt_ptr);
                        }
                        else {
                            rtn=attack_crt(e_ptr,crt_ptr);
                            if(rtn) goto crt_died2;
                        }
                        etmp=etmp->next_tag;
                    }
                    rtn = attack_crt(att_ptr, crt_ptr);
         crt_died2:
                    if(rtn) {
                        cp = first_active;
                        continue;
                    }
                }
            }
            cp = cp->next_tag;
            continue;
        }
        }

        if(F_ISSET(crt_ptr, MAGGRE) || F_ISSET(crt_ptr, MGAGGR) ||
		F_ISSET(crt_ptr, MEAGGR)) {
	if (F_ISSET(crt_ptr, MAGGRE))
           	att_ptr = lowest_piety(rom_ptr, F_ISSET(crt_ptr, MDINVI) ? 1:0);
			else {
				att_ptr = low_piety_alg(rom_ptr,
					F_ISSET(crt_ptr, MDINVI) ? 1:0,
					F_ISSET(crt_ptr,MGAGGR) ? -1 : 1,((crt_ptr->level+3)/4));
                        }

            if(!att_ptr) {
                cp = cp->next_tag;
                continue;
            }
			if(att_ptr->dexterity > crt_ptr->dexterity && mrand(1,10) <4) {
				cp = cp->next_tag;
				continue;
			}

            ANSI(att_ptr->fd, RED);
            print(att_ptr->fd, "\n%M%j 당신을 공격합니다.", crt_ptr,"1");
            ANSI(att_ptr->fd, WHITE);
            crt_ptr->lasttime[LT_ATTCK].interval = 0;
            crt_ptr->lasttime[LT_ATTCK].ltime = t;
            broadcast_rom(att_ptr->fd, att_ptr->rom_num, 
                      "\n%M%j %S%j 공격합니다.",
                      crt_ptr,"1", att_ptr,"3");
            add_enm_crt(att_ptr->name, crt_ptr); 
      }

        cp = cp->next_tag;

    }
}

/************************************************************************/
/*              choose_item             */
/************************************************************************/

/* This function randomly chooses an item that the player pointed to    */
/* by the first argument is wearing.                    */

int choose_item(ply_ptr)
creature    *ply_ptr;
{
    char    checklist[MAXWEAR];
    int numwear=0, i;

    for(i=0; i<MAXWEAR; i++) {
        checklist[i] = 0;
        if(i==WIELD-1 || i==HELD-1) continue;
        if(ply_ptr->ready[i])
            checklist[numwear++] = i+1;
    }

    if(!numwear) return(0);

    i = mrand(0, numwear-1);
    return(checklist[i]);
}

/**********************************************************************/
/*              crt_spell                 */
/**********************************************************************/

/* This function allows monsters to cast spells at players.       */

int crt_spell(crt_ptr, att_ptr)
creature    *crt_ptr;
creature    *att_ptr;
{
    cmd cmnd;
    int i, j, spl, c;
    int known[10], knowctr = 0;
    int (*fn)();

    for(i=0; i<16; i++) {
        if(!crt_ptr->spells[i]) continue;
        for(j=i*8; j<=(i*8+7); j++) {
            if(knowctr > 9) break;
            if(S_ISSET(crt_ptr, j))
                known[knowctr++] = j;
        }
        if(knowctr > 9) break;
    }

    if(!knowctr)
        spl = 1;
    else {
        i = mrand(1, knowctr);
        spl = known[i-1];
    }
	if ((spllist[spl].splno != SVIGOR) && 
		(spllist[spl].splno != SMENDW) &&
		(spllist[spl].splno != SFHEAL)){
    	strcpy(cmnd.str[2], att_ptr->name);
    	cmnd.val[2] = 1;
    	cmnd.num = 3;
	} else
	 	cmnd.num = 2;

    fn = spllist[spl].splfn;

    ANSI(att_ptr->fd, RED);
    if(fn == offensive_spell) {
        for(c=0; ospell[c].splno != spllist[spl].splno; c++)
            if(ospell[c].splno == -1) return(0);
	strcpy(cmnd.str[1],spllist[spl].splstr);
        i = (*fn)(crt_ptr, &cmnd, CAST, spllist[spl].splstr,
            &ospell[c]);
    }
    else i = (*fn)(crt_ptr, &cmnd, CAST);
    ANSI(att_ptr->fd, WHITE);
    return(i);
}

/**********************************************************************/
/*              update_time               */
/**********************************************************************/

/* This function updates the game time in hours.  When it is 6am a sunrise */
/* message is broadcast.  When it is 8pm a sunset message is broadcast.    */

void update_time(t)
long    t;
{
    int daytime;

    last_time_update = t;

    Time++;
    daytime = (int)(Time % 24L);

    if(daytime == 6)
        broadcast("\n아침이 되었습니다. \"꼬끼오~~\"");
    else if(daytime == 20)
        broadcast("\n밤이 되었습니다. 황혼이 짙게 깔리며 뺨에 빛이 맴돕니다.");

}

/* 초인의돌 떨어짐 */
void update_moonstone(t)
long t;
{
    int rtn, rom_num;
    room *new_rom=NULL;
    object *obj_ptr=NULL;

    last_moonstone_update = t;

    do {
       rom_num = mrand(1, RMAX-1);
       rtn = load_rom(rom_num, &new_rom);
          if(rtn > -1 && F_ISSET(new_rom, RNOTEL))
              rtn = -1;
    } while(rtn < 0);

    if(strlen(new_rom->name) < 2) return;

    load_obj(640, &obj_ptr);
    obj_ptr->shotsmax+=mrand(1,20);
    obj_ptr->shotscur=obj_ptr->shotsmax;
    add_obj_rom(obj_ptr, new_rom);
    broadcast("\n%s에 초인의 돌이 떨어졌습니다.", new_rom->name);
}   

/* 아래 소스 짜구 무지 후회함 .. 왜냐 못짯기 때문 캬캬.. */
void update_monster(t)
long t;
{
    int rtn, rom_num;
    room *new_rom=NULL;
    creature *crt_ptr=NULL;
    int tmp=0;

    last_monster_update = t;

    for (tmp; tmp < 50; tmp++) {
        do {
            rom_num = mrand(8000, 8300);
            rtn = load_rom(rom_num, &new_rom);
            if(rtn > -1 && F_ISSET(new_rom, RNOTEL))
                rtn = -1;
        } while(rtn < 0);

        if(strlen(new_rom->name) < 2) return;

        load_crt(mrand(732, 755), &crt_ptr);
        add_crt_rom(crt_ptr, new_rom);
    }
    broadcast("\n드레니아에 카오스의 지휘하에 몹이 침공했습니다. \"쟁취하라!!\"");
    broadcast("\n\"쟁취하라!! 단, 일반 유저를 건드리면 내가 용서치 않으리라\"");

}

void update_monster_two(t)
long t;
{
    int rom_num;
    room *new_rom=NULL;
    creature *crt_ptr=NULL;
    int tmp=0;

    last_monster_two_update = t;

    for (tmp; tmp < 10; tmp++) {
        rom_num = mrand(3601, 3630);

        load_crt(mrand(265, 299), &crt_ptr);
        load_rom(rom_num, &new_rom);
        add_crt_rom(crt_ptr, new_rom);
    }
    broadcast("\n무적존을 강탈하려고 드레니아 몹이 침공했습니다.");
    broadcast("\n라작이 부하들에게 외칩니다. \"일반은 건드리지 말아라\"");
}

/**********************************************************************/
/*              update_shutdown               */
/**********************************************************************/

/* This function broadcasts a shutdown message every 30 seconds until */
/* shutdown is achieved.  Then it saves off all rooms and players,    */
/* and exits the game.                            */

void update_shutdown(t)
long t;
{
    long    i;
    long c;
     char path[128];

    last_shutdown_update = t;
	c = time(0);

    i = Shutdown.ltime + Shutdown.interval;
    if(i > t) {
        if(i-t > 60)
            broadcast_all("\n### %d분 %02d초 후에 머드를 종료합니다.",
                  (i-t)/60L, (i-t)%60L);
        else
            broadcast_all("\n### %d초 후에 머드를 종료합니다. 종료된 다음 바로 실행됩니다.", i-t);
    }
    else {
        broadcast("\n### 머드를 종료합니다. 다시 접속해 주세요");
        output_buf();
        logf("--- 머드 종료: %d --- (%.24s)\n", Port,ctime(&c));

        resave_all_rom(1);
        save_all_ply();


#ifdef RECORD_ALL
	sprintf(path,"%s/%s",LOGPATH,"all_cmd");
	unlink(path);
#endif RECORD_ALL
	kill(getpid(), 9);
        exit(0);
    }
}

/**********************************************************************/
/*              add_active                */
/**********************************************************************/

/* This function adds a monster to the active-monster list.  A pointer */
/* to the monster is passed in the first parameter.            */

void add_active(crt_ptr)
creature    *crt_ptr;
{
    ctag    *cp, *ct;

    del_active(crt_ptr);
    if(!crt_ptr)
        return;

    ct = (ctag *)malloc(sizeof(ctag));
    if(!ct)
        merror("add_active", FATAL);

    ct->crt = crt_ptr;
    ct->next_tag = 0;

    if(!first_active)
        first_active = ct;
    else {
        ct->next_tag = first_active;
        first_active = ct;
    }

}

/**********************************************************************/
/*              del_active                */
/**********************************************************************/

/* This function removes a monster from the active-monster list.  The */
/* parameter contains a pointer to the monster which is to be removed */

void del_active(crt_ptr)
creature    *crt_ptr;
{
    ctag    *cp, *prev;

    if(!(cp = first_active)) return;
    if(cp->crt == crt_ptr) {
        first_active = cp->next_tag;
        free(cp);
        return;
    }

    prev = cp;
    cp = cp->next_tag;
    while(cp) {
        if(cp->crt == crt_ptr) {
            prev->next_tag = cp->next_tag;
            free(cp);
            return;
        }
        prev = cp;
        cp = cp->next_tag;
    }

}
/*===================================================================*/
/*              update_exit              */
/*===================================================================*/
void update_exit(t)
long    t;
{
room    *rom_ptr;
xtag    *xp;
char    *tmp;
int i,x;

    last_exit_update = t;
    for(i=0;i<MAX_TEXIT; i++){
        if(load_rom( time_x[i].room,&rom_ptr) < 0)
            return; 
        xp = rom_ptr->first_ext;
        while(xp){
            if((time_x[i].name1) && (time_x[i].exit1 == xp->ext->room)
	      && (!strcmp(xp->ext->name,time_x[i].name1))) 
                F_SET(xp->ext,XNOSEE);

            if((time_x[i].name2) && (time_x[i].exit2 == xp->ext->room)
	      && (!strcmp(xp->ext->name,time_x[i].name2))) 
                F_CLR(xp->ext,XNOSEE);

            xp = xp->next_tag;
        }

             tmp  = time_x[i].name1;
             x = time_x[i].exit1;
             time_x[i].name1 = time_x[i].name2;
             time_x[i].exit1 = time_x[i].exit2;
             time_x[i].name2 = tmp;
             time_x[i].exit2 = x;
       
             if (!t_toggle)
               tmp = time_x[i].mess1;  
             else
               tmp = time_x[i].mess2;  

            if (tmp)
                broadcast("\n%s",tmp);
            t_toggle = !t_toggle;
                    
     }
}

/*====================================================================*/

void update_allcmd(t)
long t;
{
    char path[128];

    last_allcmd = t;
    sprintf(path,"%s/%s",LOGPATH,"all_cmd");
    unlink(path);
}

/*************************************/
int list_act(ply_ptr, cmnd)
creature *ply_ptr;
cmd	*cmnd;

{
ctag	*cp;

if (ply_ptr->class < DM)
	return(0);
print(ply_ptr->fd,"현재 활동중인 괴물\n\n이름:\n");
	cp = first_active;
	
	while(cp){
    print(ply_ptr->fd,"   %s.\n",cp->crt->name);
	cp = cp->next_tag;
	}
output_buf();
return;
}
