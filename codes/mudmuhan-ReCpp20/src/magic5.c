/*
 * MAGIC5.C:
 *
 *      Additional spell-casting routines.
 *
 *      Copyright (C) 1991, 1992, 1993 Brett J. Vickers
 *
 */

#include "mstruct.h"
#include "mextern.h"

/**********************************************************************/
/*                              recall                                 */
/**********************************************************************/

/* This function allows a player to teleport himself or another player */
/* to room #1 (Town Square)                                           */

int recall(ply_ptr, cmnd, how)
creature *ply_ptr;
cmd            *cmnd;
int              how;
{
        creature *crt_ptr;
        room           *rom_ptr, *new_rom;
        int              fd;

        fd = ply_ptr->fd;
        rom_ptr = ply_ptr->parent_rom;

        if(ply_ptr->mpcur < 30 && how == CAST) {
                print(fd, "\n����� ������ �����մϴ�.\n");
                return(0);
        }

        if(ply_ptr->class != CLERIC && 
           ply_ptr->class < INVINCIBLE && how == CAST) {
                print(fd, "\n�����ڸ��� �� �ּ��� ����� �� �ֽ��ϴ�.\n");
                return(0);
        }

        if(!S_ISSET(ply_ptr, SRECAL) && how == CAST) {
                print(fd, "\n����� ���� �׷� �ֹ��� �͵����� ���߽��ϴ�.\n");
                return(0);
        }

        /* Cast recall on self */
        if(cmnd->num == 2) {

                if(how == CAST)
                        ply_ptr->mpcur -= 30;

                if(how == CAST || how == SCROLL || how == WAND) {
                        print(fd, "��ȯ �ֹ��� �ܿ����ϴ�.\n");
                        broadcast_rom(fd, ply_ptr->rom_num, 
                                      "\n%M�� %s �ڽſ��� ��ȯ �ֹ��� �ܿ����ϴ�.\n",
                                      ply_ptr,
                                      F_ISSET(ply_ptr, PMALES) ? "��":"�׳�");
                }
                else if(how == POTION) {
                        print(fd, "����� ����� �������� ��鸳�ϴ�.\n");
                        broadcast_rom(fd, ply_ptr->rom_num,
                                "%M�� ��������ϴ�.", ply_ptr);
                }

                if(load_rom(1001, &new_rom) < 0) {
                        print(fd, "�ֹ��� �����߽��ϴ�.\n");
                        return(0);
                }

                del_ply_rom(ply_ptr, rom_ptr);
                add_ply_rom(ply_ptr, new_rom);

                return(1);
        }

        /* Cast word of recall on another player */
        else {

                if(how == POTION) {
                        print(fd, "\n�� ������ �ڽſ��Ը� ����Ҽ� �ֽ��ϴ�.\n");
                        return(0);
                }

                cmnd->str[2][0] = up(cmnd->str[2][0]);
                crt_ptr = find_crt(ply_ptr, rom_ptr->first_ply,
                                   cmnd->str[2], cmnd->val[2]);

                if(!crt_ptr) {
                        print(fd, "\n�׷� ����� �������� �ʽ��ϴ�.\n");
                        return(0);
                }

                if(how == CAST)
                        ply_ptr->mpcur -=30;

                if(how == CAST || how == SCROLL || how == WAND) {
                        print(fd, "��ȯ �ֹ��� %M���� �ܿ����ϴ�.\n", crt_ptr);
                        print(crt_ptr->fd, 
                              "%M�� ��ſ��� ��ȯ �ֹ��� �ܿ����ϴ�.\n",
                              ply_ptr);
                        broadcast_rom2(fd, crt_ptr->fd, ply_ptr->rom_num,
                                       "%M�� %M���� ��ȯ �ֹ��� �ܿ����ϴ�.",
                                       ply_ptr, crt_ptr);

                        if(load_rom(1, &new_rom) < 0) {
                                print(fd, "�ֹ��� �����߽��ϴ�.\n");
                                return(0);
                        }

                        del_ply_rom(crt_ptr, rom_ptr);
                        add_ply_rom(crt_ptr, new_rom);

                        return(1);
                }
        }

        return(1);

}

/**********************************************************************/
/*                              summon                              */
/**********************************************************************/

