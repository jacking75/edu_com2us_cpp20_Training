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
                print(fd, "누구에게 뇌물을 주려구요?\n");
                return(0);
        }

        len=strlen(cmnd->str[2]);
        if(cmnd->num < 3 || len<3 || strcmp(&cmnd->str[2][len-2], "냥")) {
                print(fd, "사용법: <몹이름> <액수>냥 뇌물\n");
                return(0);
        }

        crt_ptr = find_crt(ply_ptr, rom_ptr->first_mon, cmnd->str[1],
                           cmnd->val[1]);

        if(!crt_ptr) {
                print(fd, "그런 것은 여기 없습니다.\n");
                return(0);
        }

        amount = atol(cmnd->str[2]);
        if(amount < 1 || amount > ply_ptr->gold) {
                print(fd, "돈의 단위는 음수가 될 수 없습니다.\n");
                return(0);
        }

        if(amount < ((crt_ptr->level+3)/4)*895L || F_ISSET(crt_ptr, MPERMT)) {
                print(fd, "%M가 뇌물을 슬쩍 받았습니다만 꿈쩍도 하지 않습니다.\n", crt_ptr);
                broadcast_rom(fd, ply_ptr->rom_num, "%M이 %m에게 뇌물을 줍니다.",
                              ply_ptr, crt_ptr);
                ply_ptr->gold -= amount;
                crt_ptr->gold += amount;
                savegame_nomsg(ply_ptr);
        }
        else {
                print(fd, "%M이 뇌물을 받자 미소를 지으며 어딘가로 갑니다.\n", crt_ptr);
                broadcast_rom(fd, ply_ptr->rom_num, "%M이 %m에게 뇌물을 줍니다.",
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
                print(fd, "포졸만 사용할 수 있는 기술입니다.\n");
                return(0);
        }

        if(F_ISSET(ply_ptr, PHASTE)) {
                print(fd, "당신은 지금 활보법을 사용중입니다.\n");
                return(0);
        }

        i = ply_ptr->lasttime[LT_HASTE].ltime;
        t = time(0);

        if(t-i < 600L) {
                print(fd, "%d분 %02d초 기다리세요.\n",
                      (600L-t+i)/60L, (600L-t+i)%60L);
                return(0);
        }

        chance = MIN(85, ((ply_ptr->level+3)/4)*20+bonus[ply_ptr->dexterity]);

        if(mrand(1,100) <= chance) {
                print(fd, "당신의 동작이 좀더 민첩해진것 같습니다.\n");
                broadcast_rom(fd, ply_ptr->rom_num, "%M이 활보법을 사용하였습니다.", ply_ptr);
                F_SET(ply_ptr, PHASTE);
                ply_ptr->dexterity += 15;
                ply_ptr->lasttime[LT_HASTE].ltime = t;
                ply_ptr->lasttime[LT_HASTE].interval = 120L +
                        60L * (((ply_ptr->level+3)/4)/5);
        }
        else {
                print(fd, "활보법이 실패하였습니다.\n");
                broadcast_rom(fd, ply_ptr->rom_num, "%M이 활보법을 써봅니다.",
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
                print(fd, "불제자와 무사만이 신께 기원할 수 있습니다.\n");
                return(0);
        }

        if(F_ISSET(ply_ptr, PPRAYD)) {
                print(fd, "당신은 이미 신에게 빌었습니다.\n");
                return(0);
        }

        i = ply_ptr->lasttime[LT_PRAYD].ltime;
        t = time(0);

        if(t-i < 600L) {
                print(fd, "%d분 %02d초 기다리세요.\n",
                      (600L-t+i)/60L, (600L-t+i)%60L);
                return(0);
        }

        chance = MIN(85, ((ply_ptr->level+3)/4)*20+bonus[ply_ptr->piety]);

        if(mrand(1,100) <= chance) {
                print(fd, "당신은 매우 신앙심이 깊어지는 것을 느낄 수 있습니다.\n");
                broadcast_rom(fd, ply_ptr->rom_num, "%M이 신에게 기원합니다.", ply_ptr);
                F_SET(ply_ptr, PPRAYD);
                ply_ptr->piety += 5;
                ply_ptr->lasttime[LT_PRAYD].ltime = t;
                ply_ptr->lasttime[LT_PRAYD].interval = 300L;
        }
        else {
                print(fd, "당신의 기원에 대한 신의 응답이 없습니다.\n");
                broadcast_rom(fd, ply_ptr->rom_num, "%M이 신에게 기원합니다.",
                              ply_ptr);
                ply_ptr->lasttime[LT_PRAYD].ltime = t - 590L;
        }

        return(0);

}
/**********************************************************************/
/*                              updmg 				*/
/**********************************************************************/

/* 이것은 무적에게 체력과 도력 및 +5의 타격치를 주는 것이다.    */

int up_dmg(ply_ptr, cmnd)
creature        *ply_ptr;
cmd             *cmnd;
{
        long    i, t;
        int     chance, fd;

        fd = ply_ptr->fd;

        if(ply_ptr->class < INVINCIBLE) {
                print(fd, "무적만 사용할 수 있는 기술입니다.\n");
                return(0);
        }

        if(F_ISSET(ply_ptr, PUPDMG)) {
                print(fd, "당신은 지금 잠력격발을 사용중입니다.\n");
                return(0);
        }

        i = ply_ptr->lasttime[LT_UPDMG].ltime;
        t = time(0);

        if(t-i < 1200L) {
                print(fd, "%d분 %02d초 기다리세요.\n",
                      (1200L-t+i)/60L, (1200L-t+i)%60L);
                return(0);
        }

        chance = MIN(85, ((ply_ptr->level+3)/4)*20+bonus[ply_ptr->dexterity]);

        if(mrand(1,100) <= chance) {
                print(fd, "당신은 자신의 혈도를 짚으며 몸의 잠력을 격발시킵니다.\n온몸으로 기가 퍼져나가는것을 느낍니다.\n");
                broadcast_rom(fd, ply_ptr->rom_num, "%M이 자신의 혈도를 짚으며 힘을 끌어들입니다.", ply_ptr);
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
                print(fd, "힘을 격발시키는데 실패했습니다.\n");
                broadcast_rom(fd, ply_ptr->rom_num, "%M이 잠력격발을 시도합니다.",
                              ply_ptr);
                ply_ptr->lasttime[LT_UPDMG].ltime = t - 960L;
        }

        compute_ac(ply_ptr);
        return(0);

}
/**********************************************************************/
/*                              power 기공집결                        */
/**********************************************************************/

/* 이것은 검사에게 5점의 힘을 주는 것이다.                            */

int power(ply_ptr, cmnd)
creature        *ply_ptr;
cmd             *cmnd;
{
        long    i, t;
        int     chance, fd;

        fd = ply_ptr->fd;

        if(ply_ptr->class != FIGHTER && ply_ptr->class < INVINCIBLE) {
                print(fd, "검사만 사용할 수 있는 기술입니다.\n");
                return(0);
        }

        if(F_ISSET(ply_ptr, PPOWER)) {
                print(fd, "당신은 지금 기공집결을 사용중입니다.\n");
                return(0);
        }

        i = ply_ptr->lasttime[LT_POWER].ltime;
        t = time(0);

        if(t-i < 600L) {
                print(fd, "%d분 %02d초 기다리세요.\n",
                      (600L-t+i)/60L, (600L-t+i)%60L);
                return(0);
        }

        chance = MIN(85, ((ply_ptr->level+3)/4)*20+bonus[ply_ptr->dexterity]);

        if(mrand(1,100) <= chance) {
                print(fd, "당신은 가부좌를 틀고 기를 모으기 시작합니다.\n온몸으로 기가 퍼져나가는것을 느낍니다.\n");
                broadcast_rom(fd, ply_ptr->rom_num, "%M이 가부좌를 틀고 앉아 기를 모읍니다.", ply_ptr);
                F_SET(ply_ptr, PPOWER);
                ply_ptr->strength += 3;
                ply_ptr->lasttime[LT_POWER].ltime = t;
                ply_ptr->lasttime[LT_POWER].interval = 120L +
                        60L * (((ply_ptr->level+3)/4)/5);
        }
        else {
                print(fd, "기공집결이 실패하였습니다.\n");
                broadcast_rom(fd, ply_ptr->rom_num, "%M이 기공집결을 시도합니다.",
                              ply_ptr);
                ply_ptr->lasttime[LT_POWER].ltime = t - 590L;
        }

        compute_ac(ply_ptr);
        return(0);

}


/**********************************************************************/
/*                      accurate 살기충전                             */
/**********************************************************************/

/* 이것은 자객 도둑에게 3점의 명중률을 주는 것이다.                   */

int accurate(ply_ptr, cmnd)
creature        *ply_ptr;
cmd             *cmnd;
{
        long    i, t;
        int     chance, fd;

        fd = ply_ptr->fd;

        if(ply_ptr->class != ASSASSIN && ply_ptr->class != THIEF &&
                  ply_ptr->class < INVINCIBLE) {
                print(fd, "자객 도둑만 사용할 수 있는 기술입니다.\n");
                return(0);
        }

        if(F_ISSET(ply_ptr, PSLAYE)) {
                print(fd, "당신은 지금 살기충전을 사용중입니다.\n");
                return(0);
        }
        if(!ply_ptr->ready[WIELD-1]) {
                print(fd, "장비하고 있는 무기가 없습니다!\n");
                return(0);
        }
        i = ply_ptr->lasttime[LT_SLAYE].ltime;
        t = time(0);

        if(t-i < 700L) {
                print(fd, "%d분 %02d초 기다리세요.\n",
                      (700L-t+i)/60L, (700L-t+i)%60L);
                return(0);
        }

        chance = MIN(85, ((ply_ptr->level+3)/4)*20+bonus[ply_ptr->dexterity]);

        if(mrand(1,100) <= chance) {
                print(fd, "당신은 당신의 무기에 피를 먹입니다.\n무기에 살기가 감도는 것을 느낍니다.\n");
                broadcast_rom(fd, ply_ptr->rom_num, "%M이 그의 무기에 피를 먹입니다.", ply_ptr);
                F_SET(ply_ptr, PSLAYE);
                ply_ptr->thaco -= 3;
                ply_ptr->lasttime[LT_SLAYE].ltime = t;
                ply_ptr->lasttime[LT_SLAYE].interval = 150L +
                        60L * (((ply_ptr->level+3)/4)/5);
        }
        else {
                print(fd, "살기충전이 실패하였습니다.\n");
                broadcast_rom(fd, ply_ptr->rom_num, "%M이 그의 무기에 살기충전을 시도합니다.",
                              ply_ptr);
                ply_ptr->lasttime[LT_SLAYE].ltime = t - 590L;
        }

        compute_ac(ply_ptr);
        return(0);

}

/**********************************************************************/
/*                      meditate 참선                                 */
/**********************************************************************/

/* 이것은 무사 불제자에게 3점의 지식을 주는 것이다.                   */

int meditate(ply_ptr, cmnd)
creature        *ply_ptr;
cmd             *cmnd;
{
        long    i, t;
        int     chance, fd;

        fd = ply_ptr->fd;

        if(ply_ptr->class != PALADIN && ply_ptr->class != CLERIC &&
                  ply_ptr->class < INVINCIBLE) {
                print(fd, "무사 불제자만 사용할 수 있는 기술입니다.\n");
                return(0);
        }

        if(F_ISSET(ply_ptr, PMEDIT)) {
                print(fd, "당신은 벌써 참선을 했습니다.\n");
                return(0);
        }
        i = ply_ptr->lasttime[LT_MEDIT].ltime;
        t = time(0);

        if(t-i < 700L) {
                print(fd, "%d분 %02d초 기다리세요.\n",
                      (700L-t+i)/60L, (700L-t+i)%60L);
                return(0);
        }

        chance = MIN(85, ((ply_ptr->level+3)/4)*20+bonus[ply_ptr->piety]);

        if(mrand(1,100) <= chance) {
                print(fd, "당신은 자리에 앉아 참선에 들어갑니다.\n새롭게 사물을 바라보는 눈이 뜨였습니다.\n");
                broadcast_rom(fd, ply_ptr->rom_num, "%M이 자리에 앉아 참선을 행합니다.", ply_ptr);
                F_SET(ply_ptr, PMEDIT);
                ply_ptr->intelligence += 3;
                ply_ptr->lasttime[LT_MEDIT].ltime = t;
                ply_ptr->lasttime[LT_MEDIT].interval = 150L +
                        60L * (((ply_ptr->level+3)/4)/5);
        }
        else {
                print(fd, "참선도중 주화입마에 빠졌습니다.\n");
                broadcast_rom(fd, ply_ptr->rom_num, "%M이 참선을 하다가 주화입마에 빠졌습니다.",
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
                print(fd, "당신은 이미 함정들을 주의하고 있습니다.");
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

        print(fd, "당신은 이제부터 함정이 있나 살펴보며 갑니다.");
        broadcast_rom(fd, ply_ptr->rom_num, "\n%M이 함정을 조심하며 갑니다.",
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
                print(fd, "무엇을 사용하시려구요?");
                return(0);
        }

        if(!strcmp(cmnd->str[1], "모두"))
                return(wear(ply_ptr, cmnd));

        obj_ptr = find_obj(ply_ptr, ply_ptr->first_obj, cmnd->str[1],
                           cmnd->val[1]);
        if(!obj_ptr) {
                obj_ptr = find_obj(ply_ptr, rom_ptr->first_obj, cmnd->str[1],
                                   cmnd->val[1]);
                if(!obj_ptr || !F_ISSET(obj_ptr, OUSEFL)) {
                        print(fd, "무엇을 사용하시려구요?");
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
                print(fd, "그것을 어떻게 사용하시려구요?\n");
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
                strcpy(outstr, "듣기 거부된 사용자: ");
                ign = Ply[fd].extr->first_ignore;
                n = 0;
                while(ign) {
                        strcat(outstr, ign->enemy);
                        strcat(outstr, ", ");
                        n = 1;
                        ign = ign->next_tag;
                }
                if(!n)
                        strcat(outstr, "없음.");
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
                print(fd, "%s님을 이야기 듣기 거부 대상에서 삭제합니다.", name);
                return(0);
        }

        crt_ptr = find_who(name);

        if(crt_ptr && !F_ISSET(crt_ptr, PDMINV)) {
                ign = (etag *)malloc(sizeof(etag));
                strcpy(ign->enemy, name);
                ign->next_tag = Ply[fd].extr->first_ignore;
                Ply[fd].extr->first_ignore = ign;
                print(fd, "%s님을 이야기 듣기 거부 대상에 추가합니다.\n", name);
        }
        else
                print(fd, "그 사용자는 접속중이 아닙니다.\n");

        return(0);
}


