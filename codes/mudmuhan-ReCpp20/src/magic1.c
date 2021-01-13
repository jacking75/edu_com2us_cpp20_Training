/*
 * MAGIC1.C:
 *
 *  User routines dealing with magic spells.  Potions, wands,
 *  scrolls, and casting are all covered.
 *
 *  Copyright (C) 1991, 1992, 1993 Brett J. Vickers
 *
 */

#include "mstruct.h"
#include "mextern.h"
#include "board.h"

/**********************************************************************/
/*              cast                      */
/**********************************************************************/

/* This function allows a player to cast a magical spell.  It looks at */
/* the second parsed word to find out if the spell-name is valid, and  */
/* then calls the appropriate spell function.                  */

int cast(ply_ptr, cmnd)
creature    *ply_ptr;
cmd     *cmnd;
{
    long    i, t, z, chance;
    int (*fn)();
    int fd, splno, c = 0, match = 0, n;

    fd = ply_ptr->fd;

    if(cmnd->num < 2) {
        print(fd, "어떤 주술을 펼치실겁니까?");
        return(0);
    }
    if(F_ISSET(ply_ptr, PBLIND)) {
        ANSI(fd, RED);
        print(fd, "아무것도 보이지 않습니다!");
        ANSI(fd, WHITE);
        return(0);
    }
    if(F_ISSET(ply_ptr, PSILNC)) {
        ANSI(fd, YELLOW);
        print(fd, "한마디도 할수 없습니다!");
        ANSI(fd, WHITE);
        return(0);
    }
    do {
        if(!strcmp(cmnd->str[1], spllist[c].splstr)) {
            match = 1;
            splno = c;
            break;
        }
        else if(!strncmp(cmnd->str[1], spllist[c].splstr, 
            strlen(cmnd->str[1]))) {
            match++;
            splno = c;
        }
        c++;
    } while(spllist[c].splno != -1);

    if(match == 0) {
        print(fd, "그런 주문은 존재하지 않습니다.");
        return(0);
    }

    else if(match > 1) {
        print(fd, "펼치실 주문의 이름이 이상하군요.");
        return(0);
    }

    if(F_ISSET(ply_ptr->parent_rom, RNOMAG)) {
        print(fd, "주술을 출수 하는데 실패 하셨습니다.");
        return(0);
    }

    i = LT(ply_ptr, LT_SPELL);
    t = time(0);

    if(t < i) {
        please_wait(fd, i-t);
        return(0);
    }

    F_CLR(ply_ptr, PHIDDN);

    fn = spllist[splno].splfn;

    if(fn == offensive_spell) {
        if(ply_ptr->class==0) {
            print(fd,"당신은 공격주문을 쓸 수 없는 직업을 갖고 있습니다.");
            return 0;
        }
        for(c=0; ospell[c].splno != spllist[splno].splno; c++)
            if(ospell[c].splno == -1) return(0);
        n = (*fn)(ply_ptr, cmnd, CAST, spllist[splno].splstr,
            &ospell[c]);
    }

    else
        n = (*fn)(ply_ptr, cmnd, CAST);

    if(n) {
        ply_ptr->lasttime[LT_SPELL].ltime = t;
        if(ply_ptr->class == CLERIC || ply_ptr->class == MAGE || ply_ptr->class == CARETAKER)
            ply_ptr->lasttime[LT_SPELL].interval = 3;
        else if(ply_ptr->class == DM || ply_ptr->class == SUB_DM)
            ply_ptr->lasttime[LT_SPELL].interval = 0;
        else
            ply_ptr->lasttime[LT_SPELL].interval = 5;
    }

    return(0);

}

/**********************************************************************/
/*              teach                     */
/**********************************************************************/

/* This function allows mages to teach other players the first six    */
/* spells.                                */

int teach(ply_ptr, cmnd)
creature    *ply_ptr;
cmd     *cmnd;
{
    room        *rom_ptr;
    creature    *crt_ptr;
    int     fd, splno, c = 0, match = 0;

    fd = ply_ptr->fd;
    rom_ptr = ply_ptr->parent_rom;

    if(cmnd->num < 3) {
        print(fd, "누구에게 비법을 전수시키실겁니까?");
        return(0);
    }
    if(F_ISSET(ply_ptr, PBLIND)) {
        ANSI(fd, RED);
        print(fd, "아무것도 보이지 않습니다!\n");
        ANSI(fd, WHITE);
        return(0);
    }
    if(F_ISSET(ply_ptr, PSILNC)) {
        ANSI(fd, YELLOW);
        print(fd, "당신은 한마디도 할수 없습니다!\n");
        ANSI(fd, WHITE);
        return(0);
    }

    if(ply_ptr->class != CARETAKER && ply_ptr->class  !=  MAGE  && ply_ptr->class  !=  CLERIC)  {
        print(fd, "\n도술사와 불제자만이 전수시킬 수 있는 능력이 있습니다.\n");
        return(0);
    }

    cmnd->str[1][0] = up(cmnd->str[1][0]);
    crt_ptr = find_crt(ply_ptr, rom_ptr->first_ply, cmnd->str[1], 
               cmnd->val[1]);

    if(!crt_ptr) {
        print(fd, "\n그런 사람은 존재하지 않습니다.\n");
        return(0);
    }

    do {
        if(!strcmp(cmnd->str[2], spllist[c].splstr)) {
            match = 1;
            splno = c;
            break;
        }
        else if(!strncmp(cmnd->str[2], spllist[c].splstr, 
            strlen(cmnd->str[2]))) {
            match++;
            splno = c;
        }
        c++;
    } while(spllist[c].splno != -1);

    if(match == 0) {
        print(fd, "\n그런 주문이 존재하지 않습니다.\n");
        return(0);
    }

    else if(match > 1) {
        print(fd, "\n주문이름이 이상합니다.\n");
        return(0);
    }

    if(!S_ISSET(ply_ptr, spllist[splno].splno)) {
        print(fd, "\n당신은 아직 그런 주문을 터득하지 못했습니다.\n");
        return(0);
    }

/*
    if((spllist[splno].splno  >  3)  &&  (ply_ptr->class  !=  CLERIC  &&  ply_ptr->class  < 
CARETAKER)) {
        print(fd, "\n그 주문을 다른 사람에게 전수시킬 수 없습니다.\n");
        return(0);
    }

    if((spllist[splno].splno > 5) && (ply_ptr->class < CARETAKER)) {
        print(fd, "\n그 주문을 다른 사람에게 전수시킬 수 없습니다.\n");
        return(0);
    }

    if((spllist[splno].splno  <  3)   &&  (ply_ptr->class  !=  MAGE  &&  ply_ptr->class   < 
CARETAKER)) {
        print(fd, "\n그 주문을 다른 사람에게 전수시킬 수 없습니다.\n");
        return(0);
    }
    */

    if((spllist[splno].spllv == 1) && (ply_ptr->class != CLERIC && ply_ptr->class < INVINCIBLE)) {
    	print(fd, "\n그 주문을 다른 사람에게 전수시킬 수 없습니다.\n");
    	return(0);
    }
    if((spllist[splno].spllv == 2) && (ply_ptr->class != MAGE && ply_ptr->class < INVINCIBLE)) {
    	print(fd, "\n그 주문을 다른 사람에게 전수시킬 수 없습니다.\n");
    	return(0);
    }
    if((spllist[splno].spllv == 3) && (ply_ptr->class < INVINCIBLE )) {
    	print(fd, "\n그 주문을 다른 사람에게 전수시킬 수 없습니다.\n");
    	return(0);
    }
    if((spllist[splno].spllv == 4) && (ply_ptr->class < CARETAKER )) {
    	print(fd, "\n그 주문을 다른 사람에게 전수시킬 수 없습니다.\n");
    	return(0);
    }
    if((spllist[splno].spllv == 5) && (ply_ptr->class < SUB_DM )) {
    	print(fd, "\n그 주문을 다른 사람에게 전수시킬 수 없습니다.\n");
    	return(0);
    }

    F_CLR(ply_ptr, PHIDDN);

    S_SET(crt_ptr, spllist[splno].splno);
    print(crt_ptr->fd, "\n%M이 %s 주술의 시범을 보이며 주문 전수를 시킵니다.\n오옷~~~ 이 주문을 외우자 주위에 이상한 기운이 모이는 것이 그 \n그 사람에게 상당한 도움이 될 것 같습니다.\n", ply_ptr,
          spllist[splno].splstr);
    print(fd, "\n%s 주술을 %M에게 시범을 보이며 주문 전수를 시킵니다.\n오옷~~~ 이 주문을 외우자 주위에 이상한 기운이 모이는 것이\n그 사람에게 상당한 도움이 될 것 같습니다.\n", spllist[splno].splstr,
          crt_ptr);

    broadcast_rom2(fd, crt_ptr->fd, rom_ptr->rom_num,
               "\n%M이 %M에게 %s 주술의 시범을 보이며 주문 전수를 \n시킵니다.\n오옷~~~ 이 주문을 외우자 주위에 이상한 기운이 모이는 것이\n그 사람에게 상당한 도움이 될 것 같습니다.\n", ply_ptr, crt_ptr,
               spllist[splno].splstr);

    return(0);

}

