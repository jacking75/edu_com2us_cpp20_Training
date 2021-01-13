/*
 * PLAYER.C:
 *
 *	Player routines.
 *
 *	Copyright (C) 1991, 1992, 1993 Brett J. Vickers
 *
 */

#include "mstruct.h"
#include "mextern.h"

extern char *ply_titles[PMAX];

extern long all_broad_time;

/**********************************************************************/
/*				init_ply			      */
/**********************************************************************/

/* This function initializes a player's stats and loads up the room he */
/* should be in.  This should only be called when the player first     */
/* logs on. 							       */
extern long broad_time[PMAX];
extern long login_time[PMAX];

void init_ply(ply_ptr)
creature	*ply_ptr;
{
	char	file[80], file2[80], file3[80], str[50];
	room	*rom_ptr;
	object	*obj_ptr;
	otag	*op, *otemp;
	long	t, tdiff;
	int	    fnum, ftotal, n, check_dmg, check_ac, wf, i;
 
	F_CLR(ply_ptr, PSPYON);
	F_CLR(ply_ptr, PREADI);

	if(ply_ptr->class >= DM && strcmp(ply_ptr->name, DMNAME) &&
	   strcmp(ply_ptr->name, DMNAME2) && strcmp(ply_ptr->name, DMNAME3) &&
	   strcmp(ply_ptr->name, DMNAME4) && strcmp(ply_ptr->name, DMNAME5) &&
	   strcmp(ply_ptr->name, DMNAME6) && strcmp(ply_ptr->name, DMNAME7))
		ply_ptr->class = 1;

	if(!strcmp(ply_ptr->name, DMNAME) || !strcmp(ply_ptr->name, DMNAME2) ||
	  !strcmp(ply_ptr->name, DMNAME3) || !strcmp(ply_ptr->name, DMNAME4) ||
	  !strcmp(ply_ptr->name, DMNAME5) || !strcmp(ply_ptr->name, DMNAME6) ||
	  !strcmp(ply_ptr->name, DMNAME7)) {
		ply_ptr->class = DM;
    }      
    else if(ply_ptr->class >= DM || ply_ptr->class ==0) {
        /* 불법 아이디 : 직업 이상 */
	    t = time(0);
	    strcpy(str, (char *)ctime(&t));
    	str[strlen(str)-1] = 0;
        logf("%s : ### %s 불법아이디(직업=%d)\n",str,
            ply_ptr->name,ply_ptr->class);
    }

	if(ply_ptr->class < CARETAKER) {
		ply_ptr->daily[DL_BROAD].max = 25 + ((ply_ptr->level+3)/4)/2;
		ply_ptr->daily[DL_ENCHA].max = 10;
		ply_ptr->daily[DL_FHEAL].max = MAX(10, 10 + (((ply_ptr->level+3)/4)-5)/3);
		ply_ptr->daily[DL_TRACK].max = MAX(10, 10 + (((ply_ptr->level+3)/4)-5)/4);
		ply_ptr->daily[DL_DEFEC].max = 1;
	}

	F_SET(ply_ptr, PNOSUM);
#ifdef CHECKFAMILY 
	ftotal = load_family();
#endif
	fnum = ply_ptr->daily[DL_EXPND].max;
	if(!strcmp(fmboss_str[ply_ptr->daily[DL_EXPND].max], "*해체*")) {
		F_CLR(ply_ptr, PFAMIL);
		F_CLR(ply_ptr, PRDFML);
		ply_ptr->daily[DL_EXPND].max = 0;
	}
	if(!strcmp(ply_ptr->name, fmboss_str[fnum])) {
		F_SET(ply_ptr, PFMBOS);
	}
	else F_CLR(ply_ptr, PFMBOS);

	if(ply_ptr->class < DM)
		F_SET(ply_ptr, PPROMP);

	if(!F_ISSET(ply_ptr, PDMINV) && ply_ptr->class!=DM) {
                broadcast("\n### %s %s님이 들어오셨습니다.", class_str[ply_ptr->class], ply_ptr->name);
                all_broad_time=time(0);
        }

	t = time(0);
	strcpy(str, (char *)ctime(&t));
	str[strlen(str)-1] = 0;
                logf("%s: %s (%s)님이 들어옴\n", str, ply_ptr->name,
			Ply[ply_ptr->fd].io->address);
      if(ply_ptr->class >= SUB_DM) {
		log_dm("%s : %s (%s)님이 들어옴\n", str, ply_ptr->name,
		      Ply[ply_ptr->fd].io->address);
      }

	ply_ptr->lasttime[LT_PSAVE].ltime = t;
	ply_ptr->lasttime[LT_PSAVE].interval = SAVEINTERVAL;
      login_time[ply_ptr->fd] = t;

	if(load_rom(ply_ptr->rom_num, &rom_ptr) < 0)
		load_rom(1, &rom_ptr);

	n = count_vis_ply(rom_ptr);
	if((F_ISSET(rom_ptr, RONEPL) && n > 0) ||
	  (F_ISSET(rom_ptr, RTWOPL) && n > 1) ||
	  (F_ISSET(rom_ptr, RTHREE) && n > 2) ||
      (F_ISSET(rom_ptr, RNOLOG)))
		load_rom(1, &rom_ptr);
	if(F_ISSET(rom_ptr, RONMAR) && (rom_ptr->special != 
			ply_ptr->daily[DL_MARRI].max)) 
		load_rom(1, &rom_ptr);

	add_ply_rom(ply_ptr, rom_ptr);

	tdiff = t -  ply_ptr->lasttime[LT_HOURS].ltime;
	for(i=0; i<45; i++) {
		ply_ptr->lasttime[i].ltime += tdiff;
		ply_ptr->lasttime[i].ltime =
			MIN(t, ply_ptr->lasttime[i].ltime);
	}
        broad_time[ply_ptr->fd]=0;

	op = ply_ptr->first_obj;
	while(op) {
		otemp = op->next_tag;
		obj_ptr = op->obj;
  
        if(F_ISSET(obj_ptr, OEVENT) && !F_ISSET(obj_ptr, ONEWEV)) {
            del_obj_crt(obj_ptr, ply_ptr);
            free_obj(obj_ptr);
            op = otemp;
            continue;
        }      

		if(obj_ptr->shotscur < 1) {
			op = otemp;
			continue;
		}
		if(obj_ptr->shotscur > 1500 || obj_ptr->shotsmax > 1500) {
			op = otemp;
			continue;
		}
		check_ac = obj_ptr->armor;
		if(obj_ptr->wearflag == BODY) check_ac *= 2;
		else check_ac *= 5;
		if(check_ac > 150) {
			op = otemp;
			continue;
		}
		if(check_ac < 30 ) check_ac = 0;
		if(obj_ptr->questnum == 0 && check_ac > 30 && ply_ptr->class < INVINCIBLE) 
			if(ply_ptr->level < check_ac) {
				op = otemp;
				continue;
			}
		if(obj_ptr->type < ARMOR) {
			check_dmg = obj_ptr->ndice * obj_ptr->sdice + obj_ptr->pdice;
			if(check_dmg > 100) {
				op = otemp;
				continue;
			}
			if(ply_ptr->class == FIGHTER) check_dmg -= 7;
			if(ply_ptr->class == ASSASSIN || ply_ptr->class == THIEF)
				check_dmg -= 3;
			if(ply_ptr->class == PALADIN || ply_ptr->class == RANGER)
				check_dmg -= 2;
			if(check_dmg > 15 && ply_ptr->class < INVINCIBLE &&
				ply_ptr->level < check_dmg*3 && obj_ptr->questnum == 0) {
				op = otemp;
				continue;
			}
		}
		if(F_ISSET(obj_ptr, OWEARS)) {
			wf = obj_ptr->wearflag;
			if(wf == FINGER) {
				for(i=FINGER1-1; i<=FINGER8-1; i++)
					if(!ply_ptr->ready[i]) {
						ply_ptr->ready[i] = obj_ptr;
						del_obj_crt(obj_ptr, ply_ptr);
						break;
					}
			}
			else if(wf == NECK) {
				for(i=NECK1-1; i<=NECK2-1; i++)
					if(!ply_ptr->ready[i]) {
						ply_ptr->ready[i] = obj_ptr;
						del_obj_crt(obj_ptr, ply_ptr);
						break;
					}
			}
			else if(wf == WIELD) {
				if(!ply_ptr->ready[wf-1] && !F_ISSET(obj_ptr, OWHELD)) {
						ply_ptr->ready[wf-1] = obj_ptr;
						del_obj_crt(obj_ptr, ply_ptr);
				}
				else if(!ply_ptr->ready[HELD-1] && F_ISSET(obj_ptr, OWHELD)) {
						ply_ptr->ready[HELD-1] = obj_ptr;
						del_obj_crt(obj_ptr, ply_ptr);
				}
			}	
			else{
				if(!ply_ptr->ready[wf-1]) {
					ply_ptr->ready[wf-1] = obj_ptr;
					del_obj_crt(obj_ptr, ply_ptr);
				}
			}
		}
		op = otemp;
	}

	compute_ac(ply_ptr);
	compute_thaco(ply_ptr);
	update_ply(ply_ptr);

	sprintf(file, "%s/dialin", DOCPATH);
	if (!strcmp(Ply[ply_ptr->fd].io->address, "128.200.142.2"))
		view_file(ply_ptr->fd, 1, file);

	if (ply_ptr->class != DM) {
	sprintf(file, "%s/news", LOGPATH);
	view_file(ply_ptr->fd, 1, file);
	}
	else {
	sprintf(file, "%s/DM_news", LOGPATH);
	view_file(ply_ptr->fd, 1, file);
	}

	if(F_ISSET(ply_ptr, PFAMIL)) {
	sprintf(file3, "%s/family/family_news_%d", PLAYERPATH, fnum);
	if(file_exists(file3)) {
	  ANSI(ply_ptr->fd, CYAN);
	  view_file(ply_ptr->fd, 1, file3);
	  ANSI(ply_ptr->fd, NORMAL);
	}
	}

	sprintf(file2, "%s/fal/%s", PLAYERPATH, ply_ptr->name);
	if(file_exists(file2)) {
		ANSI(ply_ptr->fd, YELLOW);
		view_file(ply_ptr->fd, 1, file2);
		ANSI(ply_ptr->fd, NORMAL);
		unlink(file2);
	}

	sprintf(str, "%s/%s", POSTPATH, ply_ptr->name);
	if(file_exists(str))
                print(ply_ptr->fd, "\n*** 우체국에 편지가 와있습니다.\n");

/* By. Latok  */
	if(ply_ptr->gold > 300000000){
		ply_ptr->gold = 300000000;
		print(ply_ptr->fd, "\n\n너무 많은 돈을 가지고 있습니다.
		\n신이 자기보다 더 많은 돈을 가지고 있다고 하여, 
		\n가지고 있는 돈중에 3억만 남겨놓고, 나머지 부분을
		\n신이 그냥 가져갑니다. (신 : 재수~~~~ )\n\n");
	}
	
}

/**********************************************************************/
/*				uninit_ply			      */
/**********************************************************************/

/* This function de-initializes a player who has left the game.  This  */
/* is called whenever a player quits or disconnects, right before save */
/* is called.							       */

void uninit_ply(ply_ptr)
creature	*ply_ptr;
{
	creature	*crt_ptr;
	ctag		*cp, *prev;
	int		i;
      long t;
      char str[128];

	if(ply_ptr->parent_rom)
		del_ply_rom(ply_ptr, ply_ptr->parent_rom);

	cp = ply_ptr->first_fol;
	while(cp) {
		cp->crt->following = 0;
                print(cp->crt->fd, "\n당신은 %s님을 그만 따라다닙니다.", ply_ptr->name);
		prev = cp->next_tag;
		free(cp);
		cp = prev;
	}
	ply_ptr->first_fol = 0;
		
	if(ply_ptr->following) {
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

		if(!F_ISSET(ply_ptr, PDMINV))
                        print(crt_ptr->fd, "\n%s님이 당신을 그만 따라다닙니다.",
			      ply_ptr->name);
	}
   
/**********************************************************************/      
    clear_enm_crt(ply_ptr);   
/**********************************************************************/
	for(i=0; i<MAXWEAR; i++)
		if(ply_ptr->ready[i]) {
			add_obj_crt(ply_ptr->ready[i], ply_ptr);
			ply_ptr->ready[i] = 0;
		}

	update_ply(ply_ptr);
	if(!F_ISSET(ply_ptr, PDMINV) && ply_ptr->class<=SUB_DM && !F_ISSET(ply_ptr, SUICD)) {
                broadcast("\n### %s님이 나가셨습니다.", ply_ptr->name);
                all_broad_time=time(0);
        }
      t = time(0);
      strcpy(str, (char *)ctime(&t));
      str[strlen(str)-1] = 0;
	if(!F_ISSET(ply_ptr, SUICD)) {
            logf("%s: %s님이 나감\n", str, ply_ptr->name);
	    if(ply_ptr->class == DM)
		    log_dm("%s : %s님이 나감\n", str, ply_ptr->name);
    }

}

/**********************************************************************/
/*				update_ply			      */
/**********************************************************************/

/* This function checks up on all a player's time-expiring flags to see */
/* if some of them have expired.  If so, flags are set accordingly.     */

void update_ply(ply_ptr)
creature	*ply_ptr;
{
	long 	t;
	int	item;
	char 	ill, prot = 1; 	/*character is prot in a pharm room */

	t = time(0);
	ply_ptr->lasttime[LT_HOURS].interval +=
		(t - ply_ptr->lasttime[LT_HOURS].ltime);
	ply_ptr->lasttime[LT_HOURS].ltime = t;

	if(F_ISSET(ply_ptr, PHASTE)) {
		if(t > LT(ply_ptr, LT_HASTE)) {
			ANSI(ply_ptr->fd, GREEN);
                        print(ply_ptr->fd, "\n당신의 몸이 느려졌습니다.");
			ANSI(ply_ptr->fd, WHITE);
			F_CLR(ply_ptr, PHASTE);
			ply_ptr->dexterity -= 15;
                      compute_ac(ply_ptr);
		}
	}
        if(F_ISSET(ply_ptr, PPOWER)) {
                if(t > LT(ply_ptr, LT_POWER)) {
			ANSI(ply_ptr->fd, GREEN);
                        print(ply_ptr->fd, "\n당신의 힘이 약해졌습니다.");
			ANSI(ply_ptr->fd, WHITE);
                        F_CLR(ply_ptr, PPOWER);
                        ply_ptr->strength -= 3;
                      compute_ac(ply_ptr);
		}
	}
        if(F_ISSET(ply_ptr, PUPDMG)) {
                if(t > LT(ply_ptr, LT_UPDMG)) {
			ANSI(ply_ptr->fd, GREEN);
                        print(ply_ptr->fd, "\n당신의 기가 빠져나갑니다.");
			ANSI(ply_ptr->fd, WHITE);
                        F_CLR(ply_ptr, PUPDMG);
                        ply_ptr->pdice -= 5;
                        ply_ptr->hpmax -= 100;
                        ply_ptr->mpmax -= 100;
                      compute_ac(ply_ptr);
		}
	}
        if(F_ISSET(ply_ptr, PSLAYE)) {
                if(t > LT(ply_ptr, LT_SLAYE)) {
			ANSI(ply_ptr->fd, GREEN);
                        print(ply_ptr->fd, "\n당신의 무기가 살기를 잃었습니다.");
			ANSI(ply_ptr->fd, WHITE);
                        F_CLR(ply_ptr, PSLAYE);
                        ply_ptr->thaco += 3;
                      compute_ac(ply_ptr);
		}
	}

        if(F_ISSET(ply_ptr, PMEDIT)) {
                if(t > LT(ply_ptr, LT_MEDIT)) {
			ANSI(ply_ptr->fd, GREEN);
                        print(ply_ptr->fd, "\n참선의 영향력이 떨어졌습니다.");
			ANSI(ply_ptr->fd, WHITE);
                        F_CLR(ply_ptr, PMEDIT);
                        ply_ptr->intelligence -= 3;
                      compute_ac(ply_ptr);
		}
	}

	if(F_ISSET(ply_ptr, PPRAYD)) {
		if(t > LT(ply_ptr, LT_PRAYD)) {
			ANSI(ply_ptr->fd, YELLOW);
                        print(ply_ptr->fd, "\n당신의 믿음이 약해졌습니다.");
			F_CLR(ply_ptr, PPRAYD);
			ANSI(ply_ptr->fd, WHITE);
			ply_ptr->piety -= 5;
		}
	}
	if(F_ISSET(ply_ptr, PINVIS)) {
		if(t > LT(ply_ptr, LT_INVIS) && ply_ptr->class < DM) {
			ANSI(ply_ptr->fd, MAGENTA);
                        print(ply_ptr->fd, "\n당신은 이제 눈에 보입니다.");
			ANSI(ply_ptr->fd, WHITE);
			F_CLR(ply_ptr, PINVIS);
		}
	}
	if(F_ISSET(ply_ptr, PDINVI)) {
		if(t > LT(ply_ptr, LT_DINVI)  && ply_ptr->class < DM) {
			ANSI(ply_ptr->fd, MAGENTA);
                        print(ply_ptr->fd, "\n당신의 눈이 침침해졌습니다.");
			ANSI(ply_ptr->fd, WHITE);
			F_CLR(ply_ptr, PDINVI);
		}
	}
	if(F_ISSET(ply_ptr, PDMAGI)) {
		if(t > LT(ply_ptr, LT_DMAGI) && ply_ptr->class < DM) {
			ANSI(ply_ptr->fd, MAGENTA);
                        print(ply_ptr->fd, "\n당신의 감지력이 떨어졌습니다.");
			ANSI(ply_ptr->fd, WHITE);
			F_CLR(ply_ptr, PDMAGI);
		}
	}
	if(F_ISSET(ply_ptr, PHIDDN)) {
		if(t-ply_ptr->lasttime[LT_HIDES].ltime > 300L)
			F_CLR(ply_ptr, PHIDDN);
	}
	if(F_ISSET(ply_ptr, PPROTE)) {
		if(t > LT(ply_ptr, LT_PROTE)) {
			ANSI(ply_ptr->fd, YELLOW);
                        print(ply_ptr->fd, "\n당신의 보호력이 떨어졌습니다.");
			ANSI(ply_ptr->fd, WHITE);
			F_CLR(ply_ptr, PPROTE);
			compute_ac(ply_ptr);
		}
	}
	if(F_ISSET(ply_ptr, PLEVIT)) {
		if(t > LT(ply_ptr, LT_LEVIT) && ply_ptr->class < DM) {
			ANSI(ply_ptr->fd, MAGENTA);
                        print(ply_ptr->fd, "\n당신은 땅에 내려섰습니다.");
			ANSI(ply_ptr->fd, WHITE);
			F_CLR(ply_ptr, PLEVIT);
		}
	}
	if(F_ISSET(ply_ptr, PBLESS)) {
		if(t > LT(ply_ptr, LT_BLESS)) {
			ANSI(ply_ptr->fd, YELLOW);
                        print(ply_ptr->fd, "\n축복력이 떨어졌습니다.");
			ANSI(ply_ptr->fd, WHITE);
			F_CLR(ply_ptr, PBLESS);
			compute_thaco(ply_ptr);
		}
	}

	if(F_ISSET(ply_ptr, PRFIRE)) {
		if(t > LT(ply_ptr, LT_RFIRE)) {
			ANSI(ply_ptr->fd, YELLOW);
                        print(ply_ptr->fd, "\n당신의 피부가 돌아왔습니다.");
			ANSI(ply_ptr->fd, WHITE);
			F_CLR(ply_ptr, PRFIRE);
		}
	}


	if(F_ISSET(ply_ptr, PRCOLD)) {
		if(t > LT(ply_ptr, LT_RCOLD)) {
			ANSI(ply_ptr->fd, BOLD);
			ANSI(ply_ptr->fd, YELLOW);
                        print(ply_ptr->fd, "\n차가운 기운이 몸을 휩쌉니다.");
			ANSI(ply_ptr->fd, NORMAL);
			ANSI(ply_ptr->fd, WHITE);
			F_CLR(ply_ptr, PRCOLD);
		}
	}


	if(F_ISSET(ply_ptr, PBRWAT)) {
		if(t > LT(ply_ptr, LT_BRWAT)) {
			ANSI(ply_ptr->fd, BOLD);
			ANSI(ply_ptr->fd, BLUE);
                        print(ply_ptr->fd, "\n당신의 폐가 줄어들었습니다.");
			ANSI(ply_ptr->fd, WHITE);
			ANSI(ply_ptr->fd, NORMAL);
			F_CLR(ply_ptr, PBRWAT);
		}
	}
	if(F_ISSET(ply_ptr, PSSHLD)) {
		if(t > LT(ply_ptr, LT_SSHLD)) {
			ANSI(ply_ptr->fd, BOLD);
			ANSI(ply_ptr->fd, GREEN);
                        print(ply_ptr->fd, "\n당신의 주술 방패가 사라졌습니다.");
			ANSI(ply_ptr->fd, WHITE);
			ANSI(ply_ptr->fd, NORMAL);
			F_CLR(ply_ptr, PSSHLD);
		}
	}
	if(F_ISSET(ply_ptr, PFLYSP)) {
		if(t > LT(ply_ptr, LT_FLYSP)  && ply_ptr->class < DM) {
			ANSI(ply_ptr->fd, YELLOW);
                        print(ply_ptr->fd, "\n당신은 더이상 날수 없습니다.");
			ANSI(ply_ptr->fd, WHITE);
			F_CLR(ply_ptr, PFLYSP);
		}
	}
	if(F_ISSET(ply_ptr, PRMAGI)) {
		if(t > LT(ply_ptr, LT_RMAGI)) {
			ANSI(ply_ptr->fd, BOLD);
			ANSI(ply_ptr->fd, MAGENTA);
            print(ply_ptr->fd,"\n마법의 방어력이 사라졌습니다.");
			ANSI(ply_ptr->fd, WHITE);
			ANSI(ply_ptr->fd, NORMAL);
			F_CLR(ply_ptr, PRMAGI);
		}
	}
	if(F_ISSET(ply_ptr, PSILNC)) {
		if(t > LT(ply_ptr, LT_SILNC)) {
			ANSI(ply_ptr->fd, GREEN);
            print(ply_ptr->fd,"\n당신의 목소리를 되찾았습니다!");
			ANSI(ply_ptr->fd, WHITE);
			F_CLR(ply_ptr, PSILNC);
		}
	}
	if(F_ISSET(ply_ptr, PFEARS)) {
		if(t > LT(ply_ptr, LT_FEARS)) {
			ANSI(ply_ptr->fd, YELLOW);
            print(ply_ptr->fd,"\n당신은 용기를 되찾았습니다.");
			ANSI(ply_ptr->fd, WHITE);
			F_CLR(ply_ptr, PFEARS);
		}
	}
	if(F_ISSET(ply_ptr, PKNOWA)) {
		if(t > LT(ply_ptr, LT_KNOWA) && ply_ptr->class < DM) {
			ANSI(ply_ptr->fd, CYAN);
            print(ply_ptr->fd,"\n당신의 분별력이 감퇴되었습니다.");
			ANSI(ply_ptr->fd, WHITE);
			F_CLR(ply_ptr, PKNOWA);
		}
	}
	if(F_ISSET(ply_ptr, PLIGHT)) {
		if(t > LT(ply_ptr, LT_LIGHT)  && ply_ptr->class < DM) {
			ANSI(ply_ptr->fd, YELLOW);
                        print(ply_ptr->fd, "\n마법의 빛이 사라졌습니다.");
			broadcast_rom(ply_ptr->fd, ply_ptr->rom_num,
                      "\n%M의 마법의 빛이 사라졌습니다.", ply_ptr);
			ANSI(ply_ptr->fd, WHITE);
			F_CLR(ply_ptr, PLIGHT);
		}
	}
	if(t > LT(ply_ptr, LT_CHRMD) && F_ISSET(ply_ptr, PCHARM)) {
                        ANSI(ply_ptr->fd, YELLOW);
                        print(ply_ptr->fd, "\n당신의 행동이 정상적으로 되었습니다.");
                        F_CLR(ply_ptr, PCHARM);
			ANSI(ply_ptr->fd, WHITE);
        }

	/* check if player is suffering from any  aliment */
	if(F_ISSET(ply_ptr, PPOISN) || F_ISSET(ply_ptr, PDISEA))
		ill = 1;
	else
		ill =0;


	/* handle normal healing for non posioned or harm room */
	if(ply_ptr->parent_rom && (t > LT(ply_ptr, LT_HEALS)))
 		if((!F_ISSET(ply_ptr->parent_rom, RPHARM) && (!ill)) && !F_ISSET(ply_ptr, PPOISN)) {
			ply_ptr->hpcur += MAX(4, 5 + bonus[ply_ptr->constitution] +
				(ply_ptr->class == BARBARIAN ? 2:0));
			ply_ptr->mpcur += MAX(4, 5+(ply_ptr->intelligence > 17 ? 1:0)+
				(ply_ptr->class == MAGE ? 2:0));
			ply_ptr->lasttime[LT_HEALS].ltime = t;
			ply_ptr->lasttime[LT_HEALS].interval = 5; 
		     /*  3*bonus[ply_ptr->piety];*/
			/* handle quick healing */
			if(F_ISSET(ply_ptr->parent_rom, RHEALR)) {
				ply_ptr->hpcur += 100;
				ply_ptr->mpcur += 100;
				ply_ptr->lasttime[LT_HEALS].interval /= 3;
			}

			ply_ptr->hpcur = MIN(ply_ptr->hpcur, ply_ptr->hpmax);
			ply_ptr->mpcur = MIN(ply_ptr->mpcur, ply_ptr->mpmax);
		}
		/* handle poison  */

		else if(!F_ISSET(ply_ptr->parent_rom, RPHARM) && ill) {
		   if(F_ISSET(ply_ptr, PPOISN)){
		ANSI(ply_ptr->fd, BLINK);
		ANSI(ply_ptr->fd, RED);
                print(ply_ptr->fd, "\n독이 당신의 핏줄로 스며듭니다.");
		ply_ptr->hpcur -= MAX(1,mrand(1,ply_ptr->hpmax/5)-bonus[ply_ptr->constitution]);
		ply_ptr->lasttime[LT_HEALS].ltime = t;
		ply_ptr->lasttime[LT_HEALS].interval = 30 - 
				   3*bonus[ply_ptr->constitution];
		ANSI(ply_ptr->fd, WHITE);
		ANSI(ply_ptr->fd, NORMAL);
		if(ply_ptr->hpcur < 1)
			die(ply_ptr, ply_ptr);
	   }
	   if (F_ISSET(ply_ptr, PDISEA)){
		ANSI(ply_ptr->fd, BLINK);
		ANSI(ply_ptr->fd, RED);
                print(ply_ptr->fd, "\n병이 당신의 마음을 잠식합니다.");
        	ply_ptr->lasttime[LT_ATTCK].ltime = time(0);
		ANSI(ply_ptr->fd, BLUE);
		print(ply_ptr->fd, "몸이 피로해 집니다.\n");
        	ply_ptr->lasttime[LT_ATTCK].interval= dice(1,6,3); 
		ply_ptr->hpcur -= MAX(1,mrand(1,6)-bonus[ply_ptr->constitution]);
		ply_ptr->lasttime[LT_HEALS].ltime = t;
		ply_ptr->lasttime[LT_HEALS].interval = 30 - 
				    3*bonus[ply_ptr->constitution];
		ANSI(ply_ptr->fd, WHITE);
		ANSI(ply_ptr->fd, NORMAL);
		if(ply_ptr->hpcur < 1)
			die(ply_ptr, ply_ptr);
	   }
	}
	/* handle player harm rooms */
	else {
		if (F_ISSET(ply_ptr->parent_rom, RPPOIS)) {
			ANSI(ply_ptr->fd, BLINK);
			ANSI(ply_ptr->fd, GREEN);
			print(ply_ptr->fd, "\n독기운이 당신을 중독시킵니다.");
			ANSI(ply_ptr->fd, WHITE);
			ANSI(ply_ptr->fd, NORMAL);
			F_SET(ply_ptr, PPOISN);    
		}

		if (F_ISSET(ply_ptr->parent_rom, RPBEFU)) {
		        print(ply_ptr->fd, "\n방이 빙글빙글 도는것 감습니다.\n이제 정신을 차립니다.");
  		        ply_ptr->lasttime[LT_ATTCK].ltime = time(0);
        		ply_ptr->lasttime[LT_ATTCK].interval= MAX(dice(2,6,0),6); 
		}

		if (F_ISSET(ply_ptr, PPOISN)){
			ANSI(ply_ptr->fd, BLINK);
			ANSI(ply_ptr->fd, RED);
       			print(ply_ptr->fd, "\n독이 당신의 핏줄로 스며듭니다.");
			ANSI(ply_ptr->fd, NORMAL);
			ANSI(ply_ptr->fd, WHITE);
			ply_ptr->hpcur -= MAX(1,mrand(1,4)-bonus[ply_ptr->constitution]);
		  	if(ply_ptr->hpcur < 1) die(ply_ptr, ply_ptr);
		}
		if (F_ISSET(ply_ptr, PDISEA)){
			ANSI(ply_ptr->fd, BLINK);
			ANSI(ply_ptr->fd, RED);
		        print(ply_ptr->fd, "\n병이 당신의 마음을 잠식합니다.");
        		ply_ptr->lasttime[LT_ATTCK].ltime = time(0);
			ANSI(ply_ptr->fd, BLUE);
        		print(ply_ptr->fd, "\n당신은 신경질적으로 됩니다.");
        		ply_ptr->lasttime[LT_ATTCK].interval= dice(1,6,3); 
        		ply_ptr->hpcur -= MAX(1,mrand(1,6)-bonus[ply_ptr->constitution]);
        		ply_ptr->lasttime[LT_HEALS].ltime = t;
        		ply_ptr->lasttime[LT_HEALS].interval = 30 - 
                            3*bonus[ply_ptr->constitution];
			ANSI(ply_ptr->fd, NORMAL);
			ANSI(ply_ptr->fd, WHITE);
        		if(ply_ptr->hpcur < 1)
                		die(ply_ptr, ply_ptr);
		}                                   

		if (F_ISSET(ply_ptr->parent_rom,RPMPDR))
 			ply_ptr->mpcur -= MIN(ply_ptr->mpcur,3); 
		else if (!ill) {
			ply_ptr->mpcur += MAX(1, 2+(ply_ptr->intelligence > 17 ? 1:0)+
                                (ply_ptr->class == MAGE ? 2:0));
		ply_ptr->mpcur = MIN(ply_ptr->mpcur, ply_ptr->mpmax);
	}
    
	if (F_ISSET(ply_ptr->parent_rom, RFIRER) && !F_ISSET(ply_ptr,PRFIRE)) {
        	print(ply_ptr->fd, "\n뜨거운 기운이 당신을 태웁니다.");
		prot = 0;
	}
	else if (F_ISSET(ply_ptr->parent_rom, RWATER) && !F_ISSET(ply_ptr,PBRWAT)) {
        	print(ply_ptr->fd, "\n물이 당신의 폐로 흘러듭니다.");
        	prot = 0;
	}
	else if (F_ISSET(ply_ptr->parent_rom, REARTH) && !F_ISSET(ply_ptr,PSSHLD)) {
        	print(ply_ptr->fd, "\n흙이 무너져 당신을 덮칩니다.");
        	prot = 0;
	}
	else if (F_ISSET(ply_ptr->parent_rom, RWINDR) && !F_ISSET(ply_ptr,PRCOLD)) {
		ANSI(ply_ptr->fd, BLUE);
        	print(ply_ptr->fd, "\n차가운 기운이 뼈속까지 스며듭니다.");
		ANSI(ply_ptr->fd, WHITE);
		prot = 0;
	}
	else  if(!F_ISSET(ply_ptr->parent_rom, RWINDR) &&
		 !F_ISSET(ply_ptr->parent_rom, REARTH) &&
		 !F_ISSET(ply_ptr->parent_rom, RFIRER) &&
		 !F_ISSET(ply_ptr->parent_rom, RWATER) &&
		 !F_ISSET(ply_ptr->parent_rom, RPPOIS) &&
		 !F_ISSET(ply_ptr->parent_rom, RPBEFU) &&
		 !F_ISSET(ply_ptr->parent_rom, RPMPDR)) {
			ANSI(ply_ptr->fd, BOLD);
			ANSI(ply_ptr->fd, MAGENTA);
       			print(ply_ptr->fd, "\n보이지않는 무엇이 당신의 생명력을 빨아들입니다.");
			ANSI(ply_ptr->fd, NORMAL);
			ANSI(ply_ptr->fd, WHITE);
			prot = 0;
	}
	if (!prot) {
		ply_ptr->hpcur -= 8 - MIN(bonus[ply_ptr->constitution],2);
        	if(ply_ptr->hpcur < 1) die(ply_ptr, ply_ptr);      
	}
	else if (!ill)
	{
		ply_ptr->hpcur += MAX(1, 3 + bonus[ply_ptr->constitution] +
				(ply_ptr->class == BARBARIAN ? 2:0));
		ply_ptr->hpcur = MIN(ply_ptr->hpcur, ply_ptr->hpmax);
	}

	ply_ptr->lasttime[LT_HEALS].ltime = t;
	ply_ptr->lasttime[LT_HEALS].interval = 5 -  3*bonus[ply_ptr->piety];

}
/*******************************/


	if(t > LT(ply_ptr, LT_PSAVE)) {
		ply_ptr->lasttime[LT_PSAVE].ltime = t;
		savegame(ply_ptr, 0);
	}

	item = has_light(ply_ptr);
	if(item && item != MAXWEAR+1) {
		if(ply_ptr->ready[item-1]->type == LIGHTSOURCE)
			if(--(ply_ptr->ready[item-1]->shotscur) < 1) {
                                print(ply_ptr->fd, "\n당신의 %S%j 꺼졌습니다.",
				      ply_ptr->ready[item-1]->name,"1");
				broadcast_rom(ply_ptr->fd, ply_ptr->parent_rom->rom_num,
                          "\n%M의 %S%j 꺼졌습니다.",
					      ply_ptr,
					      ply_ptr->ready[item-1]->name,"1");
			}
	}

}

/**********************************************************************/
/*				up_level			      */
/**********************************************************************/

/* This function should be called whenever a player goes up a level.  */
/* It raises her hit points and magic points appropriately, and if    */
/* it is initializing a new character, it sets up the character.      */

void up_level(ply_ptr)
creature	*ply_ptr;
{
	int	index;

	ply_ptr->level++;
		ply_ptr->ndice = class_stats[ply_ptr->class].ndice;
		ply_ptr->sdice = class_stats[ply_ptr->class].sdice;
		ply_ptr->pdice = class_stats[ply_ptr->class].pdice;
	if(ply_ptr->level%2) ply_ptr->hpmax += class_stats[ply_ptr->class].hp;
	else ply_ptr->mpmax += class_stats[ply_ptr->class].mp;
	if(ply_ptr->level == 1) {
		ply_ptr->hpmax = class_stats[ply_ptr->class].hpstart;
		ply_ptr->mpmax = class_stats[ply_ptr->class].mpstart;
		ply_ptr->hpcur = ply_ptr->hpmax;
		ply_ptr->mpcur = ply_ptr->mpmax;
		ply_ptr->ndice = class_stats[ply_ptr->class].ndice;
		ply_ptr->sdice = class_stats[ply_ptr->class].sdice;
		ply_ptr->pdice = class_stats[ply_ptr->class].pdice;
	}
	else {
              if(ply_ptr->level%4) return;

		index = (ply_ptr->level-2) % 10;
		switch(level_cycle[ply_ptr->class][index]) {
		case STR: ply_ptr->strength++; break;
		case DEX: ply_ptr->dexterity++; break;
		case CON: ply_ptr->constitution++; break;
		case INT: ply_ptr->intelligence++; break;
		case PTY: ply_ptr->piety++; break;
		}
	}
  
     /*  기존의 플레이어 데이타 갱신 */
    
               ply_ptr->hpmax = class_stats[ply_ptr->class].hpstart    
                               + (class_stats[ply_ptr->class].hp * (ply_ptr->level-1)/2);
               ply_ptr->mpmax = class_stats[ply_ptr->class].mpstart
                               + (class_stats[ply_ptr->class].mp * (ply_ptr->level-1)/2);
               ply_ptr->hpcur = ply_ptr->hpmax;
               ply_ptr->mpcur = ply_ptr->mpmax;
}

/**********************************************************************/
/*				down_level			      */
/**********************************************************************/

/* This function is called when a player loses a level due to dying or */
/* for some other reason.  The appropriate stats are downgraded.       */

void down_level(ply_ptr)
creature	*ply_ptr;
{
	int	index;

	ply_ptr->level--;
	if(F_ISSET(ply_ptr, PUPDMG)) { 
		F_CLR(ply_ptr, PUPDMG);
		ply_ptr->hpmax -= 100;
		ply_ptr->mpmax -= 100;
		ply_ptr->pdice -= 5;
	}

	if((ply_ptr->level-1)%2) ply_ptr->hpmax -= class_stats[ply_ptr->class].hp;
	else ply_ptr->mpmax -= class_stats[ply_ptr->class].mp;
	ply_ptr->hpcur = ply_ptr->hpmax;
	ply_ptr->mpcur = ply_ptr->mpmax;
      
      if((ply_ptr->level+1)%4) return;
	index = (ply_ptr->level-1) % 10;
	switch(level_cycle[ply_ptr->class][index]) {
	case STR: ply_ptr->strength--; break;
	case DEX: ply_ptr->dexterity--; break;
	case CON: ply_ptr->constitution--; break;
	case INT: ply_ptr->intelligence--; break;
	case PTY: ply_ptr->piety--; break;
	}

}

/**********************************************************************/
/*				add_obj_crt			      */
/**********************************************************************/

/* This function adds the object pointer to by the first parameter to */
/* the inventory of the player pointed to by the second parameter.    */

void add_obj_crt(obj_ptr, ply_ptr)
object		*obj_ptr;
creature	*ply_ptr;
{
	otag	*op, *temp, *prev;

	obj_ptr->parent_crt = ply_ptr;
	obj_ptr->parent_obj = 0;
	obj_ptr->parent_rom = 0;

	op = (otag *)malloc(sizeof(otag));
	if(!op)
		merror("add_obj_crt", FATAL);
	op->obj = obj_ptr;
	op->next_tag = 0;

	if(!ply_ptr->first_obj) {
		ply_ptr->first_obj = op;
		return;
	}

	temp = ply_ptr->first_obj;
	if(strcmp(temp->obj->name, obj_ptr->name) > 0 ||
	   (!strcmp(temp->obj->name, obj_ptr->name) &&
	   temp->obj->adjustment > obj_ptr->adjustment)) {
		op->next_tag = temp;
		ply_ptr->first_obj = op;
		return;
	}

	while(temp) {
		if(strcmp(temp->obj->name, obj_ptr->name) > 0 ||
		   (!strcmp(temp->obj->name, obj_ptr->name) &&
		   temp->obj->adjustment > obj_ptr->adjustment))
			break;
		prev = temp;
		temp = temp->next_tag;
	}
	op->next_tag = prev->next_tag;
	prev->next_tag = op;

}

/**********************************************************************/
/*              del_obj_crt                                           */
/**********************************************************************/

/* This function removes the object pointer to by the first parameter */
/* from the player pointed to by the second.			      */

void del_obj_crt(obj_ptr, ply_ptr)
object		*obj_ptr;
creature	*ply_ptr;
{
	otag 	*temp, *prev;

	if(!obj_ptr->parent_crt) {
		ply_ptr->ready[obj_ptr->wearflag-1] = 0;
		return;
	}

	obj_ptr->parent_crt = 0;
	if(ply_ptr->first_obj->obj == obj_ptr) {
		temp = ply_ptr->first_obj->next_tag;
		free(ply_ptr->first_obj);
		ply_ptr->first_obj = temp;
		return;
	}

	prev = ply_ptr->first_obj;
	temp = prev->next_tag;
	while(temp) {
		if(temp->obj == obj_ptr) {
			prev->next_tag = temp->next_tag;
			free(temp);
			return;
		}
		prev = temp;
		temp = temp->next_tag;
	}
}

/**********************************************************************/
/*				title_ply			      */
/**********************************************************************/

/* This function returns a string with the player's character title in */
/* it.  The title is determined by looking at the player's class and   */
/* level.							       */

char *title_ply(show_ply_ptr,ply_ptr)
creature   *show_ply_ptr;
creature	*ply_ptr;
{
	int	titlnum;

        /* 자신이 칭호 선택할 수 있게 함 . ply_titles에 저장 */
        if(ply_titles[ply_ptr->fd]!=NULL) {
            return (ansi_title(show_ply_ptr,ply_titles[ply_ptr->fd]));
        }

	titlnum = (((ply_ptr->level+3)/4)-1)/3;
	if(titlnum > 7)
		titlnum = 7;
	return(lev_title[ply_ptr->class][titlnum]);
}

/**********************************************************************/
/*				compute_ac			      */
/**********************************************************************/

/* This function computes a player's (or a monster's) armor class by  */
/* examining its stats and the items it is holding.		      */

void compute_ac(ply_ptr)
creature	*ply_ptr;
{
	int	ac, i;

	ac = 100;

        if(ply_ptr->dexterity>63) ac -= 5*bonus[63];
	else                      ac -= 5*bonus[ply_ptr->dexterity];

	for(i=0; i<MAXWEAR; i++)
		if(ply_ptr->ready[i])
			ac -= ply_ptr->ready[i]->armor;

	if(F_ISSET(ply_ptr, PPROTE))
		ac -= 10;

	ac = MAX(-127,MIN(127,ac));

	ply_ptr->armor = ac;
}


/**********************************************************************/
/*				compute_thaco			      */
/**********************************************************************/

/* This function computes a player's THAC0 by looking at his class, */
/* level, weapon adjustment and strength bonuses.		    */

void compute_thaco(ply_ptr)
creature	*ply_ptr;
{
	int	thaco, n;

	n = ((ply_ptr->level+3)/4) > 20 ? 19:((ply_ptr->level+3)/4)-1;

	thaco = thaco_list[ply_ptr->class][n];
	if(ply_ptr->ready[WIELD-1])
		thaco -= ply_ptr->ready[WIELD-1]->adjustment;
	thaco -= mod_profic(ply_ptr);
 	thaco -= bonus[ply_ptr->strength];

	if(ply_ptr->class < CARETAKER) {
		if(ply_ptr->level < 101) 
			ply_ptr->thaco = MAX(0, thaco);
		else ply_ptr->thaco = MAX(-5, thaco);
	}
	else ply_ptr->thaco = MAX(-10, thaco);

	if(F_ISSET(ply_ptr, PBLESS))
		thaco -= 3;

}

/**********************************************************************/
/*				mod_profic			      */
/**********************************************************************/

/* This function returns the player's currently-used proficiency div 20 */

int mod_profic(ply_ptr)
creature	*ply_ptr;
{
	int	amt;
	switch(ply_ptr->class) {
		case FIGHTER:
		case BARBARIAN:
              case INVINCIBLE:
              case CARETAKER:
			amt = 20;
			break;
		case RANGER:
		case PALADIN:
			amt = 25;
			break;
		case THIEF:
		case ASSASSIN:
		case CLERIC:
			amt = 30;
			break;
		default:
			amt = 40;
			break;
	}

	if(ply_ptr->ready[WIELD-1] && 
		ply_ptr->ready[WIELD-1]->type <= MISSILE)
		return(profic(ply_ptr, ply_ptr->ready[WIELD-1]->type)/amt);
	else
		return(profic(ply_ptr, BLUNT)/amt);
}

/**********************************************************************/
/*				weight_ply			      */
/**********************************************************************/

/* This function calculates the total weight that a player (or monster) */
/* is carrying in his inventory.					*/

int weight_ply(ply_ptr)
creature	*ply_ptr;
{
	int	i, n = 0;
	otag	*op;

	op = ply_ptr->first_obj;
	while(op) {
		if(!F_ISSET(op->obj, OWTLES))
			n += weight_obj(op->obj);
		op = op->next_tag;
	}

	for(i=0; i<MAXWEAR; i++)
		if(ply_ptr->ready[i])
			n += weight_obj(ply_ptr->ready[i]);

	return(n);

}

/**********************************************************************/
/*				max_weight			      */
/**********************************************************************/

/* This function returns the maximum weight a player can be allowed to */
/* hold in his inventory.					       */

int max_weight(ply_ptr)
creature	*ply_ptr;
{
	int	n;

	n = 20 + ply_ptr->strength*10;
	if(ply_ptr->class == BARBARIAN)
		n += ((ply_ptr->level+3)/4)*10;

	return(n);
}
	
/**********************************************************************/
/*				profic				      */
/**********************************************************************/

/* This function determines a weapons proficiency percentage.  The first */
/* parameter contains a pointer to the player whose percentage is being  */
/* determined.  The second is an integer containing the proficiency      */
/* number.								 */

int profic(ply_ptr, index)
creature	*ply_ptr;
int		index;
{
	long	prof_array[12];
	int	i, n, prof;

	switch (ply_ptr->class){
	case FIGHTER:
      case INVINCIBLE:
      case CARETAKER:
      case SUB_DM:
      case DM:
		prof_array[0] = 0L;		prof_array[1] = 768L;
		prof_array[2] = 1024L;		prof_array[3] = 1440L;
		prof_array[4] = 1910L;		prof_array[5] = 16000L;
		prof_array[6] = 31214L;		prof_array[7] = 167000L;
		prof_array[8] = 268488L;	prof_array[9] = 695000L;
		prof_array[10] = 934808L;
		prof_array[11] = 500000000L;
		break;

	case BARBARIAN: 
                prof_array[0] = 0L;             prof_array[1] = 1536L;
                prof_array[2] = 2048L;          prof_array[3] = 2880L;
                prof_array[4] = 3820L;          prof_array[5] = 32000L;
                prof_array[6] = 62428L;         prof_array[7] = 334000L;
                prof_array[8] = 536976L;        prof_array[9] = 1390000L;
                prof_array[10] = 1869616L;
		prof_array[11] = 500000000L;
		break;

        case THIEF:
	case RANGER:
                prof_array[0] = 0L;             prof_array[1] = 2304L;
                prof_array[2] = 3072L;          prof_array[3] = 4320L;
                prof_array[4] = 5730L;          prof_array[5] = 48000L;
                prof_array[6] = 93642L;         prof_array[7] = 501000L;
                prof_array[8] = 805464L;        prof_array[9] = 2085000L;
                prof_array[10] = 2804424L;
		prof_array[11] = 500000000L;
		break;

	case CLERIC:
        case PALADIN:
	case ASSASSIN:
                prof_array[0] = 0L;             prof_array[1] = 3072L;
                prof_array[2] = 4096L;          prof_array[3] = 5076L;
                prof_array[4] = 7640L;          prof_array[5] = 64000L;
                prof_array[6] = 124856L;        prof_array[7] = 668000L;
                prof_array[8] = 1073952L;       prof_array[9] = 2780000L;
                prof_array[10] = 3939232L;
		prof_array[11] = 500000000L;
		break;

	case MAGE:
                prof_array[0] = 0L;             prof_array[1] = 5376L;
                prof_array[2] = 7168L;          prof_array[3] = 10080L;
                prof_array[4] = 13370L;         prof_array[5] = 112000L;
                prof_array[6] = 218498L;        prof_array[7] = 1169000L;
                prof_array[8] = 1879416L;       prof_array[9] = 4865000L;
                prof_array[10] = 6543656L;
		prof_array[11] = 500000000L;
		break;
	}
	
	n = ply_ptr->proficiency[index];
	for(i=0; i<11; i++)
		if(n < prof_array[i+1]) {
			prof = 10*i;
			break;
		}

	prof += ((n - prof_array[i])*10) / (prof_array[i+1] - prof_array[i]);

	return(prof);
}

/************************************************************************/
/*				mprofic					*/
/************************************************************************/

/* This function returns the magical realm proficiency as a percentage	*/

int mprofic(ply_ptr, index)
creature	*ply_ptr;
int		index;
{
	long	prof_array[12];
	int	i, n, prof;

        switch(ply_ptr->class){
        case MAGE:
        case INVINCIBLE:
        case CARETAKER:
        case SUB_DM:
        case DM:
                prof_array[0] = 0L;       prof_array[1] = 1024L;
                prof_array[2] = 2048L;    prof_array[3] = 4096L;
                prof_array[4] = 8192L;    prof_array[5] = 16384L;
                prof_array[6] = 35768L;   prof_array[7] = 85536L;
                prof_array[8] = 140000L;  prof_array[9] = 459410L;
                prof_array[10] = 2073306L; prof_array[11] = 500000000L;
                break;
        case CLERIC:
                prof_array[0] = 0L;       prof_array[1] = 1024L;
                prof_array[2] = 4092L;    prof_array[3] = 8192L;
                prof_array[4] = 16384L;   prof_array[5] = 32768L;
                prof_array[6] = 70536L;   prof_array[7] = 119000L;
                prof_array[8] = 226410L;  prof_array[9] = 709410L;
                prof_array[10] = 2973307L; prof_array[11] = 500000000L;
                break;
        case PALADIN:
        case RANGER:    
                prof_array[0] = 0L;       prof_array[1] = 1024L;
                prof_array[2] = 8192L;    prof_array[3] = 16384L;
                prof_array[4] = 32768L;   prof_array[5] = 65536L;
                prof_array[6] = 105000L;  prof_array[7] = 165410L;
                prof_array[8] = 287306L;  prof_array[9] = 809410L;
                prof_array[10] = 3538232L; prof_array[11] = 500000000L;
                break;  
        default:
                prof_array[0] = 0L;       prof_array[1] = 1024L;
                prof_array[2] = 40000L;   prof_array[3] = 80000L;
                prof_array[4] = 120000L;  prof_array[5] = 160000L;
                prof_array[6] = 205000L;  prof_array[7] = 222000L;
                prof_array[8] = 380000L;  prof_array[9] = 965410L;
                prof_array[10] = 5495000; prof_array[11] = 500000000L;
                break;
        } 

	n = ply_ptr->realm[index-1];
	for(i=0; i<11; i++)
		if(n < prof_array[i+1]) {
			prof = 10*i;
			break;
		}

	prof += ((n - prof_array[i])*10) / (prof_array[i+1] - prof_array[i]);

	return(prof);
}

/**********************************************************************/
/*				fall_ply			      */
/**********************************************************************/

/* This function computes a player's bonus (or susceptibility) to falling */
/* while climbing.							  */

int fall_ply(ply_ptr)
creature	*ply_ptr;
{
	int	fall, j;

	fall = bonus[ply_ptr->dexterity]*5;
	for(j=0; j<MAXWEAR; j++)
		if(ply_ptr->ready[j])
			if(F_ISSET(ply_ptr->ready[j], OCLIMB))
				fall += ply_ptr->ready[j]->pdice*3;
	return(fall);
}

/**********************************************************************/
/*				find_who			      */
/**********************************************************************/

/* This function searches through the players who are currently logged */
/* on for a given player name.  If that player is on, a pointer to him */
/* is returned.							       */

creature *find_who(name)
char	*name;
{
	int i;

	for(i=0; i<Tablesize; i++) {
		if(!Ply[i].ply || !Ply[i].io || Ply[i].ply->fd < 0) continue;
		if(!strcmp(Ply[i].ply->name, name))
			return(Ply[i].ply);
	}

	return(0);

}

/**********************************************************************/
/*				lowest_piety			      */
/**********************************************************************/

/* This function finds the player with the lowest piety in a given room. */
/* The pointer to that player is returned.  In the case of a tie, one of */
/* them is randomly chosen.						 */

creature *lowest_piety(rom_ptr, invis)
room	*rom_ptr;
int	invis;
{
	creature	*ply_ptr = 0;
	ctag		*cp;
	int		totalpiety, pick;

	cp = rom_ptr->first_ply;
	totalpiety = 0;
	if(!cp)
		return(0);

	while(cp) {
		if(F_ISSET(cp->crt, PHIDDN) ||
		   (F_ISSET(cp->crt, PINVIS) && !invis) ||
		   F_ISSET(cp->crt, PDMINV)) {
			cp = cp->next_tag;
			continue;
		}
		totalpiety += MAX(1, (25 - cp->crt->piety));
		cp = cp->next_tag;
	}

	if(!totalpiety)
		return(0);
	pick = mrand(1, totalpiety);

	cp = rom_ptr->first_ply;
	totalpiety = 0;
	while(cp) {
		if(F_ISSET(cp->crt, PHIDDN) ||
		   (F_ISSET(cp->crt, PINVIS) && !invis) ||
		   F_ISSET(cp->crt, PDMINV)) {
			cp = cp->next_tag;
			continue;
		}
		totalpiety += MAX(1, (25 - cp->crt->piety));
		if(totalpiety >= pick) {
			ply_ptr = cp->crt;
			break;
		}
		cp = cp->next_tag;
	}

	return(ply_ptr);
}
/**********************************************************************/
/*                             enemy_ply                              */
/**********************************************************************/
/* 이기능은 적 왕국의 소속의 사용자를 발견하는 루틴이다               */

creature *enemy_ply(rom_ptr, att_ptr, crt_ptr, invis)
room    *rom_ptr;
creature *att_ptr;
creature *crt_ptr;
int	invis;
{
	creature	*ply_ptr = 0;
	ctag		*cp;

	cp = rom_ptr->first_ply;
	if(!cp)
		return(0);

	while(cp) {
		if(F_ISSET(cp->crt, PHIDDN) ||
		   (F_ISSET(cp->crt, PINVIS) && !invis) ||
		   F_ISSET(cp->crt, PDMINV)) {
			cp = cp->next_tag;
			continue;
		}
                else if ( (F_ISSET(cp->crt, PKNDM1) && F_ISSET(crt_ptr, MKNDM2)) ||
                          (F_ISSET(cp->crt, PKNDM2) && F_ISSET(crt_ptr, MKNDM1)) ) {
                        ply_ptr = cp->crt;
                        break;
                }
		cp = cp->next_tag;
	}
	if(!ply_ptr) return att_ptr;
	else return(ply_ptr);
}

/**********************************************************************/
/*				has_light			      */
/**********************************************************************/

/* This function returns true if the player in the first parameter is */
/* holding or wearing anything that generates light.		      */

int has_light(crt_ptr)
creature	*crt_ptr;
{
	int	i, light = 0;

	for(i=0; i<MAXWEAR; i++) {
		if(!crt_ptr->ready[i]) continue;
		if(F_ISSET(crt_ptr->ready[i], OLIGHT)) {
			if((crt_ptr->ready[i]->type == LIGHTSOURCE &&
			   crt_ptr->ready[i]->shotscur > 0) ||
			   crt_ptr->ready[i]->type != LIGHTSOURCE) {
				light = 1;
				break;
			}
		}
	}

	if(F_ISSET(crt_ptr, PLIGHT)) {
		light = 1;
		i = MAXWEAR;
	}

	if(light) 
		return(i+1);
	else 
		return(0);

}

/************************************************************************/
/*				ply_prompt				*/
/************************************************************************/

/* This function returns the prompt that the player should be seeing	*/

char *ply_prompt(ply_ptr)
creature	*ply_ptr;
{
	static char prompt[40];
	int fd;

	fd = ply_ptr->fd;

	if(fd < 0 || F_ISSET(ply_ptr, PSPYON) || F_ISSET(ply_ptr, PREADI))
		prompt[0] = 0;

	else if(F_ISSET(ply_ptr, PPROMP))
        sprintf(prompt, "\n\n(%d 체력 %d 도력): ", ply_ptr->hpcur,
		    ply_ptr->mpcur);

	else
        strcpy(prompt, "\n\n: ");

	return prompt;
}

 
/**********************************************************************/
/*                            low_piety_alg                          */
/**********************************************************************/
/* This function is a varation on the lowest piety function.  The    *
 * searchs the given player list, totally up all the player's piety  *
 * (30 - ply piety), and then randomly picking a player from the     *
 * based on the player's piety.  Players with lower piety have a     *
 * greater chance of being attacked.  The alg, parameter tells the   *
 * whether to ignore a given player alignemnt (alg=1, only consider  *
 * good players, -1 only consider evil players).  The invis param    *
 * tells if the monster can detect invisible. */

creature *low_piety_alg(rom_ptr, invis, alg,lvl)
room    *rom_ptr;
int     invis;
int		alg;
int  	lvl;
{
    creature    *ply_ptr = 0;
    ctag        *cp;
    int        total, pick;
 
        cp = rom_ptr->first_ply;
        total = 0;

       if(!cp)
                return(NULL);
 
        while(cp) {
            if(F_ISSET(cp->crt, PHIDDN) || 
				(F_ISSET(cp->crt, PINVIS) && !invis) || 
				F_ISSET(cp->crt, PDMINV) ||
				(((cp->crt->level+3)/4) < lvl) ||
				((alg == 1) && (cp->crt->alignment > -100)) ||
				((alg == -1) && (cp->crt->alignment < 100))) {
	                cp = cp->next_tag;
                    continue;
         	}
            total += MAX(1, (30 - cp->crt->piety));
            cp = cp->next_tag;
        }
 
        if(!total)
                return(NULL);

        pick = mrand(1, total);
 
        cp = rom_ptr->first_ply;
        total = 0;
        while(cp) {
            if(F_ISSET(cp->crt, PHIDDN) ||
               (F_ISSET(cp->crt, PINVIS) && !invis) ||
				F_ISSET(cp->crt, PDMINV) ||
				((alg == 1) && (cp->crt->alignment > -100)) ||
				((alg == -1) && (cp->crt->alignment < 100))) {
                    cp = cp->next_tag;
                    continue;
                }

            total += MAX(1, (30 - cp->crt->piety));
            if(total >= pick) {
                    ply_ptr = cp->crt;
                    break;
            }
            cp = cp->next_tag;
        }
 
        return(ply_ptr);
}
                        
