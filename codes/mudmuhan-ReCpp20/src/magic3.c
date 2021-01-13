/*
 * MAGIC3.C:
 *
 *      Additional spell-casting routines.
 *
 *      Copyright (C) 1991, 1992, 1993 Brett J. Vickers
 *
 */

#include "mstruct.h"
#include "mextern.h"

/**********************************************************************/
/*                              bless (������)                             */
/**********************************************************************/

/* This function allows a player to cast a bless spell on himself or  */
/* on another player, reducing the target's thaco by 1.          */

int bless(ply_ptr, cmnd, how)
creature *ply_ptr;
cmd            *cmnd;
int              how;
{
        creature *crt_ptr;
        room           *rom_ptr;
        long            t;
        int              fd;

        fd = ply_ptr->fd;
        rom_ptr = ply_ptr->parent_rom;
        t = time(0);

        if(ply_ptr->mpcur < 10 && how == CAST) {
                print(fd, 
                        "\n����� ������ �����մϴ�.\n");
                return(0);
        }

        if(!S_ISSET(ply_ptr, SBLESS) && how == CAST) {
                print(fd, 
                        "\n����� ���� �׷� �ּ��� �͵����� ���߽��ϴ�.\n");
                return(0);
        }
        if(spell_fail(ply_ptr)) {
                if(how==CAST)
                        ply_ptr->mpcur -= 10;
                return(0);
        }

        /* Cast bless on self */
        if(cmnd->num == 2) {

                F_SET(ply_ptr, PBLESS);
                compute_thaco(ply_ptr);

                ply_ptr->lasttime[LT_BLESS].ltime = t;
                if(how == CAST) {
                        ply_ptr->lasttime[LT_BLESS].interval = MAX(300, 1200 + 
                                bonus[ply_ptr->intelligence]*600);
                        if(ply_ptr->class == CLERIC || 
                           ply_ptr->class == PALADIN)
                                ply_ptr->lasttime[LT_BLESS].interval += 
                                60*((ply_ptr->level+3)/4);
                        ply_ptr->mpcur -= 10;
                if (F_ISSET(ply_ptr->parent_rom,RPMEXT)){
                            print(fd,
                                    "\n�� ���� ����� ����� �ּ����� ��ȭ��ŵ�ϴ�.\n");
                            ply_ptr->lasttime[LT_BLESS].interval += 800L;
                        }                                
                }
                else
                        ply_ptr->lasttime[LT_BLESS].interval = 1200;

                if(how == CAST || how == SCROLL || how == WAND) {
                        print(fd,
                                "\n����� ������ ���� ������ �����ָ� �ܿ�ϴ�.\n�����ָ� �ܿ��� �Ӹ����� ��ű��� �վ��� ���� ������ �����\n���� �ְ����ϴ�.\n");
                        broadcast_rom(fd, ply_ptr->rom_num, 
                                        "\n%M�� ������ ���� ������ �����ָ� �ܿ�ϴ�.\n���� �Ӹ����� ��ű��� �վ��� ���� ������ ����� ����\n�ְ����ϴ�.\n",
                                        ply_ptr);
                }
                else if(how == POTION)
                        print(fd, 
                                "\n�ϴÿ��� õ������ ������ ������ ����� ���� �ְ����ϴ�.\n");

                return(1);
        }

        /* Cast bless on another player */
        else {

                if(how == POTION) {
                        print(fd, 
                                "�� ������ �ڽſ��Ը� ����� �� �ֽ��ϴ�.\n");
                        return(0);
                }

                cmnd->str[2][0] = up(cmnd->str[2][0]);
                crt_ptr = find_crt(ply_ptr, rom_ptr->first_ply,
                                   cmnd->str[2], cmnd->val[2]);

                if(!crt_ptr) {
                        print(fd, 
                                "\n�׷� ����� �������� �ʽ��ϴ�.\n");
                        return(0);
                }

                F_SET(crt_ptr, PBLESS);
                compute_thaco(crt_ptr);

                crt_ptr->lasttime[LT_BLESS].ltime = t;
                if(how == CAST) {
                        crt_ptr->lasttime[LT_BLESS].interval = MAX(300, 1200 + 
                                bonus[ply_ptr->intelligence]*600);
                        if(ply_ptr->class == CLERIC || ply_ptr->class == 
                           PALADIN)
                                crt_ptr->lasttime[LT_BLESS].interval += 
                                60*((ply_ptr->level+3)/4);
                        ply_ptr->mpcur -= 10;
                if (F_ISSET(ply_ptr->parent_rom,RPMEXT)){
                            print(fd,
                                    "\n�� ���� ����� ����� �ּ����� ��ȭ��ŵ�ϴ�.\n");
                            crt_ptr->lasttime[LT_BLESS].interval += 800L;
                        }                                
                }
                else
                        crt_ptr->lasttime[LT_BLESS].interval = 1200;

                if(how == CAST || how == SCROLL || how == WAND) {
                        print(fd, 
                                "\n����� ���ʼ��� %M�� �Ӹ��� ������ �����ָ� �ܿ�ϴ�.\n���� �Ӹ����� ��ű��� �վ��� ���� �������� ����� ���� �ְ����ϴ�.\n",
                                crt_ptr);
                        print(crt_ptr->fd, 
                                "\n%M�� ����� �Ӹ��� ���ʼ��� ������ �����ָ� �ܿ�ϴ�.\n����� �Ӹ����� ��ű��� �վ��� ���� �������� ����� ����\n�ְ����ϴ�.\n",
                                ply_ptr);
                                broadcast_rom2(fd, crt_ptr->fd, ply_ptr->rom_num,
                                                    "\n%M�� %M�� �Ӹ��� ���ʼ��� ������ �����ָ� \n�ܿ�ϴ�.\n���� �Ӹ����� ��ű��� �վ��� ���� �������� ����� ����\n�ְ����ϴ�.\n",
                                                    ply_ptr, crt_ptr);
                        return(1);
                }
        }

        return(1);

}

