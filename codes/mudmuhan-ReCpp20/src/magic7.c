/*
 * MAGIC7.C:
 *
 *  Additional spell-casting routines.
 */

#include "mstruct.h"
#include "mextern.h"

/************************************************************************/
/*              resist_cold(������)             */
/************************************************************************/

/* This function allows players to cast the resist cold spell.  It will */
/* allow the player to resist fire breathed on them by dragons and  */
/* other breathers.                         */

int resist_cold(ply_ptr, cmnd, how)
creature    *ply_ptr;
cmd     *cmnd;
int     how;
{
    creature    *crt_ptr;
    room        *rom_ptr;
    int     fd;

    fd = ply_ptr->fd;
    rom_ptr = ply_ptr->parent_rom;

    if(ply_ptr->mpcur < 12 && how == CAST) {
        print(fd, "\n����� ������ �����մϴ�.\n");
        return(0);
    }

    if(!S_ISSET(ply_ptr, SRCOLD) && how == CAST) {
        print(fd, "\n����� ���� �׷� �ֹ��� �͵����� ���߽��ϴ�.\n");
        return(0);
    }
    if(spell_fail(ply_ptr)) {
                if(how==CAST)
                        ply_ptr->mpcur -= 12;
                return(0);
        }



    if(cmnd->num == 2) {
        ply_ptr->lasttime[LT_RCOLD].ltime = time(0);
        F_SET(ply_ptr, PRCOLD);
        broadcast_rom(fd, ply_ptr->rom_num, 
            "\n%M�� ��Ÿ������ ������ ��Ű�� �����ָ� �ܿ�ϴ�.\n���� ������ ������ ȭ�� ��ȣ�ɵ��� ���� �����ϸ� \n������ �ѷ��Դϴ�.", ply_ptr);
        if(how == CAST) {
            print(fd, "\n����� ��Ÿ������ ������ ��Ű�� �����ַ� �ܿ�ϴ�.\n����� ������ ������ ȭ�� ��ȣ�ɵ��� ���� �����ϸ�\n������ �ѷ��Դϴ�.\n");
            ply_ptr->mpcur -= 12;
            ply_ptr->lasttime[LT_RCOLD].interval = MAX(300, 1200 +
                bonus[ply_ptr->intelligence]*600);
    if (F_ISSET(ply_ptr->parent_rom,RPMEXT)){
            print(fd,"\n�� ���� ����� ����� �ֹ��� ��ȭ��ŵ�ϴ�.\n");
            ply_ptr->lasttime[LT_RCOLD].interval += 800L;
        }                                
        }
        else {
            print(fd, "\n����� ������ ������ ȭ�� ��ȣ�ɵ��� ���� �����ϸ�\n������ �ѷ��Դϴ�.\n");
            ply_ptr->lasttime[LT_RCOLD].interval = 1200L;
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
            print(fd, "\n�׷� ����� �������� �ʽ��ϴ� .\n");
            return(0);
        }

        F_SET(crt_ptr, PRCOLD);
        crt_ptr->lasttime[LT_RCOLD].ltime = time(0);
        broadcast_rom2(fd, crt_ptr->fd, ply_ptr->rom_num, 
            "\n%M�� %M�� �Կ� ��Ÿ������ ������ ��������� \n������ �ֹ��� �ܿ�ϴ�.", 
            ply_ptr, crt_ptr);
        print(crt_ptr->fd, "\n%M�� ����� �Կ� ��Ÿ������ ������ ���� ������\n�����ַ� �ܿ�ϴ�.\n����� ������ ������ ȭ�� ��ȣ�ɵ��� ���� �����ϸ�\n������ �ѷ��Դϴ�.\n", ply_ptr);

        if(how == CAST) {
            print(fd, "\n����� %M�� �Կ� ��Ÿ������ ������ ����\n������ �����ַ� �ܿ�ϴ�.\n���� ������ ȭ�� ��ȣ�ɵ��� ���� �����ϸ� ������\n�ѷ��Դϴ�.\n",
                crt_ptr);
            ply_ptr->mpcur -= 12;
            crt_ptr->lasttime[LT_RCOLD].interval = MAX(300, 1200 +
                bonus[ply_ptr->intelligence]*600);
        if (F_ISSET(ply_ptr->parent_rom,RPMEXT)){
            print(fd,"\n�� ���� ����� ����� �ּ����� ��ȭ��ŵ�ϴ�.\n");
            crt_ptr->lasttime[LT_RCOLD].interval += 800L;
        }                                
        }

        else {
            print(fd, "\n%M�� ������ ȭ�� ��ȣ�ɵ��� ���� �����ϸ�\n������ �ѷ��Դϴ�.\n", crt_ptr);
            crt_ptr->lasttime[LT_RCOLD].interval = 1200L;
        }

        return(1);
    }

}


