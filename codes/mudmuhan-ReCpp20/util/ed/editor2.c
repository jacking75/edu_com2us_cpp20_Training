/*
 * EDITOR2.C:
 *
 *      Object, creature, room and player editor routines (cont'd).
 *
 *      Copyright (C) 1991 Brett J. Vickers
 *
 */

#include "mstruct.h"
#include "editor.h"
#include "mtype.h"
#include "mextern.h"
#include <time.h>
#include "kstbl.h"

#ifdef TC
#include <fcntl.h>
#include <stdlib.h>
#include <sys/stat.h>
#endif

void edit_player()
{
	FILE *fp;
	char file[80];
	int 		i, x;
	long 		j;
	char		str[256];
	creature	*ply_ptr;

	clearscreen();
	posit(3,20); printf("Enter player name to edit: ");
	getstr(str, "", 80);
	if(!str[0]) return;

	load_ply_from_file(str, &ply_ptr);
	ply_ptr->type = 0;			/* player */

	while(1) {
		clearscreen();
		posit(2,25);  printf("Player: %s",ply_ptr->name);
		posit(3,30);  printf(" 1. Edit name/password");
		posit(4,30);  printf(" 2. Edit level/class/align/stats/race");
		posit(5,30);  printf(" 3. Edit HP/MP/armor/thaco");
		posit(6,30);  printf(" 4. Edit exp/gold/dmg/room");
		posit(7,30);  printf(" 5. Edit proficiencies");
		posit(8,30);  printf(" 6. Edit spells");
		posit(9,30);  printf(" 7. Edit flags");
		posit(10,30); printf(" 8. Edit daily-use variables");
		posit(11,30); printf(" 9. Edit last-time variables");
		posit(12,30); printf("10. Edit 2nd page of last-time");
		posit(13,30); printf("11. Edit 3rd page of last-time");
		posit(14,30); printf("12. Edit quests fulfilled");
		posit(15,30); printf("13. Edit inventory");
		posit(16,30); printf("14. Clear this player");
		posit(17,30); printf("15. Abort this player");
		posit(18,30); printf("16. Back to main menu");
		posit(20,30); printf("Choice: ");

		getnum(&i, 1, 16);

		switch(i) {
		case 1:			/* name */
			clearscreen();
			posit(2,1); printf("Name:");
			posit(3,1); printf("%s",ply_ptr->name);
			posit(5,1); printf("Password: %s",ply_ptr->password);
			posit(3,1); getstr(str, ply_ptr->name, 80);
			strcpy(ply_ptr->name, str);
			posit(5,11); getstr(str, ply_ptr->password, 15);
			strcpy(ply_ptr->password, str);
			break;
		case 2:			/* level/class/align/stats */
			clearscreen();
			posit(2,1); printf("Level: %-3d", ply_ptr->level);
			posit(2,20); printf("Class: %-3d", ply_ptr->class);
			posit(2,40); printf("Align: %-5d", ply_ptr->alignment);
			posit(3,1); printf("  Str: %-2d", ply_ptr->strength);
			posit(3,20); printf("  Dex: %-2d", ply_ptr->dexterity);
			posit(3,40); printf("  Con: %-2d", ply_ptr->constitution);
			posit(4,1); printf("  Int: %-2d", ply_ptr->intelligence);
			posit(4,20); printf("  Pty: %-2d", ply_ptr->piety);
			posit(5,1); printf(" Race: %-3d", ply_ptr->race);
			posit(2,8); getnums(&j, (long)ply_ptr->level, 0L, 127L);
			ply_ptr->level = j;
			posit(2,27); getnums(&j, (long)ply_ptr->class, -127L, 127L);
			ply_ptr->class = j;
			posit(2,47); getnums(&j, (long)ply_ptr->alignment, -30000L, 30000L);
			ply_ptr->alignment = j;
			posit(3,8); getnums(&j, (long)ply_ptr->strength, 0L, 25L);
			ply_ptr->strength = j;
			posit(3,27); getnums(&j, (long)ply_ptr->dexterity, 0L, 25L);
			ply_ptr->dexterity = j;
			posit(3,47); getnums(&j, (long)ply_ptr->constitution, 0L, 25L);
			ply_ptr->constitution = j;
			posit(4,8); getnums(&j, (long)ply_ptr->intelligence, 0L, 25L);
			ply_ptr->intelligence = j;
			posit(4,27); getnums(&j, (long)ply_ptr->piety, 0L, 25L);
			ply_ptr->piety = j;
			posit(5,8); getnums(&j, (long)ply_ptr->race, 0L, 10L);
			ply_ptr->race = j;
			break;
		case 3:			/* HP/MP/Armor/Thac0 */
			clearscreen();
			posit(2,1); printf("   HP: %-5d/%-5d", ply_ptr->hpcur, ply_ptr->hpmax);
			posit(2,30); printf("   MP: %-5d/%-5d", ply_ptr->mpcur, ply_ptr->mpmax);
			posit(3,1); printf("Armor: %-3d", ply_ptr->armor);
			posit(3,30); printf("Thac0: %-3d", ply_ptr->thaco);
			posit(2,8); getnums(&j, (long)ply_ptr->hpcur, 0L, 32000L);
			ply_ptr->hpcur = j;
			posit(2,14); getnums(&j, (long)ply_ptr->hpmax, 0L, 32000L);
			ply_ptr->hpmax = j;
			posit(2,37); getnums(&j, (long)ply_ptr->mpcur, 0L, 32000L);
			ply_ptr->mpcur = j;
			posit(2,43); getnums(&j, (long)ply_ptr->mpmax, 0L, 32000L);
			ply_ptr->mpmax = j;
			posit(3,8); getnums(&j, (long)ply_ptr->armor, -127L, 127L);
			ply_ptr->armor = j;
			posit(3,37); getnums(&j, (long)ply_ptr->thaco, -127L, 127L);
			ply_ptr->thaco = j;
			break;
		case 4:			/* exp/gold/dmg/room */
			clearscreen();
			posit(2,1); printf("Experience: %ld", ply_ptr->experience);
			posit(2,30); printf("  Gold: %ld", ply_ptr->gold);
			posit(3,1); printf("    # Dice: %-5d", ply_ptr->ndice);
			posit(3,30); printf(" Sides: %-5d", ply_ptr->sdice);
			posit(3,60); printf("  Plus: %-5d", ply_ptr->pdice);
			posit(4,1); printf("      Room: %-5d", ply_ptr->rom_num);
			posit(2,13); getnums(&j, (long)ply_ptr->experience, 0L, 1000000L);
			ply_ptr->experience = j;
			posit(2,38); getnums(&j, (long)ply_ptr->gold, 0L, 20000000L);
			ply_ptr->gold = j;
			posit(3,13); getnums(&j, (long)ply_ptr->ndice, 0L, 32000L);
			ply_ptr->ndice = j;
			posit(3,38); getnums(&j, (long)ply_ptr->sdice, 0L, 32000L);
			ply_ptr->sdice = j;
			posit(3,68); getnums(&j, (long)ply_ptr->pdice, 0L, 32000L);
			ply_ptr->pdice = j;
			posit(4,13); getnums(&j, (long)ply_ptr->rom_num, 0L, 32000L);
			ply_ptr->rom_num = j;
			break;
		case 5:			/* proficiencies */
			clearscreen();
			posit(2,1); printf("Proficiencies:");
			posit(3,1); printf("1:%ld", ply_ptr->proficiency[0]);
			posit(3,27); printf("2:%ld", ply_ptr->proficiency[1]);
			posit(3,54); printf("3:%ld", ply_ptr->proficiency[2]);
			posit(4,1); printf("4:%ld", ply_ptr->proficiency[3]);
			posit(4,27); printf("5:%ld", ply_ptr->proficiency[4]);
			posit(6,1); printf("Realms:");
			posit(7,1); printf("1:%ld", ply_ptr->realm[0]);
			posit(7,27); printf("2:%ld", ply_ptr->realm[1]);
			posit(7,54); printf("3:%ld", ply_ptr->realm[2]);
			posit(8,1); printf("4:%ld", ply_ptr->realm[3]);
			posit(3,3); getnums(&j, (long)ply_ptr->proficiency[0], 0L, 1000000L);
			ply_ptr->proficiency[0] = j;
			posit(3,29); getnums(&j, (long)ply_ptr->proficiency[1], 0L, 1000000L);
			ply_ptr->proficiency[1] = j;
			posit(3,56); getnums(&j, (long)ply_ptr->proficiency[2], 0L, 1000000L);
			ply_ptr->proficiency[2] = j;
			posit(4,3); getnums(&j, (long)ply_ptr->proficiency[3], 0L, 1000000L);
			ply_ptr->proficiency[3] = j;
			posit(4,29); getnums(&j, (long)ply_ptr->proficiency[4], 0L, 1000000L);
			ply_ptr->proficiency[4] = j;
			posit(7,3); getnums(&j, (long)ply_ptr->realm[0], 0L, 1000000L);
			ply_ptr->realm[0] = j;
			posit(7,29); getnums(&j, (long)ply_ptr->realm[1], 0L, 1000000L);
			ply_ptr->realm[1] = j;
			posit(7,56); getnums(&j, (long)ply_ptr->realm[2], 0L, 1000000L);
			ply_ptr->realm[2] = j;
			posit(8,3); getnums(&j, (long)ply_ptr->realm[3], 0L, 1000000L);
			ply_ptr->realm[3] = j;
			break;
		case 6:		   	/* spells */
			clearscreen();
			posit(2,10); printf("1");
			posit(2,20); printf("2");
			posit(2,30); printf("3");
			posit(2,40); printf("4");
			posit(2,50); printf("5");
			posit(2,60); printf("6");
			posit(2,70); printf("7");
			posit(7,10); printf("8");
			posit(7,20); printf("9");
			posit(7,30); printf("0");
			posit(7,40); printf("1");
			posit(7,50); printf("2");
			posit(6,30); printf("1");
			posit(6,40); printf("1");
			posit(6,50); printf("1");
			posit(3,1); printf("1234567890123456789012345678901234567890123456789012345678901234567890");
			posit(8,1); printf("1234567890123456789012345678901234567890123456789012345678");
			posit(11,1); printf("Spell #: ");
			posit(12,1); printf("(0 to exit)");
			while(1) {
				for(j=0;j<128;j++)
					str[j] = (ply_ptr->spells[j/8] & 1<<(j%8)) ? '*':'.';
				str[128] = 0; j = str[70];
				str[70] = 0;
				posit(4,1); printf("%s",str);
				str[70] = j;
				posit(9,1); printf("%s",&str[70]);
				posit(11,10); getnum(&j, 0, 128);
				if(!j) break;
				if(ply_ptr->spells[(j-1)/8] & 1<<((j-1)%8))
					ply_ptr->spells[(j-1)/8] &= ~(1<<((j-1)%8));
				else
					ply_ptr->spells[(j-1)/8] |= 1<<((j-1)%8);
			}
			break;			
		case 7:			/* flags */
			clearscreen();
			posit(2,10); printf("1");
			posit(2,20); printf("2");
			posit(2,30); printf("3");
			posit(2,40); printf("4");
			posit(2,50); printf("5");
			posit(2,60); printf("6");
			posit(3,1); printf("1234567890123456789012345678901234567890123456789012345678901234");
			posit(7,1); printf("Flag #: ");
			posit(8,1); printf("(0 to exit)");
			while(1) {
				for(j=0;j<64;j++)
					str[j] = (ply_ptr->flags[j/8] & 1<<(j%8)) ? '*':'.';
				str[64] = 0;
				posit(4,1); printf("%s",str);
				posit(7,9); getnum(&j, 0, 64);
				if(!j) break;
				if(ply_ptr->flags[(j-1)/8] & 1<<((j-1)%8))
					ply_ptr->flags[(j-1)/8] &= ~(1<<((j-1)%8));
				else
					ply_ptr->flags[(j-1)/8] |= 1<<((j-1)%8);
			}
			break;
		case 8:			/* Daily-use variables */
			clearscreen();
			posit(2,1); printf("Daily-use variables:");
			posit(4,10); printf("Max");
			posit(4,20); printf("Current");
			posit(4,30); printf("Last Time");
			for(x=0; x<10; x++) {
				posit(x+5, 2); printf("%2d:",x+1);
				posit(x+5, 10); printf("%d",ply_ptr->daily[x].max);
				posit(x+5, 20); printf("%d",ply_ptr->daily[x].cur);
				posit(x+5, 30); printf("%lu",ply_ptr->daily[x].ltime);
			}
			posit(17,1); printf("Daily #:");
			while(1) {
				posit(17,10); getnum(&x, 0, 10);
				if(!x) break;
				posit(x+4, 10); getnums(&j, (long)ply_ptr->daily[x-1].max, -127L, 127L);
				ply_ptr->daily[x-1].max = j;
				posit(x+4, 20); getnums(&j, (long)ply_ptr->daily[x-1].cur, -127L, 127L);
				ply_ptr->daily[x-1].cur = j;
				posit(x+4, 30); getnums(&j, (long)ply_ptr->daily[x-1].ltime, 0L, 10000000L);
				ply_ptr->daily[x-1].ltime = j;
			}
			break;
		case 9:			/* Last-time variables */
			clearscreen();
			posit(2,1); printf("Last-time variables:");
			posit(4,10); printf("Interval");
			posit(4,20); printf("Lastreset");
			posit(4,30); printf("Misc");
			for(x=0; x<15; x++) {
				posit(x+5, 2); printf("%2d:",x+1);
				posit(x+5, 10); printf("%lu",ply_ptr->lasttime[x].interval);
				posit(x+5, 20); printf("%lu",ply_ptr->lasttime[x].ltime);
				posit(x+5, 30); printf("%-5d",ply_ptr->lasttime[x].misc);
			}
			posit(22,1); printf("Variable #:");
			while(1) {
				posit(22,13); getnum(&x, 0, 15);
				if(!x) break;
				posit(x+4, 10); getnums(&j, (long)ply_ptr->lasttime[x-1].interval, 0L, 1000000000L);
				ply_ptr->lasttime[x-1].interval = j;
				posit(x+4, 20); getnums(&j, (long)ply_ptr->lasttime[x-1].ltime, 0L, 1000000000L);
				ply_ptr->lasttime[x-1].ltime = j;
				posit(x+4, 30); getnums(&j, (long)ply_ptr->lasttime[x-1].misc, 0L, 32000L);
				ply_ptr->lasttime[x-1].misc = j;
			}
			break;
		case 10: 		/* page 2 of lasttime */
			clearscreen();
			posit(2,1); printf("Last-time variables:");
			posit(4,10); printf("Interval");
			posit(4,20); printf("Lastreset");
			posit(4,30); printf("Misc");
			for(x=15; x<30; x++) {
				posit(x-10, 2); printf("%2d:",x+1);
				posit(x-10, 10); printf("%lu",ply_ptr->lasttime[x].interval);
				posit(x-10, 20); printf("%lu",ply_ptr->lasttime[x].ltime);
				posit(x-10, 30); printf("%-5d",ply_ptr->lasttime[x].misc);
			}
			posit(22,1); printf("Variable #:");
			while(1) {
				posit(22,13); getnum(&x, 0, 30);
				if(x<16) break;
				posit(x-11, 10); getnums(&j, (long)ply_ptr->lasttime[x-1].interval, 0L, 1000000000L);
				ply_ptr->lasttime[x-1].interval = j;
				posit(x-11, 20); getnums(&j, (long)ply_ptr->lasttime[x-1].ltime, 0L, 1000000000L);
				ply_ptr->lasttime[x-1].ltime = j;
				posit(x-11, 30); getnums(&j, (long)ply_ptr->lasttime[x-1].misc, 0L, 32000L);
				ply_ptr->lasttime[x-1].misc = j;
			}
			break;
		case 11: 		/* page 3 of lasttime */
			clearscreen();
			posit(2,1); printf("Last-time variables:");
			posit(4,10); printf("Interval");
			posit(4,20); printf("Lastreset");
			posit(4,30); printf("Misc");
			for(x=30; x<45; x++) {
				posit(x-25, 2); printf("%2d:",x+1);
				posit(x-25, 10); printf("%lu",ply_ptr->lasttime[x].interval);
				posit(x-25, 20); printf("%lu",ply_ptr->lasttime[x].ltime);
				posit(x-25, 30); printf("%-5d",ply_ptr->lasttime[x].misc);
			}
			posit(22,1); printf("Variable #:");
			while(1) {
				posit(22,13); getnum(&x, 0, 45);
				if(x<31) break;
				posit(x-26, 10); getnums(&j, (long)ply_ptr->lasttime[x-1].interval, 0L, 1000000000L);
				ply_ptr->lasttime[x-1].interval = j;
				posit(x-26, 20); getnums(&j, (long)ply_ptr->lasttime[x-1].ltime, 0L, 1000000000L);
				ply_ptr->lasttime[x-1].ltime = j;
				posit(x-26, 30); getnums(&j, (long)ply_ptr->lasttime[x-1].misc, 0L, 32000L);
				ply_ptr->lasttime[x-1].misc = j;
			}
			break;
		case 12:		/* quests */
			clearscreen();
			posit(2,10); printf("1");
			posit(2,20); printf("2");
			posit(2,30); printf("3");
			posit(2,40); printf("4");
			posit(2,50); printf("5");
			posit(2,60); printf("6");
			posit(2,70); printf("7");
			posit(7,10); printf("8");
			posit(7,20); printf("9");
			posit(7,30); printf("0");
			posit(7,40); printf("1");
			posit(7,50); printf("2");
			posit(6,30); printf("1");
			posit(6,40); printf("1");
			posit(6,50); printf("1");
			posit(3,1); printf("1234567890123456789012345678901234567890123456789012345678901234567890");
			posit(8,1); printf("1234567890123456789012345678901234567890123456789012345678");
			posit(11,1); printf("Quest #: ");
			posit(12,1); printf("(0 to exit)");
			while(1) {
				for(j=0;j<128;j++)
					str[j] = (ply_ptr->quests[j/8] & 1<<(j%8)) ? '*':'.';
				str[128] = 0; j = str[70];
				str[70] = 0;
				posit(4,1); printf("%s",str);
				str[70] = j;
				posit(9,1); printf("%s",&str[70]);
				posit(11,10); getnum(&j, 0, 128);
				if(!j) break;
				if(ply_ptr->quests[(j-1)/8] & 1<<((j-1)%8))
					ply_ptr->quests[(j-1)/8] &= ~(1<<((j-1)%8));
				else
					ply_ptr->quests[(j-1)/8] |= 1<<((j-1)%8);
			}
			break;
		case 13:		/* edit items */
			edit_items(ply_ptr);
			break;
		case 14:		/* abort */
			clearscreen();
			posit(1,1);
			printf("Are you sure you want to clear it? ");
			getstr(str, "n", 2);
			if(str[0]=='y' || str[0]=='Y') {
				free_crt(ply_ptr);
				ply_ptr = (creature *)malloc(sizeof(creature));
				zero(ply_ptr, sizeof(creature));
			}
			break;
		case 15:		/* Abort */
			free_crt(ply_ptr);
			return;
		case 16:		/* Return to main menu */
			if(!ply_ptr->name[0]) break;
			sprintf(file, "%s/edit_player", LOGPATH);
			fp = fopen(file, "a");
			fprintf(fp, "Player Edit -> %s\n", ply_ptr->name);
			save_ply_to_file(ply_ptr->name, ply_ptr);
			free_crt(ply_ptr);
			return;
		}
	}
}

