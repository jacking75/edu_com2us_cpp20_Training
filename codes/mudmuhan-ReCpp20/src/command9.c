/*
 * COMMAND9.C:
 *
 *      Additional user routines.
 *
 *      Copyright (C) 1991, 1992, 1993 Brett J. Vickers
 *
 */

#include "mstruct.h"
#include "mextern.h"

/**********************************************************************/
/*                              bribe                                 */
/**********************************************************************/

/* This function allows players to bribe monsters.  If they give the  */
/* monster enough money, it will leave the room.  If not, the monster */
/* keeps the money and stays.                                         */

int bribe(ply_ptr, cmnd)
creature        *ply_ptr;
cmd             *cmnd;
{
        creature        *crt_ptr;
        room            *rom_ptr;
        long            amount;
        int             fd;
        int             len;

        fd = ply_ptr->fd;
        rom_ptr = ply_ptr->parent_rom;

        if(cmnd->num < 2) {
                print(fd, "�������� ������ �ַ�����?\n");
                return(0);
        }

        len=strlen(cmnd->str[2]);
        if(cmnd->num < 3 || len<3 || strcmp(&cmnd->str[2][len-2], "��")) {
                print(fd, "����: <���̸�> <�׼�>�� ����\n");
                return(0);
        }

        crt_ptr = find_crt(ply_ptr, rom_ptr->first_mon, cmnd->str[1],
                           cmnd->val[1]);

        if(!crt_ptr) {
                print(fd, "�׷� ���� ���� �����ϴ�.\n");
                return(0);
        }

        amount = atol(cmnd->str[2]);
        if(amount < 1 || amount > ply_ptr->gold) {
                print(fd, "���� ������ ������ �� �� �����ϴ�.\n");
                return(0);
        }

        if(amount < ((crt_ptr->level+3)/4)*895L || F_ISSET(crt_ptr, MPERMT)) {
                print(fd, "%M�� ������ ��½ �޾ҽ��ϴٸ� ��½�� ���� �ʽ��ϴ�.\n", crt_ptr);
                broadcast_rom(fd, ply_ptr->rom_num, "%M�� %m���� ������ �ݴϴ�.",
                              ply_ptr, crt_ptr);
                ply_ptr->gold -= amount;
                crt_ptr->gold += amount;
                savegame_nomsg(ply_ptr);
        }
        else {
                print(fd, "%M�� ������ ���� �̼Ҹ� ������ ��򰡷� ���ϴ�.\n", crt_ptr);
                broadcast_rom(fd, ply_ptr->rom_num, "%M�� %m���� ������ �ݴϴ�.",
                              ply_ptr, crt_ptr);
                ply_ptr->gold -= amount;
                del_crt_rom(crt_ptr, rom_ptr);
                del_active(crt_ptr);
                free_crt(crt_ptr);
        }

        return(0);

}

/**********************************************************************/
/*                              haste                                 */
/**********************************************************************/

/* This function allows rangers to hasten themselves every 10 minutes */
/* or so, giving themselves 15 extra dexterity points.  The haste will */
/* last for 2 minutes.                                                */

