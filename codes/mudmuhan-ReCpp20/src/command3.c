/*
 * COMMAND3.C:
 *
 *      Additional user routines.
 *
 *      Copyright (C) 1991, 1992, 1993 Brett J. Vickers
 *
 */

#include "mstruct.h"
#include "mextern.h"

/**********************************************************************/
/*                              wear                                  */
/**********************************************************************/

/* This function allows the player pointed to by the first parameter */
/* to wear an item specified in the pointer to the command structure */
/* in the second parameter.  That is, if the item is wearable.       */

int wear(ply_ptr, cmnd)
creature        *ply_ptr;
cmd             *cmnd;
{
        room    *rom_ptr;
        object  *obj_ptr;
        int     fd, i, cantwear=0;
        int 	check_ac;

        fd = ply_ptr->fd;

        if(cmnd->num < 2) {
        print(fd, "뭘 입으실려구요?\n");
                return(0);
        }

        rom_ptr = ply_ptr->parent_rom;
        F_CLR(ply_ptr, PHIDDN);

        if(cmnd->num > 1) {

        if(!strcmp(cmnd->str[1], "모두")) {
                        wear_all(ply_ptr);
                        return(0);
                }

                obj_ptr = find_obj(ply_ptr, ply_ptr->first_obj,
                                   cmnd->str[1], cmnd->val[1]);

                if(!obj_ptr) {
            print(fd, "당신은 %S%j 가지고 있지 않습니다.",cmnd->str[1],"3");
                        return(0);
                }

                if(!obj_ptr->wearflag || obj_ptr->wearflag == WIELD ||
                   obj_ptr->wearflag == HELD) {
            print(fd, "%I%j 입는 물건이 아닙니다.",obj_ptr,"0");
                        return(0);
                }

                if(obj_ptr->type == ARMOR && F_ISSET(obj_ptr, ONOMAG) &&
                   (ply_ptr->class == MAGE || ply_ptr->class == CLERIC)) {
                        print(fd, "도술사, 불제자들은 사용할수 없습니다.");
                        return(0);
                }

                if(obj_ptr->type == ARMOR && F_ISSET(obj_ptr, ONOFEM) &&
                   !F_ISSET(ply_ptr,PMALES)) {
            print(fd, "%I%j 남자들만 입을 수 있습니다.",obj_ptr,"0");
                        return(0);
                }

                if(obj_ptr->type == ARMOR && F_ISSET(obj_ptr, ONOMAL) &&
                   F_ISSET(ply_ptr,PMALES)) {
            print(fd, "%I%j 여자들만 입을 수 있습니다.",obj_ptr,"0");
                        return(0);
                }

                if(obj_ptr->shotsmax > 1001 || obj_ptr->shotscur > 1001) {
                if(obj_ptr->questnum == 0) {
                	del_obj_crt(obj_ptr, ply_ptr);
                	free_obj(obj_ptr);
                	print(fd, "푸른 연기와 함께 사라졌습니다.");
                	return(0);
                }
                }

		if(obj_ptr->wearflag == BODY) check_ac = obj_ptr->armor *2;
		else check_ac = obj_ptr->armor * 5;

		if(check_ac > 151 && obj_ptr->questnum == 0) {
                	del_obj_crt(obj_ptr, ply_ptr);
                	free_obj(obj_ptr);
			print(fd, "푸른 연기와 함께 사라졌습니다.");
			return(0);
		}
		if(check_ac < 30) check_ac = 0;
		if(ply_ptr->class < INVINCIBLE && obj_ptr->questnum == 0 &&
			!F_ISSET(obj_ptr, ONEWEV) && ply_ptr->level < check_ac) {
			print(fd, "당신의 능력으로는 사용할 수 없는 물건입니다.");
			return(0);
		}


                if(obj_ptr->type == ARMOR && F_ISSET(obj_ptr, OMARRI) &&
                	!F_ISSET(ply_ptr, PMARRI)) {
                	print(fd, "%I%j 결혼한 사람들만 입을 수 있습니다.", obj_ptr, "0");
                	return(0);
                }

                if(obj_ptr->wearflag == NECK && ply_ptr->ready[NECK1-1] && 
                   ply_ptr->ready[NECK2-1]) {
            print(fd, "더이상 목에 걸 수 없습니다.");
                        return(0);
                }

                if(obj_ptr->wearflag == FINGER && ply_ptr->ready[FINGER1-1] &&
                   ply_ptr->ready[FINGER2-1] && ply_ptr->ready[FINGER3-1] &&
                   ply_ptr->ready[FINGER4-1] && ply_ptr->ready[FINGER5-1] &&
                   ply_ptr->ready[FINGER6-1] && ply_ptr->ready[FINGER7-1] &&
                   ply_ptr->ready[FINGER8-1]) {
            print(fd, "더이상 손가락에 낄 수 없습니다.");
                        return(0);
                }

                if(obj_ptr->wearflag != NECK && obj_ptr->wearflag != FINGER &&
                   ply_ptr->ready[obj_ptr->wearflag-1]) {
switch(obj_ptr->wearflag) {
    case HEAD:
            print(fd, "당신은 이미 %I%j 머리에 쓰고 있습니다.",ply_ptr->ready[obj_ptr->wearflag-1],"3");
            return(0);
    default:
            print(fd, "당신은 이미 %I%j 입고 있습니다.",ply_ptr->ready[obj_ptr->wearflag-1],"3");
            return(0);
}
                }

                if(obj_ptr->shotscur < 1) {
            print(fd, "%I%j 부서져서 입을 수 없게 되었습니다.",obj_ptr,"0");
                        return(0);
                }

                if(F_ISSET(obj_ptr, OGOODO) && ply_ptr->alignment < -50) {
            print(fd, "%I%j 당신 몸에서 튕겨져 나가 바닥에 떨어집니다.", obj_ptr,"1");
                        broadcast_rom(fd, ply_ptr->rom_num, 
                      "\n%M에게서 %i%j 튕겨져 나갑니다.", ply_ptr, obj_ptr,"1");
                        del_obj_crt(obj_ptr, ply_ptr);
                        add_obj_crt(obj_ptr, ply_ptr);
                        return(0);
                }

                if(F_ISSET(obj_ptr, OEVILO) && ply_ptr->alignment > 50) {
            print(fd, "%I%j 당신 몸에서 튕겨져 나가 바닥에 떨어집니다.", obj_ptr,"1");
                        broadcast_rom(fd, ply_ptr->rom_num, 
                      "\n%M에게서 %i%j 튕겨져 나갑니다.", ply_ptr, obj_ptr,"1");
                        del_obj_crt(obj_ptr, ply_ptr);
                        add_obj_crt(obj_ptr, ply_ptr);
                        return(0);
                }


      if(F_ISSET(obj_ptr,OCLSEL))
        if(!F_ISSET(obj_ptr,OCLSEL + ply_ptr->class) && ( ply_ptr->class < INVINCIBLE)){
            print(fd, "%i%j 당신의 직업에 맞지 않습니다.",obj_ptr,"0");
                return(0);
        }

                i = (F_ISSET(obj_ptr, OSIZE1) ? 1:0) * 2 +
                        (F_ISSET(obj_ptr, OSIZE2) ? 1:0);

                switch(i) {
                case 1:
                        if(ply_ptr->race != GNOME &&
                           ply_ptr->race != HOBBIT &&
                           ply_ptr->race != DWARF) cantwear = 1;
                        break;
                case 2:
                        if(ply_ptr->race != HUMAN &&
                           ply_ptr->race != ELF &&
                           ply_ptr->race != HALFELF &&
                           ply_ptr->race != ORC) cantwear = 1;
                        break;
                case 3:
                        if(ply_ptr->race != HALFGIANT) cantwear = 1;
                        break;
                }

                if(cantwear && ply_ptr->class < INVINCIBLE) {
            print(fd, "%I%j 당신 몸에 맞지 않습니다.", obj_ptr,"1");
                        return(0);
                }

                del_obj_crt(obj_ptr, ply_ptr);
                
                switch(obj_ptr->wearflag) {
                case BODY:
                case ARMS:
                case LEGS:
                case HANDS:
                case HEAD:
                case FEET:
                case FACE:
                case HELD:
                case SHIELD:
                        ply_ptr->ready[obj_ptr->wearflag-1] = obj_ptr;
                        break;
                case NECK:
                        if(ply_ptr->ready[NECK1-1])
                                ply_ptr->ready[NECK2-1] = obj_ptr;
                        else
                                ply_ptr->ready[NECK1-1] = obj_ptr;
                        break;
                case FINGER:
                        for(i=FINGER1; i<FINGER8+1; i++) {
                                if(!ply_ptr->ready[i-1]) {
                                        ply_ptr->ready[i-1] = obj_ptr;
                                        break;
                                }
                        }
                        break;
                }
        }

        compute_ac(ply_ptr);
switch(obj_ptr->wearflag) {
   case BODY:
    print(fd, "당신은 %1i%j 입었습니다.", obj_ptr,"3");
    broadcast_rom(fd, rom_ptr->rom_num, "\n%M%j %1i%j 입었습니다.",
            ply_ptr, "1",obj_ptr,"3"); break;
   case FEET:
    print(fd, "당신은 %1i%j 신었습니다.", obj_ptr,"3");
    broadcast_rom(fd, rom_ptr->rom_num, "\n%M%j %1i%j 신었습니다.",
            ply_ptr, "1",obj_ptr,"3"); break;
   case SHIELD:
    print(fd, "당신은 %1i%j 방패로 삼습니다.", obj_ptr,"3");
    broadcast_rom(fd, rom_ptr->rom_num, "\n%M%j %1i%j 방패로 삼습니다.",
            ply_ptr, "1",obj_ptr,"3"); break;
   case ARMS:
    print(fd, "당신은 %1i%j 팔에 장착합니다.", obj_ptr,"3");
    broadcast_rom(fd, rom_ptr->rom_num, "\n%M%j %1i%j 팔에 장착합니다.",
            ply_ptr, "1",obj_ptr,"3"); break;
   case HEAD:
    print(fd, "당신은 %1i%j 머리에 씁니다.", obj_ptr,"3");
    broadcast_rom(fd, rom_ptr->rom_num, "\n%M%j %1i%j 머리에 씁니다.",
            ply_ptr, "1",obj_ptr,"3"); break;
    case NECK:
    print(fd, "당신은 %1i%j 목에 두릅니다.",obj_ptr,"3"); 
    broadcast_rom(fd, rom_ptr->rom_num, "\n%M%j %1i%j 목에 겁니다.",
            ply_ptr, "1",obj_ptr,"3");break;
    case HANDS:
    print(fd, "당신은 %1i%j 손에 끼웁니다.",obj_ptr,"3"); 
    broadcast_rom(fd, rom_ptr->rom_num, "\n%M%j %1i%j 손에 끼웁니다.",
            ply_ptr, "1",obj_ptr,"3");break;
    case LEGS:
    print(fd, "당신은 %1i%j 입습니다.",obj_ptr,"3"); 
    broadcast_rom(fd, rom_ptr->rom_num, "\n%M%j %1i%j 입습니다.",
            ply_ptr, "1",obj_ptr,"3");break;
    case FACE:
    print(fd, "당신은 %1i%j 얼굴에 씁니다.",obj_ptr,"3"); 
    broadcast_rom(fd, rom_ptr->rom_num, "\n%M%j %1i%j 얼굴에 씁니다.",
            ply_ptr, "1",obj_ptr,"3");break;
    case FINGER:
    print(fd, "당신은 %1i%j 손가락에 끼웁니다.",obj_ptr,"3"); 
    broadcast_rom(fd, rom_ptr->rom_num, "\n%M%j %1i%j 손가락에 끼웁니다.",
            ply_ptr, "1",obj_ptr,"3");break;
    
    default:
    print(fd, "당신은 %1i%j 입었습니다.", obj_ptr,"3");
    broadcast_rom(fd, rom_ptr->rom_num, "\n%M%j %1i%j 입었습니다.",
            ply_ptr, "1",obj_ptr,"3");
}
        if(obj_ptr->use_output[0])
                print(fd, "\n%s", obj_ptr->use_output);
        F_SET(obj_ptr, OWEARS);

        return(0);

}