void edit_items(ply_ptr)
creature	*ply_ptr;
{
	int 	i, x, error;
	long 	j;
	object	*obj_ptr;
	otag	*op, *ot, *prev;

	while(1) {
		clearscreen();
		posit(5,25);  printf("Player: %s",ply_ptr->name);
		posit(6,30);  printf("1. Add item");
		posit(7,30);  printf("2. Remove item");
		posit(8,30);  printf("3. List items");
		posit(9,30);  printf("4. Back to player menu");
		posit(11,30); printf("Choice: ");
	
		getnum(&i, 1, 4);
	
		switch(i) {
		case 1: 	/* add */
			clearscreen();
			posit(2,1); printf("Enter item number: ");
			getnum(&j, 0, 32000);
			if(load_obj_from_file((int)j, &obj_ptr) < 0)
				free(obj_ptr);
			else {
				ot = (otag *)malloc(sizeof(otag));
				ot->obj = obj_ptr;
				ot->next_tag = 0;
				op = ply_ptr->first_obj;
				if(!op) {
					ply_ptr->first_obj = ot;
					break;
				}
				if(strcmp(op->obj->name,ot->obj->name) >= 0 ||
				   (strcmp(op->obj->name,ot->obj->name)==0 &&
				   op->obj->adjustment > ot->obj->adjustment) ) {
					ot->next_tag = op;
					ply_ptr->first_obj = ot;
					break;
				}
				while(op) {
					if(strcmp(op->obj->name,ot->obj->name) > 0 || 
					  (strcmp(op->obj->name,ot->obj->name)==0 && 
					  op->obj->adjustment > ot->obj->adjustment) ) break;
					prev = op;
					op = op->next_tag;
				}
				ot->next_tag = prev->next_tag;
				prev->next_tag = ot;
			}
			break;
		case 2:
			clearscreen();
			posit(2,1); printf("Delete item #");
			getnum(&i, 0, 90);
			if(!i) break;
			op = ply_ptr->first_obj; x = 1; error = 0;
			while(x <= i) {
				if(!op) {
					error = 1;
					break;
				}
				if(x == i)
					break;
				x++;
				prev = op;
				op = op->next_tag;
			}
			if(error) break;
			if(op == ply_ptr->first_obj) {
				ply_ptr->first_obj = op->next_tag;
				free_obj(op->obj);
				free(op);
			}
			else {
				prev->next_tag = op->next_tag;
				free_obj(op->obj);
				free(op);
			}
			break;
		case 3:			/* list */
			clearscreen();
			op = ply_ptr->first_obj; x = 1;
			while(op) {
				posit(x%24, (x/24>0)?40:1);
				printf("%2d: %s", x, op->obj->name);
				x++;
				op = op->next_tag;
			}
			posit(24,40); printf("Hit Enter:");
			getnum(&x, 0, 0);
			break;
		case 4:
			return;
		}
	}
}

