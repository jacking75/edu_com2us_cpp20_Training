/*
 * COMMAND10.C:
 *
 *  Additional user routines.
 *
 *
 */

#include "mstruct.h"
#include "mextern.h"

/**********************************************************************/
/*                           sneak                                   */
/**********************************************************************/


int sneak(ply_ptr, cmnd)
creature    *ply_ptr;
cmd     *cmnd;
{
    room        *rom_ptr, *old_rom_ptr;
    creature    *crt_ptr;
    ctag        *cp, *temp;
    exit_       *ext_ptr;
    long        i, t;
    int     fd, old_rom_num, fall, dmg, n;
    int         chance;

    rom_ptr = ply_ptr->parent_rom;
    fd = ply_ptr->fd;

    if(ply_ptr->class != ASSASSIN && ply_ptr->class != THIEF && ply_ptr->class < INVINCIBLE) {
                print(fd, "도둑과 자객만 사용할 수 있는 기술입니다.\n");
                return(0);
        }

    if(!F_ISSET(ply_ptr, PHIDDN)){
                print(fd, "먼저 숨어야 이 기술을 쓸 수 있습니다.\n");
                return(0);
        }

    if(cmnd->num < 2) {
        print(fd, "어디로 몰래 가시려구요?\n");
        return(0);
    }

    ext_ptr = find_ext(ply_ptr, rom_ptr->first_ext,
               cmnd->str[1], cmnd->val[1]);

    if(!ext_ptr) {
        print(fd, "그 쪽에는 출구가 없습니다.\n");
        return(0);
    }

    if(F_ISSET(ext_ptr, XLOCKD)) {
        print(fd, "그 출구는 잠겨져 있습니다.\n");
        return(0);
    }
    else if(F_ISSET(ext_ptr, XCLOSD)) {
        print(fd, "먼저 문을 열어야 겠군요.\n");
        return(0);
    }

    if(F_ISSET(ext_ptr, XFLYSP) && !F_ISSET(ply_ptr, PFLYSP)) {
        print(fd, "그 쪽에는 날아서만 갈 수 있습니다.\n");
        return(0);
    }

    if(F_ISSET(ext_ptr, XNAKED) && weight_ply(ply_ptr)) {
        print(fd, "그 쪽으로는 뭘 들고는 갈 수 없습니다.\n");
        return(0);
    }

    t = Time%24L;
    if(F_ISSET(ext_ptr, XNGHTO) && (t>6 && t < 20)) {
        print(fd, "그 출구는 밤에만 갈 수 있습니다.\n");
        return(0);
    }

    if(F_ISSET(ext_ptr, XDAYON) && (t<6 || t > 20)) {
        print(fd, "그 출구는 낮에만 갈 수 있습니다.\n");
        return(0);
    }

    if(F_ISSET(ext_ptr,XPGUAR)){
        cp = rom_ptr->first_mon;
        while(cp) {
            if(F_ISSET(cp->crt, MPGUAR)) {
                if(!F_ISSET(ply_ptr, PINVIS) && ply_ptr->class < CARETAKER){
                print(fd, "%M%j 당신의 길을 가로막습니다.\n", cp->crt,"1");
                return(0);
                }
                if(F_ISSET(cp->crt, MDINVI) && ply_ptr->class < CARETAKER) {
                print(fd, "%M%j 당신의 길을 가로막습니다.\n", cp->crt,"1");
                return(0);
                }
            }
            cp = cp->next_tag;
        }
    }

    if(F_ISSET(ext_ptr, XFEMAL) && F_ISSET(ply_ptr, PMALES)){
        print(fd, "그 쪽으로는 여성만 갈 수 있습니다.\n");
        return(0);
    }
    if(F_ISSET(ext_ptr, XMALES) && !F_ISSET(ply_ptr, PMALES)){
        print(fd, "그 쪽으로는 남성만 갈 수 있습니다.\n");
        return(0);
    }

    if((F_ISSET(ext_ptr, XCLIMB) || F_ISSET(ext_ptr, XREPEL)) &&
       !F_ISSET(ply_ptr, PLEVIT)) {
        fall = (F_ISSET(ext_ptr, XDCLIM) ? 50:0) + 50 -
               fall_ply(ply_ptr);

        if(mrand(1,100) < fall) {
            dmg = mrand(5, 15+fall/10);
            if(ply_ptr->hpcur <= dmg){
                   print(fd, "당신은 깊은 구덩이에 빠졌습니다..\n");
                   ply_ptr->hpcur=0;
                   broadcast_rom(fd, ply_ptr->rom_num, "%M이 구덩이에 빠져서 죽었습니다.\n");
                   die(ply_ptr, ply_ptr);
                   return(0);
            }

            ply_ptr->hpcur -= dmg;
            print(fd, "당신은 떨어져서 %d만큼의 상처를 입었습니다.\n",
                  dmg);
            broadcast_rom(fd, ply_ptr->rom_num, "%M이 구덩이에 빠졌습니다.",
                      ply_ptr);

                if(ply_ptr->hpcur < 1) {
                                print(fd, "당신은 죽었습니다.\n");
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
            !F_ISSET(ply_ptr, PINVIS) && ply_ptr->class < CARETAKER) {
                print(fd, "%M가 당신의 길을 가로막습니다.\n", cp->crt);
                return(0);
            }
            cp = cp->next_tag;
        }
    }

    if(!F_ISSET(rom_ptr, RPTRAK))
        strcpy(rom_ptr->track, ext_ptr->name);

    old_rom_num = rom_ptr->rom_num;
    old_rom_ptr = rom_ptr;

    load_rom(ext_ptr->room, &rom_ptr);
    if(rom_ptr == old_rom_ptr) {
        print(fd, "그쪽으로는 지도가 없습니다.\n");
        return(0);
    }

    n = count_vis_ply(rom_ptr);

    if(rom_ptr->lolevel > ply_ptr->level && ply_ptr->class < INVINCIBLE/*CARETAKER*/) {
        print(fd, "그쪽으로는 레벨 %d이상만 갈 수 있습니다.\n",
              rom_ptr->lolevel);
        return(0);
    }
    else if(ply_ptr->level > rom_ptr->hilevel && rom_ptr->hilevel &&
        ply_ptr->class < CARETAKER) {
        print(fd, "그쪽으로는 레벨 %d이하만 갈 수 있습니다.\n",
              rom_ptr->hilevel+1);
        return(0);
    }
    else if((F_ISSET(rom_ptr, RONEPL) && n > 0) ||
        (F_ISSET(rom_ptr, RTWOPL) && n > 1) ||
        (F_ISSET(rom_ptr, RTHREE) && n > 2)) {
        print(fd, "그 방에는 정원이 가득 찼습니다.\n");
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
    	print(fd, "그곳은 사유지입니다.");
    	return(0);
    }

    if(!F_ISSET(ply_ptr, PHIDDN))
        if(strcmp(ext_ptr->name, "위") && strcmp(ext_ptr->name, "밑")
           && strcmp(ext_ptr->name, "밖") && !F_ISSET(ply_ptr, PDMINV)) {
                broadcast_rom(fd, old_rom_ptr->rom_num, "%M이 %s쪽으로 갔습니다.",
                              ply_ptr, ext_ptr->name);
        }
        else if(!F_ISSET(ply_ptr, PDMINV)) {
                broadcast_rom(fd, old_rom_ptr->rom_num, "%M이 %s쪽으로 갔습니다.",
                              ply_ptr, ext_ptr->name);
        }


    del_ply_rom(ply_ptr, ply_ptr->parent_rom);
    add_ply_rom(ply_ptr, rom_ptr);


    check_traps(ply_ptr, rom_ptr);
    return(0);
}
/**********************************************************************/
/*                          Group_talk                                 */
/**********************************************************************/
/* Group talk allows a player to broadcast a message to everyone in   *
 * their group. gtalk first checks to see if the player is following  *
 * another player; in that case, the group talk message will be sent  *
 * every member in the leader's group.  If the player isn't following *
 * then it is assumed the player is a leader and the message will be    *
 * sent to all the followers in the group. */