/**********************************************************************/
/*                              turn ( ��ȥ�� )                        */
/**********************************************************************/

/* This function allows clerics and paladins to turn undead creatures. */
/* If they succeed then the creature is either disintegrated or harmed */
/* for approximately half of its hit points.                       */

int turn(ply_ptr, cmnd)
creature *ply_ptr;
cmd            *cmnd;
{
        creature *crt_ptr;
        room           *rom_ptr;
        long            i, t;
        int              chance, m, dmg, fd;

        fd = ply_ptr->fd;
        rom_ptr = ply_ptr->parent_rom;

        if(cmnd->num < 2) {
                print(fd, 
                        "\n�������� �ֹ��� �ŽǷ����?\n");
                return(0);
        }

        if(ply_ptr->class != CLERIC && ply_ptr->class != PALADIN &&
           ply_ptr->class < INVINCIBLE) {
                print(fd, 
                        "\n�����ڿ� ���縸�� ��ȥ���� ����Ҽ� �ֽ��ϴ�.\n");
                return(0);
        }

        crt_ptr = find_crt(ply_ptr, rom_ptr->first_mon,
                           cmnd->str[1], cmnd->val[1]);

        if(!crt_ptr) {
                print(fd, 
                        "\n�׷� ������ �������� �ʽ��ϴ�.\n");
                return(0);
        }

        if(!F_ISSET(crt_ptr, MUNDED)) {
                print(fd, 
                        "\n���� �������Ը� ��밡���մϴ�.\n");
                return(0);
        }

        if(F_ISSET(ply_ptr, PINVIS)) {
                F_CLR(ply_ptr, PINVIS);
                print(fd, 
                        "\n����� ����� ��Ÿ���� �����մϴ�.\n");
                broadcast_rom(fd, ply_ptr->rom_num, 
                                "%M�� ����� ���̱� �����մϴ�.",
ply_ptr);
        }

        i = LT(ply_ptr, LT_TURNS);
        t = time(0);

        if(i > t) {
                please_wait(fd, i-t);
                return(0);
        }

        if(crt_ptr->type != PLAYER && F_ISSET(crt_ptr, MUNKIL)) {
                print(fd, 
                        "\n����� %s�� ȥ�� �Ҹ��ų �� �����ϴ�.\n",
                        F_ISSET(crt_ptr, MMALES) ? "��":"�׳�");
                return(0);
        }

        add_enm_crt(ply_ptr->name, crt_ptr);

        ply_ptr->lasttime[LT_TURNS].ltime = t;
        ply_ptr->lasttime[LT_ATTCK].ltime = t;
        ply_ptr->lasttime[LT_TURNS].interval = 30L;

        chance = (((ply_ptr->level+3)/4) - ((crt_ptr->level+3)/4))*20 +
                 bonus[ply_ptr->piety]*5 + (ply_ptr->class == PALADIN ? 15:25);
        chance = MIN(chance, 80);

        if(mrand(1,100) > chance) {
                print(fd, 
                        "\n������ �ϴ÷� ������ ȥ�� ��ȯ�ϴ� ��ȥ���� �ֹ��� \n���ƽ��ϴ�.������ �ֹ��� ƨ���� �����鼭 %M�� ����� �ּ��� \n�ߵ��½��ϴ�.\n",
                        crt_ptr);
                broadcast_rom(fd, ply_ptr->rom_num, 
                                "\n%M�� ������ �ϴ÷� ������ ȥ�� ��ȯ��Ű�� ��ȥ���� \n�ֹ��� ��Ĩ�ϴ�.\n������ �ֹ��� ƨ���� �����鼭 %M�� ���� �ּ��� \n�ߵ��½��ϴ�.\n",
                                ply_ptr, crt_ptr);
                return(0);
        }

        if(mrand(1,100) > 90 - bonus[ply_ptr->piety]) {
                print(fd,  
                        "\n������ �ϴ÷� ������ ȥ�� ��ȯ��Ű�� ��ȥ���� �ֹ��� ��Ĩ�ϴ�.\n������ ���ۺ�� ���鼭 �ҿ뵹�̸� ����Ű�� �� �ڸ����� ���»��ڰ�\n�ö�� %M�� ȥ�� �Ҹ���� ���Ƚ��ϴ�.\n ",
                        crt_ptr);
                broadcast_rom(fd, ply_ptr->rom_num, 
                                "\n%M�� ������ �ϴ÷� ������ ȥ�� ��ȯ��Ű�� ��ȥ���� \n�ֹ��� ��Ĩ�ϴ�.\n������ ���ۺ��� ���鼭 �ҿ뵹�̸� ����Ű�� �� �ڸ����� \n���»��ڰ� �ö�� %M�� ȥ���Ҹ���� ���Ƚ��ϴ�.\n",
                                    ply_ptr, crt_ptr);
                add_enm_dmg(ply_ptr->name, crt_ptr, crt_ptr->hpcur);
                die(crt_ptr, ply_ptr);
        }

        else {
                dmg = MAX(1, crt_ptr->hpcur / 2);
                m = MIN(crt_ptr->hpcur, dmg);
                crt_ptr->hpcur -= dmg;
                add_enm_dmg(ply_ptr->name, crt_ptr, m);
                print(fd,  
                        "\n������ �ϴ÷� ������ ȥ�� ��ȯ��Ű�� ��ȥ���� �ֹ��� ��Ĩ�ϴ�.\n������ %M�� ���� �����ϸ� %d��ŭ�� Ÿ���� �������ϴ�\n",
                        crt_ptr, dmg);
                broadcast_rom(fd, ply_ptr->rom_num, 
                                "\n%M�� ������ �ϴ÷� ������ ȥ�� ��ȯ��Ű�� ��ȥ���� \n�ֹ��� ��Ĩ�ϴ�.\n������ %M�� ���� �����ϸ� Ÿ���� �������ϴ�\n",
                                    ply_ptr,crt_ptr);
                if(crt_ptr->hpcur < 1) {
                        print(fd, 
                                "\n����� %M�� ȥ�� �Ҹ���׽��ϴ�.\n%M�� ������ ȥ�� ������� ��ƹ����ϴ�.\n",
                                    crt_ptr);
                        broadcast_rom(fd, ply_ptr->rom_num, 
                                        "\n%M�� %M��  ȥ�� �Ҹ���׽��ϴ�.\n\n%M�� ȥ�� ������� ���� ��ƹ����ϴ�.\n",
                                            ply_ptr, crt_ptr, crt_ptr);
                        die(crt_ptr, ply_ptr);
                }
        }

        return(0);

}