/**********************************************************************/
/*              study                     */
/**********************************************************************/

/* This function allows a player to study a scroll, and learn the spell */
/* that is on it.                           */

int study(ply_ptr, cmnd)
creature    *ply_ptr;
cmd     *cmnd;
{
    object  *obj_ptr;
    int fd, n, match=0;

    fd = ply_ptr->fd;

    if(cmnd->num < 2) {
        print(fd, "\n무엇을 연마하실려고요?\n");
        return(0);
    }
    if(F_ISSET(ply_ptr, PBLIND)) {
        ANSI(fd, RED);
        print(fd, "\n당신의 능력으로는 이 비법서를 연마할 수 없습니다.\n");
        ANSI(fd, WHITE);
        return(0);
    }

    obj_ptr = find_obj(ply_ptr, ply_ptr->first_obj,
               cmnd->str[1], cmnd->val[1]);

    if(!obj_ptr || !cmnd->val[1]) {
        for(n=0; n<MAXWEAR; n++) {
            if(!ply_ptr->ready[n]) continue;
            if(EQUAL(ply_ptr->ready[n], cmnd->str[1]))
                match++;
            else continue;
            if(match == cmnd->val[1] || !cmnd->val[1]) {
                obj_ptr = ply_ptr->ready[n];
                break;
            }
        }
    }

    if(!obj_ptr) {
        print(fd, "\n그런 것을 소지하고 있지 않습니다.\n");
        return(0);
    }

    if(obj_ptr->type != SCROLL) {
        print(fd, "\n이것은 비법서가 아닙니다.\n");
        return(0);
    }
 /* 주문 배울 레벨 제한 */

        if (obj_ptr->ndice > ply_ptr->level) {
                print(fd, "\n당신의 능력으로는 %i의 내용을 파악하지 못해 연마할 수 없습니다.",obj_ptr);
        return(0);
    }


    if((F_ISSET(obj_ptr, OGOODO) && ply_ptr->alignment < -100) ||
       (F_ISSET(obj_ptr, OEVILO) && ply_ptr->alignment > 100)) {
        print(fd, "\n연마를 끝마치자 %I의 형체가 화염에 휩싸이며 어디론가 사라져\n 버렸습니다.\n", obj_ptr);
        del_obj_crt(obj_ptr, ply_ptr);
        add_obj_rom(obj_ptr, ply_ptr->parent_rom);
        return(0);
    }
     

  if(F_ISSET(obj_ptr,OCLSEL))
        if(!F_ISSET(obj_ptr,OCLSEL    +   ply_ptr->class)    &&   (    ply_ptr->class   < 
CARETAKER)){
                print(fd, "\n당신의 직업상 %i의 비법을 연마할 수 없습니다.\n",obj_ptr);
                return(0);
        }                 

    F_CLR(ply_ptr, PHIDDN);

    print(fd, "당신은 %s의 내용을 알아내고 연마하기 시작합니다.\n연마를 해 나감에 따라 몸안에서 이상한 기운이 퍼져 나가는\n것이 느껴집니다.\n이야야~~~~~얍 그 기운이 안정되면서 완전히 당신의 것이 \n되었습니다.\n", 
          spllist[obj_ptr->magicpower-1].splstr);
    print(fd, "\n연마를 마치자 %I의 형체에 화염이 휩싸이며 어디론가 사라져 버렸습니다.\n", obj_ptr);
    broadcast_rom(fd, ply_ptr->rom_num, "\n%M이 %1i의 내용을 읽고 연마합니다.",
              ply_ptr, obj_ptr);

    S_SET(ply_ptr, obj_ptr->magicpower-1);
    del_obj_crt(obj_ptr, ply_ptr);
    free_obj(obj_ptr);

    return(0);

}

/**********************************************************************/
/*              readscroll                */
/**********************************************************************/

/* This function allows a player to read a scroll and cause its magical */
/* spell to be cast.  If a third word is used in the command, then that */
/* player or monster will be the target of the spell.           */