/* This function allows players to cast summon spells on anyone who is */
/* in the game, taking that person to your current room.         */

int summon(ply_ptr, cmnd, how)
creature *ply_ptr;
cmd            *cmnd;
int              how;
{
        creature *crt_ptr;
        room           *rom_ptr, *new_rom;
        int              fd, n;

        fd = ply_ptr->fd;
        rom_ptr = ply_ptr->parent_rom;

	if(ply_ptr->class == INVINCIBLE || ply_ptr->class == CARETAKER) {
		if(ply_ptr->mpcur < 100 && how == CAST) {
                print(fd, "\n����� ������ �����մϴ�.\n");
                return(0);
                }
        }
        else {
        	if(ply_ptr->mpcur < 50 && how == CAST) {
        	print(fd, "\n����� ������ �����մϴ�.\n");
        	return(0);
        	}
        }

        if(!S_ISSET(ply_ptr, SSUMMO) && how == CAST) {
                print(fd, "\n����� ���� �׷� �ּ��� �͵����� ���߽��ϴ�.\n");
                return(0);
        }
		if(mrand(1,100) < 51) {
				print(fd, "\n��ȯ�� ���и� �Ͽ����ϴ�.\n");
				ply_ptr->mpcur -= 50;
				return(0);
		}

        if(cmnd->num == 2) {
                print(fd, "\n�ڽ��� ��ȯ�ϴٴ�?.\n");
                return(0);
        }

        else {

                if(how == POTION) {
                        print(fd, "\n�� ������ �ڽſ��Ը� ����Ҽ� �ֽ��ϴ�.\n");
                        return(0);
                }

                cmnd->str[2][0] = up(cmnd->str[2][0]);
                crt_ptr = find_who(cmnd->str[2]);

                if(!crt_ptr || crt_ptr == ply_ptr || F_ISSET(crt_ptr, PDMINV)) {
                        print(fd, 
                              "\n�׷� ����� �� ã���ϴ�.\n");
                        return(0);
                }

                if(how == CAST) { 
                	if(ply_ptr->class == INVINCIBLE || ply_ptr->class == CARETAKER)
                        	ply_ptr->mpcur -= 100;
                        else 	ply_ptr->mpcur -= 50;
                }

                n = count_vis_ply(rom_ptr);
                if(F_ISSET(rom_ptr, RNOTEL) ||
                  (F_ISSET(rom_ptr, RONEPL) && n > 0) ||
                  (F_ISSET(rom_ptr, RTWOPL) && n > 1) ||
                  (F_ISSET(rom_ptr, RTHREE) && n > 2)) {
                        print(fd, "�ֹ��� �������� ������ϴ�.\n");
                        return(0);
                }
                if(F_ISSET(rom_ptr, RFAMIL) && !F_ISSET(crt_ptr, PFAMIL)) {
                	print(fd, "�׻���� �аŸ� �����ڰ� �ƴմϴ�.");
                	return(0);
                }
                if(F_ISSET(rom_ptr, RONFML) && (crt_ptr->daily[DL_EXPND].max != rom_ptr->special)) {
                	print(fd, "�׻���� �̰��� �ü� �����ϴ�.");
                	return(0);
                }

                if(rom_ptr->lolevel > crt_ptr->level ||
                   (crt_ptr->level > rom_ptr->hilevel && rom_ptr->hilevel) ||
                   F_ISSET(crt_ptr, PNOSUM)) {
                        print(fd, "\n�ֹ��� �����߽��ϴ�.\n");
                        if(F_ISSET(crt_ptr,PNOSUM)) 
                            print(fd,"��밡 ��ȯ �ź� ���Դϴ�.");
                        return(0);
                }

                if(F_ISSET(crt_ptr->parent_rom,RNOLEA))
                {
                        print(fd, "\n��ȯ �ֹ��� %M�� ã�� ���մϴ�.\n",crt_ptr);
                        return(0);
                }

                if(how == CAST || how == SCROLL || how == WAND) {
                        print(fd, "\n����� %M�� ��ȯ�ϱ� ���� �ֹ��� �ܿ�ϴ�.\n�ֹ��� ��ġ�� £�� �Ȱ��� ������ ���ڱ� ������鼭 \n�װ� ��Ÿ�����ϴ�.\n", crt_ptr);
                        print(crt_ptr->fd, 
                              "\n��������� £�� �Ȱ��� ������ �� �� ���� ���� �̲��� ���а� ���󰩴ϴ�.\n�Ȱ��� ������ %M�� ��žտ� �� �ֽ��ϴ�.\n",
                              ply_ptr);
                        broadcast_rom2(fd, crt_ptr->fd, ply_ptr->rom_num,
                                       "\n%M�� ��ȯ�ֹ��� �ܿ��� £�� �Ȱ��� �򸮴��� ���ڱ� %M�� ��Ÿ�����ϴ�.\n",
                                       ply_ptr, crt_ptr);

                        del_ply_rom(crt_ptr, crt_ptr->parent_rom);
                        add_ply_rom(crt_ptr, rom_ptr);

                        return(1);
                }
        }

        return(1);

}

