/*
 * COMMAND6.C:
 *
 *      Additional user routines.
 *
 *      Copyright (C) 1991, 1992, 1993 Brett J. Vickers
 *
 */

#include "mstruct.h"
#include "mextern.h"

/**********************************************************************/
/*                              yell                                  */
/**********************************************************************/

/* This function allows a player to yell something that will be heard */
/* not only in his room, but also in all rooms adjacent to him.  In   */
/* the adjacent rooms, however, people will not know who yelled.      */

int yell(ply_ptr, cmnd)
creature        *ply_ptr;
cmd             *cmnd;
{
        room            *rom_ptr;
        xtag            *xp;
        char            str[300];
        int             index = -1, i, fd;
        int             len;

        fd = ply_ptr->fd;
        rom_ptr = ply_ptr->parent_rom;
        len = strlen(cmnd->fullstr);
        for(i=0; i<len; i++) {
                if(cmnd->fullstr[i] == ' ') {
                        index = i+1;
                        break;
                }
        }
        str[256]=0;

        if(index == -1 || strlen(&cmnd->fullstr[index]) < 1) {
                print(fd, "무슨말을 외치려구요?");
                return(0);
        }
        if(F_ISSET(ply_ptr, PSILNC)) {
                print(fd, "당신의 목소리가 너무 약해서 외칠수 없습니다.");
                return(0);
        }
        F_CLR(ply_ptr, PHIDDN);
        print(fd, "예. 좋습니다.");

        broadcast_rom(fd, rom_ptr->rom_num, "\n%M이 \"%s!\"라고 외칩니다.", ply_ptr,
                      &cmnd->fullstr[0]);

        sprintf(str, "\n누군가가 \"%s!\"라고 외쳤습니다.", &cmnd->fullstr[0]);

        xp = rom_ptr->first_ext;
        while(xp) {
                if(is_rom_loaded(xp->ext->room))
                        broadcast_rom(fd, xp->ext->room, "%s", str);
                xp = xp->next_tag;
        }

        return(0);

}

/**********************************************************************/
/*                              go                                    */
/**********************************************************************/

/* This function allows a player to go through an exit, other than one */
/* of the 6 cardinal directions.                                       */