int readscroll(ply_ptr, cmnd)
creature    *ply_ptr;
cmd     *cmnd;
{
    object  *obj_ptr;
    int (*fn)();
    long    i, t;
    int fd, n, match=0, c, splno;

    fd = ply_ptr->fd;
    fn = 0;

    if(cmnd->num < 2) {
        if(cmnd->val[0]>1) {
            obj_ptr = find_obj(ply_ptr,ply_ptr->parent_rom->first_obj,
                           "게시판",1);
            if(!obj_ptr) {
                print(fd,"무엇을 읽습니까?");
                return 0;
            }
            board_obj[fd]=obj_ptr;
            return(read_board(ply_ptr,cmnd));
        }
        print(fd, "무엇을 읽습니까?\n");
        return(0);
    }
    if(F_ISSET(ply_ptr, PBLIND)) {
        ANSI(fd, RED);
        print(fd, "\n당신은 그것을 읽을 수 있는 능력이 없습니다.\n");
        ANSI(fd, WHITE);
        return(0);
    }
    obj_ptr = find_obj(ply_ptr, ply_ptr->first_obj,
               cmnd->str[1], cmnd->val[1]);

    if(!obj_ptr || !cmnd->val[1]) {
        for(n=0; n<MAXWEAR; n++) {
            if(!ply_ptr->ready[n]) continue;
            if(EQUAL(ply_ptr->ready[n], cmnd->str[1]))
                match++;
            else continue;
            if(match == cmnd->val[1] || !cmnd->val[1]) {
                obj_ptr = ply_ptr->ready[n];
                break;
            }
        }
    }

    if(!obj_ptr) {
        print(fd, "\n 그런것이 존재하지 않습니다.\n");
        return(0);
    }

    if(obj_ptr->special) {
        n = special_obj(ply_ptr, cmnd, SP_MAPSC);
        if(n != -2) return(MAX(0, n));
    }

    if(obj_ptr->type != SCROLL) {
        print(fd, "\n이것은 문서구가 아닙니다.\n");
        return(0);
    }
 /* 주문 배울 레벨 제한 */

        if (obj_ptr->ndice > ply_ptr->level) {
                print(fd, "\n당신의 능력으로는 %i의 내용을 파악하지 못해 연마할 수 없습니다.",obj_ptr);
        return(0);
    }

    if((F_ISSET(obj_ptr, OGOODO) && ply_ptr->alignment < -100) ||
       (F_ISSET(obj_ptr, OEVILO) && ply_ptr->alignment > 100)) {
        print(fd, "\n모든 것을 읽고 나자 %I의 형체가 먼지로 변하면서 바람과 함께 사라져 버렸습니다.\n", obj_ptr);
        del_obj_crt(obj_ptr, ply_ptr);
        add_obj_rom(obj_ptr, ply_ptr->parent_rom);
        return(0);
    }



  if(F_ISSET(obj_ptr,OCLSEL))
        if(!F_ISSET(obj_ptr,OCLSEL    +   ply_ptr->class)    &&   (    ply_ptr->class   < 
CARETAKER)){
                print(fd, "\n이것은 당신의 직업에서 금하는 금서이기 때문에 내용을 읽을 수 없습니다.\n",obj_ptr);
                return(0);
        }                 

    if(F_ISSET(ply_ptr->parent_rom, RNOMAG) || (obj_ptr->magicpower-1 < 0)) {
        print(fd, "\n아무런 일도 일어나지 않았습니다.\n");
        return(0);
    }

    i = LT(ply_ptr, LT_READS);
    t = time(0);

    if(i > t) {
        please_wait(fd, i-t);
        return(0);
    }

    F_CLR(ply_ptr, PHIDDN);

    ply_ptr->lasttime[LT_READS].ltime = t;
    ply_ptr->lasttime[LT_READS].interval = 3;

    if(spell_fail(ply_ptr)){
        print(fd, "\n모든 것을 읽고 나자 %I의 형체가 먼지로 변하면서 바람과 함께 사라져 버렸습니다.\n", obj_ptr);
        del_obj_crt(obj_ptr, ply_ptr);
        free_obj(obj_ptr);
        return(0);
    }

    splno = obj_ptr->magicpower-1;
    fn = spllist[splno].splfn;

    if(fn == offensive_spell) {
        for(c=0; ospell[c].splno != spllist[splno].splno; c++)
            if(ospell[c].splno == -1) return(0);
        n = (*fn)(ply_ptr, cmnd, SCROLL, spllist[splno].splstr,
            &ospell[c]);
    }

    else
        n = (*fn)(ply_ptr, cmnd, SCROLL);

    if(n) {
        if(obj_ptr->use_output[0])
            print(fd, "%s\n", obj_ptr->use_output);

        print(fd, "\n모든 것을 읽고 나자 %I의 형체가 먼지로 변하면서 바람과 함께 사라져 버렸습니다.\n", obj_ptr);
        del_obj_crt(obj_ptr, ply_ptr);
        free_obj(obj_ptr);
    }

    return(0);

}

/**********************************************************************/
/*              drink                     */
/**********************************************************************/

/* This function allows players to drink potions, thereby casting any */
/* spell it was meant to contain.                     */

int drink(ply_ptr, cmnd)
creature    *ply_ptr;
cmd     *cmnd;
{
    object  *obj_ptr;
    room    *rom_ptr;
    int (*fn)();
    long    i, t;
    int fd, n, match=0, c, splno;
    int pstat, total_st;

    fd = ply_ptr->fd;
    fn = 0;

    if(cmnd->num < 2) {
        print(fd, "\n무엇을 먹습니까?\n");
        return(0);
    }

    obj_ptr = find_obj(ply_ptr, ply_ptr->first_obj,
               cmnd->str[1], cmnd->val[1]);

    if(!obj_ptr || !cmnd->val[1]) {
        for(n=0; n<MAXWEAR; n++) {
            if(!ply_ptr->ready[n]) continue;
            if(EQUAL(ply_ptr->ready[n], cmnd->str[1]))
                match++;
            else continue;
            if(match == cmnd->val[1] || !cmnd->val[1]) {
                obj_ptr = ply_ptr->ready[n];
                break;
            }
        }
    }

    if(!obj_ptr) {
        print(fd, "\n그런것은 존재하지 않습니다.\n");
        return(0);
    }

    if(obj_ptr->type != POTION) {
        print(fd, "\n이것은 먹는 물건이 아닙니다.\n");
        return(0);
    }
	if(F_ISSET(obj_ptr, OSPECI)) {
		switch(obj_ptr->pdice) {
		case 1 :
			if(ply_ptr->lasttime[LT_HOURS].interval < 86400L)
 				ply_ptr->lasttime[LT_HOURS].interval = 0;
			else    ply_ptr->lasttime[LT_HOURS].interval -= 86400L;
			break;
		case 2 :
			if(F_ISSET(ply_ptr, PCHAOS)) F_CLR(ply_ptr, PCHAOS);
			else	F_SET(ply_ptr, PCHAOS);
			break;
		case 3 :
			ply_ptr->lasttime[LT_HOURS].interval += 86400L;
			break;
		case 4:
			pstat = ply_ptr->strength + ply_ptr->dexterity +
				ply_ptr->constitution + ply_ptr->piety +
				ply_ptr->intelligence;
			total_st = 54 + ply_ptr->level/4;
			if(ply_ptr->class > THIEF) total_st += 25;
			if(F_ISSET(ply_ptr, PHASTE)) total_st += 15;
			if(F_ISSET(ply_ptr, PPOWER)) total_st += 3;
			if(F_ISSET(ply_ptr, PMEDIT)) total_st += 3;
			if(F_ISSET(ply_ptr, PPRAYD)) total_st += 3;

			if(pstat > total_st + 3) {
				break;
			}
			else {
				ply_ptr->strength += 1;
				ply_ptr->intelligence += 1;
				ply_ptr->piety += 1;
				ply_ptr->constitution += 1;
				ply_ptr->dexterity += 1;
				break;
			}
		case 5:
                if(0>load_rom(obj_ptr->value, &rom_ptr)) break;
                if(ply_ptr->level > rom_ptr->hilevel && rom_ptr->hilevel) break;
                if(ply_ptr->level < rom_ptr->lolevel && rom_ptr->lolevel) break;
                del_ply_rom(ply_ptr, ply_ptr->parent_rom);
                add_ply_rom(ply_ptr, rom_ptr);
                break;
		case 6:
				F_SET(ply_ptr, PPOISN);
				break;
		}
        	if(obj_ptr->use_output[0])
        		print(fd, "%s\n", obj_ptr->use_output);
        	print(fd, "당신은 %1i을 먹었습니다.\n", obj_ptr);
        	broadcast_rom(fd, ply_ptr->rom_num, "\n%M이 %1i을 먹었습니다.", 
		ply_ptr, obj_ptr);
            if(obj_ptr->pdice == 5) {
                obj_ptr->shotscur--;
            } 
            if((obj_ptr->pdice != 5) || obj_ptr->shotscur < 1) {
                del_obj_crt(obj_ptr, ply_ptr);
                free_obj(obj_ptr);
            }    
	return(0);
	}

    if(obj_ptr->shotscur < 1 || (obj_ptr->magicpower-1 < 0)) {
        print(fd, "\n아무것도 들어있지 않습니다.\n\n");
        return(0);
    }

        if(F_ISSET(ply_ptr->parent_rom, RNOPOT)){
        print(fd, "\n그것을 먹기전에 인조가 나타나 훔쳐가 버렸습니다.\n 잘먹겠다... 낄낄낄... \n",obj_ptr);
        return(0);
        }
		if(F_ISSET(ply_ptr->parent_rom, RSUVIV)) {
		print(fd, "\n대련장에서는 아무 것도 먹을 수 없습니다.\n");
		return(0);
		}
 
    if((F_ISSET(obj_ptr, OGOODO) && ply_ptr->alignment < -100) ||
       (F_ISSET(obj_ptr, OEVILO) && ply_ptr->alignment > 100)) {
        print(fd, "%I가 당신의 손안에서 타버립니다.\n", obj_ptr);
        del_obj_crt(obj_ptr, ply_ptr);
        add_obj_rom(obj_ptr, ply_ptr->parent_rom);
        return(0);
    }


  if(F_ISSET(obj_ptr,OCLSEL))
        if(!F_ISSET(obj_ptr,OCLSEL    +   ply_ptr->class)    &&   (    ply_ptr->class   < 
CARETAKER)){
                print(fd, "\n당신직업상 그 물건을 금하고 있기 때문에 먹을 수 없습니다.\n");
                return(0);
        }                 

    F_CLR(ply_ptr, PHIDDN);

    splno = obj_ptr->magicpower-1;
    fn = spllist[splno].splfn;

    if(fn == offensive_spell) {
        for(c=0; ospell[c].splno != spllist[splno].splno; c++)
            if(ospell[c].splno == -1) return(0);
        n = (*fn)(ply_ptr, cmnd, POTION, spllist[splno].splstr,
            &ospell[c]);
    }

    else
        n = (*fn)(ply_ptr, cmnd, POTION);

    if(n) {
        if(obj_ptr->use_output[0])
            print(fd, "%s\n", obj_ptr->use_output);

        print(fd, "\n당신은 %1i을 먹었습니다.\n", obj_ptr);
        broadcast_rom(fd, ply_ptr->rom_num, "\n%M이 %1i을 먹었습니다.", 
                  ply_ptr, obj_ptr);

        if(--obj_ptr->shotscur < 1) {
   /*         print(fd, "\n다 먹고 남은 %I의 찌꺼기를 버렸습니다.\n", obj_ptr); */
            del_obj_crt(obj_ptr, ply_ptr);
            free_obj(obj_ptr);
        }
    }

    return(0);

}