/**********************************************************************/
/*                              wear_all                              */
/**********************************************************************/

/* This function allows a player to wear everything in his inventory that */
/* he possibly can.                                                       */

void wear_all(ply_ptr)
creature        *ply_ptr;
{
        object  *obj_ptr;
        otag    *op, *temp;
        char    str[2048], str2[85];
        int     fd, i, found=0, cantwear=0;
        int check_ac;

        str[0] = 0;
        fd = ply_ptr->fd;

        op = ply_ptr->first_obj;

        while(op) {

                temp = op->next_tag;

                if((F_ISSET(ply_ptr, PDINVI) ?
                   1:!F_ISSET(op->obj, OINVIS)) && op->obj->wearflag &&
                   op->obj->wearflag != HELD && op->obj->wearflag != WIELD) {

                        obj_ptr = op->obj;

                        if(obj_ptr->type == ARMOR && F_ISSET(obj_ptr, ONOMAG) &&
                           (ply_ptr->class == MAGE || ply_ptr->class == CLERIC)) {
                                op = temp;
                                continue;
                        }

                if(obj_ptr->type == ARMOR && F_ISSET(obj_ptr, ONOFEM) &&
                   !F_ISSET(ply_ptr,PMALES)) {
                        op = temp;
                        continue;
                }

                if(obj_ptr->type == ARMOR && F_ISSET(obj_ptr, ONOMAL) &&
                   F_ISSET(ply_ptr,PMALES)) {
                        op = temp;
                        continue;
                }

                if(obj_ptr->type == ARMOR && F_ISSET(obj_ptr, OMARRI) &&
                	!F_ISSET(ply_ptr, PMARRI)) {
                	op = temp;
                	continue;
                }

                if(obj_ptr->shotsmax > 1001 || obj_ptr->shotscur > 1001) {
                if(obj_ptr->questnum == 0) {
                	op = temp;
                	continue;
                }
                }

		if(obj_ptr->wearflag == BODY) check_ac = obj_ptr->armor *2;
		else check_ac = obj_ptr->armor * 5;

		if(check_ac > 150 && obj_ptr->questnum == 0) {
                	op = temp;
                	continue;
		}
		if(check_ac < 30) check_ac = 0;
		if(ply_ptr->class < INVINCIBLE && obj_ptr->questnum == 0 &&
			!F_ISSET(obj_ptr, ONEWEV) && ply_ptr->level < check_ac) {
                	op = temp;
                	continue;
		}

                        if(obj_ptr->wearflag == NECK && 
                           ply_ptr->ready[NECK1-1] && 
                           ply_ptr->ready[NECK2-1]) {
                                op = temp;
                                continue;
                        }
        
                        if(obj_ptr->wearflag == FINGER && 
                           ply_ptr->ready[FINGER1-1] &&
                           ply_ptr->ready[FINGER2-1] && 
                           ply_ptr->ready[FINGER3-1] &&
                           ply_ptr->ready[FINGER4-1] && 
                           ply_ptr->ready[FINGER5-1] && 
                           ply_ptr->ready[FINGER6-1] && 
                           ply_ptr->ready[FINGER7-1] &&
                           ply_ptr->ready[FINGER8-1]) {
                                op = temp;
                                continue;
                        }

                        if(obj_ptr->wearflag != NECK && 
                           obj_ptr->wearflag != FINGER &&
                           ply_ptr->ready[obj_ptr->wearflag-1]) {
                                op = temp;
                                continue;
                        }

                        if(obj_ptr->shotscur < 1) {
                                op = temp;
                                continue;
                        }


      if(F_ISSET(obj_ptr,OCLSEL))
        if(!F_ISSET(obj_ptr,OCLSEL + ply_ptr->class) && ( ply_ptr->class < INVINCIBLE)){
                                op = temp;
                                continue;
        }

                        if(F_ISSET(obj_ptr, OGOODO) && 
                           ply_ptr->alignment < -50) {
                                op = temp;
                                continue;
                        }

                        if(F_ISSET(obj_ptr, OEVILO) && 
                           ply_ptr->alignment > 50) {
                                op = temp;
                                continue;
                        }

                        i = (F_ISSET(obj_ptr, OSIZE1) ? 1:0) * 2 +
                                (F_ISSET(obj_ptr, OSIZE2) ? 1:0);

                        switch(i) {
                        case 1:
                                if(ply_ptr->race != GNOME &&
                                   ply_ptr->race != HOBBIT &&
                                   ply_ptr->race != DWARF) cantwear = 1;
                                break;
                        case 2:
                                if(ply_ptr->race != HUMAN &&
                                   ply_ptr->race != ELF &&
                                   ply_ptr->race != HALFELF &&
                                   ply_ptr->race != ORC) cantwear = 1;
                                break;
                        case 3:
                                if(ply_ptr->race != HALFGIANT) cantwear = 1;
                                break;
                        }

                        if(cantwear) {
                                op = temp;
                                cantwear = 0;
                                continue;
                        }

                        if(obj_ptr->wearflag == NECK) {
                                if(ply_ptr->ready[NECK1-1])
                                        ply_ptr->ready[NECK2-1] = obj_ptr;
                                else
                                        ply_ptr->ready[NECK1-1] = obj_ptr;
                                F_SET(obj_ptr, OWEARS);
                        }

                        else if(obj_ptr->wearflag == FINGER) {
                                for(i=FINGER1; i<FINGER8+1; i++) {
                                        if(!ply_ptr->ready[i-1]) {
                                                ply_ptr->ready[i-1] = obj_ptr;
                                                F_SET(obj_ptr, OWEARS);
                                                break;
                                        }
                                }
                        }

                        else {
                                ply_ptr->ready[obj_ptr->wearflag-1] = obj_ptr;
                                if(obj_ptr->use_output[0])
                                        print(fd, "%s\n", obj_ptr->use_output);
                                F_SET(obj_ptr, OWEARS);
                        }

                        sprintf(str2, "%s, ", obj_str(obj_ptr, 1, 0));
                        strcat(str, str2);
                        del_obj_crt(obj_ptr, ply_ptr);
                        found = 1;

                }
                op = temp;
        }

        if(found) {
                compute_ac(ply_ptr);
                str[strlen(str)-2] = 0;
        print(fd, "당신은 %S%j 입었습니다.", str,"3");
        broadcast_rom(fd, ply_ptr->rom_num, "\n%M이 %S%j 입었습니다.",
                              ply_ptr, str,"3");
        }
        else
        print(fd, "당신은 입을 물건을 가지고 있지 않습니다.");

}

