#include "mtype.h"
#include "mstruct.h"
#include "mextern.h"
#include <stdio.h>
#include <sys/types.h>

int load_bank(str, obj_ptr)
char	*str;
object 	**obj_ptr;
{
	int fd;
	long n;
	char file[80];

	sprintf(file, "%s/bank/%s", PLAYERPATH, str);
	fd = open(file, O_RDONLY | O_BINARY, 0);
	if(fd < 0) {
			return(-1);
	}
	*obj_ptr = (object *)malloc(sizeof(object));
	n = read_obj(fd, *obj_ptr);
	if(n < 0) {
        // leak memory bug patch by bluesky
        // leak memory bug patch by testors, again
        free_obj(*obj_ptr);
        // end of patch by testors
        // end of patch
        return(-1);
    }
	close(fd);

	return(0);
}

int save_bank(str, obj_ptr)
char	*str;
object  *obj_ptr;
{
	int fd;
	long n;
	char file[80];

	sprintf(file, "%s/bank/%s", PLAYERPATH, str);
	fd = open(file, O_RDWR | O_BINARY, 0);
	if(fd < 0) {
		fd = open(file, O_RDWR | O_CREAT, ACC);
		if(fd < 0)
			return(-1);
	}
	n = write_obj(fd, obj_ptr, 0);
	if(n <0) return(-1);
	close(fd);

	return(0);
}


int bank_inv(ply_ptr, cmnd)
creature	*ply_ptr;
cmd	*cmnd;
{
	object	*bnk_ptr;
	int		fd, n;
	char	str[2048];

	fd = ply_ptr->fd;
	if(!F_ISSET(ply_ptr->parent_rom, RBANK)) {
		print(fd, "은행에서만 가능합니다.");
		return(0);
	}
	if(cmnd->num < 2) {
			n = load_bank(ply_ptr->name, &bnk_ptr);
			if(n < 0) {
				print(fd, "보관하고 있는 물건이 없습니다.");
				bnk_ptr = (object *)malloc(sizeof(object));
				zero(bnk_ptr, sizeof(object));
				bnk_ptr->shotsmax = 200;
				F_ISSET(bnk_ptr, OCONTN);
				n = save_bank(ply_ptr->name, bnk_ptr);
				free_obj(bnk_ptr);
			}
			else {
				print(fd, "당신의 이름이 새겨진 보관함입니다.\n");
				strcpy(str, "보관품의 목록 : ");
				n = list_obj(&str[16], ply_ptr, bnk_ptr->first_obj);
				if(n)	print(fd, "%s.\n", str);
				else  	print(fd, "보관하고 있는 물건이 없습니다.");
			}
		return(0);
	}
	else {
		input_bank(ply_ptr, cmnd);
	}
	return(0);
}

int bank(ply_ptr, cmnd)
creature *ply_ptr;
cmd *cmnd;
{
	int fd, n;
	object	*bnk_ptr;
	room	*rom_ptr;

	fd = ply_ptr->fd;
	rom_ptr = ply_ptr->parent_rom;
	if(!F_ISSET(ply_ptr->parent_rom, RBANK)) {
		print(fd, "은행에서만 가능합니다.");
		return(0);
	}
	n = load_bank(ply_ptr->name, &bnk_ptr);
	if(n < 0) {
		bnk_ptr = (object *)malloc(sizeof(object));
		zero(bnk_ptr, sizeof(object));
		bnk_ptr->shotsmax = 200;
		F_ISSET(bnk_ptr, OCONTN);
	}

	print(fd, "당신의 잔고는 %ld냥입니다.", bnk_ptr->value);
	return(0);
}