void check_player()
{
	FILE *fp, *fp2, *fp3;
	char str[10000][28], str2[256];
	char file[80], file2[80], file3[80];
	char file4[1000];
	creature	*ply_ptr;
	object 		*obj_ptr;
	otag		*op, *otemp;

    /* included by testors */
    otag	*op1, *op2;
/*
    object    *obj;
*/

	otag 		*otag_ptr;
	int i, n;
	long j;
	int max=5000;
	char class_str[][15] = {"Á¦ÀÛ", "ÀÚ°´", "±Ç¹ý°¡", "ºÒÁ¦ÀÚ",
			"°Ë»ç", "µµ¼ú»ç", "¹«»ç", "Æ÷Á¹", "µµµÏ",
			"¹«Àû", "ÃÊÀÎ", "¿î¿µÀÚ", "°ü¸®ÀÚ"};
	sprintf(file4, "ls %s/°¡ %s/³ª %s/´Ù %s/¶ó %s/¸¶ %s/¹Ù %s/»ç %s/¾Æ %s/ÀÚ %s/Â÷ %s/Ä« %s/Å¸ %s/ÆÄ %s/ÇÏ > %s/p_list", 
			PLAYERPATH, PLAYERPATH, PLAYERPATH, PLAYERPATH, PLAYERPATH,
			PLAYERPATH, PLAYERPATH, PLAYERPATH, PLAYERPATH, PLAYERPATH,
			PLAYERPATH, PLAYERPATH, PLAYERPATH, PLAYERPATH,
			PLAYERPATH);
	system(file4);
	sprintf(file, "%s/p_list", PLAYERPATH);
	fp = fopen(file, "r");

	for(i=0; i<max; i++) {
		fscanf(fp, "%s", str[i]);
	}
	
	fclose(fp);

	clearscreen();
	posit(10, 25); printf("1. Check Password ");
	posit(11, 25); printf("2. Check Gold ");
	posit(12, 25); printf("3. Check Family ");
	posit(13, 25); printf("4. Check Event item ");
	posit(14, 25); printf("5. Check Class ");
	posit(15, 25); printf("6. Check High_dmg ");
	posit(16, 25); printf("7. return ");
	posit(18, 25); printf("Choice : ");

	getnum(&n, 1, 7);

	switch(n) {
	case 1:
		sprintf(file2, "%s/check_passwd", BINPATH);
		fp2 = fopen(file2, "w");

		clearscreen();
		posit(10,30); printf("Password : ");
		getstr(str2, "", 80);
		if(!str2[0]) return;

		for(i=0;i<max;i++) {
			if(str[i][0] == 0) break;
			if(!ishan(str[i])) continue;
			load_ply_from_file(str[i], &ply_ptr);
			if(!ply_ptr) break;
			if(!strcmp(str2, ply_ptr->password)) {
				fprintf(fp2, "%-15s %-3d %-6s %ld\n", ply_ptr->name,
					ply_ptr->level, class_str[ply_ptr->class],
					ply_ptr->gold);
			}
			free_crt(ply_ptr);
		}
		fclose(fp2);
		exit(0);
	case 2:
		sprintf(file2, "%s/check_gold", BINPATH);
		fp2 = fopen(file2, "w");

		j = 10000000L;
		for(i=0;i<max;i++) {
			if(str[i][0] == 0) break;
			if(!ishan(str[i])) continue;
			load_ply_from_file(str[i], &ply_ptr);
			if(!ply_ptr) break;
			if(ply_ptr->gold > j) {
				fprintf(fp2, "%-15s %-3d %-6s %ld\n", ply_ptr->name,
					ply_ptr->level, class_str[ply_ptr->class],
					ply_ptr->gold);
			}
			free_crt(ply_ptr);
		}
		fclose(fp2);
		exit(0);
	case 3:
		for(i=0;i<max;i++) {
			if(str[i][0] == 0) break;
			if(!ishan(str[i])) continue;
			load_ply_from_file(str[i], &ply_ptr);
			if(F_ISSET(ply_ptr, PFAMIL)) {
			sprintf(file2, "%s/family/check_family_%d", PLAYERPATH, ply_ptr->daily[DL_EXPND].max);
				fp2 = fopen(file2, "a");
				fprintf(fp2, "%d %s\n", ply_ptr->class, ply_ptr->name); 
				fclose(fp2);
			}
			free_crt(ply_ptr);
		}
		for(i=0;i<16; i++) {
			sprintf(file2, "%s/family/check_family_%d", PLAYERPATH, i);
			fp2 = fopen(file2, "a");
			fprintf(fp2, "0\n");
			fclose(fp2);
		}
		exit(0);
	case 4:			
		sprintf(file2, "%s/check_event", BINPATH);
		fp2 = fopen(file2, "a");
		for(i=0;i<max;i++) {
			if(str[i][0] == 0) break;
			if(!ishan(str[i])) continue;
			load_ply_from_file(str[i], &ply_ptr);
			if(!ply_ptr) break;
			op = ply_ptr->first_obj;
			while(op) {
				otemp = op->next_tag;
				obj_ptr = op->obj;
				if(F_ISSET(obj_ptr, OEVENT)) {
					fprintf(fp2, "%s  %s  %s  %d/%d  %dd%d+%d  %d\n", ply_ptr->name,
						obj_ptr->name, obj_ptr->key[2], obj_ptr->shotscur, obj_ptr->shotsmax,
						obj_ptr->ndice, obj_ptr->sdice, obj_ptr->pdice,
						obj_ptr->armor);
				}
              /* included by testors */
				op1 = obj_ptr->first_obj;
				while(op1) {
					if(!op1) break;
				     if(F_ISSET(op1->obj, OEVENT)) {
			 	         fprintf(fp2, "%s  in  %s  %s  %d/%d  %dd%d+%d  %d\n", ply_ptr->name,
				     		op1->obj->name, op1->obj->key[2], op1->obj->shotscur, op1->obj->shotsmax,
				     		op1->obj->ndice, op1->obj->sdice, op1->obj->pdice,
				    		op1->obj->armor);
			         }
					op1 = op1->next_tag;
                  }

				op = otemp;
			}
			free_crt(ply_ptr);
		}
		fclose(fp2);
		exit(0);
	case 5:
		sprintf(file2, "%s/check_class", BINPATH);
		fp2 = fopen(file2, "a");
		for(i=0;i<max;i++) {
			if(str[i][0] == 0) break;
			if(!ishan(str[i])) continue;
			load_ply_from_file(str[i], &ply_ptr);
			if(!ply_ptr) break;
			if(ply_ptr->class == 0 || ply_ptr->class > INVINCIBLE || ply_ptr->level > 120) {
				fprintf(fp2, "%-15s %-6s %-3d %ld\n", ply_ptr->name,
					class_str[ply_ptr->class], ply_ptr->level, ply_ptr->experience);
			}
			free_crt(ply_ptr);
		}
		fclose(fp2);
		exit(0);
	case 6:			
		sprintf(file2, "%s/check_highdmg", BINPATH);
		fp2 = fopen(file2, "a");
		for(i=0;i<max;i++) {
			if(str[i][0] == 0) break;
			if(!ishan(str[i])) continue;
			load_ply_from_file(str[i], &ply_ptr);
			if(!ply_ptr) break;
			op = ply_ptr->first_obj;
			while(op) {
				otemp = op->next_tag;
				obj_ptr = op->obj;
				if(obj_ptr->questnum == 0 && !F_ISSET(obj_ptr, OEVENT)) {
				if(obj_ptr->armor > 29 || obj_ptr->ndice * obj_ptr->sdice + obj_ptr->pdice > 40) {
					fprintf(fp2, "%s  %s  %s  %d/%d  %dd%d+%d  %d\n", ply_ptr->name,
						obj_ptr->name, obj_ptr->key[2], obj_ptr->shotscur, obj_ptr->shotsmax,
						obj_ptr->ndice, obj_ptr->sdice, obj_ptr->pdice,
						obj_ptr->armor);
				}
				}
              /* included by testors */
				op1 = obj_ptr->first_obj;
				while(op1) {
					if(!op1) break;
				if(op1->obj->armor > 29 || op1->obj->ndice * op1->obj->sdice + op1->obj->pdice > 40) {
					if(op1->obj->questnum == 0 && !F_ISSET(op1->obj, OEVENT)) {
			 	         fprintf(fp2, "%s  in  %s  %s  %d/%d  %dd%d+%d  %d\n", ply_ptr->name,
				     		op1->obj->name, op1->obj->key[2], op1->obj->shotscur, op1->obj->shotsmax,
				     		op1->obj->ndice, op1->obj->sdice, op1->obj->pdice,
				    		op1->obj->armor);
			         }
					}
					op1 = op1->next_tag;
                  }

				op = otemp;
			}
			free_crt(ply_ptr);
		}
		fclose(fp2);
		exit(0);
	case 7:
		return;	
	}
}