/**********************************************************************/
/*                              remove                                */
/**********************************************************************/

/* This function allows the player pointed to by the first parameter to */
/* remove the item specified by the command structure in the second     */
/* parameter from those things which he is wearing.                     */

int remove_obj(ply_ptr, cmnd)
creature        *ply_ptr;
cmd             *cmnd;
{
        room    *rom_ptr;
        object  *obj_ptr;
        otag    *op;
        int     fd, found=0, match=0, i;

        fd = ply_ptr->fd;

        if(cmnd->num < 2) {
        print(fd, "뭘 벗고 싶으세요?");
                return(0);
        }

        rom_ptr = ply_ptr->parent_rom;
        F_CLR(ply_ptr, PHIDDN);

        i = 0;
        if(cmnd->num > 1) {

        if(!strcmp(cmnd->str[1], "모두")) {
                        remove_all(ply_ptr);
                        return(0);
                }

                while(i<20) {
                        obj_ptr = ply_ptr->ready[i];
                        if(!obj_ptr) {
                                i++;
                                continue;
                        }
                        if(EQUAL(obj_ptr, cmnd->str[1]) &&
                           (F_ISSET(ply_ptr, PDINVI) ?
                           1:!F_ISSET(obj_ptr, OINVIS))) {
                                match++;
                                if(match == cmnd->val[1]) {
                                        found = 1;
                                        break;
                                }
                        }
                        i++;
                }

                if(!found) {
            print(fd, "당신은 그것을 입고 있지 않습니다.");
                        return(0);
                }

                if(F_ISSET(obj_ptr, OCURSE)) {
            print(fd, "이쿠! 그것이 몸에서 떨어지지 않습니다! 저주받은 물건 같군요.");
                        return(0);
                }

                add_obj_crt(obj_ptr, ply_ptr);
                ply_ptr->ready[i] = 0;
                compute_ac(ply_ptr);
                compute_thaco(ply_ptr);

        print(fd, "당신은 %1i%j 벗었습니다.", obj_ptr,"3");
        broadcast_rom(fd, rom_ptr->rom_num, "\n%M이 %1i%j 벗었습니다.",
                              ply_ptr, obj_ptr,"3");
                F_CLR(obj_ptr, OWEARS);
        }

        return(0);

}