/**********************************************************************/
/*                          absorb ( ����� )                       */
/**********************************************************************/

/* �̱���� ���� ü���� ���Ƶ鿩 �ڽ��� ü�¿� ���ϴ� ��ƾ�̴�.       */
/*                                                                    */

int absorb(ply_ptr, cmnd)
creature *ply_ptr;
cmd            *cmnd;
{
        creature *crt_ptr;
        room           *rom_ptr;
        long            i, t;
        int              chance, m, dmg, fd;

        fd = ply_ptr->fd;
        rom_ptr = ply_ptr->parent_rom;

        if(cmnd->num < 2) {
                print(fd,
                        "\n�������� �ֹ��� �ŽǷ����?\n");
                return(0);
        }

        if(ply_ptr->class != MAGE && ply_ptr->class < INVINCIBLE) {
                print(fd,
                        "\n�����縸�� ������� ����Ҽ� �ֽ��ϴ�.\n");
                return(0);
        }

        crt_ptr = find_crt(ply_ptr, rom_ptr->first_mon,
                           cmnd->str[1], cmnd->val[1]);

        if(!crt_ptr) {
                print(fd,
                        "\n�׷� ������ �������� �ʽ��ϴ�.\n");
                return(0);
        }

        if(F_ISSET(ply_ptr, PINVIS)) {
                F_CLR(ply_ptr, PINVIS);
                print(fd,
                        "\n����� ����� ��Ÿ���� �����մϴ�.\n");
                broadcast_rom(fd, ply_ptr->rom_num,
                                "%M�� ����� ���̱� �����մϴ�.",
                                     ply_ptr);
        }

        i = LT(ply_ptr, LT_TURNS);
        t = time(0);

        if(i > t) {
                please_wait(fd, i-t);
                return(0);
        }

        if(crt_ptr->type != PLAYER && F_ISSET(crt_ptr, MUNKIL)) {
                print(fd,
                        "\n����� %s�� �⸦ ����Ҽ� �����ϴ�.\n",
                        F_ISSET(crt_ptr, MMALES) ? "��":"�׳�");
                return(0);
        }

        add_enm_crt(ply_ptr->name, crt_ptr);

        ply_ptr->lasttime[LT_TURNS].ltime = t;
        ply_ptr->lasttime[LT_ATTCK].ltime = t;
        ply_ptr->lasttime[LT_TURNS].interval = 30L;

        chance = (((ply_ptr->level+3)/4) - ((crt_ptr->level+3)/4))*20 +
                 bonus[ply_ptr->intelligence]*5;
        chance = MIN(chance, 80);

        if(mrand(1,100) > chance) {
                print(fd,
                        "\n���� ������ ������ �⸦ ����ϴ� ������� �ֹ��� \n���ƽ��ϴ�.������ �ֹ��� ƨ���� �����鼭 %M�� ����� �ּ��� \n�ߵ��½��ϴ�.\n",
                        crt_ptr);
                broadcast_rom(fd, ply_ptr->rom_num,
                                "\n%M�� ���� ������ ������ �⸦ ����ϴ� ������� \n�ֹ��� ��Ĩ�ϴ�.\n������ �ֹ��� ƨ���� �����鼭 %M�� ���� �ּ��� \n�ߵ��½��ϴ�.\n",
                                ply_ptr, crt_ptr);
                return(0);
        }

                print(fd,
                        "\n���� ������ ������ �⸦ ����ϴ� ������� �ֹ��� ��Ĩ�ϴ�.\n���� ������ �Ⱑ �귯���� ������\n���� ���ϴ�.\n ",
                        crt_ptr);
                broadcast_rom(fd, ply_ptr->rom_num,
                                "\n%M�� ���� ������ ������ �⸦ ����ϴ� ������� �ֹ��� ��Ĩ�ϴ�.\n%M�� ������ �Ⱑ �귯���� ������ ������ϴ�.\n",
                                    ply_ptr, crt_ptr);

        if(F_ISSET(crt_ptr, MUNDED)) {
                print(fd,
                        "\n��! �Ƿ��� ����� ����� ��ſ��� �귯��ϴ�.\n");
                ply_ptr->mpcur = 0;

                return(0);
        }


                dmg = ((ply_ptr->level+73)/4)*10;
                m = MIN(crt_ptr->hpcur, dmg);
                crt_ptr->hpcur -= dmg;
                ply_ptr->hpcur += MAX(1, dmg);
        /*        if(ply_ptr->hpcur > ply_ptr->hpmax)
                     ply_ptr->hpcur = ply_ptr->hpmax;
        */
                add_enm_dmg(ply_ptr->name, crt_ptr, m);
     /*           print(fd, "�׽�Ʈ %d %d", dmg, m);       */

                if(crt_ptr->hpcur < 1) {
                        print(fd,
                                "\n����� %M�� ���⸦ ����߽��ϴ�.\n%M�� ������ �Ⱑ ������ ������ �����ϴ�.\n",
                                    crt_ptr, crt_ptr);
                        broadcast_rom(fd, ply_ptr->rom_num,
                                        "\n%M�� %M��  ���⸦ ����߽��ϴ�.\n\n %M�� ����� %M �� �� ������ �ѷ����̸鼭 �Ͼ� ����� ���ս��ϴ�.\n\n%M�� ������ �Ⱑ ������ ������ �����ϴ�.\n",
                                            ply_ptr, crt_ptr, crt_ptr, ply_ptr, crt_ptr);
                        die(crt_ptr, ply_ptr);
                }

        return(0);

}