int go(ply_ptr, cmnd)
creature        *ply_ptr;
cmd             *cmnd;
{
		FILE			*fp;
        room            *rom_ptr, *old_rom_ptr;
        creature        *crt_ptr;
        ctag            *cp, *temp;
        exit_           *ext_ptr;
        long            i, t;
        int             fd, old_rom_num, fall, dmg, n, chance;
		char		file[80], name[10][15];
		int			j;

        rom_ptr = ply_ptr->parent_rom;
        fd = ply_ptr->fd;


        if(cmnd->num < 2) {
                print(fd, "어디로 가고 싶으세요?");
                return(0);
        }

        ext_ptr = find_ext(ply_ptr, rom_ptr->first_ext,
                           cmnd->str[1], cmnd->val[1]);

        if(!ext_ptr) {
                print(fd, "그런 출구는 없습니다.");
                return(0);
        }

        if(F_ISSET(ply_ptr, PSILNC)) {
        	print(fd, "당신은 움직일 수가 없습니다.");
        	return(0);
        }

		if(ply_is_attacking(ply_ptr, cmnd)) {
			print(fd, "싸우는 중에는 이동할 수 없습니다.");
			return(0);
		}

        if(F_ISSET(ext_ptr, XLOCKD)) {
                print(fd, "그 출구는 잠겨 있습니다.");
                return(0);
        }
        else if(F_ISSET(ext_ptr, XCLOSD)) {
                print(fd, "그 출구는 닫혀 있습니다.");
                return(0);
        }

        if(F_ISSET(ext_ptr, XFLYSP) && !F_ISSET(ply_ptr, PFLYSP)) {
                print(fd, "그곳에는 날아서만 갈 수 있습니다.");
                return(0);
        }

        t = Time%24L;
        if(F_ISSET(ext_ptr, XNGHTO) && (t>6 && t < 20)) {
                print(fd, "그 출구는 밤에만 갈 수 있습니다.");
                return(0);
        }

        if(F_ISSET(ext_ptr, XDAYON) && (t<6 || t > 20)) {
                print(fd, "그 출구로는 낮에만 갈 수 있습니다.");
                return(0);
        }

    if(F_ISSET(ext_ptr,XPGUAR)){
        cp = rom_ptr->first_mon;
        while(cp) {
            if(F_ISSET(cp->crt, MPGUAR)) {
            if(!F_ISSET(ply_ptr, PINVIS) && ply_ptr->class < SUB_DM) {
                print(fd, "%M%j 당신의 길을 막습니다.", cp->crt,"1");
                return(0);
                }
            if(F_ISSET(cp->crt, MDINVI) && ply_ptr->class < SUB_DM) {
                print(fd, "%M%j 당신의 길을 막습니다.", cp->crt,"1");
                return(0);
                }
            }
            cp = cp->next_tag;
        }
    }

        if(F_ISSET(ext_ptr, XNAKED) && weight_ply(ply_ptr)) {
                print(fd, "뭘 가지고는 들어갈 수 없습니다.");
                return(0);
        }

        if(F_ISSET(ext_ptr, XFEMAL) && F_ISSET(ply_ptr, PMALES)){
                print(fd, "여성만 들어갈 수 있습니다. 여탕인가~~");
                return(0);
        }

        if(F_ISSET(ext_ptr, XMALES) && !F_ISSET(ply_ptr, PMALES)){
                print(fd, "남성만 들어갈 수 있습니다.");
                return(0);
        }

        if((F_ISSET(ext_ptr, XCLIMB) || F_ISSET(ext_ptr, XREPEL)) &&
           !F_ISSET(ply_ptr, PLEVIT)) {
                fall = (F_ISSET(ext_ptr, XDCLIM) ? 50:0) + 50 -
                       fall_ply(ply_ptr);

                if(mrand(1,100) < fall) {
                        dmg = mrand(5, 15+fall/10);
                        if(ply_ptr->hpcur <= dmg){
                           print(fd, "당신은 죽음이 다가오는것같은 느낌이 듭니다.");
                           ply_ptr->hpcur=0;
                           broadcast_rom(fd, ply_ptr->rom_num, "\n%M이 구덩이에 떨어져서 죽었습니다.", ply_ptr);
                           die(ply_ptr, ply_ptr);
                           return(0);
                        }

                        ply_ptr->hpcur -= dmg;
                        print(fd, "당신은 구덩이에 떨어져서 %d 만큼의 상처를 입었습니다.",
                              dmg);
                        broadcast_rom(fd, ply_ptr->rom_num, "\n%M이 구덩이에 떨어졌습니다.",
                                      ply_ptr);

                        if(ply_ptr->hpcur < 1) {
                           print(fd, "당신은 죽음이 다가오는것 같은 느낌이 듭니다.");
                               die(ply_ptr, ply_ptr);
                        }
                        if(F_ISSET(ext_ptr, XCLIMB))
                                return(0);
                }
        }

        i = LT(ply_ptr, LT_ATTCK);
        t = time(0);

        if(t < i) {
                please_wait(fd, i-t);
                return(0);
        }

        if((ply_ptr->class == ASSASSIN || ply_ptr->class == THIEF || ply_ptr->class >
              INVINCIBLE) && (F_ISSET(ply_ptr, PHIDDN))) {
     
              chance = MIN(85, 5 + 6*((ply_ptr->level+3)/4) +
                    3*bonus[ply_ptr->dexterity]);
             if(F_ISSET(ply_ptr, PBLIND))
                chance = MIN(20, chance);

        if(mrand(1,100) > chance)
        {
            print(fd,"당신은 은신술을 사용하는데 실패하였습니다.\n");
            F_CLR(ply_ptr, PHIDDN);

            cp = rom_ptr->first_mon;
            while(cp) {
                if(F_ISSET(cp->crt, MBLOCK) &&
                is_enm_crt(ply_ptr->name, cp->crt) &&
                F_ISSET(ply_ptr, PINVIS) && ply_ptr->class < SUB_DM) {
                    print(fd, "%M가 당신의 길을 가로막습니다.\n", cp->crt);
                    return(0);
                    }
                cp = cp->next_tag;
            }
         }
     }
     else F_CLR(ply_ptr, PHIDDN);

        if(!F_ISSET(rom_ptr, RPTRAK))
                strcpy(rom_ptr->track, ext_ptr->name);

        old_rom_num = rom_ptr->rom_num;
        old_rom_ptr = rom_ptr;

        load_rom(ext_ptr->room, &rom_ptr);
        if(rom_ptr == old_rom_ptr) {
                print(fd, "그 방향의 지도가 없습니다.");
                return(0);
        }

        n = count_vis_ply(rom_ptr);

        if(rom_ptr->lolevel > ply_ptr->level && ply_ptr->class < INVINCIBLE/*CARETAKER*/) {
                print(fd, "레벨 %d 이상이어야 그 곳으로 갈 수 있습니다.",
                      rom_ptr->lolevel);
                return(0);
        }
        else if(ply_ptr->level > rom_ptr->hilevel && rom_ptr->hilevel &&
                ply_ptr->class < CARETAKER) {
                print(fd, "그곳으로 갈려면 레벨 %d이하여야만 합니다.",
                      rom_ptr->hilevel+1);
                return(0);
        }
        else if((F_ISSET(rom_ptr, RONEPL) && n > 0) ||
                (F_ISSET(rom_ptr, RTWOPL) && n > 1) ||
                (F_ISSET(rom_ptr, RTHREE) && n > 2)) {
                print(fd, "그 방에 있는 사용자가 너무 많습니다.");
                return(0);
        }
        else if(F_ISSET(rom_ptr, RFAMIL) && !F_ISSET(ply_ptr, PFAMIL)) {
        	print(fd, "그곳에는 패거리 가입자만 갈 수 있습니다.");
        	return(0);
        }
        else if(ply_ptr->class < DM && F_ISSET(rom_ptr, RONFML) && (ply_ptr->daily[DL_EXPND].max != rom_ptr->special)) {
        	print(fd, "그곳은 당신이 갈 수 없는 곳입니다.");
        	return(0);
        }
        else if(ply_ptr->class < DM && F_ISSET(rom_ptr, RONMAR) &&
        	(ply_ptr->daily[DL_MARRI].max != rom_ptr->special)) {
		sprintf(file, "%s/invite/invite_%d", PLAYERPATH, rom_ptr->special);
	if(file_exists(file)) {
	fp = fopen(file, "r");
	for(j=0; j<10; j++) {
		fscanf(fp, "%s", name[j]);
		if(!strcmp(ply_ptr->name, name[j])) goto mmm;
	}
	fclose(fp);
	}
        	print(fd, "그곳은 사유지입니다.");
        	return(0);
        }
mmm:

        if(strcmp(ext_ptr->name, "위") && strcmp(ext_ptr->name, "밑")
           && strcmp(ext_ptr->name, "밖") && !F_ISSET(ply_ptr, PDMINV)) {
                broadcast_rom(fd, old_rom_ptr->rom_num, "\n%M이 %s쪽으로 갔습니다.",
                              ply_ptr, ext_ptr->name);
        }

        else if(!F_ISSET(ply_ptr, PDMINV)) {
                broadcast_rom(fd, old_rom_ptr->rom_num, "\n%M이 %s쪽으로 갔습니다.",
                              ply_ptr, ext_ptr->name);
        }

        del_ply_rom(ply_ptr, ply_ptr->parent_rom);
        add_ply_rom(ply_ptr, rom_ptr);

        cp = ply_ptr->first_fol;
        while(cp) {
                if(cp->crt->rom_num == old_rom_num && cp->crt->type != MONSTER)
                        go(cp->crt, cmnd);
                if(F_ISSET(cp->crt, MDMFOL) && cp->crt->type == MONSTER) {
                del_crt_rom(cp->crt, old_rom_ptr);
                broadcast_rom(fd, old_rom_ptr->rom_num, "\n%M%j 방황하다 %s쪽으로 갔습니다.", cp->crt,"1",ext_ptr->name);
                add_crt_rom(cp->crt, rom_ptr, 1);
                add_active(cp->crt);
                }
                cp = cp->next_tag;
        }

        if(is_rom_loaded(old_rom_num)) {
                cp = old_rom_ptr->first_mon;
                while(cp) {
                        if(!F_ISSET(cp->crt, MFOLLO) || F_ISSET(cp->crt, MDMFOL)) {
                                cp = cp->next_tag;
                                continue;
                        }
                        if(!cp->crt->first_enm) {
                                cp = cp->next_tag;
                                continue;
                        }
                        if(strcmp(cp->crt->first_enm->enemy, ply_ptr->name)) {
                                cp = cp->next_tag;
                                continue;
                        }
                        if(mrand(1,50) > 10 - ply_ptr->dexterity +
                           cp->crt->dexterity) {
                                cp = cp->next_tag;
                                continue;
                        }
                        print(fd, "%M%j 당신을 따라옵니다.", cp->crt,"1");
                        broadcast_rom(fd, old_rom_num, "\n%M%j %M%j 따라갑니다.",
                                      cp->crt,"1", ply_ptr,"3");
                        temp = cp->next_tag;
                        crt_ptr = cp->crt;
                        del_crt_rom(crt_ptr, old_rom_ptr);
                        add_crt_rom(crt_ptr, rom_ptr, 1);
                        add_active(crt_ptr);
                        F_CLR(crt_ptr, MPERMT);
                        cp = temp;
                }
        }

        check_traps(ply_ptr, rom_ptr);
        return(0);
}