/**********************************************************************/
/*                              remove_all                            */
/**********************************************************************/

/* This function allows the player pointed to in the first parameter */
/* to remove everything he is wearing all at once.                   */

void remove_all(ply_ptr)
creature        *ply_ptr;
{
        char    str[2048], str2[85];
        int     fd, i, found=0;

        fd = ply_ptr->fd;
        str[0] = 0;

        for(i=0; i<MAXWEAR; i++) {
                if(ply_ptr->ready[i] && !F_ISSET(ply_ptr->ready[i], OCURSE)) {
                        sprintf(str2,"%s, ", obj_str(ply_ptr->ready[i], 1, 0));
                        strcat(str, str2);
                        F_CLR(ply_ptr->ready[i], OWEARS);
						F_CLR(ply_ptr->ready[i], OWHELD);
                        add_obj_crt(ply_ptr->ready[i], ply_ptr);
                        ply_ptr->ready[i] = 0;
                        found = 1;
                }
        }

        if(!found) {
        print(fd,"당신은 걸치고 있는게 아무것도 없습니다.");
                return;
        }

        compute_ac(ply_ptr);
        compute_thaco(ply_ptr);

        str[strlen(str)-2] = 0;
    broadcast_rom(fd, ply_ptr->rom_num, "\n%M이 %S%j 벗었습니다.", ply_ptr, str,"3");
    print(fd, "당신은 %S%j 벗었습니다.", str,"3");

}