int gtalk(ply_ptr, cmnd)
creature    *ply_ptr;
cmd     *cmnd;
{
    creature *leader;
    ctag    *cp;
    int     found=0, i, fd;
    int     len;

    fd = ply_ptr->fd;

if(ply_ptr->following)
{
        leader = ply_ptr->following;
        cp = ply_ptr->following->first_fol;
}
    else
{
        leader = ply_ptr;
        cp = ply_ptr->first_fol;
}

    if(!cp) {
        print(fd, "당신은 그룹에 속해있지 않습니다.\n");
        return(0);
    }
    len = found = cut_command(cmnd->fullstr);
    cmnd->fullstr[255] = 0;

    if(len==0) {
        print(fd, "그룹원들에게 무슨말을 하시려구요?\n");
        return(0);
    }
    if(F_ISSET(ply_ptr, PSILNC)) {
        print(fd, "입이 막혀 말이 나오질 않습니다.\n");
        return(0);
    }

    found = 0;          /*if only inv dm in group */

    while(cp) {
        if((F_ISSET(cp->crt, PIGNOR) && (ply_ptr->class < CARETAKER)) &&
            !F_ISSET(cp->crt,PDMINV))
            print(fd, "%s는 이야기 듣기 거부 상태입니다.\n", cp->crt->name);
        else
            ANSI(fd, MAGENTA);
            print(cp->crt->fd, "%M이 그룹원들에게 \"%s\"라고 말합니다.\n",
                ply_ptr, &cmnd->fullstr[0]);
            ANSI(fd, WHITE);
        if (!F_ISSET(cp->crt,PDMINV))
            found = 1;
        cp = cp->next_tag;
    }
    if(!found)
    {
        print(fd, "당신은 그룹에 속해있지 않습니다.\n");
        return(0);
    }

        if((F_ISSET(leader, PIGNOR) && (ply_ptr->class < CARETAKER)) &&
            !F_ISSET(leader,PDMINV))
            print(fd, "%s님은 이야기 듣기 거부 상태입니다.\n", leader->name);
        else
            ANSI(fd, MAGENTA);
            print(leader->fd, "%M이 그룹원들에게 \"%s\"라고 말합니다.\n",
                ply_ptr, &cmnd->fullstr[0]);
            ANSI(fd, WHITE);

    if (found)
    broadcast_eaves("# %s님이 그룹원들에게 \"%s\"라고 말합니다.", ply_ptr->name,
             &cmnd->fullstr[0]);

    return(0);

}