/**********************************************************************/
/*                              heal(��ġ��)                                   */
/**********************************************************************/

/* This function will cause the heal spell to be cast on a player or    */
/* another monster.  It heals all hit points damage but only works 3      */
/* times a day.                                                         */

int heal(ply_ptr, cmnd, how)
creature *ply_ptr;
cmd            *cmnd;
int              how;
{
        creature *crt_ptr;
        room           *rom_ptr;
        int              fd, heal;

        fd = ply_ptr->fd;
        rom_ptr = ply_ptr->parent_rom;

        if(ply_ptr->mpcur < 20 && how == CAST) {
                print(fd, "\n����� ������ �����մϴ�.\n");
                return(0);
        }

        if(ply_ptr->class != CLERIC && ply_ptr->class != PALADIN &&
           ply_ptr->class < INVINCIBLE && how == CAST) {
                print(fd, "\n�����ڿ� ���縸�� �� �ּ��� ����� �� �ֽ��ϴ�.\n");
                return(0);
        }

        if(!S_ISSET(ply_ptr, SFHEAL) && how == CAST) {
                print(fd, "\n����� ���� �׷� �ּ��� �͵����� ���߽��ϴ�.\n");
                return(0);
        }

        /* Heal self */
        if(cmnd->num == 2 ) {

                if(!dec_daily(&ply_ptr->daily[DL_FHEAL]) && how == CAST &&
                   ply_ptr->class < CARETAKER) {
                        print(fd, "\n����� ���� �ʹ� �ǰ��� �� �ּ��� �� �̻� ��ĥ �� �����ϴ�.\n");
                        return(0);
                }

                ply_ptr->hpcur = ply_ptr->hpmax;

                if(how == CAST) 
                        ply_ptr->mpcur -= 20;

                if(how == CAST || how == SCROLL) {
                        print(fd, "\n����� õ�ΰ��� ����ø��� ��ġ �ֹ��� �ܿ�ϴ�.\nõ���� ������ ����� ������ ���̸鼭 ü���� �ֻ����� \n�÷� �ݴϴ�.\n");
                        broadcast_rom(fd, ply_ptr->rom_num, 
                                      "\n%M�� õ�ΰ� �ڼ��� ���ϸ鼭 ��ġ�ֹ��� �ܿ����ϴ�.\nõ���� ������ �׿��Է� ���̴� ���� �������ϴ�.\n",
                                      ply_ptr);
                        return(1);
                }
                else {
                        print(fd, "\n���ڱ� ���� �����鼭 ���� ü���� �ְ��� ���·� �ö󰩴ϴ�.\n\"�̾�~~��.. ���� ��ģ��.\"\n");
                        return(1);
                }
        }

        /* Cast heal on another player or monster */
        else {

                if(how == POTION) {
                        print(fd, "�� ������ �ڽſ��Ը� ��밡���մϴ�.\n");
                        return(0);
                }

                cmnd->str[2][0] = up(cmnd->str[2][0]);
                crt_ptr = find_crt(ply_ptr, rom_ptr->first_ply,
                                   cmnd->str[2], cmnd->val[2]);

                if(!crt_ptr) {
                        cmnd->str[2][0] = low(cmnd->str[2][0]);
                        crt_ptr = find_crt(ply_ptr, rom_ptr->first_mon,
                                           cmnd->str[2], cmnd->val[2]);

                        if(!crt_ptr) {
                                print(fd, "\n�׷� ����� �������� ���ϴ�.\n");
                                return(0);
                        }
                }

                if(!dec_daily(&ply_ptr->daily[DL_FHEAL]) && how == CAST &&
                   ply_ptr->class < CARETAKER && ply_ptr->type != MONSTER) {
                        print(fd, "\n����� ���� �ʹ� �ǰ��� �� �ּ��� �� �̻� ��ĥ �� �����ϴ�.\n");
                        return(0);
                }

                crt_ptr->hpcur = crt_ptr->hpmax;

                if(how == CAST) 
                        ply_ptr->mpcur -= 20;
                if(how == CAST || how == SCROLL || how == WAND) {
                        print(fd, "����� %M���� ��ġ������ ���̸� �ֹ��� �ܿ�ϴ�.\n���ڱ� ���� ������ ���� ������ �Ͼ�鼭 ü���� \nȸ���Ǵ� ���� �������ϴ�.", crt_ptr);
                        print(crt_ptr->fd, "%M�� ��ſ��� ��ġ������ ���̸� �ֹ��� �ܿ�ϴ�.\n���ڱ� ����� ������ ���� ������ �Ͼ�鼭 ü���� \nȸ���Ǵ� ���� �������ϴ�.",
                              ply_ptr);
                        broadcast_rom2(fd, crt_ptr->fd, ply_ptr->rom_num,
                                       "%M�� %M���� ��ġ������ ���̸� �ֹ��� �ܿ�ϴ�.\n���� ���� ���� ������ ����Ű�ٰ� ü���� ȸ��������\n��½��½ �پ� �ٴմϴ�.\n",
                                       ply_ptr, crt_ptr);
                        return(1);
                }
        }

        return(1);

}