/**********************************************************************/
/*                              equipment                             */
/**********************************************************************/

/* This function outputs to the player all of the equipment that he is */
/* wearing/wielding/holding on his body.                               */

int equipment(crt_ptr, cmnd)
creature        *crt_ptr;
cmd             *cmnd;
{
        int     fd = crt_ptr->fd;
        int     i, found=0;

        for(i=0; i<MAXWEAR; i++)
                if(crt_ptr->ready[i])
                        found = 1;

        if(!found) {
        print(fd, "당신은 걸치고 있는게 아무것도 없습니다.");
                return(0);
        }
        if(F_ISSET(crt_ptr, PBLIND)) {
                ANSI(fd, RED);
                print(fd, "당신은 아무것도 볼수가 없습니다. 당신은 눈이 멀어 있습니다.");
                ANSI(fd, WHITE);
                return(0);
        }       
        print(fd, "착용 장비:\n");
        equip_list(fd, crt_ptr);
        return(0);

}

/**********************************************************************/
/*                              equip_list                            */
/**********************************************************************/

void equip_list(fd, crt_ptr)
int             fd;
creature        *crt_ptr;
{

        if(crt_ptr->ready[HEAD-1])
        print(fd, "[ 머리 ]  %1i\n", crt_ptr->ready[HEAD-1]);
        if(crt_ptr->ready[FACE-1])
        print(fd, "[ 얼굴 ]  %1i\n", crt_ptr->ready[FACE-1]);
        if(crt_ptr->ready[NECK1-1])
        print(fd, "[  목  ]  %1i\n", crt_ptr->ready[NECK1-1]);
        if(crt_ptr->ready[NECK2-1])
        print(fd, "[  목  ]  %1i\n", crt_ptr->ready[NECK2-1]);
        if(crt_ptr->ready[BODY-1])
        print(fd, "[  몸  ]  %1i\n", crt_ptr->ready[BODY-1]);
        if(crt_ptr->ready[ARMS-1])
        print(fd, "[  팔  ]  %1i\n", crt_ptr->ready[ARMS-1]);
        if(crt_ptr->ready[HANDS-1])
        print(fd, "[  손  ]  %1i\n", crt_ptr->ready[HANDS-1]);
        if(crt_ptr->ready[FINGER1-1])
        print(fd, "[손가락]  %1i\n", crt_ptr->ready[FINGER1-1]);
        if(crt_ptr->ready[FINGER2-1])
        print(fd, "[손가락]  %1i\n", crt_ptr->ready[FINGER2-1]);
        if(crt_ptr->ready[FINGER3-1])
        print(fd, "[손가락]  %1i\n", crt_ptr->ready[FINGER3-1]);
        if(crt_ptr->ready[FINGER4-1])
        print(fd, "[손가락]  %1i\n", crt_ptr->ready[FINGER4-1]);
        if(crt_ptr->ready[FINGER5-1])
        print(fd, "[손가락]  %1i\n", crt_ptr->ready[FINGER5-1]);
        if(crt_ptr->ready[FINGER6-1])
        print(fd, "[손가락]  %1i\n", crt_ptr->ready[FINGER6-1]);
        if(crt_ptr->ready[FINGER7-1])
        print(fd, "[손가락]  %1i\n", crt_ptr->ready[FINGER7-1]);
        if(crt_ptr->ready[FINGER8-1])
        print(fd, "[손가락]  %1i\n", crt_ptr->ready[FINGER8-1]);
        if(crt_ptr->ready[LEGS-1])
        print(fd, "[ 다리 ]  %1i\n", crt_ptr->ready[LEGS-1]);
        if(crt_ptr->ready[FEET-1])
        print(fd, "[  발  ]  %1i\n", crt_ptr->ready[FEET-1]);
        if(crt_ptr->ready[HELD-1])
        print(fd, "[쥔물건]  %1i\n", crt_ptr->ready[HELD-1]);
        if(crt_ptr->ready[SHIELD-1])
        print(fd, "[ 방패 ]  %1i\n", crt_ptr->ready[SHIELD-1]);
        if(crt_ptr->ready[WIELD-1])
        print(fd, "[ 무기 ]  %1i\n", crt_ptr->ready[WIELD-1]);
}