/**********************************************************************/
/*                              openexit                              */
/**********************************************************************/

/* This function allows a player to open a door if it is not already */
/* open and if it is not locked.                                     */

int openexit(ply_ptr, cmnd)
creature        *ply_ptr;
cmd             *cmnd;
{
        exit_   *ext_ptr;
        room    *rom_ptr;
        int     fd;

        rom_ptr = ply_ptr->parent_rom;
        fd = ply_ptr->fd;

        if(cmnd->num < 2) {
                print(fd, "무엇을 열고 싶으세요?");
                return(0);
        }

        ext_ptr = find_ext(ply_ptr, rom_ptr->first_ext,
                           cmnd->str[1], cmnd->val[1]);

        if(!ext_ptr) {
                print(fd, "그런 출구는 없습니다.");
                return(0);
        }

        if(F_ISSET(ext_ptr, XLOCKD)) {
                print(fd, "그것은 잠겨져 있습니다.");
                return(0);
        }

        if(!F_ISSET(ext_ptr, XCLOSD)) {
                print(fd, "벌써 열려져 있습니다.");
                return(0);
        }

        F_CLR(ply_ptr, PHIDDN);

        F_CLR(ext_ptr, XCLOSD);
        ext_ptr->ltime.ltime = time(0);

        print(fd, "당신은 %s쪽 출구를 열었습니다.", ext_ptr->name);
        broadcast_rom(fd, ply_ptr->rom_num, "\n%M이 %s쪽 출구를 열었습니다.",
                      ply_ptr, ext_ptr->name);

        return(0);

}