/**********************************************************************/
/*                              track(����)                                    */
/**********************************************************************/

/* This function allows rangers to cast the track spell which takes them */
/* to any other player in the game.                                       */

int magictrack(ply_ptr, cmnd, how)
creature *ply_ptr;
cmd            *cmnd;
int              how;
{
        creature *crt_ptr;
        room           *rom_ptr, *new_rom;
        int              fd, n;

        fd = ply_ptr->fd;
        rom_ptr = ply_ptr->parent_rom;

        if(ply_ptr->class != RANGER && ply_ptr->class < INVINCIBLE &&
           how==CAST) {
                print(fd, "\n�������� �� �ּ��� ����� �� �ֽ��ϴ�.\n");
                return(0);
        }

        if(ply_ptr->mpcur < 13 && how == CAST) {
                print(fd, "\n����� ������ �����մϴ�.\n");
                return(0);
        }

        if(!S_ISSET(ply_ptr, STRACK) && how == CAST) {
                print(fd, "\n����� ���� �׷� �ּ��� �͵����� ���߽��ϴ�.\n");
                return(0);
        }

        if(cmnd->num == 2) {
                print(fd, "\n��� �ڽ��� �����Ѵٰ��?.\n");
                return(0);
        }

        else {

                if(how == POTION) {
                        print(fd, "\n�� ������ �ڽſ��Ը� ����Ҽ� �ֽ��ϴ�.\n");
                        return(0);
                }

                cmnd->str[2][0] = up(cmnd->str[2][0]);
                crt_ptr = find_who(cmnd->str[2]);

                if(!crt_ptr || crt_ptr == ply_ptr || F_ISSET(crt_ptr, PDMINV)) {
                        print(fd, 
                              "\n�׷� ����� �������� �ʽ��ϴ�.\n");
                        return(0);
                }

                if(crt_ptr->class > CARETAKER) {
                        print(fd, "\n�� ����� ��� �ִ��� �����Ҽ� �����ϴ�.\n");
                        return(0);
                }
/*                
                if(F_ISSET(crt_ptr, PMARRI)) {
                	print(fd, "\n�� ����� ��� �ִ��� ������ �� �����ϴ�.\n");
                	return(0);
                }
*/

                n = count_vis_ply(crt_ptr->parent_rom);
                if(F_ISSET(crt_ptr->parent_rom, RNOTEL) ||
                  (F_ISSET(crt_ptr->parent_rom, RONEPL) && n > 0) ||
                  (F_ISSET(crt_ptr->parent_rom, RTWOPL) && n > 1) ||
                  (F_ISSET(crt_ptr->parent_rom, RTHREE) && n > 2)) {
                        print(fd, "\n�ֹ��� �����߽��ϴ�.\n");
                        return(0);
                }
                if(F_ISSET(crt_ptr->parent_rom, RFAMIL) && !F_ISSET(ply_ptr, PFAMIL)) {
                	print(fd, "�� ����� �ִ� ������ �� ���� �����ϴ�.");
                	return(0);
                }
                if(F_ISSET(crt_ptr->parent_rom, RONFML) && (ply_ptr->daily[DL_EXPND].max != crt_ptr->parent_rom->special)) {
                	print(fd, "�� ����� �ִ� ������ �� ���� �����ϴ�.");
                	return(0);
                }

                if(crt_ptr->parent_rom->lolevel > ply_ptr->level ||
                   (ply_ptr->level > crt_ptr->parent_rom->hilevel &&
                   crt_ptr->parent_rom->hilevel)) {
                        print(fd, "\n�ֹ��� �����߽��ϴ�.\n");
                        return(0);
                }

                if(!dec_daily(&ply_ptr->daily[DL_TRACK]) && how == CAST &&
                   ply_ptr->class < CARETAKER) {
                        print(fd, "\n����� ���� �ʹ� �ǰ��� �� �ּ��� �� �̻� ��ĥ �� �����ϴ�.\n");
                        return(0);
                }

                if(how == CAST)
                        ply_ptr->mpcur -= 13;

                if(how == CAST || how == SCROLL || how == WAND) {
                        print(fd, "\n!!����� %M�� ������ ã�Ƴ��µ� �����߽��ϴ�.!!\n�׸� �����Ͽ� �޷����ϴ�.\n", crt_ptr);
                        print(crt_ptr->fd, 
                              "\n%M�� ����� ������ ã�� ���µ� �����Ͽ� ����� \nã�� �Խ��ϴ�.\n", ply_ptr);
                        broadcast_rom2(fd, crt_ptr->fd, ply_ptr->rom_num,
                                       "\n%M�� %M�� ������ ã�Ƴ��µ� �����Ͽ� \n������ �����߽��ϴ�.\n", ply_ptr, crt_ptr);

                        del_ply_rom(ply_ptr, rom_ptr);
                        add_ply_rom(ply_ptr, crt_ptr->parent_rom);

                        return(1);
                }
        }

        return(1);

}