int haste(ply_ptr, cmnd)
creature        *ply_ptr;
cmd             *cmnd;
{
        long    i, t;
        int     chance, fd;

        fd = ply_ptr->fd;

        if(ply_ptr->class != RANGER && ply_ptr->class < INVINCIBLE) {
                print(fd, "������ ����� �� �ִ� ����Դϴ�.\n");
                return(0);
        }

        if(F_ISSET(ply_ptr, PHASTE)) {
                print(fd, "����� ���� Ȱ������ ������Դϴ�.\n");
                return(0);
        }

        i = ply_ptr->lasttime[LT_HASTE].ltime;
        t = time(0);

        if(t-i < 600L) {
                print(fd, "%d�� %02d�� ��ٸ�����.\n",
                      (600L-t+i)/60L, (600L-t+i)%60L);
                return(0);
        }

        chance = MIN(85, ((ply_ptr->level+3)/4)*20+bonus[ply_ptr->dexterity]);

        if(mrand(1,100) <= chance) {
                print(fd, "����� ������ ���� ��ø������ �����ϴ�.\n");
                broadcast_rom(fd, ply_ptr->rom_num, "%M�� Ȱ������ ����Ͽ����ϴ�.", ply_ptr);
                F_SET(ply_ptr, PHASTE);
                ply_ptr->dexterity += 15;
                ply_ptr->lasttime[LT_HASTE].ltime = t;
                ply_ptr->lasttime[LT_HASTE].interval = 120L +
                        60L * (((ply_ptr->level+3)/4)/5);
        }
        else {
                print(fd, "Ȱ������ �����Ͽ����ϴ�.\n");
                broadcast_rom(fd, ply_ptr->rom_num, "%M�� Ȱ������ �Ế�ϴ�.",
                              ply_ptr);
                ply_ptr->lasttime[LT_HASTE].ltime = t - 590L;
        }

        compute_ac(ply_ptr);
        return(0);

}

/**********************************************************************/
/*                              pray                                  */
/**********************************************************************/

/* This function allows clerics and paladins to pray every 10 minutes */
/* to increase their piety for a duration of 5 minutes.               */

int pray(ply_ptr, cmnd)
creature        *ply_ptr;
cmd             *cmnd;
{
        long    i, t;
        int     chance, fd;

        fd = ply_ptr->fd;

        if(ply_ptr->class != CLERIC && ply_ptr->class != PALADIN &&
           ply_ptr->class < INVINCIBLE) {
                print(fd, "�����ڿ� ���縸�� �Ų� ����� �� �ֽ��ϴ�.\n");
                return(0);
        }

        if(F_ISSET(ply_ptr, PPRAYD)) {
                print(fd, "����� �̹� �ſ��� �������ϴ�.\n");
                return(0);
        }

        i = ply_ptr->lasttime[LT_PRAYD].ltime;
        t = time(0);

        if(t-i < 600L) {
                print(fd, "%d�� %02d�� ��ٸ�����.\n",
                      (600L-t+i)/60L, (600L-t+i)%60L);
                return(0);
        }

        chance = MIN(85, ((ply_ptr->level+3)/4)*20+bonus[ply_ptr->piety]);

        if(mrand(1,100) <= chance) {
                print(fd, "����� �ſ� �žӽ��� ������� ���� ���� �� �ֽ��ϴ�.\n");
                broadcast_rom(fd, ply_ptr->rom_num, "%M�� �ſ��� ����մϴ�.", ply_ptr);
                F_SET(ply_ptr, PPRAYD);
                ply_ptr->piety += 5;
                ply_ptr->lasttime[LT_PRAYD].ltime = t;
                ply_ptr->lasttime[LT_PRAYD].interval = 300L;
        }
        else {
                print(fd, "����� ����� ���� ���� ������ �����ϴ�.\n");
                broadcast_rom(fd, ply_ptr->rom_num, "%M�� �ſ��� ����մϴ�.",
                              ply_ptr);
                ply_ptr->lasttime[LT_PRAYD].ltime = t - 590L;
        }

        return(0);

}
/**********************************************************************/
/*                              updmg 				*/
/**********************************************************************/

/* �̰��� �������� ü�°� ���� �� +5�� Ÿ��ġ�� �ִ� ���̴�.    */