/**********************************************************************/
/*                              invisibility ( ���м� )                  */
/**********************************************************************/

/* This function allows a player to cast an invisibility spell on himself */
/* or on another player.                                           */

int invisibility(ply_ptr, cmnd, how)
creature *ply_ptr;
cmd            *cmnd;
int              how;
{
        creature *crt_ptr;
        room           *rom_ptr;
        ctag            *cp;
        long            t;
        int              fd;

        fd = ply_ptr->fd;
        rom_ptr = ply_ptr->parent_rom;
        t = time(0);

        if(ply_ptr->mpcur < 15 && how == CAST) {
                print(fd, 
                        "\n����� ������ �����մϴ�.\n");
                return(0);
        }

        if(!S_ISSET(ply_ptr, SINVIS) && how == CAST) {
                print(fd, 
                        "\n����� ���� �� �ּ��� �͵����� ���߽��ϴ�.\n");
                return(0);
        }
        if(spell_fail(ply_ptr)) {
                if(how==CAST)
                        ply_ptr->mpcur -= 15;
                return(0);
        }

        cp = rom_ptr->first_mon;
        while(cp) {
                if(is_enm_crt(ply_ptr->name, cp->crt)) {
                        print(fd, 
                                "\n���� �ο�� ���ݾƿ�..!!.\n");
                        return(0);
                }
                cp = cp->next_tag;
        }

        /* Cast invisibility on self */
        if(cmnd->num == 2) {

                ply_ptr->lasttime[LT_INVIS].ltime = t;
                if(how == CAST) {
                        ply_ptr->lasttime[LT_INVIS].interval = 1200 + 
                                bonus[ply_ptr->intelligence]*600;
                        if(ply_ptr->class == MAGE)
                                ply_ptr->lasttime[LT_INVIS].interval += 
                                60*((ply_ptr->level+3)/4);
                        ply_ptr->mpcur -= 15;
                if (F_ISSET(ply_ptr->parent_rom,RPMEXT)){
                            print(fd,
                                    "\n�� ���� ����� ����� �ּ����� ��ȭ��ŵ�ϴ�.\n");
                            ply_ptr->lasttime[LT_INVIS].interval += 600L;
                        }                                
                }
                else
                        ply_ptr->lasttime[LT_INVIS].interval = 1200;

                if(how == CAST || how == SCROLL || how == WAND) {
                        print(fd,
                                "\n����� �Ҹ�θ� ��Ű�鼭 ���й��� �ֹ��� �ܿ�ϴ�.\n���� ���ν� ������ ������ ���� ���ٰ� ���ڱ� ��������ϴ�.\n");
                        broadcast_rom(fd, ply_ptr->rom_num, 
                                        "\n%M�� �Ҹ�θ� ��Ű�鼭  ���й��� �ֹ��� �ܿ�ϴ�.\n���� ���� ���ν� ������ ������ ���� ���ٰ� ���ڱ� \n��������ϴ�.\n ",
                                        ply_ptr->name );
                }
                else if(how == POTION)
                        print(fd, 
                                "\n����� ���� ���ν� ������ ������ ���� ���ٰ� ���ڱ� ��������ϴ�.\n");

                F_SET(ply_ptr, PINVIS);

                return(1);
        }

        /* Cast invisibility on another player */
        else {

                if(how == POTION) {
                        print(fd, 
                                "\n�� ������ �ڽſ��Ը� ����Ҽ� �ֽ��ϴ�.\n");
                        return(0);
                }

                cmnd->str[2][0] = up(cmnd->str[2][0]);
                crt_ptr = find_crt(ply_ptr, rom_ptr->first_ply,
                                   cmnd->str[2], cmnd->val[2]);

                if(!crt_ptr) {
                        print(fd, 
                                "\n�׷� ����� �������� �ʽ��ϴ�.\n");
                        return(0);
                }

                crt_ptr->lasttime[LT_INVIS].ltime = t;
                if(how == CAST) {
                        crt_ptr->lasttime[LT_INVIS].interval = 1200 + 
                                bonus[ply_ptr->intelligence]*600;
                        if(ply_ptr->class == MAGE)
                                crt_ptr->lasttime[LT_INVIS].interval += 
                                60*((ply_ptr->level+3)/4);
                        ply_ptr->mpcur -= 15;
                if (F_ISSET(ply_ptr->parent_rom,RPMEXT)){
                            print(fd,
                                    "\n�� ���� ����� ����� �ּ����� ��ȭ��ŵ�ϴ�.\n");
                            crt_ptr->lasttime[LT_INVIS].interval += 600L;
                        }                                
                }
                else
                        crt_ptr->lasttime[LT_INVIS].interval = 1200;

                if(how == CAST || how == SCROLL || how == WAND) {
                        print(fd, 
                                "\n%M���� �Ҹ�θ� ���̰� ���й��� �ֹ��� �̴ϴ�.\n%M�� ���� ���ν� ������ ������ ���� ���ٰ� ���ڱ� \n��������ϴ�.\n",
                                    crt_ptr,crt_ptr);
                        print(crt_ptr->fd, 
                                        "\n%M�� ��ſ��� �Ҹ�θ� ���̰� ���й��� �ֹ��� �̴ϴ�.\n���� ���ν� ������ ������ ���� ���ٰ� ���ڱ� ��������ϴ�.\n",
                                            ply_ptr);
                        broadcast_rom2(fd, crt_ptr->fd, ply_ptr->rom_num,
                                        "\n%M�� %M���� �Ҹ�θ� ���̰� ���й��� �ֹ��� �̴ϴ�.\n���� ���� ���ν� ������ ������ ���� ���ٰ� ���ڱ� ��������ϴ�.\n",
                                            ply_ptr, crt_ptr);
                        F_SET(crt_ptr, PINVIS);
                        return(1);
                }
        }

        return(1);

}