/**********************************************************************/
/*                              levitate(�ξ��)                        */
/**********************************************************************/

/* This function allows players to cast the levitate spell, thus allowing */
/* them to levitate over traps or up mountain cliffs.                        */

int levitate(ply_ptr, cmnd, how)
creature *ply_ptr;
cmd            *cmnd;
int              how;
{
        creature *crt_ptr;
        room           *rom_ptr, *new_rom;
        int              fd;

        fd = ply_ptr->fd;
        rom_ptr = ply_ptr->parent_rom;

        if(ply_ptr->mpcur < 10 && how == CAST) {
                print(fd, "\n����� ������ �����մϴ�.\n");
                return(0);
        }

        if(!S_ISSET(ply_ptr, SLEVIT) && how == CAST) {
                print(fd, "\n����� ���� �׷� �ּ��� �͵����� ���߽��ϴ�.\n");
                return(0);
        }
        if(spell_fail(ply_ptr)) {
                if(how == CAST)
                     ply_ptr->mpcur -= 10;
                return(0);
        }

        if(cmnd->num == 2) {
                ply_ptr->lasttime[LT_LEVIT].ltime = time(0);
                F_SET(ply_ptr, PLEVIT);
                broadcast_rom(fd, ply_ptr->rom_num, 
                        "\n%M�� ����� ���� ������ �ξ���� �ֹ��� �ܿ���\n���� ��¦ �������ϴ�.", ply_ptr);
                if(how == CAST) {
                        print(fd, "\n����� ����� ������ �ξ���� �ֹ��� �ܿ��� ����\n��¦ �������ϴ�.\n");
                        ply_ptr->mpcur -= 10;
                        ply_ptr->lasttime[LT_LEVIT].interval = 2400L +
                                bonus[ply_ptr->intelligence]*600L;
if (F_ISSET(ply_ptr->parent_rom,RPMEXT)){
            print(fd,"\n�� ���� ����� ����� �ֹ��� ��ȭ��ŵ�ϴ�.\n");
            ply_ptr->lasttime[LT_LEVIT].interval += 800L;
        }                                
                }
                else {
                        print(fd, "\n����� ���� �������� ��¦ �������ϴ�.\n");
                        ply_ptr->lasttime[LT_LEVIT].interval = 1200L;
                }
                return(1);
        }
        else {

                if(how == POTION) {
                        print(fd, "\n�� ������ �ڽſ��Ը� ����Ҽ� �ֽ��ϴ�.\n");
                        return(0);
                }

                cmnd->str[2][0] = up(cmnd->str[2][0]);
                crt_ptr = find_crt(ply_ptr, rom_ptr->first_ply,
                                   cmnd->str[2], cmnd->val[2]);
                if(!crt_ptr) {
                        print(fd, "\n�׷� ����� �������� �ʽ��ϴ�.\n");
                        return(0);
                }

                F_SET(crt_ptr, PLEVIT);
                crt_ptr->lasttime[LT_LEVIT].ltime = time(0);
                broadcast_rom2(fd, crt_ptr->fd, ply_ptr->rom_num, 
                        "\n%M�� %M���� �ξ������ ������ �ֹ��� �ܿ�ϴ�.\n�ֹ��� �ܿ��� ���� ���� ��¦ �������ϴ�.\n", ply_ptr, crt_ptr);
                print(crt_ptr->fd, "\n%M�� ��ſ��� �ξ������ ������ �ֹ��� �ܿ�ϴ�.\n����� ���� ��¦ �������� ���� �մϴ�.\n", ply_ptr);

                if(how == CAST) {
                        print(fd, "\n����� %M���� �ξ���� �� �� �����ϴ�.\n",crt_ptr);
                        ply_ptr->mpcur -= 10;
                        crt_ptr->lasttime[LT_LEVIT].interval = 2400L +
                                bonus[ply_ptr->intelligence]*600L;
                if (F_ISSET(ply_ptr->parent_rom,RPMEXT)){
                  print(fd,"\n�� ���� ����� ����� �ֹ��� ��ȭ��ŵ�ϴ�.\n");
                  crt_ptr->lasttime[LT_LEVIT].interval += 800L;
                }                                
                }

                else {
                        print(fd, "%M�� ���ٴϱ� �����մϴ�.\n", crt_ptr);
                        crt_ptr->lasttime[LT_LEVIT].interval = 1200L;
                }

                return(1);
        }

}