/**********************************************************************/
/*                              closeexit                             */
/**********************************************************************/

/* This function allows a player to close a door, if the door is not */
/* already closed, and if indeed it is a door.                       */

int closeexit(ply_ptr, cmnd)
creature        *ply_ptr;
cmd             *cmnd;
{
        exit_   *ext_ptr;
        room    *rom_ptr;
        int     fd;

        rom_ptr = ply_ptr->parent_rom;
        fd = ply_ptr->fd;

        if(cmnd->num < 2) {
                print(fd, "무엇을 닫고 싶으세요?");
                return(0);
        }

        ext_ptr = find_ext(ply_ptr, rom_ptr->first_ext,
                           cmnd->str[1], cmnd->val[1]);

        if(!ext_ptr) {
                print(fd, "그런 출구는 없습니다.");
                return(0);
        }

        if(F_ISSET(ext_ptr, XCLOSD)) {
                print(fd, "벌써 닫혀져 있습니다.");
                return(0);
        }

        if(!F_ISSET(ext_ptr, XCLOSS)) {
                print(fd, "당신은 그 출구를 닫을 수 없습니다.");
                return(0);
        }

        F_CLR(ply_ptr, PHIDDN);

        F_SET(ext_ptr, XCLOSD);

        print(fd, "당신은 %s쪽 출구를 닫습니다.", ext_ptr->name);
        broadcast_rom(fd, ply_ptr->rom_num, "\n%M이 %s쪽 출구를 닫습니다.",
                      ply_ptr, ext_ptr->name);

        return(0);

}