/**********************************************************************/
/*                              ready                                 */
/**********************************************************************/

/* This function allows the player pointed to by the first parameter to */
/* ready a weapon specified in the second, if it is a weapon.           */

int ready(ply_ptr, cmnd)
creature        *ply_ptr;
cmd             *cmnd;
{
        object  *obj_ptr;
        otag    *op;
        int     fd, i, cantwear=0;
        int 	check_dmg;

        fd = ply_ptr->fd;

        if(cmnd->num < 2) {
        print(fd, "무엇을 무장하시려구요?");
                return(0);
        }

        F_CLR(ply_ptr, PHIDDN);

        if(cmnd->num > 1) {

                obj_ptr = find_obj(ply_ptr, ply_ptr->first_obj,
                                   cmnd->str[1], cmnd->val[1]);

                if(!obj_ptr) {
            print(fd, "당신은 그런 물건을 가지고 있지 않습니다.");
                        return(0);
                }

                if(obj_ptr->wearflag != WIELD) {
            print(fd, "당신은 그것을 무장할 수 없습니다.");
                        return(0);
                }

		if((obj_ptr->type == SHARP || obj_ptr->type == THRUST) &&
		  	(obj_ptr->questnum == 0 ) && !F_ISSET(obj_ptr, ONEWEV) && 
		  	(obj_ptr->ndice*obj_ptr->sdice+obj_ptr->pdice ) > 14 && 
			(ply_ptr->class == MAGE || ply_ptr->class == CLERIC )) {
			print(fd, "도술사, 불제자는 사용할수 없습니다.");
			return(0);
		}

                if((obj_ptr->type == SHARP || obj_ptr->type == THRUST) &&
		   F_ISSET(obj_ptr, ONOFEM) && !F_ISSET(ply_ptr,PMALES)) {
		        print(fd, "%I%j 남자들만 무장할 수 있습니다.",obj_ptr,"0");
                        return(0);
                }

                if((obj_ptr->type == SHARP || obj_ptr->type == THRUST) &&
		   F_ISSET(obj_ptr, ONOMAL) && F_ISSET(ply_ptr,PMALES)) {
		        print(fd, "%I%j 여자들만 무장할 수 있습니다.",obj_ptr,"0");
                        return(0);
                }

                if(ply_ptr->ready[WIELD-1]) {
		        print(fd, "당신은 이미 %I%j 무장하고 있습니다.",ply_ptr->ready[WIELD-1],"3");
                        return(0);
                }


      if(F_ISSET(obj_ptr,OCLSEL))
        if(!F_ISSET(obj_ptr,OCLSEL + ply_ptr->class) && ( ply_ptr->class < INVINCIBLE)){
            print(fd, "%i%j 당신의 직업에 맞지 않습니다.",obj_ptr,"0");
                return(0);
        }

                if(F_ISSET(obj_ptr, OGOODO) && ply_ptr->alignment < -50) {
            print(fd, "%I%j 당신의 몸에서 튕겨져 나가 바닥에 떨어집니다.", obj_ptr,"1");
                        broadcast_rom(fd, ply_ptr->rom_num, 
                      "\n%M에게서 %i%j 튕겨져 나갑니다.", ply_ptr, obj_ptr,"1");
                        del_obj_crt(obj_ptr, ply_ptr);
                        add_obj_rom(obj_ptr, ply_ptr->parent_rom);
                        return(0);
                }

                if(F_ISSET(obj_ptr, OEVILO) && ply_ptr->alignment > 50) {
            print(fd, "%I%j 당신의 몸에서 튕겨져 나가 바닥에 떨어집니다.", obj_ptr,"1");
                        broadcast_rom(fd, ply_ptr->rom_num, 
                      "\n%M에게서 %i%j 튕겨져 나갑니다.", ply_ptr, obj_ptr,"1");
                        del_obj_crt(obj_ptr, ply_ptr);
                        add_obj_rom(obj_ptr, ply_ptr->parent_rom);
                        return(0);
                }

                i = (F_ISSET(obj_ptr, OSIZE1) ? 1:0) * 2 +
                        (F_ISSET(obj_ptr, OSIZE2) ? 1:0);

                switch(i) {
                case 1:
                        if(ply_ptr->race != GNOME &&
                           ply_ptr->race != HOBBIT &&
                           ply_ptr->race != DWARF) cantwear = 1;
                        break;
                case 2:
                        if(ply_ptr->race != HUMAN &&
                           ply_ptr->race != ELF &&
                           ply_ptr->race != HALFELF &&
                           ply_ptr->race != ORC) cantwear = 1;
                        break;
                case 3:
                        if(ply_ptr->race != HALFGIANT) cantwear = 1;
                        break;
                }

                if(cantwear && ply_ptr->class < INVINCIBLE) {
            print(fd, "%I%j 당신의 몸 크기와 맞지 않습니다.",
                                obj_ptr,"0");
                        return(0);
                }

                if((obj_ptr->ndice*obj_ptr->sdice+obj_ptr->pdice)>39) {
                  if(obj_ptr->questnum == 0 && !F_ISSET(obj_ptr, ONEWEV)) {
                    del_obj_crt(obj_ptr, ply_ptr);
                    free_obj(obj_ptr);
                    print(fd,"당신이 무기를 쥐자 푸른 빛을 내며 사라집니다.");
                    return(0);
                  }
                }
                if(!F_ISSET(obj_ptr, ONEWEV) && (obj_ptr->shotsmax > 600 || obj_ptr->shotscur > 600)) {
                	del_obj_crt(obj_ptr, ply_ptr);
                	free_obj(obj_ptr);
                	print(fd,"당신이 무기를 쥐자 푸른 빛을 내며 사라집니다.");
                	return(0);
                }
                if(F_ISSET(obj_ptr, ONSHAT) && F_ISSET(obj_ptr, OALCRT)) {
                	del_obj_crt(obj_ptr, ply_ptr);
                	free_obj(obj_ptr);
                	print(fd,"당신이 무기를 쥐자 푸른 빛을 내며 사라집니다.");
                	return(0);
                }
                check_dmg = obj_ptr->ndice * obj_ptr->sdice + obj_ptr->pdice;
                if(ply_ptr->class == FIGHTER) check_dmg -= 7;
                if(ply_ptr->class == ASSASSIN || ply_ptr->class == THIEF)
                	check_dmg -= 3;
                if(ply_ptr->class == PALADIN || ply_ptr->class == RANGER)
                	check_dmg -= 2;
		if(ply_ptr->class < INVINCIBLE && check_dmg > 15 &&
			!F_ISSET(obj_ptr, ONEWEV) && ply_ptr->level < (check_dmg * 3) && obj_ptr->questnum == 0) {
			print(fd, "당신의 능력으로는 사용할 수 없는 무기입니다.");
			return(0);
		}
		if(F_ISSET(obj_ptr, ONEWEV) && strcmp(obj_ptr->key[2], ply_ptr->name)) {
			print(fd, "다른 사람의 물건은 사용할 수 없습니다.");
			return(0);
		}

                ply_ptr->ready[WIELD-1] = obj_ptr;
                compute_thaco(ply_ptr);
                del_obj_crt(obj_ptr, ply_ptr);

        print(fd, "당신은 %1i%j 전투태세를 취합니다.", obj_ptr,"4");
        broadcast_rom(fd, ply_ptr->rom_num, "\n%M%j %1I%j 무장합니다.",
                              ply_ptr, "1", obj_ptr,"4");

                if(obj_ptr->use_output[0])
                        print(fd, "\n%s", obj_ptr->use_output);
                F_SET(obj_ptr, OWEARS);
        }

        return(0);

}