/**********************************************************************/
/*                              restore ( ����õ )                         */
/**********************************************************************/

/* This function allows a player to cast the restore spell using either */
/* a potion or a wand.  Restore should not be a cast-able spell because */
/* it can restore magic points to full.                                    */

int restore(ply_ptr, cmnd, how)
creature *ply_ptr;
cmd            *cmnd;
int              how;
{
        creature *crt_ptr;
        room           *rom_ptr;
        int              fd;

        fd = ply_ptr->fd;
        rom_ptr = ply_ptr->parent_rom;

        if(how == CAST && ply_ptr->class < INVINCIBLE) {
                print(fd, 
                        "\n����� �� �ּ��� ����� �ɷ��� �����ϴ�.\n");
                return(0);
        }

        /* Cast restore on self */
        if(cmnd->num == 2) {

        /* ���� �ڽſ��� �� �ܿ� */
        if((ply_ptr->class == INVINCIBLE) && (how == CAST)) {
                print(fd,"\n�ڽſ��� �ܿ�� �����ϴ�.\n");
                return(0);
        }
                ply_ptr->hpcur += dice(2,10,0);
                ply_ptr->hpcur = MIN(ply_ptr->hpcur, ply_ptr->hpmax);

                if(mrand(1,100) < 60) {
                        ply_ptr->mpcur = ply_ptr->mpmax;

                if(how == CAST || how == WAND) {
                        print(fd, 
                                "\n����� �������� ���� �ö�  ����õ�� �ֹ��� ��Ĩ�ϴ�.\nõ���� ����� ������ ��ǳ�� �����ϸ鼭 ������ �����\n���� ������� ������ ȸ����ŵ�ϴ�.\n");
                        broadcast_rom(fd, ply_ptr->rom_num,
                                        "\n%M�� �������� ���ƿö� ����õ�� �ֹ��� ��Ĩ�ϴ�.\nõ���� ����� ������ ��ǳ�� �����ϸ鼭 ������ ������ \nȸ����ŵ�ϴ�.\n", ply_ptr);
                }
                else if(how == POTION)
                        print(fd, 
                                "\n�¸��� ������ �Ͼ�鼭 ������ �游�մϴ�.\n");

                return(1);
		}
		else {
			if(how == CAST || how == WAND) {
				print(fd,
					"\n����� �������� ���� �ö�  ����õ�� �ֹ��� ��Ĩ�ϴ�.\n������ �ƹ��� ������ �Ͼ�� �ʽ��ϴ�.\n");
				broadcast_rom(fd, ply_ptr->rom_num,
						"\n%M�� �������� ���ƿö� ����õ�� �ֹ��� ��Ĩ�ϴ�.\n������ �ƹ��� ������ �Ͼ�� �ʽ��ϴ�.\n", ply_ptr);
			}
			else { if(how == POTION)
				print(fd,
					"\n���� ȸ���� �����߽��ϴ�!!\n");
			}
		}
			
        }

        /* Cast restore on another player */
        else {

                if(how == POTION) {
                        print(fd, 
                                "\n�� ������ �ڽſ��Ը� ����Ҽ� �ֽ��ϴ�.\n");
                        return(0);
                }

                cmnd->str[2][0] = up(cmnd->str[2][0]);
                crt_ptr = find_crt(ply_ptr, rom_ptr->first_ply,
                                   cmnd->str[2], cmnd->val[2]);

                if((!crt_ptr) ||(crt_ptr == ply_ptr)) {
                        print(fd, 
                                "\n�׷� ����� �������� �ʽ��ϴ�.\n");
                        return(0);
                }

                crt_ptr->hpcur += dice(2,10,0);
                crt_ptr->hpcur = MIN(crt_ptr->hpcur, crt_ptr->hpmax);

                if(mrand(1,100) < 60) {
                        crt_ptr->mpcur = crt_ptr->mpmax;

                	if(how == CAST || how == WAND) {
                        	print(fd, 
                                	"\n%M���� ��ȭ�� ���� ���̸� ����õ�� �ֹ��� �ܿ�ϴ�.\n���� ������ ȭ�Ⱑ ���̸鼭 ������ ȸ���˴ϴ�.\n", crt_ptr);
                       		print(crt_ptr->fd, 
                                	"\n%M�� ��ſ��� ��ȭ�� ���� ���̸� ����õ�� �ֹ��� \n�ܿ�ϴ�.\n����� ������ ȭ�Ⱑ ���̸鼭 ������ ȸ���˴ϴ�.\n",
                                     	ply_ptr);
                        	broadcast_rom2(fd, crt_ptr->fd, ply_ptr->rom_num,
                                        "\n%M�� %M���� ��ȭ�� ���� ���̸� ����õ�� �ֹ��� \n�ܿ�ϴ�.\n���� ������ ȸ���Ǿ����ϴ�.\n",
                                        ply_ptr, crt_ptr);
                        	return(1);
                	}
		}
		else {
			if(how == CAST || how == WAND) {
				print(fd,
					"\n%M���� ��ȭ�� ���� ���̸� ����õ�� �ֹ��� �ܿ�ϴ�.\n������ �ƹ��� ������ �Ͼ�� �ʽ��ϴ�.\n", crt_ptr);
				print(crt_ptr->fd,
					"\n%M�� ��ſ��� ��ȭ�� ���� ���̸� ����õ�� �ֹ��� \n�ܿ�ϴ�.\n������ �ƹ��� ������ �Ͼ�� �ʽ��ϴ�.\n",
					ply_ptr);
				broadcast_rom2(fd, crt_ptr->fd, ply_ptr->rom_num,
					"\n%M�� %M���� ��ȭ�� ���� ���̸� ����õ�� �ֹ��� \n�ܿ�ϴ�.\n������ �ƹ��� ������ �Ͼ�� �ʽ��ϴ�.\n",
					ply_ptr, crt_ptr);
				return(1);
			}
		}
        }

        return(1);

}