/************************************************************************/
/*                              resist_fire(�濭��)                                */
/************************************************************************/

/* This function allows players to cast the resist fire spell.  It will        */
/* allow the player to resist fire breathed on them by dragons and        */
/* other breathers.                                                       */

int resist_fire(ply_ptr, cmnd, how)
creature *ply_ptr;
cmd            *cmnd;
int              how;
{
        creature *crt_ptr;
        room           *rom_ptr, *new_rom;
        int              fd;

        fd = ply_ptr->fd;
        rom_ptr = ply_ptr->parent_rom;

        if(ply_ptr->mpcur < 12 && how == CAST) {
                print(fd, "\n����� ������ �����մϴ�.\n");
                return(0);
        }

        if(!S_ISSET(ply_ptr, SRFIRE) && how == CAST) {
                print(fd, "\n����� ���� �׷� �ּ��� �͵����� ���߽��ϴ�.\n");
                return(0);
        }
        if(spell_fail(ply_ptr)) {
                if(how == CAST)
                     ply_ptr->mpcur -= 12;
                return(0);
        }

        if(cmnd->num == 2) {
                ply_ptr->lasttime[LT_RFIRE].ltime = time(0);
                F_SET(ply_ptr, PRFIRE);
                broadcast_rom(fd, ply_ptr->rom_num, 
                        "\n%M�� �濭���� ���и� ���ø��鼭 �ֹ��� �ܿ���ϴ�.\n������ ���� ��ȣ�ɵ��� ��Ÿ�� ���� ������ ���� �����մϴ�.\n", ply_ptr);
                if(how == CAST) {
                        print(fd, "\n����� �濭���� ���и� ���ø��鼭 �ֹ��� �ܿ�ϴ�.\n������ ���� ��ȣ�ɵ��� ��Ÿ�� ����������� ���� �����մϴ�.\n");
                        ply_ptr->mpcur -= 12;
                        ply_ptr->lasttime[LT_RFIRE].interval = MAX(300, 1200 +
                                bonus[ply_ptr->intelligence]*600);
                if (F_ISSET(ply_ptr->parent_rom,RPMEXT)){
                  print(fd,"\n�� ���� ����� ����� �ֹ��� ��ȭ��ŵ�ϴ�.\n");
                  ply_ptr->lasttime[LT_RFIRE].interval += 800L;
                }                                
                }
                else {
                        print(fd, "\n���ڱ� ������ ���� ��ȣ�ɵ��� ��Ÿ�� ��������� \n���� �����մϴ�.\n");
                        ply_ptr->lasttime[LT_RFIRE].interval = 1200L;
                }
                return(1);
        }
        else {

                if(how == POTION) {
                        print(fd, "\n�� ������ �ڽſ��Ը� ����Ҽ� �ֽ��ϴ�.\n");
                        return(0);
                }

                cmnd->str[2][0] = up(cmnd->str[2][0]);
                crt_ptr = find_crt(ply_ptr, rom_ptr->first_ply,
                                   cmnd->str[2], cmnd->val[2]);
                if(!crt_ptr) {
                        print(fd, "\n�׷� ����� �������� �ʽ��ϴ�.\n");
                        return(0);
                }

                F_SET(crt_ptr, PRFIRE);
                crt_ptr->lasttime[LT_RFIRE].ltime = time(0);
                broadcast_rom2(fd, crt_ptr->fd, ply_ptr->rom_num, 
                        "%M�� %M���� �濭������ ���̸� �ֹ��� �ܿ�ϴ�.\n������ ���� ��ȣ�ɵ��� ��Ÿ�� ���� ������ ���� �����մϴ�.\n", 
                        ply_ptr, crt_ptr);
                print(crt_ptr->fd, "\n%M�� ��ſ��� �濭������ ���̸� �ֹ��� �ܿ�ϴ�.\n���ڱ� ������ ���� ��ȣ�ɵ��� ��Ÿ�� ��������� \n���� �����մϴ�.\n", ply_ptr);

                if(how == CAST) {
                        print(fd, "����� %M���� �濭������ ���̸� �ֹ��� �ܿ�ϴ�.\n������ ���� ��ȣ�ɵ��� ��Ÿ�� ���� ������ ���� �����մϴ�.\n",
                                crt_ptr);
                        ply_ptr->mpcur -= 12;
                        crt_ptr->lasttime[LT_RFIRE].interval = MAX(300, 1200 +
                                bonus[ply_ptr->intelligence]*600);
                if (F_ISSET(ply_ptr->parent_rom,RPMEXT)){
                  print(fd,"\n�̹��� ����� ����� �ֹ��� ��ȭ��ŵ�ϴ�.\n");
                  crt_ptr->lasttime[LT_RFIRE].interval += 800L;
                }                                
                }

                else {
                        print(fd, "\n������ ���� ��ȣ�ɵ��� ��Ÿ�� %M�� ������ \n���� �����մϴ�.\n", crt_ptr);
                        crt_ptr->lasttime[LT_RFIRE].interval = 1200L;
                }

                return(1);
        }

}