/**********************************************************************/
/*                              unlock                                */
/**********************************************************************/

/* This function allows a player to unlock a door if he has the correct */
/* key, and it is locked.                                               */

int unlock(ply_ptr, cmnd)
creature        *ply_ptr;
cmd             *cmnd;
{
        room    *rom_ptr;
        object  *obj_ptr;
        exit_   *ext_ptr;
        int     fd;

        fd = ply_ptr->fd;
        rom_ptr = ply_ptr->parent_rom;

        if(cmnd->num < 2) {
                print(fd, "무엇을 풀고 싶으세요?");
                return(0);
        }

        ext_ptr = find_ext(ply_ptr, rom_ptr->first_ext,
                           cmnd->str[1], cmnd->val[1]);

        if(!ext_ptr) {
                print(fd, "무엇을 풀고 싶으세요?");
                return(0);
        }

        if(!F_ISSET(ext_ptr, XLOCKD)) {
                print(fd, "그것은 잠궈져 있지 않습니다.");
                return(0);
        }

        if(cmnd->num < 3) {
                print(fd, "뭘 가지고 열려구요?");
                return(0);
        }

        obj_ptr = find_obj(ply_ptr, ply_ptr->first_obj,
                           cmnd->str[2], cmnd->val[2]);

        if(!obj_ptr) {
                print(fd, "당신은 그런것을 갖고 있지 않습니다.");
                return(0);
        }

        if(obj_ptr->type != KEY) {
                print(fd, "그것은 열쇠가 아닙니다.");
                return(0);
        }

        if(obj_ptr->shotscur < 1) {
                print(fd, "%I%j 부숴져 버렸습니다.", obj_ptr,"1");
                return(0);
        }

        if(obj_ptr->ndice != ext_ptr->key) {
                print(fd, "열쇠가 맞지 않습니다.");
                return(0);
        }

        F_CLR(ply_ptr, PHIDDN);

        F_CLR(ext_ptr, XLOCKD);
        ext_ptr->ltime.ltime = time(0);
        obj_ptr->shotscur--;

        if(obj_ptr->use_output[0])
                print(fd, "%s", obj_ptr->use_output);
        else
                print(fd, "## 찰칵 ##");
        broadcast_rom(fd, ply_ptr->rom_num, "\n%M이 %s쪽 출구를 풀었습니다.",
                      ply_ptr, ext_ptr->name);

        return(0);

}