int up_dmg(ply_ptr, cmnd)
creature        *ply_ptr;
cmd             *cmnd;
{
        long    i, t;
        int     chance, fd;

        fd = ply_ptr->fd;

        if(ply_ptr->class < INVINCIBLE) {
                print(fd, "������ ����� �� �ִ� ����Դϴ�.\n");
                return(0);
        }

        if(F_ISSET(ply_ptr, PUPDMG)) {
                print(fd, "����� ���� ��°ݹ��� ������Դϴ�.\n");
                return(0);
        }

        i = ply_ptr->lasttime[LT_UPDMG].ltime;
        t = time(0);

        if(t-i < 1200L) {
                print(fd, "%d�� %02d�� ��ٸ�����.\n",
                      (1200L-t+i)/60L, (1200L-t+i)%60L);
                return(0);
        }

        chance = MIN(85, ((ply_ptr->level+3)/4)*20+bonus[ply_ptr->dexterity]);

        if(mrand(1,100) <= chance) {
                print(fd, "����� �ڽ��� ������ ¤���� ���� ����� �ݹ߽�ŵ�ϴ�.\n�¸����� �Ⱑ ���������°��� �����ϴ�.\n");
                broadcast_rom(fd, ply_ptr->rom_num, "%M�� �ڽ��� ������ ¤���� ���� ������Դϴ�.", ply_ptr);
                F_SET(ply_ptr, PUPDMG);
                ply_ptr->pdice += 5;
                ply_ptr->hpmax += 100;
                ply_ptr->mpmax += 100;
                ply_ptr->lasttime[LT_UPDMG].ltime = t;
                ply_ptr->lasttime[LT_UPDMG].interval = 120L; 
                ply_ptr->hpcur = ply_ptr->hpmax;
                ply_ptr->mpcur = ply_ptr->mpmax;
        }
        else {
                print(fd, "���� �ݹ߽�Ű�µ� �����߽��ϴ�.\n");
                broadcast_rom(fd, ply_ptr->rom_num, "%M�� ��°ݹ��� �õ��մϴ�.",
                              ply_ptr);
                ply_ptr->lasttime[LT_UPDMG].ltime = t - 960L;
        }

        compute_ac(ply_ptr);
        return(0);

}
/**********************************************************************/
/*                              power �������                        */
/**********************************************************************/

/* �̰��� �˻翡�� 5���� ���� �ִ� ���̴�.                            */

int power(ply_ptr, cmnd)
creature        *ply_ptr;
cmd             *cmnd;
{
        long    i, t;
        int     chance, fd;

        fd = ply_ptr->fd;

        if(ply_ptr->class != FIGHTER && ply_ptr->class < INVINCIBLE) {
                print(fd, "�˻縸 ����� �� �ִ� ����Դϴ�.\n");
                return(0);
        }

        if(F_ISSET(ply_ptr, PPOWER)) {
                print(fd, "����� ���� ��������� ������Դϴ�.\n");
                return(0);
        }

        i = ply_ptr->lasttime[LT_POWER].ltime;
        t = time(0);

        if(t-i < 600L) {
                print(fd, "%d�� %02d�� ��ٸ�����.\n",
                      (600L-t+i)/60L, (600L-t+i)%60L);
                return(0);
        }

        chance = MIN(85, ((ply_ptr->level+3)/4)*20+bonus[ply_ptr->dexterity]);

        if(mrand(1,100) <= chance) {
                print(fd, "����� �����¸� Ʋ�� �⸦ ������ �����մϴ�.\n�¸����� �Ⱑ ���������°��� �����ϴ�.\n");
                broadcast_rom(fd, ply_ptr->rom_num, "%M�� �����¸� Ʋ�� �ɾ� �⸦ �����ϴ�.", ply_ptr);
                F_SET(ply_ptr, PPOWER);
                ply_ptr->strength += 3;
                ply_ptr->lasttime[LT_POWER].ltime = t;
                ply_ptr->lasttime[LT_POWER].interval = 120L +
                        60L * (((ply_ptr->level+3)/4)/5);
        }
        else {
                print(fd, "��������� �����Ͽ����ϴ�.\n");
                broadcast_rom(fd, ply_ptr->rom_num, "%M�� ��������� �õ��մϴ�.",
                              ply_ptr);
                ply_ptr->lasttime[LT_POWER].ltime = t - 590L;
        }

        compute_ac(ply_ptr);
        return(0);

}