/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*                              lower_prof                              */
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
void lower_prof(ply_ptr, exp)
/* The lower_prof function adjusts a player's magical realm and weapon *
 * proficiency after a player loses (exp) amount of experience         */

creature   *ply_ptr;
long        exp;
{
    long    profloss, total;
    int     n, below=0;

        for(n=0,total=0L; n<5; n++)
            total += ply_ptr->proficiency[n];
        for(n=0; n<4; n++)
            total += ply_ptr->realm[n];

        profloss = MIN(exp,total);
        while(profloss > 9L && below < 9) {
            below = 0;
            for(n=0; n<9; n++) {
                if(n < 5) {
                    ply_ptr->proficiency[n] -=
                       profloss/(9-n);
                    profloss -= profloss/(9-n);
                    if(ply_ptr->proficiency[n] < 0L) {
                        below++;
                        profloss -=
                           ply_ptr->proficiency[n];
                        ply_ptr->proficiency[n] = 0L;
                    }
                }
                else {
                    ply_ptr->realm[n-5] -= profloss/(9-n);
                    profloss -= profloss/(9-n);
                    if(ply_ptr->realm[n-5] < 0L) {
                        below++;
                        profloss -=
                            ply_ptr->realm[n-5];
                        ply_ptr->realm[n-5] = 0L;
                    }
                }
            }
        }
        for(n=1,total=0; n<5; n++)
            if(ply_ptr->proficiency[n] >
                ply_ptr->proficiency[total]) total = n;
        if(ply_ptr->proficiency[total] < 1024L)
            ply_ptr->proficiency[total] = 1024L;
    return;
}
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*                              add_prof                                */
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
void add_prof(ply_ptr, exp)
/* The add_prof function adjusts a player's magical realm and weapon     *
 * proficiency after the player gains an unassigned amount of experience *
 * such as from a quest item or pkilling.  The exp gain is divided       *
 * between the player's 5 weapon proficiency and 4 magic realms.      */