int input_bank(ply_ptr, cmnd)
creature	*ply_ptr;
cmd	*cmnd;
{
		int	fd, n;
		object	*cnt_ptr, *obj_ptr;
		room *rom_ptr;

		fd = ply_ptr->fd;
		rom_ptr = ply_ptr->parent_rom;
	if(!F_ISSET(ply_ptr->parent_rom, RBANK)) {
		print(fd, "은행에서만 가능합니다.");
		return(0);
	}
	n = load_bank(ply_ptr->name, &cnt_ptr);
	if(n < 0) {
		cnt_ptr = (object *)malloc(sizeof(object));
		zero(cnt_ptr, sizeof(object));
		cnt_ptr->shotsmax = 200;
		F_ISSET(cnt_ptr, OCONTN);
	}

        if(!strcmp(cmnd->str[1], "모두")) {
            drop_all_bank(ply_ptr, cnt_ptr,cmnd->str[1]);
            return(0);
        }
        if(!strncmp(cmnd->str[1], "모든",4)) {
            drop_all_bank(ply_ptr, cnt_ptr,cmnd->str[1]+4);
            return(0);
        }

        obj_ptr = find_obj(ply_ptr, ply_ptr->first_obj,
                   cmnd->str[1], cmnd->val[1]);

        if(!obj_ptr) {
            print(fd, "당신은 그런것을 갖고 있지 않습니다.");
            return(0);
        }

        if(cnt_ptr->shotscur >= cnt_ptr->shotsmax) {
            print(fd, "보관함 안에 더이상 넣을 수 없습니다.\n");
            return(0);
        }

        if(F_ISSET(obj_ptr, OCONTN)) {
            print(fd, "보따리 종류는 보관할 수 없습니다.\n");
            return(0);
        }

        del_obj_crt(obj_ptr, ply_ptr);
        add_obj_obj(obj_ptr, cnt_ptr);
        cnt_ptr->shotscur++;
        print(fd, "당신은 %1i%j 보관시켰습니다.\n", obj_ptr,"3");
        broadcast_rom(fd, rom_ptr->rom_num, "\n%M%j %1i%j 보관시켰습니다.",
                  ply_ptr, "1", obj_ptr,"3");
		savegame_nomsg(ply_ptr);
		save_bank(ply_ptr->name, cnt_ptr);
		free_obj(cnt_ptr);
		return(0);
}


int output_bank(ply_ptr, cmnd)
creature	*ply_ptr;
cmd	*cmnd;
{
		int	fd, n;
		object	*cnt_ptr, *obj_ptr;
		room *rom_ptr;
		int cnt=0, i;

		fd = ply_ptr->fd;
		rom_ptr = ply_ptr->parent_rom;
	if(!F_ISSET(ply_ptr->parent_rom, RBANK)) {
		print(fd, "은행에서만 가능합니다.");
		return(0);
	}
	n = load_bank(ply_ptr->name, &cnt_ptr);
	if(n < 0) {
		cnt_ptr = (object *)malloc(sizeof(object));
		zero(cnt_ptr, sizeof(object));
		cnt_ptr->shotsmax = 200;
		F_ISSET(cnt_ptr, OCONTN);
	}

	for(i=0,cnt=0; i<MAXWEAR; i++) {
			if(ply_ptr->ready[i]) cnt++;
	}
	cnt += count_inv(ply_ptr, -1);

	if(cmnd->num < 2) {
		print(fd, "은행으로부터 무엇을 받으시려고요?");
		return(0);
	}

        if(!strcmp(cmnd->str[1], "모두")) {
            get_all_bank(ply_ptr, cnt_ptr,cmnd->str[1]);
            return(0);
        }
        if(!strncmp(cmnd->str[1], "모든",4)) {
            get_all_bank(ply_ptr, cnt_ptr,cmnd->str[1]+4);
            return(0);
        }

        obj_ptr = find_obj(ply_ptr, cnt_ptr->first_obj,
                   cmnd->str[1], cmnd->val[1]);

        if(!obj_ptr) {
            print(fd, "그 안에 그런것은 없어요.");
            return(0);
        }

        if((weight_ply(ply_ptr) + weight_obj(obj_ptr) >
           max_weight(ply_ptr) && cnt_ptr->parent_rom) || cnt>150) {
            print(fd, "당신은 더이상 가질 수 없습니다.");
            return(0);
        }

        if(F_ISSET(obj_ptr, OPERMT))
            get_perm_obj(obj_ptr);

        cnt_ptr->shotscur--;
        del_obj_obj(obj_ptr, cnt_ptr);
        print(fd, "당신은 %1i%j 받았습니다.", obj_ptr,"3");
        broadcast_rom(fd, rom_ptr->rom_num, "\n%M이 %1i%j 받았습니다.",
                  ply_ptr, obj_ptr,"3");

            add_obj_crt(obj_ptr, ply_ptr);
		save_bank(ply_ptr->name, cnt_ptr);
		free_obj(cnt_ptr);
		savegame_nomsg(ply_ptr);
  
        if(F_ISSET(obj_ptr, OEVENT) && !F_ISSET(obj_ptr, ONEWEV)) {
            print(fd, "\n%I%j 사라졌습니다.", obj_ptr, "1");
            del_obj_crt(obj_ptr, ply_ptr);
            free_obj(obj_ptr);
            savegame_nomsg(ply_ptr);
        }      

        return(0);

}