/**********************************************************************/
/*                      accurate �������                             */
/**********************************************************************/

/* �̰��� �ڰ� ���Ͽ��� 3���� ���߷��� �ִ� ���̴�.                   */

int accurate(ply_ptr, cmnd)
creature        *ply_ptr;
cmd             *cmnd;
{
        long    i, t;
        int     chance, fd;

        fd = ply_ptr->fd;

        if(ply_ptr->class != ASSASSIN && ply_ptr->class != THIEF &&
                  ply_ptr->class < INVINCIBLE) {
                print(fd, "�ڰ� ���ϸ� ����� �� �ִ� ����Դϴ�.\n");
                return(0);
        }

        if(F_ISSET(ply_ptr, PSLAYE)) {
                print(fd, "����� ���� ��������� ������Դϴ�.\n");
                return(0);
        }
        if(!ply_ptr->ready[WIELD-1]) {
                print(fd, "����ϰ� �ִ� ���Ⱑ �����ϴ�!\n");
                return(0);
        }
        i = ply_ptr->lasttime[LT_SLAYE].ltime;
        t = time(0);

        if(t-i < 700L) {
                print(fd, "%d�� %02d�� ��ٸ�����.\n",
                      (700L-t+i)/60L, (700L-t+i)%60L);
                return(0);
        }

        chance = MIN(85, ((ply_ptr->level+3)/4)*20+bonus[ply_ptr->dexterity]);

        if(mrand(1,100) <= chance) {
                print(fd, "����� ����� ���⿡ �Ǹ� ���Դϴ�.\n���⿡ ��Ⱑ ������ ���� �����ϴ�.\n");
                broadcast_rom(fd, ply_ptr->rom_num, "%M�� ���� ���⿡ �Ǹ� ���Դϴ�.", ply_ptr);
                F_SET(ply_ptr, PSLAYE);
                ply_ptr->thaco -= 3;
                ply_ptr->lasttime[LT_SLAYE].ltime = t;
                ply_ptr->lasttime[LT_SLAYE].interval = 150L +
                        60L * (((ply_ptr->level+3)/4)/5);
        }
        else {
                print(fd, "��������� �����Ͽ����ϴ�.\n");
                broadcast_rom(fd, ply_ptr->rom_num, "%M�� ���� ���⿡ ��������� �õ��մϴ�.",
                              ply_ptr);
                ply_ptr->lasttime[LT_SLAYE].ltime = t - 590L;
        }

        compute_ac(ply_ptr);
        return(0);

}

/**********************************************************************/
/*                      meditate ����                                 */
/**********************************************************************/

/* �̰��� ���� �����ڿ��� 3���� ������ �ִ� ���̴�.                   */