creature   *ply_ptr;
long        exp;
{
    int     n;

            if (exp)
                exp  = exp/9L;
            else return;
              
            for(n=0; n<9; n++) 
                if(n < 5)
                    ply_ptr->proficiency[n] += exp;
                else
                    ply_ptr->realm[n-5]  += exp;

        return;
}

/*====================================================================*/
/*                                              lose_all                                             */
/*====================================================================*/
/* lose_all causes the given player to lose all his or her possessions *
 * including any equipted items.                                       */

void lose_all(ply_ptr)
creature    *ply_ptr;
{
    object  *obj_ptr;
    room    *rom_ptr;
    otag    *op;
    int fd, i;

    fd = ply_ptr->fd;
    rom_ptr = ply_ptr->parent_rom;

/* remove all equipted items */
    for(i=0; i<MAXWEAR; i++) {
        if(ply_ptr->ready[i] && !F_ISSET(ply_ptr->ready[i], OCURSE)) {
            F_CLR(ply_ptr->ready[i], OWEARS);
            add_obj_crt(ply_ptr->ready[i], ply_ptr);
            ply_ptr->ready[i] = 0;
        }
    }


    compute_ac(ply_ptr);
    compute_thaco(ply_ptr);

/* remove and delete all possessions */
    op = ply_ptr->first_obj;
    while(op) {
            obj_ptr = op->obj;
            op = op->next_tag;
            del_obj_crt(obj_ptr, ply_ptr);
            free_obj(obj_ptr);
    }

}

/**********************************************************************/
/*                      dissolve_item                                 */
/**********************************************************************/
/* dissolve_item will randomly select one equipted (including held or *
 * wield) items on the given player and then delete it. The player    *
 * receives a message that the item was destroyed as well as who is   *
 * responsible for the deed.                                                                              */
void dissolve_item (ply_ptr,crt_ptr)
creature    *ply_ptr;
creature    *crt_ptr;
{
    int     n;
    char    checklist[MAXWEAR];
    int     numwear=0, i;

        for(i=0; i<MAXWEAR; i++) {
                checklist[i] = 0;
/*                if(i==WIELD-1 || i==HELD-1) continue; */
                if(ply_ptr->ready[i])
                        checklist[numwear++] = i+1;
        }

        if(!numwear)
            n = 0;
        else {
            i = mrand(0, numwear-1);
            n = (int) checklist[i];
        }

        if(n) {
        	if(!F_ISSET(ply_ptr->ready[n-1], ONEWEV)) {
            broadcast_rom(ply_ptr->fd,ply_ptr->rom_num,"%M가 %m님의 %s를 부숴버립니다.",
                crt_ptr, ply_ptr, ply_ptr->ready[n-1]->name);
            print(ply_ptr->fd,"%M가 당신의 %s를 부숴버립니다.\n",crt_ptr,
                ply_ptr->ready[n-1]->name);
            free_obj(ply_ptr->ready[n-1]);
            ply_ptr->ready[n-1] = 0;
            compute_ac(ply_ptr);
                    }
                    }
        return;
}