int load_ply_from_file(str, ply_ptr)
char		*str;
creature	**ply_ptr;
{
	int fd;
	long n;
	char file[80];
#ifdef COMPRESS
	char *a_buf, *b_buf;
	int size;
#endif

	*ply_ptr = (creature *)malloc(sizeof(creature));
	if(!*ply_ptr)
		merror("load_from_file", FATAL);
	zero(*ply_ptr, sizeof(creature));

#ifdef TC
	sprintf(file, "%s\\%s", PLAYERPATH, str);
#else
	sprintf(file, "%s/%s/%s", PLAYERPATH, first_han(str), str);
#endif
	fd = open(file, O_RDONLY | O_BINARY, 0);
	if(fd < 0) 
		return(-1);

#ifdef COMPRESS
	a_buf = (char *)malloc(30000);
	if(!a_buf) merror(FATAL, "Memory allocation");
	size = read(fd, a_buf, 30000);
	if(size >= 30000) merror(FATAL, "Player too large");
	if(size < 1) {
		close(fd);
		return(-1);
	}
	b_buf = (char *)malloc(80000);
	if(!b_buf) merror(FATAL, "Memory allocation");
	n = uncompress(a_buf, b_buf, size);
	if(n > 80000) merror(FATAL, "Player too large");
	n = read_crt_from_mem(b_buf, *ply_ptr, 0);
	free(a_buf);
	free(b_buf);
#else
	n = read_crt(fd, *ply_ptr);
	if(n < 0) {
		close(fd);
		return(-1);
	}
#endif

	close(fd);

	return(0);
}