/************************************************************************/
/*                              fly(����)                              */
/************************************************************************/

/* This function allows players to cast the fly spell.  It will              */
/* allow the player to fly to areas that are otherwise unreachable */
/* by foot.                                                              */

int fly(ply_ptr, cmnd, how)
creature *ply_ptr;
cmd            *cmnd;
int              how;
{
        creature *crt_ptr;
        room           *rom_ptr;
        int              fd;

        fd = ply_ptr->fd;
        rom_ptr = ply_ptr->parent_rom;

        if(ply_ptr->mpcur < 15 && how == CAST) {
                print(fd, "\n����� ������ �����մϴ�.\n");
                return(0);
        }

        if(!S_ISSET(ply_ptr, SFLYSP) && how == CAST) {
                print(fd, "\n����� ���� �׷� �ּ��� �͵����� ���߽��ϴ�.\n");
                return(0);
        }
        if(spell_fail(ply_ptr)) {
                if(how == CAST)
                     ply_ptr->mpcur -= 15;
                return(0);
        }

        if(cmnd->num == 2) {
                ply_ptr->lasttime[LT_FLYSP].ltime = time(0);
                F_SET(ply_ptr, PFLYSP);
                broadcast_rom(fd, ply_ptr->rom_num, 
                        "\n%M�� ������ �ֹ��� �ܿ��� ���� �������� \n�ϴ÷� ���� �����մϴ�.\n", ply_ptr);
                if(how == CAST) {
                        print(fd, "\n����� ������ �ֹ��� �ܿ��� ���� ����ó�� ����������\n�ϴ÷� ���ö� ���� �����մϴ�.\n");
                        ply_ptr->mpcur -= 15;
                        ply_ptr->lasttime[LT_FLYSP].interval = MAX(300, 1200 +
                                bonus[ply_ptr->intelligence]*600);
                if (F_ISSET(ply_ptr->parent_rom,RPMEXT)){
                  print(fd,"\n�� ���� ����� ����� �ֹ��� ��ȭ��ŵ�ϴ�.\n");
                  ply_ptr->lasttime[LT_FLYSP].interval += 600L;
                }                                
                }
                else {
                        print(fd, "����� ���� �ϴ÷� �������� ���� �ֽ��ϴ�!\n");
                        ply_ptr->lasttime[LT_FLYSP].interval = 1200L;
                }
                return(1);
        }
        else {

                if(how == POTION) {
                        print(fd, "\n�� ������ �ڽſ��Ը� ����Ҽ� �ֽ��ϴ�.\n");
                        return(0);
                }

                cmnd->str[2][0] = up(cmnd->str[2][0]);
                crt_ptr = find_crt(ply_ptr, rom_ptr->first_ply,
                                   cmnd->str[2], cmnd->val[2]);
                if(!crt_ptr) {
                        print(fd, "\n�׷� ����� �������� �ʽ��ϴ�.\n");
                        return(0);
                }

                F_SET(crt_ptr, PFLYSP);
                crt_ptr->lasttime[LT_FLYSP].ltime = time(0);
                broadcast_rom2(fd, crt_ptr->fd, ply_ptr->rom_num, 
                        "\n%M�� %M���� ���θ� ������ �ֹ��� �ܿ����ϴ�.\n���� ���� �ϴ÷� �������� ���� �����մϴ�.\n", 
                        ply_ptr, crt_ptr);
                print(crt_ptr->fd, "\n%M�� ��ſ��� ���θ� ������ �ֹ��� �ܿ����ϴ�.\n���ڱ� ����� ���� ����ó�� ���������� �ϴ÷� ���ö�\n���� �����մϴ�.\n", ply_ptr);

                if(how == CAST) {
                        print(fd, "\n����� %M���� ���θ� ������ �ֹ��� �ܿ�ϴ�.\n���� ���� �ϴ÷� �������� ���� �����մϴ�.\n",
                                crt_ptr);
                        ply_ptr->mpcur -= 15;
                        crt_ptr->lasttime[LT_FLYSP].interval = MAX(300, 1200 +
                                bonus[ply_ptr->intelligence]*600);
                if (F_ISSET(ply_ptr->parent_rom,RPMEXT)){
                  print(fd,"\n�� ���� ����� ����� �ֹ��� ��ȭ��ŵ�ϴ�.\n");
                  crt_ptr->lasttime[LT_FLYSP].interval += 600L;
                }                                
                }

                else {
                        print(fd, "%M�� ���� �ϴ÷� �������� ���� �����մϴ�.\n", crt_ptr);
                        crt_ptr->lasttime[LT_FLYSP].interval = 1200L;
                }

                return(1);
        }

}