/************************************************************************/
/*                          Breathe-water(������)                               */
/************************************************************************/

/* This function allows players to cast the resist cold spell.  It will */
/* allow the player to resist fire breathed on them by dragons and  */
/* other breathers.                         */

int breathe_water(ply_ptr, cmnd, how)
creature    *ply_ptr;
cmd     *cmnd;
int     how;
{
    creature    *crt_ptr;
    room        *rom_ptr;
    int     fd;

    fd = ply_ptr->fd;
    rom_ptr = ply_ptr->parent_rom;

    if(ply_ptr->mpcur < 12 && how == CAST) {
        print(fd, "\n����� ������ �����մϴ�.\n");
        return(0);
    }

    if(!S_ISSET(ply_ptr, SBRWAT) && how == CAST) {
        print(fd, "\n����� ���� �׷� �ֹ��� �͵����� ���߽��ϴ�.\n");
        return(0);
    }
    if(spell_fail(ply_ptr)) {
                if(how==CAST)
                        ply_ptr->mpcur -= 12;
                return(0);
        }


    if(cmnd->num == 2) {
        ply_ptr->lasttime[LT_BRWAT].ltime = time(0);
        F_SET(ply_ptr, PBRWAT);
        broadcast_rom(fd, ply_ptr->rom_num, 
            "\n%M�� ũ�� ȣ���� ����Ű�缭 �������� \n�ֹ��� �ܿ����ϴ�.\n���� ������ ��Һ��� �ι質 Ŀ�� ���ӿ��� ��������\n�ߵ� �� ���� �� �����ϴ�.\n", ply_ptr);
        if(how == CAST) {
            print(fd, "\n����� ũ�� ȣ���� ����Ű�缭 �������� �ֹ��� �ܿ�ϴ�.\n������ ��Һ��� �ι質 Ŀ�� ���ӿ��� ��������\n�ߵ��� ���� �� �����ϴ�.\n");
            ply_ptr->mpcur -= 12;
            ply_ptr->lasttime[LT_BRWAT].interval = MAX(300, 1200 +
                bonus[ply_ptr->intelligence]*600);
        if (F_ISSET(ply_ptr->parent_rom,RPMEXT)){
            print(fd,"\n�� ���� ����� ����� �ּ����� ��ȭ��ŵ�ϴ�.\n");
            ply_ptr->lasttime[LT_BRWAT].interval += 800L;
        }                                
        }
        else {
            print(fd, "\n����� ������ ��Һ��� �ι質 Ŀ�� ���ӿ��� ��������\n�ߵ��� ���� �� �����ϴ�.\n");
            ply_ptr->lasttime[LT_BRWAT].interval = 1200L;
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
            print(fd, "\n�׷� ����� �������� �ʽ��ϴ� .\n");
            return(0);
        }

        F_SET(crt_ptr, PBRWAT);
        crt_ptr->lasttime[LT_BRWAT].ltime = time(0);
        broadcast_rom2(fd, crt_ptr->fd, ply_ptr->rom_num, 
            "\n%M�� %M���� �����θ� ���̸� �ֹ��� �ܿ����ϴ�.\n���� ������ ��Һ��� �ι質 Ŀ�� ���ӿ��� ��������\n�ߵ��� ���� �� �����ϴ�.\n",
            ply_ptr, crt_ptr);
        print(crt_ptr->fd, "\n%M�� ��ſ��� �����θ� ���̸� �ֹ��� �ܿ����ϴ�.\n����� ������ ��Һ��� �ι質 Ŀ�� ���ӿ��� ��������\n�ߵ� �� ���� �� �����ϴ�.\n", ply_ptr);

        if(how == CAST) {
            print(fd, "\n����� %M���� �����θ� ���̸� �ֹ��� �ܿ�ϴ�.\n���� ������ ��Һ��� �ι質 Ŀ�� ���ӿ��� ��������\n�ߵ� �� ���� �� �����ϴ�.\n",
                crt_ptr);
            ply_ptr->mpcur -= 12;
            crt_ptr->lasttime[LT_BRWAT].interval = MAX(300, 1200 +
                bonus[ply_ptr->intelligence]*600);
        if (F_ISSET(ply_ptr->parent_rom,RPMEXT)){
            print(fd,"\n�� ���� ����� ����� �ֹ��� ��ȭ��ŵ�ϴ�.\n");
            crt_ptr->lasttime[LT_BRWAT].interval += 800L;
        }                                
        }

        else {
            print(fd, "\n%M�� ������ ��Һ��� �ι� Ŀ�� ���ӿ��� \n�������� �ߵ��� ���� �� �����ϴ�.\n", crt_ptr);
            crt_ptr->lasttime[LT_BRWAT].interval = 1200L;
        }

        return(1);
    }

}