int save_ply_to_file(str, ply_ptr)
char		*str;
creature	*ply_ptr;
{
	int fd;
	long n;
	char file[80];
#ifdef COMPRESS
	char *a_buf, *b_buf;
	int size;
#endif

#ifdef TC
	sprintf(file, "%s\\%s", PLAYERPATH, str);
#else
	sprintf(file, "%s/%s/%s", PLAYERPATH, first_han(str), str);
#endif
	unlink(file);
#ifdef TC
	fd = open(file, O_RDWR | O_CREAT | O_BINARY, S_IWRITE | S_IREAD);
#else
	fd = open(file, O_RDWR | O_CREAT, ACC);
#endif
	if(fd < 0)
		return(-1);

#ifdef COMPRESS
	a_buf = (char *)malloc(80000);
	if(!a_buf) merror(FATAL, "Memory allocation");
	n = write_crt_to_mem(a_buf, ply_ptr, 0);
	if(n > 80000) merror(FATAL, "Player too large");
	b_buf = (char *)malloc(n);
	if(!b_buf) merror(FATAL, "Memory allocation");
	size = compress(a_buf, b_buf, n);
	n = write(fd, b_buf, size);
	free(a_buf);
	free(b_buf);
#else
	n = write_crt(fd, ply_ptr, 0);
	if(n < 0) {
		close(fd);
		return(-1);
	}
#endif

	close(fd);

	return(0);
}