/**********************************************************************/
/*              zap                   */
/**********************************************************************/

/* This function allows players to zap a wand or staff at another player */
/* or monster.                               */

int zap(ply_ptr, cmnd)
creature    *ply_ptr;
cmd     *cmnd;
{
    object  *obj_ptr;
    long    i, t;
    int fd, n, match=0;

    fd = ply_ptr->fd;

    if(cmnd->num < 2) {
        print(fd, "\n무엇을 사용합니까?\n");
        return(0);
    }
    if(F_ISSET(ply_ptr, PBLIND)) {
        ANSI(fd, RED);
        print(fd, "아무 것도 보이지 않습니다!\n");
        ANSI(fd, WHITE);
        return(0);
    }

    obj_ptr = find_obj(ply_ptr, ply_ptr->first_obj,
               cmnd->str[1], cmnd->val[1]);

    if(!obj_ptr || !cmnd->val[1]) {
        for(n=0; n<MAXWEAR; n++) {
            if(!ply_ptr->ready[n]) continue;
            if(EQUAL(ply_ptr->ready[n], cmnd->str[1]))
                match++;
            else continue;
            if(match == cmnd->val[1] || !cmnd->val[1]) {
                obj_ptr = ply_ptr->ready[n];
                break;
            }
        }
    }

    if(!obj_ptr) {
        print(fd, "\n그런것이 존재하지 않습니다.\n");
        return(0);
    }

    if(obj_ptr->type != WAND) {
        print(fd, "\n막대나 지팡이가 아닙니다.\n");
        return(0);
    }

    if(obj_ptr->shotscur < 1) {
        print(fd, "\n모두 써버렸습니다.\n");
        return(0);
    }

    if((F_ISSET(obj_ptr, OGOODO) && ply_ptr->alignment < -100) ||
       (F_ISSET(obj_ptr, OEVILO) && ply_ptr->alignment > 100)) {
        print(fd, "\n%I의 수명이 다한 듯 수증기처럼 증발해 버렸습니다.\n", obj_ptr);
        del_obj_crt(obj_ptr, ply_ptr);
        add_obj_rom(obj_ptr, ply_ptr->parent_rom);
        return(0);
    }



  if(F_ISSET(obj_ptr,OCLSEL))
        if(!F_ISSET(obj_ptr,OCLSEL    +   ply_ptr->class)    &&   (    ply_ptr->class   < 
CARETAKER)){
                print(fd, "\n당신직업세계에서 금하는 물건이기에 사용할 수 없습니다.\n");
                return(0);
        }                 


    if(F_ISSET(ply_ptr->parent_rom, RNOMAG) || (obj_ptr->magicpower < 1)) {
        print(fd, "\n아무런 일도 일어나지 않습니다.\n");
        return(0);
    }

    i = LT(ply_ptr, LT_SPELL);
    t = time(0);

    if(i > t) {
        please_wait(fd, i-t);
        return(0);
    }

    F_CLR(ply_ptr, PHIDDN);

    ply_ptr->lasttime[LT_SPELL].ltime = t;
    ply_ptr->lasttime[LT_SPELL].interval = 3;

    if(spell_fail(ply_ptr)){
        obj_ptr->shotscur--;
        return(0);
    }

    return(zap_obj(ply_ptr, obj_ptr, cmnd));
}

/************************************************************************/
/*              zap_obj                 */
/************************************************************************/

/* This function is a subfunction of zap that accepts a player and  */
/* an object as its parameters.                     */

int zap_obj(ply_ptr, obj_ptr, cmnd)
creature    *ply_ptr;
object      *obj_ptr;
cmd     *cmnd;
{
    int splno, c, fd, n;
    int (*fn)();

    fd = ply_ptr->fd;
    splno = obj_ptr->magicpower-1;
    if (splno < 0) 
        return(0);
    fn = spllist[splno].splfn;

    if(fn == offensive_spell) {
        for(c=0; ospell[c].splno != spllist[splno].splno; c++)
            if(ospell[c].splno == -1) return(0);
        n = (*fn)(ply_ptr, cmnd, WAND, spllist[splno].splstr,
            &ospell[c]);
    }
    else
        if (!F_ISSET(obj_ptr,ODDICE))
            n = (*fn)(ply_ptr, cmnd, WAND);
        else
            n = (*fn)(ply_ptr, cmnd, WAND, obj_ptr);

    if(n) {
        if(obj_ptr->use_output[0])
            print(fd, "%s\n", obj_ptr->use_output);

        obj_ptr->shotscur--;
    }

    return(0);

}

/************************************************************************/
/*              offensive_spell             */
/************************************************************************/

/* This function is called by all spells whose sole purpose is to do    */
/* damage to a given creature.                      */