/**********************************************************************/
/*                              purchase                              */
/**********************************************************************/
/* purchase allows a playe rto buy an item from a monster.  The      *
 * purchase item flag must be set, and the monster must have an    *
 * object to sell.  The object for sale is determined by the first   *
 * object listed in carried items. */

int purchase(ply_ptr, cmnd)
creature        *ply_ptr;
cmd             *cmnd;
{
        creature        *crt_ptr;
        room            *rom_ptr;
                object          *obj_ptr, *obj_list[10];
                int                     maxitem = 0,obj_num[10];
                long            amt;
                int             fd, i, j, cnt, found = 0, match = 0;

        fd = ply_ptr->fd;
        rom_ptr = ply_ptr->parent_rom;

        if(cmnd->num < 2) {
                print(fd, "무엇을 구입하시려구요?\n");
                return(0);
        }

        if(cmnd->num < 3 ) {
                print(fd, "사용법: <괴물이름> <물건> 구입\n");
                return(0);
        }

        crt_ptr = find_crt(ply_ptr, rom_ptr->first_mon, cmnd->str[1],
                           cmnd->val[1]);

        if(!crt_ptr) {
                print(fd, "그것은 여기 없습니다.\n");
                return(0);
        }

        if (!F_ISSET(crt_ptr,MPURIT)){
                print(fd, "당신은 %m에게서 물건을 구입할 수 없습니다.\n",crt_ptr);
                return(0);
        }

        for (i=0;i <  10; i++)
                if (crt_ptr->carry[i] > 0){
                    found = 0;
                    for(j=0; j< maxitem; j++)
                        if (crt_ptr->carry[i] == obj_num[j])
                                found = 1;
                    if (!found){
                        maxitem++;
                        obj_num[i] = crt_ptr->carry[i];
                    }
                }

        if (!maxitem){
                print(fd, "%M%j 팔 물건을 갖고 있지 않습니다.\n",crt_ptr, "0");
                return(0);
        }

        found = 0;
        for(i=0;i< maxitem; i++){
                if (!(load_obj(crt_ptr->carry[i], &obj_ptr) < 0))
                if(EQUAL(obj_ptr, cmnd->str[2]) && (F_ISSET(ply_ptr, PDINVI) ?
                   1:!F_ISSET(obj_ptr, OINVIS))) {
                        match++;
                        if(match == cmnd->val[2]) {
                                found = 1;
                                break;
                        }
                }
        }

        if(!found){
                print(fd, "%M%j \"미안합니다. 그런 물건은 갖고 있지 않습니다.\"라고 말합니다.\n", crt_ptr, "1");
        return(0);
        }

        cnt = count_inv(ply_ptr, -1);

	if((weight_ply(ply_ptr) + weight_obj(obj_ptr) > max_weight(ply_ptr))
		|| cnt>150) {
		print(fd, "당신은 더이상 가질 수 없습니다.\n");

		return(0);
	}

        amt =  MAX(10,obj_ptr->value*1);
        if(ply_ptr->gold < amt){
                print(fd,
                "%M%j \"%d냥입니다. 깎아줄순 없습니다.\"라고 말합니다.\n",crt_ptr,"1", amt);
        }
        else {
                print(fd, "당신은 %m에게 %d냥을 줍니다.\n",crt_ptr,amt);
                print(fd, "%M%j \"고맙습니다. 여기 %s%j 있습니다.\"라고 말합니다.\n",crt_ptr,"1", obj_ptr->name,"1");
                broadcast_rom(fd, ply_ptr->rom_num, "%M이 %m에게 %i를 구입할 돈 %d냥을 줍니다.\n", ply_ptr, crt_ptr, obj_ptr, amt);
                ply_ptr->gold -= amt;
                add_obj_crt(obj_ptr, ply_ptr);
        }

        return(0);

}


/**********************************************************************/
/*                              selection                              */
/**********************************************************************/
/* The selection command  lists all the items  a monster is selling.  *
 * The monster needs the MPURIT flag set to denote it can sell. */