/**********************************************************************/
/*                              lock                                  */
/**********************************************************************/

/* This function allows a player to lock a door with the correct key. */

int lock(ply_ptr, cmnd)
creature        *ply_ptr;
cmd             *cmnd;
{
        room    *rom_ptr;
        object  *obj_ptr;
        exit_   *ext_ptr;
        int     fd;

        fd = ply_ptr->fd;
        rom_ptr = ply_ptr->parent_rom;

        if(cmnd->num < 2) {
                print(fd, "무엇을 잠굴려구요?");
                return(0);
        }

        ext_ptr = find_ext(ply_ptr, rom_ptr->first_ext,
                           cmnd->str[1], cmnd->val[1]);

        if(!ext_ptr) {
                print(fd, "무엇을 잠굴려구요?");
                return(0);
        }

        if(F_ISSET(ext_ptr, XLOCKD)) {
                print(fd, "그것은 이미 잠궈져 있습니다.");
                return(0);
        }

        if(cmnd->num < 3) {
                print(fd, "뭘 가지고 잠구려구요?");
                return(0);
        }

        obj_ptr = find_obj(ply_ptr, ply_ptr->first_obj,
                           cmnd->str[2], cmnd->val[2]);

        if(!obj_ptr) {
                print(fd, "당신은 그런것을 가지고 있지 않습니다.");
                return(0);
        }

        if(obj_ptr->type != KEY) {
                print(fd, "%I%j 열쇠가 아닙니다.", obj_ptr,"0");
                return(0);
        }

        if(!F_ISSET(ext_ptr, XLOCKS)) {
                print(fd, "당신은 그것을 잠굴수 없습니다.");
                return(0);
        }

        if(!F_ISSET(ext_ptr, XCLOSD)) {
                print(fd, "먼저 문을 닫아야 될것 같군요.");
                return(0);
        }

        if(obj_ptr->shotscur < 1) {
                print(fd, "%I%j 부서져 버렸습니다.", obj_ptr,"1");
                return(0);
        }

        if(obj_ptr->ndice != ext_ptr->key) {
                print(fd, "열쇠가 맞지 않습니다.");
                return(0);
        }

        F_CLR(ply_ptr, PHIDDN);

        F_SET(ext_ptr, XLOCKD);

        print(fd, "## 찰칵 ##");
        broadcast_rom(fd, ply_ptr->rom_num, "\n%M이 %s쪽 출구를 잠궜습니다.",
                      ply_ptr, ext_ptr->name);

        return(0);

}

/**********************************************************************/
/*                              picklock                              */
/**********************************************************************/

/* This function is called when a thief or assassin attempts to pick a */
/* lock.  If the lock is pickable, there is a chance (depending on the */
/* player's level) that the lock will be picked.                       */