/**********************************************************************/
/*                              hold                                  */
/**********************************************************************/

/* This function allows a player to hold an item if it is designated */
/* as a hold-able item.                                              */

int hold(ply_ptr, cmnd)
creature        *ply_ptr;
cmd             *cmnd;
{
        object  *obj_ptr;
        otag    *op;
        int     fd, found=0, match=0;

        fd = ply_ptr->fd;

        if(cmnd->num < 2) {
        print(fd, "무엇을 쥐실려구요?");
                return(0);
        }

        F_CLR(ply_ptr, PHIDDN);

        if(cmnd->num > 1) {

                obj_ptr = find_obj(ply_ptr, ply_ptr->first_obj,
                                   cmnd->str[1], cmnd->val[1]);

                if(!obj_ptr) {
            print(fd, "당신은 그런 것을 가지고 있지 않습니다.");
                        return(0);
                }

                if(obj_ptr->wearflag != HELD && obj_ptr->wearflag != WIELD) {
            print(fd, "당신은 그것을 쥘 수 없습니다.");
                        return(0);
                }
				if(F_ISSET(obj_ptr, OEVENT) || obj_ptr->questnum > 0) {
					print(fd, "당신은 그것을 쥘 수 없습니다.");
						return(0);
				}
                if(F_ISSET(obj_ptr, ONEWEV) || obj_ptr->questnum > 0) {
                    print(fd, "당신은 그것을 쥘 수 없습니다.");
                        return(0);
                }

                if(ply_ptr->ready[HELD-1]) {
            print(fd, "당신은 이미 다른것을 쥐고 있습니다.");
                        return(0);
                }
			if(obj_ptr->ndice * obj_ptr->sdice + obj_ptr->pdice > 100) {
					print(fd, "당신은 그것을 쥘 수 없습니다.");
						return(0);
				}



      if(F_ISSET(obj_ptr,OCLSEL))
        if(!F_ISSET(obj_ptr,OCLSEL + ply_ptr->class) && ( ply_ptr->class < INVINCIBLE)){
            print(fd, "%i%j 당신의 직업에 맞지 않습니다.",obj_ptr,"0");
                return(0);
        }

                if(F_ISSET(obj_ptr, OGOODO) && ply_ptr->alignment < -50) {
            print(fd, "%I%j 당신의 손에서 튕겨져 나가 땅에 떨어집니다.", obj_ptr,"1");
                        broadcast_rom(fd, ply_ptr->rom_num, 
                      "\n%M에게서 %i%j 튕겨져 나갑니다.", ply_ptr, obj_ptr,"1");
                        del_obj_crt(obj_ptr, ply_ptr);
                        add_obj_rom(obj_ptr, ply_ptr->parent_rom);
                        return(0);
                }

                if(F_ISSET(obj_ptr, OEVILO) && ply_ptr->alignment > 50) {
            print(fd, "%I%j 당신의 손에서 튕겨져 나가 땅에 떨어집니다.", obj_ptr,"1");
                        broadcast_rom(fd, ply_ptr->rom_num, 
                      "\n%M에게서 %i%j 튕겨져 나갑니다.", ply_ptr, obj_ptr,"1");
                        del_obj_crt(obj_ptr, ply_ptr);
                        add_obj_rom(obj_ptr, ply_ptr->parent_rom);
                        return(0);
                }

                ply_ptr->ready[HELD-1] = obj_ptr;
                del_obj_crt(obj_ptr, ply_ptr);

        print(fd, "당신은 %1i%j 쥐었습니다.", obj_ptr,"3");
        broadcast_rom(fd, ply_ptr->rom_num, "\n%M이 %1i%j 쥐었습니다.",
                              ply_ptr, obj_ptr,"3");
                if(obj_ptr->use_output[0] && obj_ptr->type != POTION)
                        print(fd, "\n%s", obj_ptr->use_output);
                F_SET(obj_ptr, OWEARS);
				if(obj_ptr->type < ARMOR) {
					F_SET(obj_ptr, OWHELD);
				}
        }

        return(0);

}
/************************************************************************/
/*                              info_obj                                */
/************************************************************************/