/************************************************************************/
/*                      Stone-Shield(����ȣ)                                    */
/************************************************************************/

/* This function allows players to cast the earth shield spell.  It will */
/* allow the player to resist fire breathed on them by dragons and  */
/* other breathers.                         */

int earth_shield(ply_ptr, cmnd, how)
creature    *ply_ptr;
cmd     *cmnd;
int     how;
{
    creature    *crt_ptr;
    room        *rom_ptr;
    int     fd;

    fd = ply_ptr->fd;
    rom_ptr = ply_ptr->parent_rom;

    if(ply_ptr->mpcur < 12 && how == CAST) {
        print(fd, "\n����� ������ �����մϴ�.\n");
        return(0);
    }

    if(!S_ISSET(ply_ptr, SSSHLD) && how == CAST) {
        print(fd, "\n����� ���� �׷� �ֹ��� �͵����� ���߽��ϴ�.\n");
        return(0);
    }
if(spell_fail(ply_ptr)) {
                if(how==CAST)
                        ply_ptr->mpcur -= 12;
                return(0);
        }

    if(cmnd->num == 2) {
        ply_ptr->lasttime[LT_SSHLD].ltime = time(0);
        F_SET(ply_ptr, PSSHLD);
        broadcast_rom(fd, ply_ptr->rom_num, 
            "\n%M�� ���μ��� ������ ����ȣ �ֹ��� �ܿ����ϴ�.\n������ ������ ���� ��ȣ�ɵ��� �ö�� ���� ������\n���� �����߽��ϴ�.\n", ply_ptr);
        if(how == CAST) {
            print(fd, "\n����� ���μ��� ������ ����ȣ �ֹ��� �ܿ�ϴ�.\n������ ������ ���� ��ȣ�ɵ��� �ö�� ���������\n���� �����մϴ�.\n");
            ply_ptr->mpcur -= 12;
            ply_ptr->lasttime[LT_SSHLD].interval = MAX(300, 1200 +
                bonus[ply_ptr->intelligence]*600);
        if (F_ISSET(ply_ptr->parent_rom,RPMEXT)){
            print(fd,"\n�� ���� ����� ����� �ּ����� ��ȭ��ŵ�ϴ�.\n");
            ply_ptr->lasttime[LT_SSHLD].interval += 800L;
        }                                
        }
        else {
            print(fd, "\n������ ������ ���� ��ȣ�ɵ��� �ö�� ���������\n���� �����մϴ�.\n");
            ply_ptr->lasttime[LT_SSHLD].interval = 1200L;
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

        F_SET(crt_ptr, PSSHLD);
        crt_ptr->lasttime[LT_SSHLD].ltime = time(0);
        broadcast_rom2(fd, crt_ptr->fd, ply_ptr->rom_num, 
            "\n%M�� %M���� ������ �Ѹ��� ����ȣ �ֹ��� �ܿ�ϴ�.\n������ ������ ���� ��ȣ�ɵ��� �ö�� ���� ������\n���� �����մϴ�.\n",
            ply_ptr, crt_ptr);
        print(crt_ptr->fd, "\n%M�� ��ſ��� ������ �Ѹ��� ����ȣ �ֹ��� �ܿ����ϴ�.\n���ڱ� ������ ������ ���� ��ȣ�ɵ��� �ö�� ���������\n���� �����߽��ϴ�.\n", ply_ptr);

        if(how == CAST) {
            print(fd, "\n����� %M���� ������ �ֹ��� �ܿ����ϴ�.\n",
                crt_ptr);
            ply_ptr->mpcur -= 12;
            crt_ptr->lasttime[LT_SSHLD].interval = MAX(300, 1200 +
                bonus[ply_ptr->intelligence]*600);
        if (F_ISSET(ply_ptr->parent_rom,RPMEXT)){
            print(fd,"\n�� ���� ����� ����� �ֹ��� ��ȭ��ŵ�ϴ�.\n");
            crt_ptr->lasttime[LT_SSHLD].interval += 800L;
        }                                
        }

        else {
            print(fd, "\n������ ������ ���� ��ȣ�ɵ��� �ö�� %M��\n������ ���� �����մϴ�.\n", crt_ptr);
            crt_ptr->lasttime[LT_SSHLD].interval = 1200L;
        }

        return(1);
    }

}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*                  locate_person(õ����)                               */
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int locate_player(ply_ptr, cmnd, how)
creature    *ply_ptr;
cmd     *cmnd;
int     how;
{
    creature    *crt_ptr;
    room        *rom_ptr;
    int     fd, chance;
 
    fd = ply_ptr->fd;
    rom_ptr = ply_ptr->parent_rom;

    if(fd < 0) return(0);

    if(!S_ISSET(ply_ptr, SLOCAT) && how == CAST) {
        print(fd, "\n����� ���� �׷� �ֹ��� �͵����� ���߽��ϴ�.\n");
        return(0);
    }
    if(ply_ptr->mpcur < 15 && how == CAST) {
        print(fd, "\n����� ������ �����մϴ�.\n");
        return(0);
    }


 
    if(cmnd->num < 3) {
        print(fd, "\n������ �����մϱ�?\n");
        return(0);
    }
 
    lowercize(cmnd->str[2], 1);
    crt_ptr = find_who(cmnd->str[2]);
 
    if(!crt_ptr || F_ISSET(crt_ptr, PDMINV) ||
       (F_ISSET(crt_ptr, PINVIS) && !F_ISSET(ply_ptr, PDINVI))) {
        print(fd, "\n�׷� ����� �������� �ʽ��ϴ�.\n");
        return(0);
    }
    if(crt_ptr->class == DM && ply_ptr->class != DM) {
        print(fd,"�� ����� ���ŷ��� �ʹ� ���� ���ø� �� �� �����ϴ�.");
        return 0;
    }
    if(F_ISSET(crt_ptr, PMARRI)) {
    	print(fd,"�� ����� ���Ȱ�� ���� ���� �����ϴ�.");
    	return 0;
    }
    if(spell_fail(ply_ptr)) {
                if(how==CAST)
                        ply_ptr->mpcur -= 15;
                return(0);
        } 
        broadcast_rom(fd, ply_ptr->rom_num, 
            "\n%M�� õ���� �ֹ��� �ܿ����ϴ�.\n", ply_ptr);
        if(how == CAST) {
            print(fd, "\n����� ������ %M���� �����߽��ϴ�.\n",
                crt_ptr);
            ply_ptr->mpcur -= 15;
        }

    chance =  50 + (((ply_ptr->level+3)/4) - ((crt_ptr->level+3)/4))*5  +
        (bonus[ply_ptr->intelligence] - bonus[crt_ptr->intelligence])*5; 

    chance += (ply_ptr->class == MAGE) ? 5 : 0;
    chance = MIN(85,chance);

    if ((crt_ptr->class < SUB_DM) && (mrand(1,100) < chance)){
        display_rom(ply_ptr,crt_ptr->parent_rom);   
        chance += (crt_ptr->class == MAGE) ? 5 : 0;
        chance = MIN(85,chance);

        chance = 60 + (((crt_ptr->level+3)/4) - ((ply_ptr->level+3)/4))*5  +
           (bonus[crt_ptr->intelligence] - bonus[ply_ptr->intelligence])*5;
        chance += (crt_ptr->class == MAGE) ? 5 : 0;
        chance = MIN(85,chance);

        if  (mrand(1,100) < chance)
                print(crt_ptr->fd, "\n%M�� ����� ������ ������ ���� �ֽ��ϴ�.\n",
                    ply_ptr);
    }
    else{
        print(fd,"\n����� ������ ����ɼ� �����ϴ�.\n");

        chance = 65 + (((crt_ptr->level+3)/4) - ((ply_ptr->level+3)/4))*5  +
           (bonus[crt_ptr->intelligence] - bonus[ply_ptr->intelligence])*5;

        if  (mrand(1,100) < chance)
        print(crt_ptr->fd, "\n%M�� ����� ������ �����մϴ�.\n",
            ply_ptr);

    }
 
    return(1);
}
 
 
/**********************************************************************/
/*                          drain_exp(��ġ��)                               */
/**********************************************************************/
/* The spell drain_exp causes a player to lose a selected amout of  *
 * exp.  When a player loses exp, the player's magical realm and    *
 * weapon procifiency will reflect the change.  This spell is not   *
 * intended to be learned or casted by a player.  The 4th parameter *
 * if given, will base the exp loss on the DnS damage of the object */

int drain_exp(ply_ptr, cmnd, how, obj_ptr)
creature    *ply_ptr;
cmd     *cmnd;
int     how;
object  *obj_ptr;
{
    room        *rom_ptr;
    creature    *crt_ptr;
    int     fd;
    long    loss;
 
    fd = ply_ptr->fd;
    rom_ptr = ply_ptr->parent_rom;

    if(!S_ISSET(ply_ptr, SDREXP) && how == CAST) {
        print(fd, "\n����� ���� �׷� �ֹ��� �͵����� ���߽��ϴ�.\n");
        return(0);
    }
 
    if(how == CAST && ply_ptr->class < DM) {
        print(fd, "\n�׷� �ֹ��� �ܿ�� �����ϴ�.\n");
        return(0);
        }      

    if(how ==  SCROLL){
        print(fd, "\n�׷� �ֹ��� �ܿ�� �����ϴ�.\n");
        return(0);
        }      

    /* drain exp on self */
    if(cmnd->num == 2) {
 
        if(how == POTION || how == WAND) 
            loss= dice(((ply_ptr->level+3)/4), ((ply_ptr->level+3)/4),(((ply_ptr->level+3)/4))*10);

        else if(how == CAST)
            loss = dice(((ply_ptr->level+3)/4), ((ply_ptr->level+3)/4),1)*10;
            loss = MIN(loss, ply_ptr->experience);

        if(how == CAST || how == WAND)  {
            print(fd, "\n����� ���ڱ� ��û�� ���鼭 ���ݱ��� �ο��Դ� �������\n�������� �ʽ��ϴ�.\n!!��~~~ ����ġ�� �������� �𸣰ڴ�.!!\n");
            print(fd, "\n����� %d��ŭ�� ������� �������� �ʽ��ϴ�.\n",loss);
            broadcast_rom(fd, ply_ptr->rom_num, 
                      "\n%M�� �ڽſ��� ��ġ�� �ֹ��� �ܿ�ϴ�.\n�״� ���ڱ� ��û������ ������ �θ��� �Ÿ��ϴ�.\n",
                      ply_ptr);
        }
        else if(how == POTION){
            print(fd, "\n����� ���ڱ� ��û�� ���鼭 ���ݱ��� �ο��Դ� �������\n�������� �ʽ��ϴ�.\n!!��~~~ ����ġ�� �������� �𸣰ڴ�.!!\n");
            print(fd, "\n����� %d��ŭ�� ������� �������� �ʽ��ϴ�.\n",loss);
        }
            ply_ptr->experience -= loss;    
            lower_prof(ply_ptr,loss);
 
    }
 
    /* energy drain a monster or player */
    else {
 
        if(how == POTION || how == WAND) 
            loss = mdice(obj_ptr);
        else if(how == CAST)
            loss = dice(((ply_ptr->level+3)/4), ((ply_ptr->level+3)/4),1)*30;

        if(how == POTION) {
            print(fd, "\n�� ������ �ڽſ��Ը� ����Ҽ� �ֽ��ϴ�.\n");
            return(0);
        }
 
        cmnd->str[2][0] = up(cmnd->str[2][0]);
        crt_ptr = find_crt(ply_ptr, rom_ptr->first_ply,
                   cmnd->str[2], cmnd->val[2]);
 
        if(!crt_ptr) {
            cmnd->str[2][0] = low(cmnd->str[2][0]);
            crt_ptr = find_crt(ply_ptr, rom_ptr->first_mon,
                       cmnd->str[2], cmnd->str[2]);
 
            if(!crt_ptr) {
                print(fd, "\n�׷� ����� �������� �ʽ��ϴ� .\n");
                return(0);
            }
        }
 
 
        loss = MIN(loss, crt_ptr->experience);
        if(how == CAST || how == WAND) {
            print(fd, "\n����� %M���� ��ġ���� �ֹ��� �ܿ�ϴ�.\n�״� ���ڱ� ��û������ ������ �θ��� �Ÿ��ϴ�.\n", crt_ptr);
            broadcast_rom2(fd, crt_ptr->fd, ply_ptr->rom_num,
                       "\n%M�� %M���� ��ġ���� �ֹ��� �ܿ�ϴ�.\n�״� ���ڱ� ��û������ ������ �θ��� �Ÿ��ϴ�.\n",
                       ply_ptr, crt_ptr);
            print(crt_ptr->fd,
                     "\n%M�� ��ſ��� ��ġ���� �ֹ��� �ܿ�ϴ�.\n����� ���ڱ� ��û�����鼭 ���ݱ��� �ο��Դ� �������\n�������� �ʽ��ϴ�.\n!!��~~~ ����ġ�� �������� �𸣰ڴ�.!!\n",
         ply_ptr);
           print(crt_ptr->fd,"\n����� %d��ŭ�� ������� �������� �ʽ��ϴ�.\n",loss);
           print(fd,"\n%M�� �ο��� �󸶰��� ������� �ؾ������\n�����ϴ�.\n",crt_ptr);
        }
 
           crt_ptr->experience -= loss;    
            lower_prof(crt_ptr,loss);
    }
 
    return(1);
 
}
               
/**********************************************************************/
/*                      remove disease(ġ��)                            */
/**********************************************************************/
 
int rm_disease(ply_ptr, cmnd, how)
creature    *ply_ptr;
cmd     *cmnd;
int     how;
{
    room        *rom_ptr;
    creature    *crt_ptr;
    int     fd;
 
    fd = ply_ptr->fd;
    rom_ptr = ply_ptr->parent_rom;
 
    if(ply_ptr->mpcur < 12 && how == CAST) {
        print(fd, "\n����� ������ �����մϴ�.\n");
        return(0);
    }
 
        if(ply_ptr->class != CLERIC &&
           ply_ptr->class < INVINCIBLE && how == CAST) {
                print(fd, "\n�� �ּ��� �����ڸ��� ����� �� �ֽ��ϴ�.\n");
                return(0);
        }                         
 
    if(!S_ISSET(ply_ptr, SRMDIS) && how == CAST) {
        print(fd, "\n����� ���� �׷� �ֹ��� �͵����� ���߽��ϴ�.\n");
        return(0);
    }
        if(spell_fail(ply_ptr)) {
                if(how==CAST)
                        ply_ptr->mpcur -= 12;
                return(0);
        }

 
    if(cmnd->num == 2) {
 
        if(how == CAST)
            ply_ptr->mpcur -= 12;
 
        if(how == CAST || how == SCROLL || how == WAND)  {
            print(fd, "\n����� ������� ������ ġ�� �ֹ��� �ܿ�ϴ�.\n������ �ô޸��� ����� ���� Ȱ�⸦ ��� �����մϴ�.\n");
            broadcast_rom(fd, ply_ptr->rom_num, 
                      "\n%M�� ������� ������ ġ�� �ֹ��� �ܿ����ϴ�.\n������ �ô޸��� ���� ���� Ȱ�⸦ ��� �����մϴ�.\n",
                      ply_ptr);
        }
        else if(how == POTION & F_ISSET(ply_ptr, PDISEA))
            print(fd, "\n������ �ô޸��� ����� ���� Ȱ�⸦ ��� �����մϴ�.\n");
        else if(how == POTION)
            print(fd, "\n����� ���� �ؼҵǾ� ���� �Ŷ������ϴ�.\n");
 
        F_CLR(ply_ptr, PDISEA);
 
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
            cmnd->str[2][0] = low(cmnd->str[2][0]);
            crt_ptr = find_crt(ply_ptr, rom_ptr->first_mon,
                       cmnd->str[2], cmnd->str[2]);
 
            if(!crt_ptr) {
                print(fd, "\n�׷������ �������� �ʽ��ϴ� .\n");
                return(0);
            }
        }
 
        if(how == CAST) 
            ply_ptr->mpcur -= 12;
 
        F_CLR(crt_ptr, PDISEA);
 
        if(how == CAST || how == SCROLL || how == WAND) {
            print(fd, "\n����� %M�� ������ ������ ������ ���� ����\nġ�Ḧ �����մϴ�.\n���� ���� ���� �ִ� ���� Ǯ���鼭 ���� Ȱ�⸦ \n��� �����մϴ�.\n", crt_ptr);
            broadcast_rom2(fd, crt_ptr->fd, ply_ptr->rom_num,
                       "\n%M�� %M�� ������ ������ ������ ���� ����\nġ�Ḧ �����մϴ�.\n���� ���� ���� Ȱ�⸦ ��� �����ϴ� ���� ���Դϴ�.\n",
                       ply_ptr, crt_ptr);
            print(crt_ptr->fd, "\n%M�� ����� ������ ������ ������ ���� ���� ġ�Ḧ �����մϴ�.\n����� ���� ����� ���� ���� ���� Ǯ�� ���� \nȰ�⸦ ��� �����մϴ�.\n", ply_ptr);
        }
 
    }
 
    return(1);
 
}
 