int selection(ply_ptr, cmnd)
creature        *ply_ptr;
cmd             *cmnd;
{
        creature        *crt_ptr;
        room            *rom_ptr;
        object          *obj_ptr[10];
        int             obj_list[10];
        int             fd, i, j, found = 0, maxitem = 0;

        fd = ply_ptr->fd;
        rom_ptr = ply_ptr->parent_rom;

        if(cmnd->num < 2) {
                print(fd, "누구의 물건을 봅니까?\n");
                return(0);
        }


        crt_ptr = find_crt(ply_ptr, rom_ptr->first_mon, cmnd->str[1],
                           cmnd->val[1]);

        if(!crt_ptr) {
                print(fd, "그런 사람은 없습니다.\n");
                return(0);
        }

        if(!F_ISSET(crt_ptr,MPURIT)){
                print(fd, "%M는 아무것도 없습니다.\n",crt_ptr);
                return(0);
        }
        
        for (i=0;i <  10; i++)
                if (crt_ptr->carry[i] > 0){
                    found = 0;
                    for(j=0; j< maxitem; j++)
                        if (crt_ptr->carry[i] == obj_list[j])
                                found = 1;
                    
                    if (!found){
                        maxitem++;
                        obj_list[i] = crt_ptr->carry[i];
                    }
                }

        if(!maxitem){
                print(fd, "%M은 팔 물건이 없습니다.\n",crt_ptr);
                return(0);
        }
        print(fd, "%M의 물건들:\n",crt_ptr);
        for (i=0;i<maxitem;i++)
                if ((load_obj(crt_ptr->carry[i], &(obj_ptr[i])) < 0)  ||
                        (crt_ptr->carry[i] == 0))
                        print(fd,"%d) � 매진.\n",i+1);
                else 
                        print(fd,"%d) %-22s    %ld냥\n",i+1, 
                                (obj_ptr[i])->name,MAX(10,((obj_ptr[i])->value)));
        print(fd,"\n"); 
        return(0);
 
}                 

 
 