int ishan(str)
unsigned char *str;
{
	int i;
	for(i=0;i<strlen(str);i+=2) {
		if(!is_hangul(str+i)) return 0;
	}
	return 1;
}

int is_hangul(str)
unsigned char *str;    /* one character */
{
	/* ¼ø¼öÇÑ ÇÑ±ÛÀÎÁö °Ë»ç */
	if(str[0]>=0xb0 && str[0]<=0xc8 && str[1]>=0xa1 && str[1]<=0xfe) return 1;
	return 0;
}

char *first_han(str)
unsigned char *str;
{
    unsigned char high,low;
    int len,i;
    char *p = "temp";
    static unsigned char *exam[]={
        "°¡", "°¡", "³ª", "´Ù", "´Ù",
        "¶ó", "¸¶", "¹Ù", "¹Ù", "»ç",
        "»ç", "¾Æ", "ÀÚ", "ÀÚ", "Â÷", 
        "Ä«", "Å¸", "ÆÄ", "ÇÏ", "" };
    static unsigned char *johab_exam[]={
        "ˆa", "Œa", "a", "”a", "˜a",
        "œa", " a", "¤a", "¨a", "¬a",
        "°a", "´a", "¸a", "¼a", "Àa",
        "Äa", "Èa", "Ìa", "Ða", "" };

    len=strlen(str);
    if(len<2) return p;

    high=str[0];
    low=str[1];

    if(!is_hangul(&str[0])) return p;
    high=(KStbl[(high-0xb0)*94+low-0xa1] >> 8) & 0x7c;
    for(i=0;johab_exam[i][0];i++) {
        low= (johab_exam[i][0] & 0x7f);
        if(low==high) return exam[i];
    }
    return p;
}

int is_number(str)
unsigned char *str;
{
	int i;
	for(i=0;i<strlen(str);i++) {
		if(!isdigit(str[i])) return 0;
	}
	return 1;
}