/************************************************************************/
/*                           object_send(����)                               */
/************************************************************************/
/* The send object spell allows a mage  or DM/Caretaker to magically     *
 * transport an object to another login (detectable) player.  The maxium *
 * weight of the object being transported is based on the player's       *
 * level and intellignece. */

int object_send(ply_ptr, cmnd, how)
creature    *ply_ptr;
cmd     *cmnd;
int     how;
{
    creature    *crt_ptr;
    object      *obj_ptr;
    room        *rom_ptr;
    otag        *otag_ptr;
    int         fd, cost;
 
    fd = ply_ptr->fd;
    rom_ptr = ply_ptr->parent_rom;

    if(!S_ISSET(ply_ptr, STRANO) && how == CAST) {
        print(fd, "\n����� ���� �׷� �ֹ��� �͵����� ���߽��ϴ�.\n");
        return(0);
    }

    if(ply_ptr->class != MAGE && ply_ptr->class < INVINCIBLE && how == CAST) {
       print(fd, "\n�� �ּ��� ������鸸�� ����� �� �ֽ��ϴ�.\n");
       return(0);
    }                         

    if(ply_ptr->level <20){
        print(fd, "\n����� �ɷ��� �����մϴ�.\n");
        return(0);
    }
 
    if(cmnd->num < 4) {
        print(fd, "\n�������� ���� �����ϱ�?\n");
        return(0);
    }

    lowercize(cmnd->str[3], 1);
    crt_ptr = find_who(cmnd->str[3]);
 
    if(!crt_ptr || F_ISSET(crt_ptr, PDMINV) ||
       (F_ISSET(crt_ptr, PINVIS) && !F_ISSET(ply_ptr, PDINVI))) {
        print(fd, "\n�׷� ����� �������� �ʽ��ϴ� .\n");
        return(0);
    }
 
    obj_ptr = find_obj(ply_ptr, ply_ptr->first_obj,
        cmnd->str[2], cmnd->val[2]);
 
    if(!obj_ptr) {
        print(fd, "\n�׷� ������ �������� �ʽ��ϴ�.\n");
        return(0);
    }

        if(obj_ptr->questnum) {
                print(fd, "\n�ӹ��� ���õǾ� �ٸ��ڿ��� ���� �� �����ϴ�.\n");
                return(0);
        }

        if(F_ISSET(obj_ptr, ONEWEV)) {
        		print(fd, "\n�̺�Ʈ �������� �ٸ��ڿ��� ���� �� �����ϴ�.\n");
        		return(0);
        }

    if(obj_ptr->first_obj) {
        for(otag_ptr=obj_ptr->first_obj; otag_ptr; otag_ptr=otag_ptr->next_tag) {
            if(otag_ptr->obj->questnum && ply_ptr->class < DM) {
        		print(fd, "\n���ۿ� �����߽��ϴ�.\n");
        		return(0);
            }
            if(F_ISSET(otag_ptr->obj, ONEWEV) && ply_ptr->class < DM) {
        		print(fd, "\n���ۿ� �����߽��ϴ�.\n");
        		return(0);
            }
        }
    }

    cost = 5 + bonus[ply_ptr->intelligence] + (((ply_ptr->level+3)/4) - 5)*2;
    if (obj_ptr->weight > cost){
        print(fd,"\n%I�� ������ ����� �ɷ����� �ʹ� ���ſ� \n���� �� �����ϴ�.\n", obj_ptr);
        return(0);
    }

    cost = 8 + (obj_ptr->weight)/4;
    if(ply_ptr->mpcur < cost && how == CAST) {
        print(fd, "\n����� ������ �����մϴ�.\n");
        return(0);
    } else if(how == CAST) {
            ply_ptr->mpcur -= cost;
        }
        if(spell_fail(ply_ptr)) {
                return(0);
        }
    

    if (max_weight(crt_ptr) < weight_ply(crt_ptr)+obj_ptr->weight){
        print(fd,"\n%M�� %1i�� ���� �� �����ϴ� .\n",crt_ptr,obj_ptr);
        return(0);
    }
    del_obj_crt(obj_ptr, ply_ptr);
    add_obj_crt(obj_ptr, crt_ptr);

    savegame_nomsg(ply_ptr);
    if(crt_ptr->type==PLAYER) savegame_nomsg(crt_ptr);

    if(!F_ISSET(ply_ptr, PDMINV)){
            broadcast_rom(fd, ply_ptr->rom_num, "\n%M�� ������ ���а��� �������Ƚ��ϴ�.\n", ply_ptr);
    }
    print(fd,"\n����� %i�� ���������� �ϸ鼭 �����ָ� �ܿ�ϴ�.\n�� ������ �������� �������鼭 ���а��� �Ѿ˰���\n���󰩴ϴ�.\n",
        obj_ptr);
    print(fd, "\n����� %1i�� %M���� ���½��ϴ�.\n",
        obj_ptr, crt_ptr);
    if(!F_ISSET(ply_ptr, PDMINV)){
         print(crt_ptr->fd, "\n���ڱ� �ϴÿ��� ���𰡰� �ϴÿ��� �� ���������ϴ�.\n�ڼ��� ���� %1i�ε�, %M�� ������ ������ �� �����ϴ�.\n", obj_ptr, ply_ptr);
    }
    return(1);
 
}