/*  도둑의 물건 살펴보는 루틴.                */

int info_obj(ply_ptr, cmnd)
creature        *ply_ptr;
cmd             *cmnd;
{
    room            *rom_ptr;
    char            str[1024];
    object          *obj_ptr;
    int             fd, n, match=0;
    int len;

    fd = ply_ptr->fd;

    if(ply_ptr->class != THIEF && ply_ptr->class < INVINCIBLE) {
        print(fd, "도둑만 물건을 감정할수 있습니다.");
        return(0);
    }

    if(cmnd->num < 2) {
        print(fd, "무엇을 감정하실려구요?");
        return(0);
    }


        obj_ptr = find_obj(ply_ptr, ply_ptr->first_obj,
                   cmnd->str[1], cmnd->val[1]);

        if(!obj_ptr) {
            print(fd, "당신은 그런것을 갖고 있지 않습니다.");
            return(0);
        }


        print(fd, "이름: %s\n", obj_ptr->name);


        print(fd, "사용회수 %d\n", obj_ptr->shotscur);

        print(fd, "종류: ");
        if(obj_ptr->type <= MISSILE) {
                switch(obj_ptr->type) {
                case SHARP: print(fd, "도"); break;
                case THRUST: print(fd, "검"); break;
                case BLUNT: print(fd, "봉"); break;
                case POLE: print(fd, "창"); break;
                case MISSILE: print(fd, "궁"); break;
                }
                print(fd, " 무기.\n");
                print(fd, "타격치: %d면%d굴림 더하기 %d", obj_ptr->sdice, obj_ptr->ndice,
                    obj_ptr->pdice);
                if(obj_ptr->adjustment)
                    print(fd, " (+%d)\n", obj_ptr->adjustment);
                else
                    print(fd, "\n");

        }
        else {
                switch(obj_ptr->type) {
                case ARMOR: print(fd, "방어구"); 
                     print(fd, "\n방어력: %2.2d", obj_ptr->armor); break;
                case POTION: print(fd, "약"); break;
                case SCROLL: print(fd, "주문서"); break;
                case WAND: print(fd, "주문걸린 물건"); break;
                case CONTAINER: print(fd, "담는 종류"); break;
                case KEY: print(fd, "열쇠"); break;
                case LIGHTSOURCE: print(fd, "광원"); break;
                case MISC: print(fd, "모르겠음"); break;
                }
                print(fd,"\n");
        }

        strcpy(str, "특성 : ");
        if(F_ISSET(obj_ptr, ONOMAG)) strcat(str, "도술사 불제자 거부, ");
        if(F_ISSET(obj_ptr, OGOODO)) strcat(str, "선한 사람용, ");
        if(F_ISSET(obj_ptr, OEVILO)) strcat(str, "악한 사람용, ");
        if(F_ISSET(obj_ptr, OENCHA)) strcat(str, "빙의 되있음, ");
        if(F_ISSET(obj_ptr, ONOMAL)) strcat(str, "남성 금지, ");
        if(F_ISSET(obj_ptr, ONOFEM)) strcat(str, "여성 금지, ");


        if(strlen(str) > 11) {
                str[strlen(str)-2] = '.';
                str[strlen(str)-1] = 0;
        }
        else
                strcat(str, "특성 없음.");
        print(fd, "%s\n", str);

}