/**********************************************************************/
/*                      trade                                         */
/**********************************************************************/
int trade(ply_ptr, cmnd)
creature        *ply_ptr;
cmd             *cmnd;
{
        creature    *crt_ptr;
        room        *rom_ptr;
        object          *obj_ptr, *trd_ptr;
        int         obj_list[5][2];
        int         fd, i, j, found = 0, maxitem = 0;

        fd = ply_ptr->fd;
        rom_ptr = ply_ptr->parent_rom;

        if(cmnd->num < 2) {
                print(fd, "누구와 교환하시려구요?\n");
                return(0);
        }

        if(cmnd->num < 3) {
                print(fd, "사용법: <물건> <괴물이름> 교환\n");
                return(0);
        }

        crt_ptr = find_crt(ply_ptr, rom_ptr->first_mon, cmnd->str[2],
                           cmnd->val[2]);

        if(!crt_ptr) {
                print(fd, "그것은 여기 없습니다.\n");
                return(0);
        }

        if(!F_ISSET(crt_ptr,MTRADE)){
                print(fd, "당신은 %M%j 교역할 수 없습니다.\n",crt_ptr, "2");
		print(fd, "%x %d %d %d\n",crt_ptr->flags[4],F_ISSET(crt_ptr,36),F_ISSET(crt_ptr,37),F_ISSET(crt_ptr,38));
                return(0);
        }

        obj_ptr = find_obj(ply_ptr, ply_ptr->first_obj,
                           cmnd->str[1], cmnd->val[1]);

        if(!obj_ptr) {
                print(fd, "당신은 그런 물건을 갖고 있지 않습니다.\n");
                return(0);
        }

        if(F_ISSET(obj_ptr, ONAMED)) {
                print(fd, "교역할 수 있는 물건이 아닙니다.\n");
                return(0);
        }

        for (i=0;i <  5; i++)
                if (crt_ptr->carry[i] > 0){
                    found = 0;
                    for(j=0; j< maxitem; j++)
                        if (crt_ptr->carry[i] == obj_list[j][0])
                                found = 1;

                    if (!found){
                        maxitem++;
                        obj_list[i][0] = crt_ptr->carry[i];
                        obj_list[i][1] = crt_ptr->carry[i+5];
                    }
                }

        if(!maxitem){
                print(fd, "%M%j 교역할 물건을 갖고있지 않습니다.\n",crt_ptr,"0");
                return(0);
        }

        found = 0;
        for(i=0;i < maxitem; i++){
        if (load_obj(obj_list[i][0], &trd_ptr) < 0)
                continue;
                if (!strcmp(obj_ptr->key[0], trd_ptr->key[0]) && !strcmp(obj_ptr->name, trd_ptr->name)){
                        found = i+1;
                        break;
                }
        }

        if(!found || ((obj_ptr->shotscur <= obj_ptr->shotsmax/10)&&(obj_ptr->type != MISC)))
                print(fd,"%M%j \"난 그런거 필요없어요!\"라고 말합니다.\n",crt_ptr,"1");
        else{
                if (crt_ptr->carry[found+4] == 0){
                        del_obj_crt(obj_ptr, ply_ptr);
                 /* 교환한 물건을 몹이 가지고 있을 이유 없음 */
                /*        add_obj_crt(obj_ptr, crt_ptr);   */
                        print(fd, "%M%j \"고맙습니다! %i%j 필요했는데 잘됐군요.\n",
                                crt_ptr,"1",obj_ptr,"1");
                        print(fd, "그런데 당신에게 줄게 없는데..\"라고 말합니다.\n");
                        broadcast_rom(fd, ply_ptr->rom_num,"%M이 %m에게 %i를 교환합니다.\n",
                                ply_ptr,crt_ptr,obj_ptr);
                }
                else if(!(load_obj((obj_list[found-1][1]), &trd_ptr) < 0)){

              if(trd_ptr->questnum && Q_ISSET(ply_ptr, trd_ptr->questnum-1)) {
                        print(fd, "당신은 이미 임무를 완수했습니다.\n");
                        return(0);
                }
                        del_obj_crt(obj_ptr, ply_ptr);
               /* 교환한 물건을 몹이 가지고 있을 이유 없음 */
                 /*       add_obj_crt(obj_ptr, crt_ptr);     */
                        add_obj_crt(trd_ptr, ply_ptr);

                        print(fd,"%M가 \"고맙습니다. 절 위해 %i를 찾아주시다니..\n",
                                crt_ptr,obj_ptr);
                        print(fd,"당신에게 %i로 보답을 하고싶습니다.\"라고 말합니다.\n",trd_ptr);
                        print(fd,"%M가 당신에게 %i를 줍니다.\n",crt_ptr,trd_ptr);
                        broadcast_rom(fd, ply_ptr->rom_num,"%M이 %m에게 %i를 교환합니다.\n",
                                ply_ptr,crt_ptr,obj_ptr);
        /*              broadcast_rom(fd, ply_ptr->rom_num,"%M이 %m에게 %i를 줍니다.\n", */
        /*                      ply_ptr,crt_ptr,obj_ptr); */

/* if return object is a quest item */
                        if(trd_ptr->questnum) {
                         print(fd, "임무를 완수했습니다! 버리지 마십시요!\n");
                         print(fd, "당신은 버리면 그걸 다시 주울 수 없습니다.\n");
                         Q_SET(ply_ptr, trd_ptr->questnum-1);
                         ply_ptr->experience += quest_exp[trd_ptr->questnum-1];
                         print(fd, "당신은 경험치 %ld 를 얻었습니다.\n",
                                quest_exp[trd_ptr->questnum-1]);
                         add_prof(ply_ptr,quest_exp[trd_ptr->questnum-1]);
                }
           }

        }
        return(0);
}

int room_obj_count(first_obj)
otag    *first_obj;
{

        long num = 0;

        while (first_obj)
{
                num++;
                first_obj = first_obj->next_tag;
}
        return (num);
}