int picklock(ply_ptr, cmnd)
creature        *ply_ptr;
cmd             *cmnd;
{
        room    *rom_ptr;
        exit_   *ext_ptr;
        long    i, t;
        int     fd, chance;

        fd = ply_ptr->fd;
        rom_ptr = ply_ptr->parent_rom;

        if(ply_ptr->class != THIEF && ply_ptr->class < INVINCIBLE) {
                print(fd, "도둑만 자물쇠를 딸 수 있습니다.");
                return(0);
        }

        if(cmnd->num < 2) {
                print(fd, "무엇을 따시려구요?");
                return(0);
        }

        ext_ptr = find_ext(ply_ptr, rom_ptr->first_ext,
                           cmnd->str[1], cmnd->val[1]);

        if(!ext_ptr) {
                print(fd, "그런건 여기 없습니다.");
                return(0);
        }
        if(F_ISSET(ply_ptr, PBLIND)) {
                print(fd, "당신은 눈이 멀어 있어 딸 수 없습니다.");
                return(0);
        }
        if(!F_ISSET(ext_ptr, XLOCKD)) {
                print(fd, "그것은 잠궈져 있지 않습니다.");
                return(0);
        }

        F_CLR(ply_ptr, PHIDDN);

        i = LT(ply_ptr, LT_PICKL);
        t = time(0);

        if(t < i) {
                please_wait(fd, i-t);
                return(0);
        }

        ply_ptr->lasttime[LT_PICKL].ltime = t;
        ply_ptr->lasttime[LT_PICKL].interval = 10;

        chance = (ply_ptr->class == THIEF) ? 10*((ply_ptr->level+3)/4) :
                 5*((ply_ptr->level+3)/4);
        chance += bonus[ply_ptr->dexterity]*2;

        if(F_ISSET(ext_ptr, XUNPCK))
                chance = 0;

        broadcast_rom(fd, ply_ptr->rom_num, "\n%M이 %s쪽 출구를 따려고 합니다.",
                      ply_ptr, ext_ptr->name);

        if(mrand(1,100) <= chance) {
                print(fd, "당신은 문을 따는데 성공했습니다.");
                F_CLR(ext_ptr, XLOCKD);
                broadcast_rom(fd, ply_ptr->rom_num, "\n%M이 문을 땄습니다.",
                              ply_ptr);
        }
        else
                print(fd, "실패하였습니다!");

        return(0);

}

/**********************************************************************/
/*                              steal                                 */
/**********************************************************************/

/* This function allows a player to steal an item from a monster within  */
/* the game.  Only thieves may steal, and the monster must               */
/* have the object which is being stolen within his inventory.           */