int meditate(ply_ptr, cmnd)
creature        *ply_ptr;
cmd             *cmnd;
{
        long    i, t;
        int     chance, fd;

        fd = ply_ptr->fd;

        if(ply_ptr->class != PALADIN && ply_ptr->class != CLERIC &&
                  ply_ptr->class < INVINCIBLE) {
                print(fd, "���� �����ڸ� ����� �� �ִ� ����Դϴ�.\n");
                return(0);
        }

        if(F_ISSET(ply_ptr, PMEDIT)) {
                print(fd, "����� ���� ������ �߽��ϴ�.\n");
                return(0);
        }
        i = ply_ptr->lasttime[LT_MEDIT].ltime;
        t = time(0);

        if(t-i < 700L) {
                print(fd, "%d�� %02d�� ��ٸ�����.\n",
                      (700L-t+i)/60L, (700L-t+i)%60L);
                return(0);
        }

        chance = MIN(85, ((ply_ptr->level+3)/4)*20+bonus[ply_ptr->piety]);

        if(mrand(1,100) <= chance) {
                print(fd, "����� �ڸ��� �ɾ� ������ ���ϴ�.\n���Ӱ� �繰�� �ٶ󺸴� ���� �߿����ϴ�.\n");
                broadcast_rom(fd, ply_ptr->rom_num, "%M�� �ڸ��� �ɾ� ������ ���մϴ�.", ply_ptr);
                F_SET(ply_ptr, PMEDIT);
                ply_ptr->intelligence += 3;
                ply_ptr->lasttime[LT_MEDIT].ltime = t;
                ply_ptr->lasttime[LT_MEDIT].interval = 150L +
                        60L * (((ply_ptr->level+3)/4)/5);
        }
        else {
                print(fd, "�������� ��ȭ�Ը��� �������ϴ�.\n");
                broadcast_rom(fd, ply_ptr->rom_num, "%M�� ������ �ϴٰ� ��ȭ�Ը��� �������ϴ�.",
                              ply_ptr);
                ply_ptr->lasttime[LT_MEDIT].ltime = t - 590L;
        }

        compute_ac(ply_ptr);
        return(0);

}

/**********************************************************************/
/*                              prepare                               */
/**********************************************************************/

/* This function allows players to prepare themselves for traps that */
/* might be in the next room they enter.  This way, they can hope to */
/* avoid a trap that they already know is there.                     */

int prepare(ply_ptr, cmnd)
creature        *ply_ptr;
cmd             *cmnd;
{
        long    i, t;
        int     fd;

        fd = ply_ptr->fd;

        if(F_ISSET(ply_ptr, PPREPA)) {
                print(fd, "����� �̹� �������� �����ϰ� �ֽ��ϴ�.");
                return(0);
        }

        t = time(0);
        i = LT(ply_ptr, LT_PREPA);

        if(t < i) {
                please_wait(fd, i-t);
                return(0);
        }

        ply_ptr->lasttime[LT_PREPA].ltime = t;
        ply_ptr->lasttime[LT_PREPA].interval = ply_ptr->class == DM ? 0:15;

        print(fd, "����� �������� ������ �ֳ� ���캸�� ���ϴ�.");
        broadcast_rom(fd, ply_ptr->rom_num, "\n%M�� ������ �����ϸ� ���ϴ�.",
                ply_ptr);
        F_SET(ply_ptr, PPREPA);
        if(F_ISSET(ply_ptr, PBLIND))
                F_CLR(ply_ptr, PPREPA);

        return(0);
}

/************************************************************************/
/*                              use                                     */
/************************************************************************/

/* This function allows a player to use an item without specifying the  */
/* special command for its type.  Use determines which type of item     */
/* it is, and calls the appropriate functions.                          */