/**********************************************************************/
/*                              befuddle ( ȥ���� )                       */
/**********************************************************************/

int befuddle(ply_ptr, cmnd, how)
creature *ply_ptr;
cmd            *cmnd;
int              how;
{
        creature *crt_ptr;
        room           *rom_ptr;
        int              fd, dur;

        fd = ply_ptr->fd;
        rom_ptr = ply_ptr->parent_rom;

        if(ply_ptr->mpcur < 10 && how == CAST) {
                print(fd, 
                        "\n����� ������ �����մϴ�.\n");
                return(0);
        }

        if(!S_ISSET(ply_ptr, SBEFUD) && how == CAST) {
                print(fd, 
                        "\n����� ���� �� �ּ��� �͵����� ���߽��ϴ�.\n");
                return(0);
        }

        if(F_ISSET(ply_ptr, PINVIS)) {
                F_CLR(ply_ptr, PINVIS);
                print(fd, 
                        "\n����� ����� ���̱� �����մϴ�.\n");
                broadcast_rom(fd, ply_ptr->rom_num, 
                        "\n%M�� ���̱� �����մϴ�.\n",
                              ply_ptr);
        }
        if(spell_fail(ply_ptr)) {
                if(how==CAST)
                        ply_ptr->mpcur -= 10;
                return(0);
        }

        /* Befuddle self */
        if(cmnd->num == 2) {

                if(how == CAST) {
                        dur = bonus[ply_ptr->intelligence]*2 + 
                              dice(2,6,0) + (ply_ptr->class == MAGE ?
                              ((ply_ptr->level+3)/4)/2 : 0);
                        ply_ptr->mpcur -= 10;
                }
                else
                        dur = dice(2,6,0);

                dur = MAX(6, dur);
                ply_ptr->lasttime[LT_ATTCK].ltime = time(0);
                ply_ptr->lasttime[LT_ATTCK].interval = dur;

                if(how == CAST || how == SCROLL || how == WAND)  {
                        print(fd, 
                                "\n����� ��⸦ ���� ������ ȥ������ ������ ��ȹ��� \n�ڽſ��� �ɾ����ϴ�.\n�ּ��� ���� ���ڱ� ��⿡�� ��������� �Ǿ�ö� ����� \n������ ȥ�����¿� ���߸��ϴ�.");
                        broadcast_rom(fd, ply_ptr->rom_num, 
                                        "\n%M�� ��⸦ ���� ������ ȥ������ ������ ��ȹ��� \n�ڽſ��� �ɾ����ϴ�.\n�ֹ��� ���� ���ڱ� ��⿡�� ��������� �Ǿ�ö� ���� \n������ ȥ�����¿� ���߸��ϴ�.",ply_ptr);
                }
                else if(how == POTION)
                        print(fd, 
                                "\n����� �¸��� �ǰ� �����ϸ鼭 ��ȭ�Ը��� �����ϴ�.\nȥ�����¿� �������ϴ�.\n");

        }

        /* Befuddle a monster or player */
        else {

                if(how == POTION) {
                        print(fd, 
                                "\n�� ������ �ڽſ��Ը� ����Ҽ� �ֽ��ϴ�.\n");
                        return(0);
                }

                crt_ptr = find_crt(ply_ptr, rom_ptr->first_mon,
                                   cmnd->str[2], cmnd->val[2]);

                if(!crt_ptr) {
                        cmnd->str[2][0] = up(cmnd->str[2][0]);
                        crt_ptr = find_crt(ply_ptr, rom_ptr->first_ply,
                                           cmnd->str[2], cmnd->val[2]);

                        if(!crt_ptr || crt_ptr == ply_ptr || 
                           strlen(cmnd->str[2]) < 3) {
                                print(fd, 
                                        "\n�׷� ����� �������� �ʽ��ϴ�.\n");
                                return(0);
                        }

                }

                if(crt_ptr->type != PLAYER && F_ISSET(crt_ptr, MUNKIL)) {
                        print(fd, 
                                "\n����� %s�� ��ĥ�� �����ϴ�.\n",
                                F_ISSET(crt_ptr, MMALES) ? "��":"�׳�");
                        return(0);
                }

                if(how == CAST) {
                        dur = bonus[ply_ptr->intelligence] + dice(2,6,0);
                        ply_ptr->mpcur -= 10;
                }
                else
                        dur = dice(2,5,0);

                if((crt_ptr->type == PLAYER && F_ISSET(crt_ptr, PRMAGI)) ||
                   (crt_ptr->type   !=   PLAYER  &&   (F_ISSET(crt_ptr,   MRMAGI)   || 
F_ISSET(crt_ptr, MRBEFD))))
                        dur = 3;
                else
                        dur = MAX(5, dur);

                crt_ptr->lasttime[LT_BEFUD].ltime = time(0);
                crt_ptr->lasttime[LT_BEFUD].interval = dur;
                if(crt_ptr->type == MONSTER)
                        F_SET(crt_ptr, MBEFUD);
                crt_ptr->lasttime[LT_SPELL].ltime = time(0);
                dur = MIN(9, dur);
                crt_ptr->lasttime[LT_SPELL].interval = dur;
                if(crt_ptr->type == PLAYER) {
                        crt_ptr->lasttime[LT_ATTCK].ltime = time(0);
                        crt_ptr->lasttime[LT_ATTCK].interval = dur;
                }


                if(how == CAST || how == SCROLL || how == WAND) {
                        print(fd, "����� ��⸦ ���� ������ %s���� ������ ��ȹ��� �ɾ����ϴ�.\n�ּ��� ���� ���ڱ� ��⿡�� ��������� �Ǿ�ö� ����\n������ ȥ�����¿� ���߸��ϴ�.\n", crt_ptr->name);
                        broadcast_rom2(fd, crt_ptr->fd, ply_ptr->rom_num,
                                            "\n%M�� ��⸦ ���� ������ ȥ������ ������ ��ȹ��� \n%M���� �ɾ����ϴ�.\n�ּ��� ���� ���ڱ� ��⿡�� ��������� �Ǿ�ö� ����\n������ ȥ�����¿� ���߸��ϴ�.\n",
                                                ply_ptr, crt_ptr);
                        print(crt_ptr->fd, 
                                        "\n%M�� ��⸦ ���� ������ ȥ������ ������ ��ȹ��� ��ſ��� �ɾ����ϴ�.\n�ּ��� ���� ���ڱ� ��⿡�� ��������� �Ǿ�ö� �����\n������ ȥ�����¿� ���߸��ϴ�.\n",
                                            ply_ptr);
                }

                if(crt_ptr->type != PLAYER)
                        add_enm_crt(ply_ptr->name, crt_ptr);

        }

        return(1);

}