int steal(ply_ptr, cmnd)
creature        *ply_ptr;
cmd             *cmnd;
{
        creature        *crt_ptr;
        room            *rom_ptr;
        object          *obj_ptr;
        otag            *otag_ptr;
        long            i, t;
        int             fd, chance;

        fd = ply_ptr->fd;
        rom_ptr = ply_ptr->parent_rom;

        if(cmnd->num < 2) {
                print(fd, "무엇을 훔치려구요?");
                return(0);
        }

        if(cmnd->num < 3) {
                print(fd, "누구한테서 훔치려구요?");
                return(0);
        }

        if(ply_ptr->class != THIEF && ply_ptr->class < INVINCIBLE) {
                print(fd, "도둑만 훔칠수 있습니다.");
                return(0);
        }

        F_CLR(ply_ptr, PHIDDN);

        i = LT(ply_ptr, LT_STEAL);
        t = time(0);

        if(t < i) {
                please_wait(fd, i-t);
                return(0);
        }

        if(F_ISSET(ply_ptr, PINVIS)) {
                F_CLR(ply_ptr, PINVIS);
                print(fd, "당신의 모습이 서서히 드러납니다.");
                broadcast_rom(fd, ply_ptr->rom_num, "\n%M의 모습이 서서히 드러납니다.",
                              ply_ptr);
        }

        ply_ptr->lasttime[LT_STEAL].ltime = t;
        ply_ptr->lasttime[LT_STEAL].interval = 5;

        crt_ptr = find_crt(ply_ptr, rom_ptr->first_mon,
                           cmnd->str[2], cmnd->val[2]);

        if(!crt_ptr) {
                cmnd->str[2][0] = up(cmnd->str[2][0]);
                crt_ptr = find_crt(ply_ptr, rom_ptr->first_ply,
                                   cmnd->str[2], cmnd->val[2]);

                if(!crt_ptr || crt_ptr == ply_ptr || F_ISSET(ply_ptr, PBLIND)) {
                        print(fd, "그런건 여기 없습니다.");
                        return(0);
                }

        }

        if(crt_ptr->type == MONSTER) {
                if(F_ISSET(crt_ptr, MUNKIL)) {
                        print(fd, "당신은 %s를 해칠수 없습니다.",
                                F_ISSET(crt_ptr, MMALES) ? "그":"그녀");
                        return(0);
                }
                if(is_enm_crt(ply_ptr->name, crt_ptr)) {
                        print(fd, "%s는 싸우는 중이 아닙니다.",
                              F_ISSET(crt_ptr, MMALES) ? "그":"그녀");
                        return(0);
                }
        }
        else {
                if(F_ISSET(rom_ptr, RNOKIL)) {
                        print(fd, "이 방에서는 훔칠 수 없습니다.");
                        return(0);
                }

                if(!F_ISSET(ply_ptr, PCHAOS) && ply_ptr->class<SUB_DM) {
                    print(fd, "당신은 선해서 훔칠 수 없습니다.");
                    return (0);
                }

                if(!F_ISSET(crt_ptr, PCHAOS) && ply_ptr->class<SUB_DM) {
                    print(fd, "그 사용자는 선해서 보호받고 있습니다.");
                    return (0);
                }
        }
        if(F_ISSET(ply_ptr, PBLIND)) {
                print(fd, "당신은 눈이 멀어 훔칠 수 없습니다.");
                return(0);
        }
        obj_ptr = find_obj(ply_ptr, crt_ptr->first_obj,
                           cmnd->str[1], cmnd->val[1]);

        if(!obj_ptr) {
                print(fd, "%s는 그런 물건을 갖고 있지 않습니다.",
                      F_ISSET(crt_ptr, MMALES) ? "그":"그녀");
                return(0);
        }

        chance = (ply_ptr->class == THIEF) ? 4*((ply_ptr->level+3)/4) :
                 3*((ply_ptr->level+3)/4);
        chance += bonus[ply_ptr->dexterity]*3;
        if(crt_ptr->level > ply_ptr->level)
                chance -= 15*(((crt_ptr->level+3)/4) - ((ply_ptr->level+3)/4));
        if(obj_ptr->questnum) chance = 0;
        chance = MIN(chance, 65);

        if(obj_ptr->questnum || F_ISSET(crt_ptr, MUNSTL))
                chance = 0;

        if(ply_ptr->class == DM)
                chance = 100;
               
        if(crt_ptr->class == DM)  chance =0;
        if(F_ISSET(obj_ptr, ONEWEV)) chance =0;
        if(obj_ptr->questnum && ply_ptr->class < DM) chance=0;
        if(obj_ptr->first_obj) {
            for(otag_ptr=obj_ptr->first_obj; otag_ptr; otag_ptr=otag_ptr->next_tag) {
                if(otag_ptr->obj->questnum && ply_ptr->class < DM) chance=0;
                if(F_ISSET(otag_ptr->obj, ONEWEV) && ply_ptr->class < DM) chance=0;
            }
        }



        if(mrand(1,100) <= chance) {
                print(fd, "훔쳤습니다.");
                del_obj_crt(obj_ptr, crt_ptr);
                add_obj_crt(obj_ptr, ply_ptr);
                savegame_nomsg(ply_ptr);
                if(crt_ptr->type == PLAYER) savegame_nomsg(crt_ptr);
                if(crt_ptr->type == PLAYER) {
                        ply_ptr->lasttime[LT_PLYKL].ltime = t;
                        ply_ptr->lasttime[LT_PLYKL].interval =
                                (long)mrand(7,10) * 86400L;

                }
        }

        else {
                print(fd, "실패하였습니다.%s", crt_ptr->type == MONSTER ?
                      "\n그가 당신을 공격합니다." : "");
                broadcast_rom2(fd, crt_ptr->fd, ply_ptr->rom_num,
                              "\n%M이 %M에게서 물건을 훔치려고 합니다.",
                              ply_ptr, crt_ptr);

                if(crt_ptr->type == PLAYER)
                        print(crt_ptr->fd, "\n%M이 당신에게서 %1i%j 훔치려고 합니다.",
                              ply_ptr, obj_ptr,"3");
                else
                        add_enm_crt(ply_ptr->name, crt_ptr);
        }

        return(0);

}