int deposit(ply_ptr, cmnd)
creature *ply_ptr;
cmd	*cmnd;
{
	int fd, n;
	object	*bnk_ptr;
	int len;
	long	amt;

	fd = ply_ptr->fd;

	if(!F_ISSET(ply_ptr->parent_rom, RBANK)) {
		print(fd, "은행에서만 가능합니다.");
		return(0);
	}
	if(cmnd->num != 2) {
		print(fd, "얼마를 입금하시려고요?");
		return(0);
	}
	if(!strcmp(cmnd->str[1], "모두")) {
		amt = ply_ptr->gold;
		goto input_bank_all;
	}
	len = strlen(cmnd->str[1]);
	if(len>2 && !strcmp(&cmnd->str[1][len-2],"냥")) {
		amt = atol(cmnd->str[1]);
		if(amt < 1) {
			print(fd, "돈의 단위는 음수가 될수 없습니다.");
			return(0);
		}
		if(amt > ply_ptr->gold) {
			print(fd, "당신은 그만큼의 돈을 가지고 있지 않습니다.");
			return(0);
		}
	input_bank_all :
		n = load_bank(ply_ptr->name, &bnk_ptr);
		if(n < 0) {
			bnk_ptr = (object *)malloc(sizeof(object));
			zero(bnk_ptr, sizeof(object));
			bnk_ptr->shotsmax = 200;
			F_ISSET(bnk_ptr, OCONTN);
		}
/* By Latok */
		if( bnk_ptr->value + amt > 300000000){
			print(fd,"\n은행에는 3억이상 입금할 수 없습니다. 죄송합니다\n");
			return(0);
		}

		bnk_ptr->value += amt;
		ply_ptr->gold -= amt;
		print(fd, "당신은 %ld냥을 입금했습니다.\n", amt);
		print(fd, "은행의 잔고가 %ld냥이 되었습니다.", bnk_ptr->value);
		save_bank(ply_ptr->name, bnk_ptr);
		free_obj(bnk_ptr);
		savegame_nomsg(ply_ptr);
	}
	else {
		print(fd, "사용법 : 몇냥 입금");
	}
	return(0);
}
	
