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
        print(fd, "� �ּ��� ��ġ�ǰ̴ϱ�?");
        return(0);
    }
    if(F_ISSET(ply_ptr, PBLIND)) {
        ANSI(fd, RED);
        print(fd, "�ƹ��͵� ������ �ʽ��ϴ�!");
        ANSI(fd, WHITE);
        return(0);
    }
    if(F_ISSET(ply_ptr, PSILNC)) {
        ANSI(fd, YELLOW);
        print(fd, "�Ѹ��� �Ҽ� �����ϴ�!");
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
        print(fd, "�׷� �ֹ��� �������� �ʽ��ϴ�.");
        return(0);
    }

    else if(match > 1) {
        print(fd, "��ġ�� �ֹ��� �̸��� �̻��ϱ���.");
        return(0);
    }

    if(F_ISSET(ply_ptr->parent_rom, RNOMAG)) {
        print(fd, "�ּ��� ��� �ϴµ� ���� �ϼ̽��ϴ�.");
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
            print(fd,"����� �����ֹ��� �� �� ���� ������ ���� �ֽ��ϴ�.");
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
        print(fd, "�������� ����� ������Ű�ǰ̴ϱ�?");
        return(0);
    }
    if(F_ISSET(ply_ptr, PBLIND)) {
        ANSI(fd, RED);
        print(fd, "�ƹ��͵� ������ �ʽ��ϴ�!\n");
        ANSI(fd, WHITE);
        return(0);
    }
    if(F_ISSET(ply_ptr, PSILNC)) {
        ANSI(fd, YELLOW);
        print(fd, "����� �Ѹ��� �Ҽ� �����ϴ�!\n");
        ANSI(fd, WHITE);
        return(0);
    }

    if(ply_ptr->class != CARETAKER && ply_ptr->class  !=  MAGE  && ply_ptr->class  !=  CLERIC)  {
        print(fd, "\n������� �����ڸ��� ������ų �� �ִ� �ɷ��� �ֽ��ϴ�.\n");
        return(0);
    }

    cmnd->str[1][0] = up(cmnd->str[1][0]);
    crt_ptr = find_crt(ply_ptr, rom_ptr->first_ply, cmnd->str[1], 
               cmnd->val[1]);

    if(!crt_ptr) {
        print(fd, "\n�׷� ����� �������� �ʽ��ϴ�.\n");
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
        print(fd, "\n�׷� �ֹ��� �������� �ʽ��ϴ�.\n");
        return(0);
    }

    else if(match > 1) {
        print(fd, "\n�ֹ��̸��� �̻��մϴ�.\n");
        return(0);
    }

    if(!S_ISSET(ply_ptr, spllist[splno].splno)) {
        print(fd, "\n����� ���� �׷� �ֹ��� �͵����� ���߽��ϴ�.\n");
        return(0);
    }