int offensive_spell(ply_ptr, cmnd, how, spellname, osp)
creature    *ply_ptr;
cmd     *cmnd;
int     how;
char        *spellname;
osp_t       *osp;
{
    creature    *crt_ptr;
    room        *rom_ptr;
    int     m, fd, dmg, bns=0;
    long        addrealm;

    fd = ply_ptr->fd;
    rom_ptr = ply_ptr->parent_rom;

    if(ply_ptr->mpcur < osp->mp && how == CAST) {
        print(fd, "\n당신의 도력이 부족합니다.\n");
        return(0);
    }

    if(!S_ISSET(ply_ptr, osp->splno) && how == CAST) {
        print(fd, "\n당신은 아직 그런 주문을 터득하지 못했습니다.\n");
        return(0);
    }


    if(F_ISSET(ply_ptr, PINVIS)) {
        F_CLR(ply_ptr, PINVIS);
        print(fd, "\n당신의 모습이 원래대로 돌아왔습니다.\n");
        broadcast_rom(fd, ply_ptr->rom_num, "\n%M이 모습이 나타납니다.\n",
                  ply_ptr);
    }

    if(how == CAST) switch(osp->bonus_type) {
    case 1:
        bns = bonus[ply_ptr->intelligence] +
            mprofic(ply_ptr, osp->realm)/10;
        break;
    case 2:
        bns = bonus[ply_ptr->intelligence] +
            mprofic(ply_ptr, osp->realm)/6;
        break;
    case 3:
        bns = bonus[ply_ptr->intelligence] +
            mprofic(ply_ptr, osp->realm)/4;
        break;
    }

    if(F_ISSET(rom_ptr, RWATER)) {
        switch(osp->realm) {
        case WATER: bns *= 2; break;
        case FIRE:  bns = MIN(-bns, -5); break;
        }
    }
    else if(F_ISSET(rom_ptr, RFIRER)) {
        switch(osp->realm) {
        case FIRE:  bns *= 2; break;
        case WATER: bns = MIN(-bns, -5); break;
        }
    }
    else if(F_ISSET(rom_ptr, RWINDR)) {
        switch(osp->realm) {
        case WIND:  bns *= 2; break;
        case EARTH: bns = MIN(-bns, -5); break;
        }
    }
    else if(F_ISSET(rom_ptr, REARTH)) {
        switch(osp->realm) {
        case EARTH: bns *= 2; break;
        case WIND:  bns = MIN(-bns, -5); break;
        }
    }
        
    /* Cast on self */
    if(cmnd->num == 2) {

        dmg = dice(osp->ndice, osp->sdice, osp->pdice+bns);
        dmg = MAX(1, dmg);

        if(!strncmp(cmnd->str[1], spllist[14].splstr,strlen(cmnd->str[1])))
		if(ply_ptr->class != MAGE && ply_ptr->class < INVINCIBLE) {
			print(fd, "\n도술사만이 쓸 수 있는 마법입니다.\n");
			return(0);
		}
        if(!strncmp(cmnd->str[1], spllist[38].splstr,strlen(cmnd->str[1])))
		if(ply_ptr->class != MAGE && ply_ptr->class < INVINCIBLE) {
			print(fd, "\n도술사만이 쓸 수 있는 마법입니다.\n");
			return(0);
		}
        if(!strncmp(cmnd->str[1], spllist[39].splstr,strlen(cmnd->str[1])))
		if(ply_ptr->class != MAGE && ply_ptr->class < INVINCIBLE) {
			print(fd, "\n도술사만이 쓸 수 있는 마법입니다.\n");
			return(0);
		}
        if(!strncmp(cmnd->str[1], spllist[40].splstr,strlen(cmnd->str[1])))
		if(ply_ptr->class != MAGE && ply_ptr->class < INVINCIBLE) {
			print(fd, "\n도술사만이 쓸 수 있는 마법입니다.\n");
			return(0);
		}

        ply_ptr->hpcur -= dmg;

        if(how == CAST)
            ply_ptr->mpcur -= osp->mp;

        if(how == CAST || how == SCROLL || how == WAND)  {
           print(fd, "\n당신은 %s 주문을 자신에게 겁니다.\n",
                spellname);
            

           /*  내가 만든주문  */
        /* 삭풍 */  
        if(!strncmp(cmnd->str[1], spllist[1].splstr,strlen(cmnd->str[1])))
              print(fd, "\n삭풍소멸주... 삭풍이여! 너의 힘으로 모든 것을 소멸시켜라.\n주문을 외우자 북방으로부터 칼날과 같은 거센 바람이 불어 명령에 따라 공격합니다.\n");
        /* 불화살  */  
        if(!strncmp(cmnd->str[1], spllist[6].splstr,strlen(cmnd->str[1])))
              print(fd, "\n멸겁화궁주... 멸겁화의 화살이 모든 것을 태워버린다.\n손을 끌어 기를 모으자 손 끝에서 적색의 불꽃이 공간을 가르며 적에게 날라갑니다.\n");
        /* 번개 */  
        if(!strncmp(cmnd->str[1], spllist[13].splstr,strlen(cmnd->str[1])))
              print(fd, "\n뇌락천주... 하늘의 분노가 눈길로 변하니 그것이 뇌락이로다.\n양손의 검지를 하늘로 향해 모으자 양쪽눈에서 푸른번개가 작렬하면서 날라 갑니다.\n");
        /* 빙설검 */  
        if(!strncmp(cmnd->str[1], spllist[14].splstr,strlen(cmnd->str[1])))
              print(fd, "\n동설주... 북방동장군의 힘은 불도 얼려버리니... 거칠것이 없도다.\n주문을 외우며 부적에 도력을 모아 하늘로 날리자 모든 것을 얼릴 듯 한 눈보라가 휘몰아 칩니다.\n");
        /* 파격술 */  
        if(!strncmp(cmnd->str[1], spllist[25].splstr,strlen(cmnd->str[1])))
              print(fd, "\n파격주... 북한의 기운이요, 동왕의 출수니 나의 손으로 오라.\n주문을 외우며 손을 내밀자 한풍이 소용둘이를 일으키며 주의를 쓸어 버립니다.\n");
        /* 지동술 */  
        if(!strncmp(cmnd->str[1], spllist[26].splstr,strlen(cmnd->str[1])))
              print(fd, "\n지동주... 대지의 정령들이여 나의 명령을 들어 적을 공격하라.\n땅위의 있는 수만마리의 벌레들이 적의 기운을 쫓아 공격을 합니다.\n");
        /* 화선도 */  
        if(!strncmp(cmnd->str[1], spllist[27].splstr,strlen(cmnd->str[1])))
              print(fd, "\n화기선주... 북두의 화두성이 깃발에 실리니 이것으로 모든 것을 태우리라.\n등에 숨겨져 있던 붉은 깃발들이 하늘로 날아올라 진을 형성하자 적의 몸이 불타 오릅니다.\n");
        /* 수구진 */  
        if(!strncmp(cmnd->str[1], spllist[28].splstr,strlen(cmnd->str[1])))
              print(fd, "\n탄지수통주... 관음의 눈물이 손 끝에 맺히니 마도 무릎을 꿇으리라.\n손 끝에 빛나는 이슬이 맺히면서 그것을 튕기자 총알같이 날라가 적의 몸을 꿰뚫어 버립니다.\n");
        /* 회오리*/  
        if(!strncmp(cmnd->str[1], spllist[29].splstr,strlen(cmnd->str[1])))
              print(fd, "\n현무마압주... 현무의 검은 바람이 마를 삼키니 오행의 흑이 진동하리라.\n주문을 외무며 몸을 돌리자 갑자기 검은 태풍이 날라와 적의 몸을 삼켜 버립니다.\n");
        /* 해일 */  
        if(!strncmp(cmnd->str[1], spllist[30].splstr,strlen(cmnd->str[1])))
              print(fd, "\n파초식주... 수검도가 파도를 부르니 거대한 바다의 힘으로 적을 날려버리리라.\n소매에 숨겨져있던 얇은 검을 뽑아 검초를 뿌리자 그 안에 숨겨져 있던 수의 기운들이 상대방에게 분출됩니다.\n");
        /* 폭진 */  
        if(!strncmp(cmnd->str[1], spllist[31].splstr,strlen(cmnd->str[1])))
              print(fd, "\n폭진호출주... 지진을 부르는 지룡이여 땅위로 올라와 너의 힘을 보여라.\n목검을 땅에 꽂자 땅이 갈라지면서 지룡이 올라와 날카로운 손톱으로 적을 공격합니다.\n");
        /* 낙석 */  
        if(!strncmp(cmnd->str[1], spllist[32].splstr,strlen(cmnd->str[1])))
              print(fd, "낙석주... 백호의 힘은 산도 무너뜨리니 대암의 정령이여 암우를 떨어뜨려라.\n갑자기 옆에 있던 산만한 바위가 폭파하면서 커다란 바위들이 적에게 떨어집니다.\n");
        /* 화풍술 */  
        if(!strncmp(cmnd->str[1], spllist[33].splstr,strlen(cmnd->str[1])))
              print(fd, "\n화풍주... 화마도현의 입김이 바람으로 나타나 적을 태운다.\n수많은 부적을 태우며 하늘로 날리자 화염이 불타오르는 커다란 회오리 바람이 적을 둘러쌉니다.\n");
        /* 화열진 */  
        if(!strncmp(cmnd->str[1], spllist[34].splstr,strlen(cmnd->str[1])))
              print(fd, "\n화룡호출주... 화산의 용이여 너의 입김으로 적을 소멸시켜라.\n당신의 몸이 화룡으로 변하면서 불꽃이 타오르는 몸으로 적을 공격합니다.\n");
        /* 토사술 */  
        if(!strncmp(cmnd->str[1], spllist[35].splstr,strlen(cmnd->str[1])))
              print(fd, "토분합주... 지옥의 문을 열어 삼켜버리니 모든 죄과를 심판하리...\n갑자기 적의 바로 밑으로 지진이 일어나 땅이 갈라지면서 적을 삼켜버립니다.\n");
        /* 화염술 */  
        if(!strncmp(cmnd->str[1], spllist[36].splstr,strlen(cmnd->str[1])))
              print(fd, "주작호출주... 오행중 화는 주작의 현신이니 천상에서 내려와 마를 정화시키니...\n주문을 외우자 갑자기 불꽃을 내뿜는 주작이 내려와 대지를 불태워 버립니다.\n");
        /* 혈사천 */  
        if(!strncmp(cmnd->str[1], spllist[37].splstr,strlen(cmnd->str[1])))
              print(fd, "\n열사천주... 염라의 불꽃이 이곳까지 미치니 모든 마는 이곳에 빠지리라.\n잠잠하던 땅이 흔들리더니 갑자기 용암이 분출하자 적은 놀라 그곳에 빠집니다.\n");
        /* 파천운 */  
        if(!strncmp(cmnd->str[1], spllist[38].splstr,strlen(cmnd->str[1])))
              print(fd, "\n파천운주... 지국천왕의 현신이 검은 구름으로 나타나 마를 심판한다.\n갖자기 검은 구름이 나타나 천지를 붉은 벼락이 진동하면서 적을 강타합니다.\n");
        /* 지진 */  
        if(!strncmp(cmnd->str[1], spllist[39].splstr,strlen(cmnd->str[1])))
              print(fd, "\n지옥도주... 지옥의 야차들이여 생사부의 힘을 빌어 몸을 나타내라.\n주위가 검은 안개로 싸이며 검을 든 33명의 야차가 나타나 적을 무참히 도륙해 버립니다.\n");
        /* 불벼락 */  
        if(!strncmp(cmnd->str[1], spllist[40].splstr,strlen(cmnd->str[1])))
              print(fd, "\n화안진노주... /천상태자의 눈빛이 나에게 나타나 모든 것을 소멸 시키리라.\n눈을 감고 주문을 외우자 강렬한 빛을 내뿜는 삼지안이 열리면서 모든 것을 불태워 버립니다.\n"); 
        /* 변수나한권 */
    /*    if(!strncmp(cmnd->str[1], spllist[56].splstr,strlen(cmnd->str[1])))  */
    /*          print(fd, "\n주문을 외우자 주먹이 우두두 쏟아지며 적의 급소를 공격합니다. 우다다닷~~~ ");  */
            
            print(fd, "\n주술이 %d만큼의 피해를 주었습니다.\n", dmg);
            broadcast_rom(fd, ply_ptr->rom_num, 
                      "\n%M이 %s 주술을 %s 자신에게 걸고 있습니다.\n", 
                      ply_ptr, spellname,
                      F_ISSET(ply_ptr, PMALES) ? "그":"그녀");
            display_status(fd, ply_ptr);
            print(fd, "\n");
        }
        else if(how == POTION) {
            print(fd, "\n윽.. 당신은 독을 먹었습니다. 몸에 힘이 빠져나가는 것이 느껴집니다.\n으~~~ 이러면 안되는데....\n");
            print(fd, "\n%d만큼의 체력이 떨어졌습니다.\n", dmg);
            display_status(fd, crt_ptr);
            print(fd, "\n");
        }

        if(ply_ptr->hpcur < 1) {
            print(fd, "\n!! 좋아요.. 죽을려면 무슨짓을 못하겠어요. !!\n");
            ply_ptr->hpcur = 1;
            return(2);
        }

    }

    /* Cast on monster or player */
    else {

        if(how == POTION) {
            print(fd, "\n당신에게만 사용할수 있습니다.\n");
            return(0);
        }

        crt_ptr = find_crt(ply_ptr, rom_ptr->first_mon,
                   cmnd->str[2], cmnd->val[2]);

        if(!crt_ptr) {
            cmnd->str[2][0] = up(cmnd->str[2][0]);
            crt_ptr = find_crt(ply_ptr, rom_ptr->first_ply,
                       cmnd->str[2], cmnd->val[2]);

            if(!crt_ptr || crt_ptr == ply_ptr || 
               strlen(cmnd->str[2]) < 2) {
                print(fd, "\n그런 것은 여기에 존재하지 않습니다.\n");
                return(0);
            }

        }

        if(crt_ptr->type == MONSTER && F_ISSET(crt_ptr, MUNKIL)) {
            print(fd, "\n당신은 %s를 공격할 수 없습니다.\n",
                F_ISSET(crt_ptr, MMALES) ? "그":"그녀");
            return(0);
        }

        if(ply_ptr->type == PLAYER && crt_ptr->type == PLAYER &&
            crt_ptr != ply_ptr) {
            if(F_ISSET(rom_ptr, RNOKIL)) {
                print(fd,"\n이 방에선 전투가 금지되있습니다.\n");
                return;
            }
            if(((ply_ptr->level+3)/4)<4 && ((crt_ptr->level+3)/4)>5){
                print(fd, "\n!! 좋아요.. 죽을려면 무슨짓을 못하겠어요. !!\n");
				return;
            }
		    
			if(!F_ISSET(ply_ptr, PFAMIL) || !F_ISSET(crt_ptr, PFAMIL)) {
                if(!F_ISSET(ply_ptr, PCHAOS) && !F_ISSET(rom_ptr, RSUVIV)) {
                    print(fd, "\n미안하지만 당신은 선합니다.\n");
                    return(0);
                }
                if(!F_ISSET(crt_ptr, PCHAOS) && !F_ISSET(rom_ptr, RSUVIV)) {
                    print(fd, "\n미안하지만 그사람은 선합니다.\n");
                    return(0);
                }
			}
			else if(check_war(ply_ptr->daily[DL_EXPND].max, crt_ptr->daily[DL_EXPND].max)) {
                if(!F_ISSET(ply_ptr, PCHAOS) && !F_ISSET(rom_ptr, RSUVIV)) {
                    print(fd, "\n미안하지만 당신은 선합니다.\n");
                    return(0);
                }
                if(!F_ISSET(crt_ptr, PCHAOS) && !F_ISSET(rom_ptr, RSUVIV)) {
                    print(fd, "\n미안하지만 그사람은 선합니다.\n");
                    return(0);
                }
			}
        }
        if(crt_ptr->type != MONSTER) 
        if(is_charm_crt(ply_ptr->name, crt_ptr) && F_ISSET(ply_ptr, PCHARM)){
                print(fd, "You just can't bring yourself to do that.\n");
                return(0);                  
                }

        if(how == CAST)
            ply_ptr->mpcur -= osp->mp;

        if(spell_fail(ply_ptr)){
            return(0);
        }

        dmg = dice(osp->ndice, osp->sdice, osp->pdice+bns);
        dmg = MAX(1, dmg);

        if(!strncmp(cmnd->str[1], spllist[14].splstr,strlen(cmnd->str[1])))
		if(ply_ptr->class != MAGE && ply_ptr->class < INVINCIBLE) {
			print(fd, "\n도술사만이 쓸 수 있는 마법입니다.\n");
			return(0);
		}
        if(!strncmp(cmnd->str[1], spllist[38].splstr,strlen(cmnd->str[1])))
		if(ply_ptr->class != MAGE && ply_ptr->class < INVINCIBLE) {
			print(fd, "\n도술사만이 쓸 수 있는 마법입니다.\n");
			return(0);
		}
        if(!strncmp(cmnd->str[1], spllist[39].splstr,strlen(cmnd->str[1])))
		if(ply_ptr->class != MAGE && ply_ptr->class < INVINCIBLE) {
			print(fd, "\n도술사만이 쓸 수 있는 마법입니다.\n");
			return(0);
		}
        if(!strncmp(cmnd->str[1], spllist[40].splstr,strlen(cmnd->str[1])))
		if(ply_ptr->class != MAGE && ply_ptr->class < INVINCIBLE) {
			print(fd, "\n도술사만이 쓸 수 있는 마법입니다.\n");
			return(0);
		}

        if((crt_ptr->type == PLAYER && F_ISSET(crt_ptr, PRMAGI)) ||
           (crt_ptr->type != PLAYER && F_ISSET(crt_ptr, MRMAGI)))
            dmg -= (dmg * 2 *
              MIN(50, crt_ptr->piety + crt_ptr->intelligence)) /
              100;

        m = MIN(crt_ptr->hpcur, dmg);

        addrealm = (m * crt_ptr->experience) / MAX(1,crt_ptr->hpmax);
        addrealm = MIN(addrealm, crt_ptr->experience);
        if(crt_ptr->type != PLAYER)
            ply_ptr->realm[osp->realm-1] += addrealm;
        if(crt_ptr->type != PLAYER) {
            add_enm_crt(ply_ptr->name, crt_ptr);
            add_enm_dmg(ply_ptr->name, crt_ptr, m);
        }

        crt_ptr->hpcur -= dmg;
        if(how == CAST || how == SCROLL || how == WAND) {
           print(fd, "\n당신은 %s 주문을 %s에게 걸었습니다.\n", spellname,
                crt_ptr->name);
            broadcast_rom2(fd, crt_ptr->fd, ply_ptr->rom_num,
                       "\n%M이 %s 주문을 %M에게 외웁니다.",
                       ply_ptr, spellname, crt_ptr);



           /*  내가 만든주문  */
        /* 삭풍 */  
        if(!strncmp(cmnd->str[1], spllist[1].splstr,strlen(cmnd->str[1]))) {
              print(fd, "삭풍소멸주... 삭풍이여! 너의 힘으로 모든 것을 소멸시켜라.\n주문을 외우자 북방으로부터 칼날과 같은 거센 바람이 불어 명령에 따라 공격합니다.\n");
              broadcast_rom(fd, ply_ptr->rom_num, "\n%M이 주문을 외우자 북방으로부터 칼날과 같은 거센 바람이\n불어 명령에 따라 공격합니다.",ply_ptr);
              }
        /* 불화살  */  
        if(!strncmp(cmnd->str[1], spllist[6].splstr,strlen(cmnd->str[1]))) {
              print(fd, "멸겁화궁주... 멸겁화의 화살이 모든 것을 태워버린다.\n손을 끌어 기를 모으자 손 끝에서 적색의 불꽃이 공간을 가르며 적에게 날라갑니다.\n");
              broadcast_rom(fd, ply_ptr->rom_num, "\n%M이 손을 끌어 기를 모으자 손 끝에서 적색의 불꽃이 공간을\n가르며 적에게 날라갑니다.",ply_ptr);
              }
        /* 번개 */  
        if(!strncmp(cmnd->str[1], spllist[13].splstr,strlen(cmnd->str[1]))) {
              print(fd, "뇌락천주... 하늘의 분노가 눈길로 변하니 그것이 뇌락이로다.\n양손의 검지를 하늘로 향해 모으자 양쪽눈에서 푸른번개가 작렬하면서 날라 갑니다.\n");
              broadcast_rom(fd, ply_ptr->rom_num, "\n%M이 양손의 검지를 하늘로 향해 모으자 양쪽눈에서 푸른번개가\n작렬하면서 날라 갑니다.",ply_ptr);
              }
        /* 빙설검 */  
        if(!strncmp(cmnd->str[1], spllist[14].splstr,strlen(cmnd->str[1]))) {
              print(fd, "동설주... 북방동장군의 힘은 불도 얼려버리니... 거칠것이 없도다..\n주문을 외우며 부적에 도력을 모아 하늘로 날리자 모든 것을 얼릴 듯 한 눈보라가 휘몰아 칩니다.\n");
              broadcast_rom(fd, ply_ptr->rom_num, "\n%M이 주문을 외우며 부적에 도력을 모아 하늘로 날리자 모든 것을\n얼릴 듯 한 눈보라가 휘몰아 칩니다.",ply_ptr);
              }
        /* 파격술 */  
        if(!strncmp(cmnd->str[1], spllist[25].splstr,strlen(cmnd->str[1]))) {
              print(fd, "파격주... 북한의 기운이요, 동왕의 출수니 나의 손으로 오라.\n주문을 외우며 손을 내밀자 한풍이 소용둘이를 일으키며 주의를 쓸어 버립니다.\n");
              broadcast_rom(fd, ply_ptr->rom_num, "\n%M이 주문을 외우며 손을 내밀자 한풍이 소용둘이를 일으키며\n주의를 쓸어 버립니다.",ply_ptr);
              }
        /* 지동술 */  
        if(!strncmp(cmnd->str[1], spllist[26].splstr,strlen(cmnd->str[1]))) {
              print(fd, "지동주... 대지의 정령들이여 나의 명령을 들어 적을 공격하라.\n땅위의 있는 수만마리의 벌레들이 적의 기운을 쫓아 공격을 합니다.\n");
              broadcast_rom(fd, ply_ptr->rom_num, "\n%M이 주문을 외우자 땅위의 있는 수만마리의 벌레들이 적의 \n기운을 쫓아 공격을 합니다.",ply_ptr);
              }
        /* 화선도 */  
        if(!strncmp(cmnd->str[1], spllist[27].splstr,strlen(cmnd->str[1]))) {
              print(fd, "화기선주... 북두의 화두성이 깃발에 실리니 이것으로 모든 것을 태우리라.\n등에 숨겨져 있던 붉은 깃발들이 하늘로 날아올라 진을 형성하자 적의 몸이 불타 오릅니다.\n");
              broadcast_rom(fd, ply_ptr->rom_num, "\n%M의 등에 숨겨져 있던 붉은 깃발들이 하늘로 날아올라 진을\n형성하자 적의 몸이 불타 오릅니다.",ply_ptr);
              }
        /* 수구진 */  
        if(!strncmp(cmnd->str[1], spllist[28].splstr,strlen(cmnd->str[1])))  {
              print(fd, "탄지수통주... 관음의 눈물이 손 끝에 맺히니 마도 무릎을 꿇으리라.\n손 끝에 빛나는 이슬이 맺히면서 그것을 튕기자 총알같이 날라가 적의 몸을 꿰뚫어 버립니다.\n");
              broadcast_rom(fd, ply_ptr->rom_num, "\n%M의 손 끝에 빛나는 이슬이 맺히면서 그것을 튕기자 총알같이\n날라가 적의 몸을 꿰뚫어 버립니다.",ply_ptr);
              }
        /* 회오리*/  
        if(!strncmp(cmnd->str[1], spllist[29].splstr,strlen(cmnd->str[1])))  {
              print(fd, "현무마압주... 현무의 검은 바람이 마를 삼키니 오행의 흑이 진동하리라.\n주문을 외무며 몸을 돌리자 갑자기 검은 태풍이 날라와 적의 몸을 삼켜 버립니다.\n");
              broadcast_rom(fd, ply_ptr->rom_num, "\n%M이 주문을 외무며 몸을 돌리자 갑자기 검은 태풍이 날라와\n적의 몸을 삼켜 버립니다.",ply_ptr);
              }
        /* 해일 */                                                           
        if(!strncmp(cmnd->str[1], spllist[30].splstr,strlen(cmnd->str[1])))  {
              print(fd, "파초식주... 수검도가 파도를 부르니 거대한 바다의 힘으로 적을 날려버리리라.\n소매에 숨겨져있던 얇은 검을 뽑아 검초를 뿌리자 그 안에 숨겨져 있던 수의 기운들이 상대방에게 분출됩니다.\n");
              broadcast_rom(fd, ply_ptr->rom_num, "\n%M의 소매에 숨겨져있던 얇은 검을 뽑으며 검초를 뿌리자 \n그 안에 숨겨져 있던 수의 기운들이 상대방에게 분출됩니다.",ply_ptr);
              }
        /* 폭진 */  
        if(!strncmp(cmnd->str[1], spllist[31].splstr,strlen(cmnd->str[1])))  {
              print(fd, "폭진호출주... 지진을 부르는 지룡이여 땅위로 올라와 너의 힘을 보여라.\n목검을 땅에 꽂자 땅이 갈라지면서 지룡이 올라와 날카로운 손톱으로 적을 공격합니다.\n");
              broadcast_rom(fd, ply_ptr->rom_num, "\n%M이 목검을 땅에 꽂자 땅이 갈라지면서 지룡이 올라와 날카로운\n손톱으로 적을 공격합니다.",ply_ptr);
              }
        /* 낙석 */  
        if(!strncmp(cmnd->str[1], spllist[32].splstr,strlen(cmnd->str[1])))  {
              print(fd, "낙석주... 백호의 힘은 산도 무너뜨리니 대암의 정령이여 암우를 떨어뜨려라.\n갑자기 옆에 있던 산만한 바위가 폭파하면서 커다란 바위들이 적에게 떨어집니다.\n");
              broadcast_rom(fd, ply_ptr->rom_num, "\n%M의 옆에 있던 산만한 바위가 갑자기 폭파하면서 커다란\n바위들이 적에게 떨어집니다.",ply_ptr);
              }
        /* 화풍술 */  
        if(!strncmp(cmnd->str[1], spllist[33].splstr,strlen(cmnd->str[1]))) {
              print(fd, "화풍주... 화마도현의 입김이 바람으로 나타나 적을 태운다.\n수많은 부적을 태우며 하늘로 날리자 화염이 불타오르는 커다란 회오리 바람이 적을 둘러쌉니다.\n");
              broadcast_rom(fd, ply_ptr->rom_num, "\n%M이 수많은 부적을 태우며 하늘로 날리자 화염이 불타오르는\n커다란 회오리 바람이 적을 둘러쌉니다.",ply_ptr);
              }
        /* 화열진 */  
        if(!strncmp(cmnd->str[1], spllist[34].splstr,strlen(cmnd->str[1]))) {
              print(fd, "화룡호출주... 화산의 용이여 너의 입김으로 적을 소멸시켜라.\n당신의 몸이 화룡으로 변하면서 불꽃이 타오르는 몸으로 적을 공격합니다.\n");
              broadcast_rom(fd, ply_ptr->rom_num, "\n%M의 몸이 화룡으로 변하면서 불꽃이 타오르는 몸으로\n적을 공격합니다.",ply_ptr);
              }
        /* 토사술 */  
        if(!strncmp(cmnd->str[1], spllist[35].splstr,strlen(cmnd->str[1]))) {
              print(fd, "토분합주... 지옥의 문을 열어 삼켜버리니 모든 죄과를 심판하리...\n갑자기 적의 바로 밑으로 지진이 일어나 땅이 갈라지면서 적을 삼켜버립니다.\n");
              broadcast_rom(fd, ply_ptr->rom_num, "\n%M이 갑자기 적의 바로 밑으로 지진이 일어나 땅이 갈라지면서\n적을 삼켜버립니다.",ply_ptr);
              }
        /* 화염술 */  
        if(!strncmp(cmnd->str[1], spllist[36].splstr,strlen(cmnd->str[1]))) {
              print(fd, "주작호출주... 오행중 화는 주작의 현신이니 천상에서 내려와 마를 정화시키니...\n주문을 외우자 갑자기 불꽃을 내뿜는 주작이 내려와 대지를 불태워 버립니다.\n");
              broadcast_rom(fd, ply_ptr->rom_num, "\n%M이 주문을 외우자 갑자기 불꽃을 내뿜는 주작이 내려와\n대지를 불태워 버립니다.",ply_ptr);
              }
        /* 혈사천 */  
        if(!strncmp(cmnd->str[1], spllist[37].splstr,strlen(cmnd->str[1]))) {
              print(fd, "열사천주... 염라의 불꽃이 이곳까지 미치니 모든 마는 이곳에 빠지리라.\n잠잠하던 땅이 흔들리더니 갑자기 용암이 분출하자 적은 놀라 그곳에 빠집니다.\n");
              broadcast_rom(fd, ply_ptr->rom_num, "\n%M이 주문을 외우자 잠잠하던 땅이 흔들리더니 갑자기 용암이\n분출하자 적은 놀라 그곳에 빠집니다.",ply_ptr);
              }
        /* 파천운 */  
        if(!strncmp(cmnd->str[1], spllist[38].splstr,strlen(cmnd->str[1]))) {
              print(fd, "파천운주... 지국천왕의 현신이 검은 구름으로 나타나 마를 심판한다.\n갑자기 검은 구름이 나타나 천지를 붉은 벼락이 진동하면서 적을 강타합니다.\n");
              broadcast_rom(fd, ply_ptr->rom_num, "\n%M이 주문을 외우자 갑자기 검은 구름이 나타나 천지를 \n붉은 벼락이 진동하면서 적을 강타합니다.",ply_ptr);
              }
        /* 지진 */  
        if(!strncmp(cmnd->str[1], spllist[39].splstr,strlen(cmnd->str[1]))) {
              print(fd, "지옥도주... 지옥의 야차들이여 생사부의 힘을 빌어 몸을 나타내라.\n주위가 검은 안개로 싸이며 검을 든 33명의 야차가 나타나 적을 무참히 도륙해 버립니다.\n");                     
              broadcast_rom(fd, ply_ptr->rom_num, "\n%M의 주위가 검은 안개로 싸이며 검을 든 33명의 야차가 나타나 적을 무참히 도륙해 버립니다.",ply_ptr);
              }
        /* 불벼락 */  
        if(!strncmp(cmnd->str[1], spllist[40].splstr,strlen(cmnd->str[1]))) {
              print(fd, "화안진노주... 천상태자의 눈빛이 나에게 나타나 모든 것을 소멸 시키리라.\n눈을 감고 주문을 외우자 강렬한 빛을 내뿜는 삼지안이 열리면서 모든 것을 불태워 버립니다.\n"); 
              broadcast_rom(fd, ply_ptr->rom_num, "\n%M이 눈을 감고 주문을 외우자 강렬한 빛을 내뿜는 삼지안이 열리면서 모든 것을 불태워 버립니다.",ply_ptr);
              }
            
            
            
            print(fd, "\n주문이 %d만큼의 피해를 주었습니다.\n", dmg);
            print(crt_ptr->fd, 
                  "\n%M이 %s 주술로 당신에게 %d만큼의 피해를 주었습니다.\n",
                  ply_ptr, spellname, dmg);
            display_status(fd, crt_ptr);
            print(fd, "\n");
        }


        if(crt_ptr->hpcur < 1) {
            print(fd, "\n당신은 %M을 죽였습니다.\n", crt_ptr);
            broadcast_rom2(fd, crt_ptr->fd, ply_ptr->rom_num,
                      "\n%M이 %M을 죽여버렸습니다.\n", ply_ptr, crt_ptr);
            die(crt_ptr, ply_ptr);
            return(2);
        }

    }

    return(1);

}