int withdraw(ply_ptr, cmnd)
creature *ply_ptr;
cmd	*cmnd;
{
	int fd, n;
	object	*bnk_ptr;
	int len;
	long	amt;

	fd = ply_ptr->fd;

	if(!F_ISSET(ply_ptr->parent_rom, RBANK)) {
		print(fd, "은행에서만 가능합니다.");
		return(0);
	}
	if(cmnd->num != 2) {
		print(fd, "얼마를 출금하시려고요?");
		return(0);
	}
		n = load_bank(ply_ptr->name, &bnk_ptr);
		if(n < 0) {
			bnk_ptr = (object *)malloc(sizeof(object));
			zero(bnk_ptr, sizeof(object));
			bnk_ptr->shotsmax = 200;
			F_ISSET(bnk_ptr, OCONTN);
		}
	if(!strcmp(cmnd->str[1], "모두")) {
		amt = bnk_ptr->value;
		goto output_bank_all;
	}
	len = strlen(cmnd->str[1]);
	if(len>2 && !strcmp(&cmnd->str[1][len-2],"냥")) {
		amt = atol(cmnd->str[1]);
		if(amt < 1) {
			print(fd, "돈의 단위는 음수가 될수 없습니다.");
			return(0);
		}
		if(amt > bnk_ptr->value) {
			print(fd, "당신은 그만큼의 돈을 저금해두지 않았습니다.");
			return(0);
		}
	output_bank_all :
		bnk_ptr->value -= amt;
		ply_ptr->gold += amt;
		print(fd, "당신은 %ld냥을 출금했습니다.\n", amt);
		print(fd, "은행의 잔고가 %ld냥이 되었습니다.", bnk_ptr->value);
		save_bank(ply_ptr->name, bnk_ptr);
		free_obj(bnk_ptr);
		savegame_nomsg(ply_ptr);
	}
	else {
		print(fd, "사용법 : 몇냥 출금");
	}
	return(0);
}

void drop_all_bank(ply_ptr, cnt_ptr,part_obj)
creature        *ply_ptr;
object          *cnt_ptr;
char *part_obj;
{
    object  *obj_ptr, *last_obj;
    room    *rom_ptr;
    otag    *op;
    char    str[2048];
    int     fd, n = 1, found = 0, full = 0;
    int index=1;

    if(!strcmp(part_obj,"모두")) {
        index=0;
    }
    fd = ply_ptr->fd;

    last_obj = 0; str[0] = 0;
    rom_ptr = ply_ptr->parent_rom;

    op = ply_ptr->first_obj;
    while(op) {
        if((F_ISSET(ply_ptr, PDINVI) ? 1:!F_ISSET(op->obj, OINVIS)) &&
           op->obj != cnt_ptr) {
            if(index) {
                obj_ptr = find_obj(ply_ptr, op, part_obj, 1);
                if(!obj_ptr) break;
                while(op->obj!=obj_ptr) op=op->next_tag;
                op=op->next_tag;
                found++;
            }
            else {
                found++;
                obj_ptr = op->obj;
                op = op->next_tag;
            }

            if(obj_ptr->questnum && ply_ptr->class<DM) continue;
            if(F_ISSET(obj_ptr, OEVENT)) continue;
            if(F_ISSET(obj_ptr, OCONTN)) {
                full++;
                continue;
            }
            if(F_ISSET(cnt_ptr, OCNDES))
            {
                full++;
                del_obj_crt(obj_ptr, ply_ptr);
                free(obj_ptr);
                continue;

            }

            if(cnt_ptr->shotscur >= cnt_ptr->shotsmax) {
                full++;
                continue;
            }
            cnt_ptr->shotscur++;
            del_obj_crt(obj_ptr, ply_ptr);
            add_obj_obj(obj_ptr, cnt_ptr);
            if(last_obj && !strcmp(last_obj->name, obj_ptr->name) &&
               last_obj->adjustment == obj_ptr->adjustment)
                n++;
            else if(last_obj) {
                strcat(str, obj_str(last_obj, n, 0));
                strcat(str, ", ");
                n = 1;
            }
            last_obj = obj_ptr;
        }
        else
            op = op->next_tag;
    }

    if(found && last_obj)
        strcat(str, obj_str(last_obj, n, 0));
    else {
        print(fd, "당신은 보관시킬 물건을 아무것도 갖고 있지 않습니다.");
        return;
    }

    if(full)
        print(fd, "더이상 물건을 보관시킬 수 없습니다.");

    if(full == found) return;

        print(fd, "당신은 %1i%j 보관시켰습니다.\n", str,"3");
        broadcast_rom(fd, rom_ptr->rom_num, "\n%M%j %1i%j 보관시켰습니다.",
                  ply_ptr, "1", str,"3");
		savegame_nomsg(ply_ptr);
		save_bank(ply_ptr->name, cnt_ptr);
		free_obj(cnt_ptr);

}