/*
    if((spllist[splno].splno  >  3)  &&  (ply_ptr->class  !=  CLERIC  &&  ply_ptr->class  < 
CARETAKER)) {
        print(fd, "\n�� �ֹ��� �ٸ� ������� ������ų �� �����ϴ�.\n");
        return(0);
    }

    if((spllist[splno].splno > 5) && (ply_ptr->class < CARETAKER)) {
        print(fd, "\n�� �ֹ��� �ٸ� ������� ������ų �� �����ϴ�.\n");
        return(0);
    }

    if((spllist[splno].splno  <  3)   &&  (ply_ptr->class  !=  MAGE  &&  ply_ptr->class   < 
CARETAKER)) {
        print(fd, "\n�� �ֹ��� �ٸ� ������� ������ų �� �����ϴ�.\n");
        return(0);
    }
    */

    if((spllist[splno].spllv == 1) && (ply_ptr->class != CLERIC && ply_ptr->class < INVINCIBLE)) {
    	print(fd, "\n�� �ֹ��� �ٸ� ������� ������ų �� �����ϴ�.\n");
    	return(0);
    }
    if((spllist[splno].spllv == 2) && (ply_ptr->class != MAGE && ply_ptr->class < INVINCIBLE)) {
    	print(fd, "\n�� �ֹ��� �ٸ� ������� ������ų �� �����ϴ�.\n");
    	return(0);
    }
    if((spllist[splno].spllv == 3) && (ply_ptr->class < INVINCIBLE )) {
    	print(fd, "\n�� �ֹ��� �ٸ� ������� ������ų �� �����ϴ�.\n");
    	return(0);
    }
    if((spllist[splno].spllv == 4) && (ply_ptr->class < CARETAKER )) {
    	print(fd, "\n�� �ֹ��� �ٸ� ������� ������ų �� �����ϴ�.\n");
    	return(0);
    }
    if((spllist[splno].spllv == 5) && (ply_ptr->class < SUB_DM )) {
    	print(fd, "\n�� �ֹ��� �ٸ� ������� ������ų �� �����ϴ�.\n");
    	return(0);
    }

    F_CLR(ply_ptr, PHIDDN);

    S_SET(crt_ptr, spllist[splno].splno);
    print(crt_ptr->fd, "\n%M�� %s �ּ��� �ù��� ���̸� �ֹ� ������ ��ŵ�ϴ�.\n����~~~ �� �ֹ��� �ܿ��� ������ �̻��� ����� ���̴� ���� �� \n�� ������� ����� ������ �� �� �����ϴ�.\n", ply_ptr,
          spllist[splno].splstr);
    print(fd, "\n%s �ּ��� %M���� �ù��� ���̸� �ֹ� ������ ��ŵ�ϴ�.\n����~~~ �� �ֹ��� �ܿ��� ������ �̻��� ����� ���̴� ����\n�� ������� ����� ������ �� �� �����ϴ�.\n", spllist[splno].splstr,
          crt_ptr);

    broadcast_rom2(fd, crt_ptr->fd, rom_ptr->rom_num,
               "\n%M�� %M���� %s �ּ��� �ù��� ���̸� �ֹ� ������ \n��ŵ�ϴ�.\n����~~~ �� �ֹ��� �ܿ��� ������ �̻��� ����� ���̴� ����\n�� ������� ����� ������ �� �� �����ϴ�.\n", ply_ptr, crt_ptr,
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
        print(fd, "\n������ �����ϽǷ����?\n");
        return(0);
    }
    if(F_ISSET(ply_ptr, PBLIND)) {
        ANSI(fd, RED);
        print(fd, "\n����� �ɷ����δ� �� ������� ������ �� �����ϴ�.\n");
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
        print(fd, "\n�׷� ���� �����ϰ� ���� �ʽ��ϴ�.\n");
        return(0);
    }

    if(obj_ptr->type != SCROLL) {
        print(fd, "\n�̰��� ������� �ƴմϴ�.\n");
        return(0);
    }
 /* �ֹ� ��� ���� ���� */

        if (obj_ptr->ndice > ply_ptr->level) {
                print(fd, "\n����� �ɷ����δ� %i�� ������ �ľ����� ���� ������ �� �����ϴ�.",obj_ptr);
        return(0);
    }


    if((F_ISSET(obj_ptr, OGOODO) && ply_ptr->alignment < -100) ||
       (F_ISSET(obj_ptr, OEVILO) && ply_ptr->alignment > 100)) {
        print(fd, "\n������ ����ġ�� %I�� ��ü�� ȭ���� �۽��̸� ���а� �����\n ���Ƚ��ϴ�.\n", obj_ptr);
        del_obj_crt(obj_ptr, ply_ptr);
        add_obj_rom(obj_ptr, ply_ptr->parent_rom);
        return(0);
    }
     

  if(F_ISSET(obj_ptr,OCLSEL))
        if(!F_ISSET(obj_ptr,OCLSEL    +   ply_ptr->class)    &&   (    ply_ptr->class   < 
CARETAKER)){
                print(fd, "\n����� ������ %i�� ����� ������ �� �����ϴ�.\n",obj_ptr);
                return(0);
        }                 

    F_CLR(ply_ptr, PHIDDN);

    print(fd, "����� %s�� ������ �˾Ƴ��� �����ϱ� �����մϴ�.\n������ �� ������ ���� ���ȿ��� �̻��� ����� ���� ������\n���� �������ϴ�.\n�̾߾�~~~~~�� �� ����� �����Ǹ鼭 ������ ����� ���� \n�Ǿ����ϴ�.\n", 
          spllist[obj_ptr->magicpower-1].splstr);
    print(fd, "\n������ ��ġ�� %I�� ��ü�� ȭ���� �۽��̸� ���а� ����� ���Ƚ��ϴ�.\n", obj_ptr);
    broadcast_rom(fd, ply_ptr->rom_num, "\n%M�� %1i�� ������ �а� �����մϴ�.",
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
                           "�Խ���",1);
            if(!obj_ptr) {
                print(fd,"������ �н��ϱ�?");
                return 0;
            }
            board_obj[fd]=obj_ptr;
            return(read_board(ply_ptr,cmnd));
        }
        print(fd, "������ �н��ϱ�?\n");
        return(0);
    }
    if(F_ISSET(ply_ptr, PBLIND)) {
        ANSI(fd, RED);
        print(fd, "\n����� �װ��� ���� �� �ִ� �ɷ��� �����ϴ�.\n");
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
        print(fd, "\n �׷����� �������� �ʽ��ϴ�.\n");
        return(0);
    }

    if(obj_ptr->special) {
        n = special_obj(ply_ptr, cmnd, SP_MAPSC);
        if(n != -2) return(MAX(0, n));
    }

    if(obj_ptr->type != SCROLL) {
        print(fd, "\n�̰��� �������� �ƴմϴ�.\n");
        return(0);
    }
 /* �ֹ� ��� ���� ���� */

        if (obj_ptr->ndice > ply_ptr->level) {
                print(fd, "\n����� �ɷ����δ� %i�� ������ �ľ����� ���� ������ �� �����ϴ�.",obj_ptr);
        return(0);
    }

    if((F_ISSET(obj_ptr, OGOODO) && ply_ptr->alignment < -100) ||
       (F_ISSET(obj_ptr, OEVILO) && ply_ptr->alignment > 100)) {
        print(fd, "\n��� ���� �а� ���� %I�� ��ü�� ������ ���ϸ鼭 �ٶ��� �Բ� ����� ���Ƚ��ϴ�.\n", obj_ptr);
        del_obj_crt(obj_ptr, ply_ptr);
        add_obj_rom(obj_ptr, ply_ptr->parent_rom);
        return(0);
    }



  if(F_ISSET(obj_ptr,OCLSEL))
        if(!F_ISSET(obj_ptr,OCLSEL    +   ply_ptr->class)    &&   (    ply_ptr->class   < 
CARETAKER)){
                print(fd, "\n�̰��� ����� �������� ���ϴ� �ݼ��̱� ������ ������ ���� �� �����ϴ�.\n",obj_ptr);
                return(0);
        }                 

    if(F_ISSET(ply_ptr->parent_rom, RNOMAG) || (obj_ptr->magicpower-1 < 0)) {
        print(fd, "\n�ƹ��� �ϵ� �Ͼ�� �ʾҽ��ϴ�.\n");
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
        print(fd, "\n��� ���� �а� ���� %I�� ��ü�� ������ ���ϸ鼭 �ٶ��� �Բ� ����� ���Ƚ��ϴ�.\n", obj_ptr);
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

        print(fd, "\n��� ���� �а� ���� %I�� ��ü�� ������ ���ϸ鼭 �ٶ��� �Բ� ����� ���Ƚ��ϴ�.\n", obj_ptr);
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
        print(fd, "\n������ �Խ��ϱ�?\n");
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
        print(fd, "\n�׷����� �������� �ʽ��ϴ�.\n");
        return(0);
    }

    if(obj_ptr->type != POTION) {
        print(fd, "\n�̰��� �Դ� ������ �ƴմϴ�.\n");
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
        	print(fd, "����� %1i�� �Ծ����ϴ�.\n", obj_ptr);
        	broadcast_rom(fd, ply_ptr->rom_num, "\n%M�� %1i�� �Ծ����ϴ�.", 
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
        print(fd, "\n�ƹ��͵� ������� �ʽ��ϴ�.\n\n");
        return(0);
    }

        if(F_ISSET(ply_ptr->parent_rom, RNOPOT)){
        print(fd, "\n�װ��� �Ա����� ������ ��Ÿ�� ���İ� ���Ƚ��ϴ�.\n �߸԰ڴ�... ������... \n",obj_ptr);
        return(0);
        }
		if(F_ISSET(ply_ptr->parent_rom, RSUVIV)) {
		print(fd, "\n����忡���� �ƹ� �͵� ���� �� �����ϴ�.\n");
		return(0);
		}
 
    if((F_ISSET(obj_ptr, OGOODO) && ply_ptr->alignment < -100) ||
       (F_ISSET(obj_ptr, OEVILO) && ply_ptr->alignment > 100)) {
        print(fd, "%I�� ����� �վȿ��� Ÿ�����ϴ�.\n", obj_ptr);
        del_obj_crt(obj_ptr, ply_ptr);
        add_obj_rom(obj_ptr, ply_ptr->parent_rom);
        return(0);
    }


  if(F_ISSET(obj_ptr,OCLSEL))
        if(!F_ISSET(obj_ptr,OCLSEL    +   ply_ptr->class)    &&   (    ply_ptr->class   < 
CARETAKER)){
                print(fd, "\n��������� �� ������ ���ϰ� �ֱ� ������ ���� �� �����ϴ�.\n");
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

        print(fd, "\n����� %1i�� �Ծ����ϴ�.\n", obj_ptr);
        broadcast_rom(fd, ply_ptr->rom_num, "\n%M�� %1i�� �Ծ����ϴ�.", 
                  ply_ptr, obj_ptr);

        if(--obj_ptr->shotscur < 1) {
   /*         print(fd, "\n�� �԰� ���� %I�� ��⸦ ���Ƚ��ϴ�.\n", obj_ptr); */
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
        print(fd, "\n������ ����մϱ�?\n");
        return(0);
    }
    if(F_ISSET(ply_ptr, PBLIND)) {
        ANSI(fd, RED);
        print(fd, "�ƹ� �͵� ������ �ʽ��ϴ�!\n");
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
        print(fd, "\n�׷����� �������� �ʽ��ϴ�.\n");
        return(0);
    }

    if(obj_ptr->type != WAND) {
        print(fd, "\n���볪 �����̰� �ƴմϴ�.\n");
        return(0);
    }

    if(obj_ptr->shotscur < 1) {
        print(fd, "\n��� ����Ƚ��ϴ�.\n");
        return(0);
    }

    if((F_ISSET(obj_ptr, OGOODO) && ply_ptr->alignment < -100) ||
       (F_ISSET(obj_ptr, OEVILO) && ply_ptr->alignment > 100)) {
        print(fd, "\n%I�� ������ ���� �� ������ó�� ������ ���Ƚ��ϴ�.\n", obj_ptr);
        del_obj_crt(obj_ptr, ply_ptr);
        add_obj_rom(obj_ptr, ply_ptr->parent_rom);
        return(0);
    }



  if(F_ISSET(obj_ptr,OCLSEL))
        if(!F_ISSET(obj_ptr,OCLSEL    +   ply_ptr->class)    &&   (    ply_ptr->class   < 
CARETAKER)){
                print(fd, "\n����������迡�� ���ϴ� �����̱⿡ ����� �� �����ϴ�.\n");
                return(0);
        }                 


    if(F_ISSET(ply_ptr->parent_rom, RNOMAG) || (obj_ptr->magicpower < 1)) {
        print(fd, "\n�ƹ��� �ϵ� �Ͼ�� �ʽ��ϴ�.\n");
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
        print(fd, "\n����� ������ �����մϴ�.\n");
        return(0);
    }

    if(!S_ISSET(ply_ptr, osp->splno) && how == CAST) {
        print(fd, "\n����� ���� �׷� �ֹ��� �͵����� ���߽��ϴ�.\n");
        return(0);
    }


    if(F_ISSET(ply_ptr, PINVIS)) {
        F_CLR(ply_ptr, PINVIS);
        print(fd, "\n����� ����� ������� ���ƿԽ��ϴ�.\n");
        broadcast_rom(fd, ply_ptr->rom_num, "\n%M�� ����� ��Ÿ���ϴ�.\n",
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
			print(fd, "\n�����縸�� �� �� �ִ� �����Դϴ�.\n");
			return(0);
		}
        if(!strncmp(cmnd->str[1], spllist[38].splstr,strlen(cmnd->str[1])))
		if(ply_ptr->class != MAGE && ply_ptr->class < INVINCIBLE) {
			print(fd, "\n�����縸�� �� �� �ִ� �����Դϴ�.\n");
			return(0);
		}
        if(!strncmp(cmnd->str[1], spllist[39].splstr,strlen(cmnd->str[1])))
		if(ply_ptr->class != MAGE && ply_ptr->class < INVINCIBLE) {
			print(fd, "\n�����縸�� �� �� �ִ� �����Դϴ�.\n");
			return(0);
		}
        if(!strncmp(cmnd->str[1], spllist[40].splstr,strlen(cmnd->str[1])))
		if(ply_ptr->class != MAGE && ply_ptr->class < INVINCIBLE) {
			print(fd, "\n�����縸�� �� �� �ִ� �����Դϴ�.\n");
			return(0);
		}

        ply_ptr->hpcur -= dmg;

        if(how == CAST)
            ply_ptr->mpcur -= osp->mp;

        if(how == CAST || how == SCROLL || how == WAND)  {
           print(fd, "\n����� %s �ֹ��� �ڽſ��� �̴ϴ�.\n",
                spellname);
            

           /*  ���� �����ֹ�  */
        /* ��ǳ */  
        if(!strncmp(cmnd->str[1], spllist[1].splstr,strlen(cmnd->str[1])))
              print(fd, "\n��ǳ�Ҹ���... ��ǳ�̿�! ���� ������ ��� ���� �Ҹ���Ѷ�.\n�ֹ��� �ܿ��� �Ϲ����κ��� Į���� ���� �ż� �ٶ��� �Ҿ� ��ɿ� ���� �����մϴ�.\n");
        /* ��ȭ��  */  
        if(!strncmp(cmnd->str[1], spllist[6].splstr,strlen(cmnd->str[1])))
              print(fd, "\n���ȭ����... ���ȭ�� ȭ���� ��� ���� �¿�������.\n���� ���� �⸦ ������ �� ������ ������ �Ҳ��� ������ ������ ������ ���󰩴ϴ�.\n");
        /* ���� */  
        if(!strncmp(cmnd->str[1], spllist[13].splstr,strlen(cmnd->str[1])))
              print(fd, "\n����õ��... �ϴ��� �г밡 ����� ���ϴ� �װ��� �����̷δ�.\n����� ������ �ϴ÷� ���� ������ ���ʴ����� Ǫ�������� �۷��ϸ鼭 ���� ���ϴ�.\n");
        /* ������ */  
        if(!strncmp(cmnd->str[1], spllist[14].splstr,strlen(cmnd->str[1])))
              print(fd, "\n������... �Ϲ浿�屺�� ���� �ҵ� ���������... ��ĥ���� ������.\n�ֹ��� �ܿ�� ������ ������ ��� �ϴ÷� ������ ��� ���� �� �� �� ������ �ָ��� Ĩ�ϴ�.\n");
        /* �İݼ� */  
        if(!strncmp(cmnd->str[1], spllist[25].splstr,strlen(cmnd->str[1])))
              print(fd, "\n�İ���... ������ ����̿�, ������ ����� ���� ������ ����.\n�ֹ��� �ܿ�� ���� ������ ��ǳ�� �ҿ���̸� ����Ű�� ���Ǹ� ���� �����ϴ�.\n");
        /* ������ */  
        if(!strncmp(cmnd->str[1], spllist[26].splstr,strlen(cmnd->str[1])))
              print(fd, "\n������... ������ ���ɵ��̿� ���� ����� ��� ���� �����϶�.\n������ �ִ� ���������� �������� ���� ����� �Ѿ� ������ �մϴ�.\n");
        /* ȭ���� */  
        if(!strncmp(cmnd->str[1], spllist[27].splstr,strlen(cmnd->str[1])))
              print(fd, "\nȭ�⼱��... �ϵ��� ȭ�μ��� ��߿� �Ǹ��� �̰����� ��� ���� �¿츮��.\n� ������ �ִ� ���� ��ߵ��� �ϴ÷� ���ƿö� ���� �������� ���� ���� ��Ÿ �����ϴ�.\n");
        /* ������ */  
        if(!strncmp(cmnd->str[1], spllist[28].splstr,strlen(cmnd->str[1])))
              print(fd, "\nź��������... ������ ������ �� ���� ������ ���� ������ ��������.\n�� ���� ������ �̽��� �����鼭 �װ��� ƨ���� �Ѿ˰��� ���� ���� ���� ��վ� �����ϴ�.\n");
        /* ȸ����*/  
        if(!strncmp(cmnd->str[1], spllist[29].splstr,strlen(cmnd->str[1])))
              print(fd, "\n����������... ������ ���� �ٶ��� ���� ��Ű�� ������ ���� �����ϸ���.\n�ֹ��� �ܹ��� ���� ������ ���ڱ� ���� ��ǳ�� ����� ���� ���� ���� �����ϴ�.\n");
        /* ���� */  
        if(!strncmp(cmnd->str[1], spllist[30].splstr,strlen(cmnd->str[1])))
              print(fd, "\n���ʽ���... ���˵��� �ĵ��� �θ��� �Ŵ��� �ٴ��� ������ ���� ������������.\n�Ҹſ� �������ִ� ���� ���� �̾� ���ʸ� �Ѹ��� �� �ȿ� ������ �ִ� ���� ������ ���濡�� ����˴ϴ�.\n");
        /* ���� */  
        if(!strncmp(cmnd->str[1], spllist[31].splstr,strlen(cmnd->str[1])))
              print(fd, "\n����ȣ����... ������ �θ��� �����̿� ������ �ö�� ���� ���� ������.\n����� ���� ���� ���� �������鼭 ������ �ö�� ��ī�ο� �������� ���� �����մϴ�.\n");
        /* ���� */  
        if(!strncmp(cmnd->str[1], spllist[32].splstr,strlen(cmnd->str[1])))
              print(fd, "������... ��ȣ�� ���� �굵 ���ʶ߸��� ����� �����̿� �Ͽ츦 ����߷���.\n���ڱ� ���� �ִ� �길�� ������ �����ϸ鼭 Ŀ�ٶ� �������� ������ �������ϴ�.\n");
        /* ȭǳ�� */  
        if(!strncmp(cmnd->str[1], spllist[33].splstr,strlen(cmnd->str[1])))
              print(fd, "\nȭǳ��... ȭ�������� �Ա��� �ٶ����� ��Ÿ�� ���� �¿��.\n������ ������ �¿�� �ϴ÷� ������ ȭ���� ��Ÿ������ Ŀ�ٶ� ȸ���� �ٶ��� ���� �ѷ��Դϴ�.\n");
        /* ȭ���� */  
        if(!strncmp(cmnd->str[1], spllist[34].splstr,strlen(cmnd->str[1])))
              print(fd, "\nȭ��ȣ����... ȭ���� ���̿� ���� �Ա����� ���� �Ҹ���Ѷ�.\n����� ���� ȭ������ ���ϸ鼭 �Ҳ��� Ÿ������ ������ ���� �����մϴ�.\n");
        /* ���� */  
        if(!strncmp(cmnd->str[1], spllist[35].splstr,strlen(cmnd->str[1])))
              print(fd, "�������... ������ ���� ���� ���ѹ����� ��� �˰��� �����ϸ�...\n���ڱ� ���� �ٷ� ������ ������ �Ͼ ���� �������鼭 ���� ���ѹ����ϴ�.\n");
        /* ȭ���� */  
        if(!strncmp(cmnd->str[1], spllist[36].splstr,strlen(cmnd->str[1])))
              print(fd, "����ȣ����... ������ ȭ�� ������ �����̴� õ�󿡼� ������ ���� ��ȭ��Ű��...\n�ֹ��� �ܿ��� ���ڱ� �Ҳ��� ���մ� ������ ������ ������ ���¿� �����ϴ�.\n");
        /* ����õ */  
        if(!strncmp(cmnd->str[1], spllist[37].splstr,strlen(cmnd->str[1])))
              print(fd, "\n����õ��... ������ �Ҳ��� �̰����� ��ġ�� ��� ���� �̰��� ��������.\n�����ϴ� ���� ��鸮���� ���ڱ� ����� �������� ���� ��� �װ��� �����ϴ�.\n");
        /* ��õ�� */  
        if(!strncmp(cmnd->str[1], spllist[38].splstr,strlen(cmnd->str[1])))
              print(fd, "\n��õ����... ����õ���� ������ ���� �������� ��Ÿ�� ���� �����Ѵ�.\n���ڱ� ���� ������ ��Ÿ�� õ���� ���� ������ �����ϸ鼭 ���� ��Ÿ�մϴ�.\n");
        /* ���� */  
        if(!strncmp(cmnd->str[1], spllist[39].splstr,strlen(cmnd->str[1])))
              print(fd, "\n��������... ������ �������̿� ������� ���� ���� ���� ��Ÿ����.\n������ ���� �Ȱ��� ���̸� ���� �� 33���� ������ ��Ÿ�� ���� ������ ������ �����ϴ�.\n");
        /* �Һ��� */  
        if(!strncmp(cmnd->str[1], spllist[40].splstr,strlen(cmnd->str[1])))
              print(fd, "\nȭ��������... /õ�������� ������ ������ ��Ÿ�� ��� ���� �Ҹ� ��Ű����.\n���� ���� �ֹ��� �ܿ��� ������ ���� ���մ� �������� �����鼭 ��� ���� ���¿� �����ϴ�.\n"); 
        /* �������ѱ� */
    /*    if(!strncmp(cmnd->str[1], spllist[56].splstr,strlen(cmnd->str[1])))  */
    /*          print(fd, "\n�ֹ��� �ܿ��� �ָ��� ��ε� ������� ���� �޼Ҹ� �����մϴ�. ��ٴٴ�~~~ ");  */
            
            print(fd, "\n�ּ��� %d��ŭ�� ���ظ� �־����ϴ�.\n", dmg);
            broadcast_rom(fd, ply_ptr->rom_num, 
                      "\n%M�� %s �ּ��� %s �ڽſ��� �ɰ� �ֽ��ϴ�.\n", 
                      ply_ptr, spellname,
                      F_ISSET(ply_ptr, PMALES) ? "��":"�׳�");
            display_status(fd, ply_ptr);
            print(fd, "\n");
        }
        else if(how == POTION) {
            print(fd, "\n��.. ����� ���� �Ծ����ϴ�. ���� ���� ���������� ���� �������ϴ�.\n��~~~ �̷��� �ȵǴµ�....\n");
            print(fd, "\n%d��ŭ�� ü���� ���������ϴ�.\n", dmg);
            display_status(fd, crt_ptr);
            print(fd, "\n");
        }

        if(ply_ptr->hpcur < 1) {
            print(fd, "\n!! ���ƿ�.. �������� �������� ���ϰھ��. !!\n");
            ply_ptr->hpcur = 1;
            return(2);
        }

    }

    /* Cast on monster or player */
    else {

        if(how == POTION) {
            print(fd, "\n��ſ��Ը� ����Ҽ� �ֽ��ϴ�.\n");
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
                print(fd, "\n�׷� ���� ���⿡ �������� �ʽ��ϴ�.\n");
                return(0);
            }

        }

        if(crt_ptr->type == MONSTER && F_ISSET(crt_ptr, MUNKIL)) {
            print(fd, "\n����� %s�� ������ �� �����ϴ�.\n",
                F_ISSET(crt_ptr, MMALES) ? "��":"�׳�");
            return(0);
        }

        if(ply_ptr->type == PLAYER && crt_ptr->type == PLAYER &&
            crt_ptr != ply_ptr) {
            if(F_ISSET(rom_ptr, RNOKIL)) {
                print(fd,"\n�� �濡�� ������ �������ֽ��ϴ�.\n");
                return;
            }
            if(((ply_ptr->level+3)/4)<4 && ((crt_ptr->level+3)/4)>5){
                print(fd, "\n!! ���ƿ�.. �������� �������� ���ϰھ��. !!\n");
				return;
            }
		    
			if(!F_ISSET(ply_ptr, PFAMIL) || !F_ISSET(crt_ptr, PFAMIL)) {
                if(!F_ISSET(ply_ptr, PCHAOS) && !F_ISSET(rom_ptr, RSUVIV)) {
                    print(fd, "\n�̾������� ����� ���մϴ�.\n");
                    return(0);
                }
                if(!F_ISSET(crt_ptr, PCHAOS) && !F_ISSET(rom_ptr, RSUVIV)) {
                    print(fd, "\n�̾������� �׻���� ���մϴ�.\n");
                    return(0);
                }
			}
			else if(check_war(ply_ptr->daily[DL_EXPND].max, crt_ptr->daily[DL_EXPND].max)) {
                if(!F_ISSET(ply_ptr, PCHAOS) && !F_ISSET(rom_ptr, RSUVIV)) {
                    print(fd, "\n�̾������� ����� ���մϴ�.\n");
                    return(0);
                }
                if(!F_ISSET(crt_ptr, PCHAOS) && !F_ISSET(rom_ptr, RSUVIV)) {
                    print(fd, "\n�̾������� �׻���� ���մϴ�.\n");
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
			print(fd, "\n�����縸�� �� �� �ִ� �����Դϴ�.\n");
			return(0);
		}
        if(!strncmp(cmnd->str[1], spllist[38].splstr,strlen(cmnd->str[1])))
		if(ply_ptr->class != MAGE && ply_ptr->class < INVINCIBLE) {
			print(fd, "\n�����縸�� �� �� �ִ� �����Դϴ�.\n");
			return(0);
		}
        if(!strncmp(cmnd->str[1], spllist[39].splstr,strlen(cmnd->str[1])))
		if(ply_ptr->class != MAGE && ply_ptr->class < INVINCIBLE) {
			print(fd, "\n�����縸�� �� �� �ִ� �����Դϴ�.\n");
			return(0);
		}
        if(!strncmp(cmnd->str[1], spllist[40].splstr,strlen(cmnd->str[1])))
		if(ply_ptr->class != MAGE && ply_ptr->class < INVINCIBLE) {
			print(fd, "\n�����縸�� �� �� �ִ� �����Դϴ�.\n");
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
           print(fd, "\n����� %s �ֹ��� %s���� �ɾ����ϴ�.\n", spellname,
                crt_ptr->name);
            broadcast_rom2(fd, crt_ptr->fd, ply_ptr->rom_num,
                       "\n%M�� %s �ֹ��� %M���� �ܿ�ϴ�.",
                       ply_ptr, spellname, crt_ptr);



           /*  ���� �����ֹ�  */
        /* ��ǳ */  
        if(!strncmp(cmnd->str[1], spllist[1].splstr,strlen(cmnd->str[1]))) {
              print(fd, "��ǳ�Ҹ���... ��ǳ�̿�! ���� ������ ��� ���� �Ҹ���Ѷ�.\n�ֹ��� �ܿ��� �Ϲ����κ��� Į���� ���� �ż� �ٶ��� �Ҿ� ��ɿ� ���� �����մϴ�.\n");
              broadcast_rom(fd, ply_ptr->rom_num, "\n%M�� �ֹ��� �ܿ��� �Ϲ����κ��� Į���� ���� �ż� �ٶ���\n�Ҿ� ��ɿ� ���� �����մϴ�.",ply_ptr);
              }
        /* ��ȭ��  */  
        if(!strncmp(cmnd->str[1], spllist[6].splstr,strlen(cmnd->str[1]))) {
              print(fd, "���ȭ����... ���ȭ�� ȭ���� ��� ���� �¿�������.\n���� ���� �⸦ ������ �� ������ ������ �Ҳ��� ������ ������ ������ ���󰩴ϴ�.\n");
              broadcast_rom(fd, ply_ptr->rom_num, "\n%M�� ���� ���� �⸦ ������ �� ������ ������ �Ҳ��� ������\n������ ������ ���󰩴ϴ�.",ply_ptr);
              }
        /* ���� */  
        if(!strncmp(cmnd->str[1], spllist[13].splstr,strlen(cmnd->str[1]))) {
              print(fd, "����õ��... �ϴ��� �г밡 ����� ���ϴ� �װ��� �����̷δ�.\n����� ������ �ϴ÷� ���� ������ ���ʴ����� Ǫ�������� �۷��ϸ鼭 ���� ���ϴ�.\n");
              broadcast_rom(fd, ply_ptr->rom_num, "\n%M�� ����� ������ �ϴ÷� ���� ������ ���ʴ����� Ǫ��������\n�۷��ϸ鼭 ���� ���ϴ�.",ply_ptr);
              }
        /* ������ */  
        if(!strncmp(cmnd->str[1], spllist[14].splstr,strlen(cmnd->str[1]))) {
              print(fd, "������... �Ϲ浿�屺�� ���� �ҵ� ���������... ��ĥ���� ������..\n�ֹ��� �ܿ�� ������ ������ ��� �ϴ÷� ������ ��� ���� �� �� �� ������ �ָ��� Ĩ�ϴ�.\n");
              broadcast_rom(fd, ply_ptr->rom_num, "\n%M�� �ֹ��� �ܿ�� ������ ������ ��� �ϴ÷� ������ ��� ����\n�� �� �� ������ �ָ��� Ĩ�ϴ�.",ply_ptr);
              }
        /* �İݼ� */  
        if(!strncmp(cmnd->str[1], spllist[25].splstr,strlen(cmnd->str[1]))) {
              print(fd, "�İ���... ������ ����̿�, ������ ����� ���� ������ ����.\n�ֹ��� �ܿ�� ���� ������ ��ǳ�� �ҿ���̸� ����Ű�� ���Ǹ� ���� �����ϴ�.\n");
              broadcast_rom(fd, ply_ptr->rom_num, "\n%M�� �ֹ��� �ܿ�� ���� ������ ��ǳ�� �ҿ���̸� ����Ű��\n���Ǹ� ���� �����ϴ�.",ply_ptr);
              }
        /* ������ */  
        if(!strncmp(cmnd->str[1], spllist[26].splstr,strlen(cmnd->str[1]))) {
              print(fd, "������... ������ ���ɵ��̿� ���� ����� ��� ���� �����϶�.\n������ �ִ� ���������� �������� ���� ����� �Ѿ� ������ �մϴ�.\n");
              broadcast_rom(fd, ply_ptr->rom_num, "\n%M�� �ֹ��� �ܿ��� ������ �ִ� ���������� �������� ���� \n����� �Ѿ� ������ �մϴ�.",ply_ptr);
              }
        /* ȭ���� */  
        if(!strncmp(cmnd->str[1], spllist[27].splstr,strlen(cmnd->str[1]))) {
              print(fd, "ȭ�⼱��... �ϵ��� ȭ�μ��� ��߿� �Ǹ��� �̰����� ��� ���� �¿츮��.\n� ������ �ִ� ���� ��ߵ��� �ϴ÷� ���ƿö� ���� �������� ���� ���� ��Ÿ �����ϴ�.\n");
              broadcast_rom(fd, ply_ptr->rom_num, "\n%M�� � ������ �ִ� ���� ��ߵ��� �ϴ÷� ���ƿö� ����\n�������� ���� ���� ��Ÿ �����ϴ�.",ply_ptr);
              }
        /* ������ */  
        if(!strncmp(cmnd->str[1], spllist[28].splstr,strlen(cmnd->str[1])))  {
              print(fd, "ź��������... ������ ������ �� ���� ������ ���� ������ ��������.\n�� ���� ������ �̽��� �����鼭 �װ��� ƨ���� �Ѿ˰��� ���� ���� ���� ��վ� �����ϴ�.\n");
              broadcast_rom(fd, ply_ptr->rom_num, "\n%M�� �� ���� ������ �̽��� �����鼭 �װ��� ƨ���� �Ѿ˰���\n���� ���� ���� ��վ� �����ϴ�.",ply_ptr);
              }
        /* ȸ����*/  
        if(!strncmp(cmnd->str[1], spllist[29].splstr,strlen(cmnd->str[1])))  {
              print(fd, "����������... ������ ���� �ٶ��� ���� ��Ű�� ������ ���� �����ϸ���.\n�ֹ��� �ܹ��� ���� ������ ���ڱ� ���� ��ǳ�� ����� ���� ���� ���� �����ϴ�.\n");
              broadcast_rom(fd, ply_ptr->rom_num, "\n%M�� �ֹ��� �ܹ��� ���� ������ ���ڱ� ���� ��ǳ�� �����\n���� ���� ���� �����ϴ�.",ply_ptr);
              }
        /* ���� */                                                           
        if(!strncmp(cmnd->str[1], spllist[30].splstr,strlen(cmnd->str[1])))  {
              print(fd, "���ʽ���... ���˵��� �ĵ��� �θ��� �Ŵ��� �ٴ��� ������ ���� ������������.\n�Ҹſ� �������ִ� ���� ���� �̾� ���ʸ� �Ѹ��� �� �ȿ� ������ �ִ� ���� ������ ���濡�� ����˴ϴ�.\n");
              broadcast_rom(fd, ply_ptr->rom_num, "\n%M�� �Ҹſ� �������ִ� ���� ���� ������ ���ʸ� �Ѹ��� \n�� �ȿ� ������ �ִ� ���� ������ ���濡�� ����˴ϴ�.",ply_ptr);
              }
        /* ���� */  
        if(!strncmp(cmnd->str[1], spllist[31].splstr,strlen(cmnd->str[1])))  {
              print(fd, "����ȣ����... ������ �θ��� �����̿� ������ �ö�� ���� ���� ������.\n����� ���� ���� ���� �������鼭 ������ �ö�� ��ī�ο� �������� ���� �����մϴ�.\n");
              broadcast_rom(fd, ply_ptr->rom_num, "\n%M�� ����� ���� ���� ���� �������鼭 ������ �ö�� ��ī�ο�\n�������� ���� �����մϴ�.",ply_ptr);
              }
        /* ���� */  
        if(!strncmp(cmnd->str[1], spllist[32].splstr,strlen(cmnd->str[1])))  {
              print(fd, "������... ��ȣ�� ���� �굵 ���ʶ߸��� ����� �����̿� �Ͽ츦 ����߷���.\n���ڱ� ���� �ִ� �길�� ������ �����ϸ鼭 Ŀ�ٶ� �������� ������ �������ϴ�.\n");
              broadcast_rom(fd, ply_ptr->rom_num, "\n%M�� ���� �ִ� �길�� ������ ���ڱ� �����ϸ鼭 Ŀ�ٶ�\n�������� ������ �������ϴ�.",ply_ptr);
              }
        /* ȭǳ�� */  
        if(!strncmp(cmnd->str[1], spllist[33].splstr,strlen(cmnd->str[1]))) {
              print(fd, "ȭǳ��... ȭ�������� �Ա��� �ٶ����� ��Ÿ�� ���� �¿��.\n������ ������ �¿�� �ϴ÷� ������ ȭ���� ��Ÿ������ Ŀ�ٶ� ȸ���� �ٶ��� ���� �ѷ��Դϴ�.\n");
              broadcast_rom(fd, ply_ptr->rom_num, "\n%M�� ������ ������ �¿�� �ϴ÷� ������ ȭ���� ��Ÿ������\nĿ�ٶ� ȸ���� �ٶ��� ���� �ѷ��Դϴ�.",ply_ptr);
              }
        /* ȭ���� */  
        if(!strncmp(cmnd->str[1], spllist[34].splstr,strlen(cmnd->str[1]))) {
              print(fd, "ȭ��ȣ����... ȭ���� ���̿� ���� �Ա����� ���� �Ҹ���Ѷ�.\n����� ���� ȭ������ ���ϸ鼭 �Ҳ��� Ÿ������ ������ ���� �����մϴ�.\n");
              broadcast_rom(fd, ply_ptr->rom_num, "\n%M�� ���� ȭ������ ���ϸ鼭 �Ҳ��� Ÿ������ ������\n���� �����մϴ�.",ply_ptr);
              }
        /* ���� */  
        if(!strncmp(cmnd->str[1], spllist[35].splstr,strlen(cmnd->str[1]))) {
              print(fd, "�������... ������ ���� ���� ���ѹ����� ��� �˰��� �����ϸ�...\n���ڱ� ���� �ٷ� ������ ������ �Ͼ ���� �������鼭 ���� ���ѹ����ϴ�.\n");
              broadcast_rom(fd, ply_ptr->rom_num, "\n%M�� ���ڱ� ���� �ٷ� ������ ������ �Ͼ ���� �������鼭\n���� ���ѹ����ϴ�.",ply_ptr);
              }
        /* ȭ���� */  
        if(!strncmp(cmnd->str[1], spllist[36].splstr,strlen(cmnd->str[1]))) {
              print(fd, "����ȣ����... ������ ȭ�� ������ �����̴� õ�󿡼� ������ ���� ��ȭ��Ű��...\n�ֹ��� �ܿ��� ���ڱ� �Ҳ��� ���մ� ������ ������ ������ ���¿� �����ϴ�.\n");
              broadcast_rom(fd, ply_ptr->rom_num, "\n%M�� �ֹ��� �ܿ��� ���ڱ� �Ҳ��� ���մ� ������ ������\n������ ���¿� �����ϴ�.",ply_ptr);
              }
        /* ����õ */  
        if(!strncmp(cmnd->str[1], spllist[37].splstr,strlen(cmnd->str[1]))) {
              print(fd, "����õ��... ������ �Ҳ��� �̰����� ��ġ�� ��� ���� �̰��� ��������.\n�����ϴ� ���� ��鸮���� ���ڱ� ����� �������� ���� ��� �װ��� �����ϴ�.\n");
              broadcast_rom(fd, ply_ptr->rom_num, "\n%M�� �ֹ��� �ܿ��� �����ϴ� ���� ��鸮���� ���ڱ� �����\n�������� ���� ��� �װ��� �����ϴ�.",ply_ptr);
              }
        /* ��õ�� */  
        if(!strncmp(cmnd->str[1], spllist[38].splstr,strlen(cmnd->str[1]))) {
              print(fd, "��õ����... ����õ���� ������ ���� �������� ��Ÿ�� ���� �����Ѵ�.\n���ڱ� ���� ������ ��Ÿ�� õ���� ���� ������ �����ϸ鼭 ���� ��Ÿ�մϴ�.\n");
              broadcast_rom(fd, ply_ptr->rom_num, "\n%M�� �ֹ��� �ܿ��� ���ڱ� ���� ������ ��Ÿ�� õ���� \n���� ������ �����ϸ鼭 ���� ��Ÿ�մϴ�.",ply_ptr);
              }
        /* ���� */  
        if(!strncmp(cmnd->str[1], spllist[39].splstr,strlen(cmnd->str[1]))) {
              print(fd, "��������... ������ �������̿� ������� ���� ���� ���� ��Ÿ����.\n������ ���� �Ȱ��� ���̸� ���� �� 33���� ������ ��Ÿ�� ���� ������ ������ �����ϴ�.\n");                     
              broadcast_rom(fd, ply_ptr->rom_num, "\n%M�� ������ ���� �Ȱ��� ���̸� ���� �� 33���� ������ ��Ÿ�� ���� ������ ������ �����ϴ�.",ply_ptr);
              }
        /* �Һ��� */  
        if(!strncmp(cmnd->str[1], spllist[40].splstr,strlen(cmnd->str[1]))) {
              print(fd, "ȭ��������... õ�������� ������ ������ ��Ÿ�� ��� ���� �Ҹ� ��Ű����.\n���� ���� �ֹ��� �ܿ��� ������ ���� ���մ� �������� �����鼭 ��� ���� ���¿� �����ϴ�.\n"); 
              broadcast_rom(fd, ply_ptr->rom_num, "\n%M�� ���� ���� �ֹ��� �ܿ��� ������ ���� ���մ� �������� �����鼭 ��� ���� ���¿� �����ϴ�.",ply_ptr);
              }
            
            
            
            print(fd, "\n�ֹ��� %d��ŭ�� ���ظ� �־����ϴ�.\n", dmg);
            print(crt_ptr->fd, 
                  "\n%M�� %s �ּ��� ��ſ��� %d��ŭ�� ���ظ� �־����ϴ�.\n",
                  ply_ptr, spellname, dmg);
            display_status(fd, crt_ptr);
            print(fd, "\n");
        }


        if(crt_ptr->hpcur < 1) {
            print(fd, "\n����� %M�� �׿����ϴ�.\n", crt_ptr);
            broadcast_rom2(fd, crt_ptr->fd, ply_ptr->rom_num,
                      "\n%M�� %M�� �׿����Ƚ��ϴ�.\n", ply_ptr, crt_ptr);
            die(crt_ptr, ply_ptr);
            return(2);
        }

    }

    return(1);

}