int use(ply_ptr, cmnd)
creature        *ply_ptr;
cmd             *cmnd;
{
        object  *obj_ptr;
        room    *rom_ptr;
        int     fd, n, rtn, onfloor=0;

        fd = ply_ptr->fd;
        rom_ptr = ply_ptr->parent_rom;

        if(cmnd->num < 2) {
                print(fd, "������ ����Ͻ÷�����?");
                return(0);
        }

        if(!strcmp(cmnd->str[1], "���"))
                return(wear(ply_ptr, cmnd));

        obj_ptr = find_obj(ply_ptr, ply_ptr->first_obj, cmnd->str[1],
                           cmnd->val[1]);
        if(!obj_ptr) {
                obj_ptr = find_obj(ply_ptr, rom_ptr->first_obj, cmnd->str[1],
                                   cmnd->val[1]);
                if(!obj_ptr || !F_ISSET(obj_ptr, OUSEFL)) {
                        print(fd, "������ ����Ͻ÷�����?");
                        return(0);
                }
                else if(F_ISSET(obj_ptr, OUSEFL)) {
                        onfloor = 1;
                        cmnd->num = 2;
                }
        }

        F_CLR(ply_ptr, PHIDDN);

        if(obj_ptr->special==SP_WAR) {
            special_obj(ply_ptr,cmnd,SP_WAR);
            return 0;
        }

        switch(obj_ptr->type) {
        case SHARP:
        case THRUST:
        case BLUNT:
        case POLE:
        case MISSILE:
                rtn = ready(ply_ptr, cmnd); break;
        case ARMOR:
                rtn = wear(ply_ptr, cmnd); break;
        case POTION:
                rtn = drink(ply_ptr, cmnd); break;
        case SCROLL:
                rtn = readscroll(ply_ptr, cmnd); break;
        case WAND:
                if(onfloor) {
                        cmnd->num = 2;
                        obj_ptr->shotscur -= 1;
                        rtn = zap_obj(ply_ptr, obj_ptr, cmnd);
                }
                else
                        rtn = zap(ply_ptr, cmnd); break;
        case KEY:
                rtn = unlock(ply_ptr, cmnd); break;
        case LIGHTSOURCE:
                rtn = hold(ply_ptr, cmnd); break;
        default:
                print(fd, "�װ��� ��� ����Ͻ÷�����?\n");
                rtn = 0;
        }
        return(rtn);
}

/************************************************************************/
/*                              ignore                                  */
/************************************************************************/

/* This function allows players to ignore the flashes of other players. */
/* If used without an argument, it lists the names of players currently */
/* being ignored.  If included with a name, the player will be added    */
/* to the ignore list if he is not already on it.  Otherwise the player */
/* will be taken off the list.  The ignore list lasts only for the      */
/* duration of the player's connection.                                 */

int ignore(ply_ptr, cmnd)
creature        *ply_ptr;
cmd             *cmnd;
{
        int             n, fd;
        etag            *ign, *prev;
        char            outstr[1024], *name;
        creature        *crt_ptr;

        fd = ply_ptr->fd;

        if(cmnd->num == 1) {
                strcpy(outstr, "��� �źε� �����: ");
                ign = Ply[fd].extr->first_ignore;
                n = 0;
                while(ign) {
                        strcat(outstr, ign->enemy);
                        strcat(outstr, ", ");
                        n = 1;
                        ign = ign->next_tag;
                }
                if(!n)
                        strcat(outstr, "����.");
                else {
                        outstr[strlen(outstr)-2] = '.';
                        outstr[strlen(outstr)-1] = 0;
                }
                print(fd, "%s\n", outstr);
                return(0);
        }

        cmnd->str[1][0] = up(cmnd->str[1][0]);
        name = cmnd->str[1];
        n = 0;
        ign = Ply[fd].extr->first_ignore;
        if(!ign) {}
        else if(!strcmp(ign->enemy, name)) {
                Ply[fd].extr->first_ignore = ign->next_tag;
                free(ign);
                n = 1;
        }
        else {
                prev = ign;
                ign = ign->next_tag;
                while(ign) {
                        if(!strcmp(ign->enemy, name)) {
                                prev->next_tag = ign->next_tag;
                                free(ign);
                                n = 1;
                                break;
                        }
                        prev = ign;
                        ign = ign->next_tag;
                }
        }

        if(n) {
                print(fd, "%s���� �̾߱� ��� �ź� ��󿡼� �����մϴ�.", name);
                return(0);
        }

        crt_ptr = find_who(name);

        if(crt_ptr && !F_ISSET(crt_ptr, PDMINV)) {
                ign = (etag *)malloc(sizeof(etag));
                strcpy(ign->enemy, name);
                ign->next_tag = Ply[fd].extr->first_ignore;
                Ply[fd].extr->first_ignore = ign;
                print(fd, "%s���� �̾߱� ��� �ź� ��� �߰��մϴ�.\n", name);
        }
        else
                print(fd, "�� ����ڴ� �������� �ƴմϴ�.\n");

        return(0);
}