void get_all_bank(ply_ptr, cnt_ptr,part_obj)
creature        *ply_ptr;
object          *cnt_ptr;
char *part_obj;
{
    room    *rom_ptr;
    object  *obj_ptr, *last_obj;
    otag    *op;
    char    str[2048];
    int     fd, n = 1, found = 0, heavy = 0;
    int index=1;
    int i,cnt;

        for(i=0,cnt=0; i<MAXWEAR; i++)
                if(ply_ptr->ready[i]) cnt++;
        cnt += count_inv(ply_ptr, -1);

    if(!strcmp(part_obj,"모두")) {
        index=0;
    }
    last_obj = 0; str[0] = 0;

    fd = ply_ptr->fd;
    rom_ptr = ply_ptr->parent_rom;

    op = cnt_ptr->first_obj;
    while(op) {
        if(!F_ISSET(op->obj, OSCENE) &&
           !F_ISSET(op->obj, ONOTAK) && !F_ISSET(op->obj, OHIDDN) &&
           (F_ISSET(ply_ptr, PDINVI) ? 1:!F_ISSET(op->obj, OINVIS))) {
            if(index) {
                obj_ptr = find_obj(ply_ptr, op, part_obj, 1);
                if(!obj_ptr) break;
                while(op->obj!=obj_ptr) op=op->next_tag;
                op=op->next_tag;
                found++;
            }
            else {
                found++;
                obj_ptr = op->obj;
                op = op->next_tag;
            }

            if(weight_ply(ply_ptr) + weight_obj(obj_ptr) >
               max_weight(ply_ptr) || cnt>150) {
                heavy++;
                continue;
            }
            if(F_ISSET(obj_ptr, OTEMPP)) {
                F_CLR(obj_ptr, OPERM2);
                F_CLR(obj_ptr, OTEMPP);
            }
            if(F_ISSET(obj_ptr, OEVENT) && !F_ISSET(obj_ptr, ONEWEV)) {
                continue;
            }
            if(F_ISSET(obj_ptr, OPERMT))
                get_perm_obj(obj_ptr);
            cnt_ptr->shotscur--;
            cnt++;
            del_obj_obj(obj_ptr, cnt_ptr);
            if(last_obj && !strcmp(last_obj->name, obj_ptr->name) &&
                last_obj->adjustment == obj_ptr->adjustment)
                n++;
            else if(last_obj) {
                strcat(str, obj_str(last_obj, n, 0));
                strcat(str, ", ");
                n = 1;
            }
            if(obj_ptr->type == MONEY) {
                ply_ptr->gold += obj_ptr->value;
                free_obj(obj_ptr);
                last_obj = 0;
                print(fd, "\n당신은 이제 %ld냥을 가지고 있습니다.",
                    ply_ptr->gold);
            }
            else {
                add_obj_crt(obj_ptr, ply_ptr);
                last_obj = obj_ptr;
            }
        }
        else
            op = op->next_tag;
    }

    if(found && last_obj)
        strcat(str, obj_str(last_obj, n, 0));
    else if(!found) {
        print(fd, "보관품이 아무것도 없습니다.");
        return;
    }

    if(heavy) {
        print(fd, "가지고 있는 물건이 너무 무거워 들 수가 없습니다.\n");
        if(heavy == found) return;
    }

    if(!strlen(str)) return;

        print(fd, "당신은 %1i%j 받았습니다.", str,"3");
        broadcast_rom(fd, rom_ptr->rom_num, "\n%M이 %1i%j 받았습니다.",
                  ply_ptr, str,"3");
		save_bank(ply_ptr->name, cnt_ptr);
		free_obj(cnt_ptr);
		savegame_nomsg(ply_ptr);
}












