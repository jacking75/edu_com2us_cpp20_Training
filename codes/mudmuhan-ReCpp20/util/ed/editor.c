/*
 * EDITOR.C:
 *
 *      Object, creature, room and player editor routines.
 *
 *      Copyright (C) 1991 Brett J. Vickers
 *
 */

#include "mstruct.h"
#include "editor.h"
#include "mtype.h"
#include <time.h>

#ifdef TC
#include <fcntl.h>
#include <stdlib.h>
#include <sys/stat.h>
#endif

void main()
{
        int i;

	umask(0);
	while(1) {
		clearscreen();
		posit(10,30); printf("1. Edit Object");
		posit(11,30); printf("2. Edit Creature");
		posit(12,30); printf("3. Edit Room");
		posit(13,30); printf("4. Edit Player");
		posit(14,30); printf("5. Check Player");
		posit(15,30); printf("6. Quit");

		posit(17,30); printf("Choice: ");

		getnum(&i, 1, 6);

		switch(i) {
		case 1:
			edit_object();
			break;
		case 2:
			edit_monster();
			break;
		case 3:
			edit_room();
			break;
		case 4:
			edit_player();
			break;
		case 5:
			check_player();
			break;
		case 6:
			exit(0);
		}
	}
}

void edit_object()
{
	int 	i, num;
	long 	j;
	char	str[256];
	object	*obj_ptr;

	clearscreen();
	posit(3,20); printf("Enter object number to edit: ");
	getnum(&num, 0, 3000);

	load_obj_from_file(num, &obj_ptr);

	while(1) {
		clearscreen();
		posit(2,25);  printf("Object: %s", obj_ptr->name);
		posit(4,30);  printf(" 1. Edit object name/description");
		posit(5,30);  printf(" 2. Edit key names");
		posit(6,30);  printf(" 3. Edit output string");
		posit(7,30);  printf(" 4. Edit value/weight/type/adj/quest");
		posit(8,30);  printf(" 5. Edit shots/dmg/armor/wear/mgc/spcl");
		posit(9,30);  printf(" 6. Edit flags");
		posit(10,30); printf(" 7. Clear this object");
		posit(11,30); printf(" 8. Abort edit");
		posit(12,30); printf(" 9. Back to main menu");

		posit(15,30); printf("Choice: ");

		getnum(&i, 1, 9);

		switch(i) {
		case 1:			/* name */
			clearscreen();
			posit(2,1); printf("Object's name:");
			posit(3,1); printf("%s", obj_ptr->name);
			posit(5,1); printf("Description:");
			posit(6,1); printf("%s", obj_ptr->description);
			posit(3,1); getstr(str, obj_ptr->name, 80);
			strcpy(obj_ptr->name, str);
			posit(6,1); getstr(str, obj_ptr->description, 80);
			strcpy(obj_ptr->description, str);
			break;
		case 2:       		/* keys */
			clearscreen();
			posit(2,1); printf("Keys:");
			posit(3,1); printf("1:%s", obj_ptr->key[0]);
			posit(3,27); printf("2:%s", obj_ptr->key[1]);
			posit(3,54); printf("3:%s", obj_ptr->key[2]);
			posit(3,3); getstr(str, obj_ptr->key[0], 20);
			strcpy(obj_ptr->key[0], str);
			posit(3,29); getstr(str, obj_ptr->key[1], 20);
			strcpy(obj_ptr->key[1], str);
			posit(3,56); getstr(str, obj_ptr->key[2], 20);
			strcpy(obj_ptr->key[2], str);
			break;
		case 3:			/* output string */
			clearscreen();
			posit(2,1); printf("Current output string:");
			posit(3,1); printf("%s", obj_ptr->use_output);
			posit(3,1); getstr(str, obj_ptr->use_output, 80);
			strcpy(obj_ptr->use_output, str);
			break;
		case 4:			/* value/weight/type/adj/quest */
			clearscreen();
			posit(2,1); printf("Value: %-8d", obj_ptr->value);
			posit(2,30); printf("Weight: %-5d",obj_ptr->weight);
			posit(3,1); printf(" Type: %-3d", obj_ptr->type);
			posit(3,30); printf("   Adj: %-3d", obj_ptr->adjustment);
			posit(3,54); printf("Quest: %-3d", obj_ptr->questnum);
			posit(2,8); getnums(&j, (long)obj_ptr->value, 0L, 10000000L);
			obj_ptr->value = j;
			posit(2,38); getnums(&j, (long)obj_ptr->weight, 0L, 32000L);
			obj_ptr->weight = j;
			posit(3,8); getnums(&j, (long)obj_ptr->type, -127L, 127L);
			obj_ptr->type = j;
			posit(3,38); getnums(&j, (long)obj_ptr->adjustment, -127L, 127L);
			obj_ptr->adjustment = j;
			posit(3,61); getnums(&j, (long)obj_ptr->questnum, -1L, 127L);
			obj_ptr->questnum = j;
			break;
		case 5:			/* shots/dmg/armor/wear/magic */
			clearscreen();
			posit(2,1); printf(" Shots: %-5d/%-5d", obj_ptr->shotscur, obj_ptr->shotsmax);
			posit(3,1); printf("# Dice: %-5d", obj_ptr->ndice);
			posit(3,30); printf(" Sides: %-5d", obj_ptr->sdice);
			posit(3,60); printf("  Plus: %-5d", obj_ptr->pdice);
			posit(4,1); printf(" Armor: %-3d", obj_ptr->armor);
			posit(4,30); printf("  Wear: %-3d", obj_ptr->wearflag);
			posit(4,60); printf(" Magic: %-3d", obj_ptr->magicpower);
			posit(5,30); printf(" Realm: %-1d", obj_ptr->magicrealm);
			posit(5,59); printf("Special: %-3d", obj_ptr->special);
			posit(2,9); getnums(&j, (long)obj_ptr->shotscur, 0L, 32000L);
			obj_ptr->shotscur = j;
			posit(2,15); getnums(&j, (long)obj_ptr->shotsmax, 0L, 32000L);
			obj_ptr->shotsmax = j;
			posit(3,9); getnums(&j, (long)obj_ptr->ndice, 0L, 32000L);
			obj_ptr->ndice = j;
			posit(3,38); getnums(&j, (long)obj_ptr->sdice, 0L, 32000L);
			obj_ptr->sdice = j;
			posit(3,68); getnums(&j, (long)obj_ptr->pdice, 0L, 32000L);
			obj_ptr->pdice = j;
			posit(4,9); getnums(&j, (long)obj_ptr->armor, -127L, 127L);
			obj_ptr->armor = j;
			posit(4,38); getnums(&j, (long)obj_ptr->wearflag, -127L, 127L);
			obj_ptr->wearflag = j;
			posit(4,68); getnums(&j, (long)obj_ptr->magicpower, -127L, 127L);
			obj_ptr->magicpower = j;
			posit(5,38); getnums(&j, (long)obj_ptr->magicrealm, 0L, 128L);
			obj_ptr->magicrealm = j;
			posit(5,68); getnums(&j, (long)obj_ptr->special, 0L, 32000L);
			obj_ptr->special = j;
			break;
		case 6:			/* flags */
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
					str[j] = (obj_ptr->flags[j/8] & 1<<(j%8)) ? '*':'.';
				str[64] = 0;
				posit(4,1); printf("%s",str);
				posit(7,9); getnum(&j, 0, 64);
				if(!j) break;
				if(obj_ptr->flags[(j-1)/8] & 1<<((j-1)%8))
					obj_ptr->flags[(j-1)/8] &= ~(1<<((j-1)%8));
				else
					obj_ptr->flags[(j-1)/8] |= 1<<((j-1)%8);
			}
			break;
		case 7:
			clearscreen();
			posit(1,1);
			printf("Are you sure you want to clear it? ");
			getstr(str, "n", 2);
			if(str[0]=='y' || str[0]=='Y')
				zero(obj_ptr, sizeof(object));
			break;
		case 8:
			free(obj_ptr);
			return;
		case 9:
			save_obj_to_file(num, obj_ptr);
			free(obj_ptr);
			return;
		}
	}
}

int load_obj_from_file(num, obj_ptr)
int 	num;
object 	**obj_ptr;
{
	int fd;
	long n;
	char file[80];

	*obj_ptr = (object *)malloc(sizeof(object));
	if(!*obj_ptr)
		merror("load_from_file", FATAL);
	zero(*obj_ptr, sizeof(object));

#ifdef TC
	sprintf(file, "%s\\o%02d", OBJPATH, num/OFILESIZE);
#else
	sprintf(file, "%s/o%02d", OBJPATH, num/OFILESIZE);
#endif
	fd = open(file, O_RDONLY | O_BINARY, 0);
	if(fd < 0) 
		return(-1);
	n = lseek(fd, (long)((num%OFILESIZE)*sizeof(object)), 0);
	if(n < 0L) {
		close(fd);
		return(-1);
	}
	n = read(fd, *obj_ptr, sizeof(object));
	if(n < sizeof(object)) {
		close(fd);
		return(-1);
	}
	close(fd);

	return(0);
}

int save_obj_to_file(num, obj_ptr)
int 	num;
object  *obj_ptr;
{
	int fd;
	long n;
	char file[80];

#ifdef TC
	sprintf(file, "%s\\o%02d", OBJPATH, num/OFILESIZE);
#else
	sprintf(file, "%s/o%02d", OBJPATH, num/OFILESIZE);
#endif
	fd = open(file, O_RDWR | O_BINARY, 0);
	if(fd < 0) {
#ifdef TC
		fd = open(file, O_RDWR | O_CREAT | O_BINARY, S_IWRITE | S_IREAD);
#else
		fd = open(file, O_RDWR | O_CREAT, ACC);
#endif
		if(fd < 0)
			return(-1);
	}
	n = lseek(fd, (long)((num%OFILESIZE)*sizeof(object)), 0);
	if(n < 0L) {
		close(fd);
		return(-1);
	}
	n = write(fd, obj_ptr, sizeof(object));
	if(n < sizeof(object)) {
		close(fd);
		return(-1);
	}
	close(fd);

	return(0);
}

void edit_monster()
{
	int 		i, num;
	long 		j;
	char		str[256];
	creature	*crt_ptr;

	clearscreen();
	posit(3,20); printf("Enter creature number to edit: ");
	getnum(&num, 0, 3000);

	load_crt_from_file(num, &crt_ptr);
	crt_ptr->type = 1;			/* monster */

	while(1) {
		clearscreen();
		posit(2,25);  printf("Creature: %s",crt_ptr->name);
		posit(4,30);  printf(" 1. Edit creature name/desc/talk");
		posit(5,30);  printf(" 2. Edit creature keys");
		posit(6,30);  printf(" 3. Edit level/class/align/stats");
		posit(7,30);  printf(" 4. Edit HP/MP/armor/thaco/num");
		posit(8,30);  printf(" 5. Edit exp/gold/dmg/quest/special");
		posit(9,30);  printf(" 6. Edit spells");
		posit(10,30); printf(" 7. Edit flags");
		posit(11,30); printf(" 8. Edit proficiencies");
		posit(12,30); printf(" 9. Edit carryable objects");
		posit(13,30); printf("10. Clear this creature");
		posit(14,30); printf("11. Abort edit");
		posit(15,30); printf("12. Back to main menu");
		posit(18,30); printf("Choice: ");

		getnum(&i, 1, 12);

		switch(i) {
		case 1:
			clearscreen();
			posit(2,1); printf("Creature name:");
			posit(3,1); printf("%s",crt_ptr->name);
			posit(5,1); printf("Description:");
			posit(6,1); printf("%s",crt_ptr->description);
			posit(8,1); printf("%s", "Talk:");
			posit(9,1); printf("%s",crt_ptr->talk);
			posit(3,1); getstr(str, crt_ptr->name, 80);
			strcpy(crt_ptr->name, str);
			posit(6,1); getstr(str, crt_ptr->description, 80);
			strcpy(crt_ptr->description, str);
			posit(9,1); getstr(str, crt_ptr->talk, 80);
			strcpy(crt_ptr->talk, str);
			break;
		case 2:			/* keys */
			clearscreen();
			posit(2,1); printf("Keys:");
			posit(3,1); printf("1:%s", crt_ptr->key[0]);
			posit(3,27); printf("2:%s", crt_ptr->key[1]);
			posit(3,54); printf("3:%s", crt_ptr->key[2]);
			posit(3,3); getstr(str, crt_ptr->key[0], 20);
			strcpy(crt_ptr->key[0], str);
			posit(3,29); getstr(str, crt_ptr->key[1], 20);
			strcpy(crt_ptr->key[1], str);
			posit(3,56); getstr(str, crt_ptr->key[2], 20);
			strcpy(crt_ptr->key[2], str);
			break;
		case 3:			/* level/class/align/stats */
			clearscreen();
			posit(2,1); printf("Level: %-3d", crt_ptr->level);
			posit(2,20); printf("Class: %-3d", crt_ptr->class);
			posit(2,40); printf("Align: %-5d", crt_ptr->alignment);
			posit(3,1); printf("  Str: %-2d", crt_ptr->strength);
			posit(3,20); printf("  Dex: %-2d", crt_ptr->dexterity);
			posit(3,40); printf("  Con: %-2d", crt_ptr->constitution);
			posit(4,1); printf("  Int: %-2d", crt_ptr->intelligence);
			posit(4,20); printf("  Pty: %-2d", crt_ptr->piety);
			posit(2,8); getnums(&j, (long)crt_ptr->level, 0L, 127L);
			crt_ptr->level = j;
			posit(2,27); getnums(&j, (long)crt_ptr->class, -127L, 127L);
			crt_ptr->class = j;
			posit(2,47); getnums(&j, (long)crt_ptr->alignment, -30000L, 30000L);
			crt_ptr->alignment = j;
			posit(3,8); getnums(&j, (long)crt_ptr->strength, 0L, 25L);
			crt_ptr->strength = j;
			posit(3,27); getnums(&j, (long)crt_ptr->dexterity, 0L, 25L);
			crt_ptr->dexterity = j;
			posit(3,47); getnums(&j, (long)crt_ptr->constitution, 0L, 25L);
			crt_ptr->constitution = j;
			posit(4,8); getnums(&j, (long)crt_ptr->intelligence, 0L, 25L);
			crt_ptr->intelligence = j;
			posit(4,27); getnums(&j, (long)crt_ptr->piety, 0L, 25L);
			crt_ptr->piety = j;
			break;
		case 4:			/* HP/MP/Armor/Thaco */
			clearscreen();
			posit(2,1); printf("   HP: %-5d/%-5d", crt_ptr->hpcur, crt_ptr->hpmax);
			posit(2,30); printf("   MP: %-5d/%-5d", crt_ptr->mpcur, crt_ptr->mpmax);
			posit(3,1); printf("Armor: %-3d", crt_ptr->armor);
			posit(3,30); printf("Thac0: %-3d", crt_ptr->thaco);
			posit(4,1); printf("  Num: %-3d", crt_ptr->numwander);
			posit(2,8); getnums(&j, (long)crt_ptr->hpcur, 0L, 32000L);
			crt_ptr->hpcur = j;
			posit(2,14); getnums(&j, (long)crt_ptr->hpmax, 0L, 32000L);
			crt_ptr->hpmax = j;
			posit(2,37); getnums(&j, (long)crt_ptr->mpcur, 0L, 32000L);
			crt_ptr->mpcur = j;
			posit(2,43); getnums(&j, (long)crt_ptr->mpmax, 0L, 32000L);
			crt_ptr->mpmax = j;
			posit(3,8); getnums(&j, (long)crt_ptr->armor, -127L, 127L);
			crt_ptr->armor = j;
			posit(3,37); getnums(&j, (long)crt_ptr->thaco, -127L, 127L);
			crt_ptr->thaco = j;
			posit(4,8); getnums(&j, (long)crt_ptr->numwander, -127L, 127L);
			crt_ptr->numwander = j;
			break;
		case 5: 		/* exp/gold/dmg/quest */
			clearscreen();
			posit(2,1); printf("Experience: %ld", crt_ptr->experience);
			posit(2,30); printf("  Gold: %ld", crt_ptr->gold);
			posit(3,1); printf("    # Dice: %-5d", crt_ptr->ndice);
			posit(3,30); printf(" Sides: %-5d", crt_ptr->sdice);
			posit(3,60); printf("  Plus: %-5d", crt_ptr->pdice);
			posit(4,1); printf("     Quest: %-3d", crt_ptr->questnum);
			posit(4,29); printf("Special: %-3d", crt_ptr->special);
			posit(2,13); getnums(&j, (long)crt_ptr->experience, 0L, 1000000L);
			crt_ptr->experience = j;
			posit(2,38); getnums(&j, (long)crt_ptr->gold, 0L, 20000000L);
			crt_ptr->gold = j;
			posit(3,13); getnums(&j, (long)crt_ptr->ndice, 0L, 32000L);
			crt_ptr->ndice = j;
			posit(3,38); getnums(&j, (long)crt_ptr->sdice, 0L, 32000L);
			crt_ptr->sdice = j;
			posit(3,68); getnums(&j, (long)crt_ptr->pdice, 0L, 32000L);
			crt_ptr->pdice = j;
			posit(4,13); getnums(&j, (long)crt_ptr->questnum, -1L, 127L);
			crt_ptr->questnum = j;
			posit(4,38); getnums(&j, (long)crt_ptr->special, 0L, 32000L);
			crt_ptr->special = j;
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
					str[j] = (crt_ptr->spells[j/8] & 1<<(j%8)) ? '*':'.';
				str[128] = 0; j = str[70];
				str[70] = 0;
				posit(4,1); printf("%s",str);
				str[70] = j;
				posit(9,1); printf("%s",&str[70]);
				posit(11,10); getnum(&j, 0, 128);
				if(!j) break;
				if(crt_ptr->spells[(j-1)/8] & 1<<((j-1)%8))
					crt_ptr->spells[(j-1)/8] &= ~(1<<((j-1)%8));
				else
					crt_ptr->spells[(j-1)/8] |= 1<<((j-1)%8);
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
					str[j] = (crt_ptr->flags[j/8] & 1<<(j%8)) ? '*':'.';
				str[64] = 0;
				posit(4,1); printf("%s",str);
				posit(7,9); getnum(&j, 0, 64);
				if(!j) break;
				if(crt_ptr->flags[(j-1)/8] & 1<<((j-1)%8))
					crt_ptr->flags[(j-1)/8] &= ~(1<<((j-1)%8));
				else
					crt_ptr->flags[(j-1)/8] |= 1<<((j-1)%8);
			}
			break;
		case 8:
			clearscreen();
			posit(2,1); printf("Proficiencies:");
			posit(3,1); printf("1:%ld", crt_ptr->proficiency[0]);
			posit(3,27); printf("2:%ld", crt_ptr->proficiency[1]);
			posit(3,54); printf("3:%ld", crt_ptr->proficiency[2]);
			posit(4,1); printf("4:%ld", crt_ptr->proficiency[3]);
			posit(4,27); printf("5:%ld", crt_ptr->proficiency[4]);
			posit(3,3); getnums(&j, (long)crt_ptr->proficiency[0], 0L, 1000000L);
			crt_ptr->proficiency[0] = j;
			posit(3,29); getnums(&j, (long)crt_ptr->proficiency[1], 0L, 1000000L);
			crt_ptr->proficiency[1] = j;
			posit(3,56); getnums(&j, (long)crt_ptr->proficiency[2], 0L, 1000000L);
			crt_ptr->proficiency[2] = j;
			posit(4,3); getnums(&j, (long)crt_ptr->proficiency[3], 0L, 1000000L);
			crt_ptr->proficiency[3] = j;
			posit(4,29); getnums(&j, (long)crt_ptr->proficiency[4], 0L, 1000000L);
			crt_ptr->proficiency[4] = j;
			break;
		case 9:			/* carryables */
			clearscreen();
			posit(2,1); printf("Items carried:");
			posit(3,1); printf("1:%-5d",crt_ptr->carry[0]);
			posit(3,17); printf("2:%-5d",crt_ptr->carry[1]);
			posit(3,33); printf("3:%-5d",crt_ptr->carry[2]);
			posit(3,49); printf("4:%-5d",crt_ptr->carry[3]);
			posit(3,65); printf("5:%-5d",crt_ptr->carry[4]);
			posit(4,1); printf("6:%-5d",crt_ptr->carry[5]);
			posit(4,17); printf("7:%-5d",crt_ptr->carry[6]);
			posit(4,33); printf("8:%-5d",crt_ptr->carry[7]);
			posit(4,49); printf("9:%-5d",crt_ptr->carry[8]);
			posit(4,64); printf("10:%-5d",crt_ptr->carry[9]);
			posit(3,3); getnums(&j, (long)crt_ptr->carry[0], 0L, 32000L);
			crt_ptr->carry[0] = j;
			posit(3,19); getnums(&j, (long)crt_ptr->carry[1], 0L, 32000L);
			crt_ptr->carry[1] = j;
			posit(3,35); getnums(&j, (long)crt_ptr->carry[2], 0L, 32000L);
			crt_ptr->carry[2] = j;
			posit(3,51); getnums(&j, (long)crt_ptr->carry[3], 0L, 32000L);
			crt_ptr->carry[3] = j;
			posit(3,67); getnums(&j, (long)crt_ptr->carry[4], 0L, 32000L);
			crt_ptr->carry[4] = j;
			posit(4,3); getnums(&j, (long)crt_ptr->carry[5], 0L, 32000L);
			crt_ptr->carry[5] = j;
			posit(4,19); getnums(&j, (long)crt_ptr->carry[6], 0L, 32000L);
			crt_ptr->carry[6] = j;
			posit(4,35); getnums(&j, (long)crt_ptr->carry[7], 0L, 32000L);
			crt_ptr->carry[7] = j;
			posit(4,51); getnums(&j, (long)crt_ptr->carry[8], 0L, 32000L);
			crt_ptr->carry[8] = j;
			posit(4,67); getnums(&j, (long)crt_ptr->carry[9], 0L, 32000L);
			crt_ptr->carry[9] = j;
			break;
		case 10:
			clearscreen();
			posit(1,1);
			printf("Are you sure you want to clear it? ");
			getstr(str, "n", 2);
			if(str[0]=='y' || str[0]=='Y')
				zero(crt_ptr, sizeof(creature));
			break;
		case 11:
			free(crt_ptr);
			return;
		case 12:
			save_crt_to_file(num, crt_ptr);
			free(crt_ptr);
			return;
		}
	}
}

int load_crt_from_file(num, crt_ptr)
int 		num;
creature 	**crt_ptr;
{
	int fd;
	long n;
	char file[80];

	*crt_ptr = (creature *)malloc(sizeof(creature));
	if(!*crt_ptr)
		merror("load_from_file", FATAL);
	zero(*crt_ptr, sizeof(creature));

#ifdef TC
	sprintf(file, "%s\\m%02d", MONPATH, num/MFILESIZE);
#else
	sprintf(file, "%s/m%02d", MONPATH, num/MFILESIZE);
#endif
	fd = open(file, O_RDONLY | O_BINARY, 0);
	if(fd < 0) 
		return(-1);
	n = lseek(fd, (long)((num%MFILESIZE)*sizeof(creature)), 0);
	if(n < 0L) {
		close(fd);
		return(-1);
	}
	n = read(fd, *crt_ptr, sizeof(creature));
	if(n < sizeof(creature)) {
		close(fd);
		return(-1);
	}
	close(fd);

	return(0);
}

int save_crt_to_file(num, crt_ptr)
int 		num;
creature	*crt_ptr;
{
	int fd;
	long n;
	char file[80];

#ifdef TC
	sprintf(file, "%s\\m%02d", MONPATH, num/MFILESIZE);
#else
	sprintf(file, "%s/m%02d", MONPATH, num/MFILESIZE);
#endif
	fd = open(file, O_RDWR | O_BINARY, 0);
	if(fd < 0) {
#ifdef TC
		fd = open(file, O_RDWR | O_CREAT | O_BINARY, S_IWRITE | S_IREAD);
#else
		fd = open(file, O_RDWR | O_CREAT, ACC);
#endif
		if(fd < 0)
			return(-1);
	}
	n = lseek(fd, (long)((num%MFILESIZE)*sizeof(creature)), 0);
	if(n < 0L) {
		close(fd);
		return(-1);
	}
	n = write(fd, crt_ptr, sizeof(creature));
	if(n < sizeof(creature)) {
		close(fd);
		return(-1);
	}
	close(fd);

	return(0);
}

void edit_room()
{
	int 	i, num, x;
	long 	j;
	char	str[256];
	char	*shortd, *longd;
	room	*rom_ptr;

	clearscreen();
	posit(3,20); printf("Enter room number to edit: ");
	getnum(&num, 0, 10000);

	load_rom_from_file(num, &rom_ptr);
	rom_ptr->rom_num = num;
	if(!rom_ptr->established)
		rom_ptr->established = time(0);

	while(1) {
		clearscreen();
		posit(2,25); printf("Room: (%d) %s",num,rom_ptr->name);
		posit(3,30);  printf(" 1. Edit name");
		posit(4,30);  printf(" 2. Edit short description");
		posit(5,30);  printf(" 3. Edit long description");
		posit(6,30);  printf(" 4. Edit level/trap/track/special");
		posit(7,30);  printf(" 5. Edit flags");
		posit(8,30);  printf(" 6. Edit random monsters");
		posit(9,30);  printf(" 7. Edit permanent monsters");
		posit(10,30); printf(" 8. Edit permanent objects");
		posit(11,30); printf(" 9. Edit room statistics");
		posit(12,30); printf("10. Edit exits");
		posit(13,30); printf("11. Clear this room");
		posit(14,30); printf("12. Abort this room");
		posit(15,30); printf("13. Back to main menu");
		posit(18,30); printf("Choice: ");

		getnum(&i, 1, 13);

		switch(i) {
		case 1:
			clearscreen();
			posit(2,1); printf("Room name:");
			posit(3,1); printf("%s",rom_ptr->name);
			posit(3,1); getstr(str, rom_ptr->name, 80);
			strcpy(rom_ptr->name, str);
			break;
		case 2:
			clearscreen();
			posit(1,1); printf("Current short description:");
			posit(2,1); printf("%s",rom_ptr->short_desc);
			posit(14,1); printf("Enter new description:");
			x = 15; 
			do {
				posit(x,1); printf("]");
				posit(x,2); getstr(str, "", 80);
				if(!str[0])
					break;
				if(x==15) {
					shortd = (char *)malloc(2000);
					shortd[0] = 0;
				}
				strcat(shortd, str);
				strcat(shortd, "\n");
				x++;
			} while(x < 24);
			if(x>15) {
				if(rom_ptr->short_desc)
					free(rom_ptr->short_desc);
				shortd[strlen(shortd)-1] = 0;
				rom_ptr->short_desc = shortd;
			}
			break;
		case 3:
			clearscreen();
			posit(1,1); printf("Current long description:");
			posit(2,1); printf("%s",rom_ptr->long_desc);
			posit(14,1); printf("Enter new description:");
			x = 15; 
			do {
				posit(x,1); printf("]");
				posit(x,2); getstr(str, "", 80);
				if(!str[0])
					break;
				if(x==15) {
					longd = (char *)malloc(2000);
					longd[0] = 0;
				}
				strcat(longd, str);
				strcat(longd, "\n");
				x++;
			} while(x < 25);
			if(x>15) {
				if(rom_ptr->long_desc)
					free(rom_ptr->long_desc);
				longd[strlen(longd)-1] = 0;
				rom_ptr->long_desc = longd;
			}
			break;
		case 4:			/* level/trap/track */
			clearscreen();
			posit(2,1); printf("Trap: %-3d", rom_ptr->trap);
			posit(2,30); printf("Trap exit: %-5d", rom_ptr->trapexit);
			posit(2,59); printf("Special: %-3d", rom_ptr->special);
			posit(4,1); printf("Track string:");
			posit(5,1); printf("%s",rom_ptr->track);
			posit(7,1); printf("Low level: %-2d", rom_ptr->lolevel);
			posit(8,1); printf(" Hi level: %-2d", rom_ptr->hilevel);
			posit(2,7); getnums(&j, (long)rom_ptr->trap, -127L, 127L);
			rom_ptr->trap = j;
			posit(2,41); getnums(&j, (long)rom_ptr->trapexit, 0L, 32000L);
			rom_ptr->trapexit = j;
			posit(2, 68); getnums(&j, (long)rom_ptr->special, 0L, 32000L);
			rom_ptr->special = j;
			posit(5,1); getstr(str, rom_ptr->track, 80);
			strcpy(rom_ptr->track, str);
			posit(7,12); getnums(&j, (long)rom_ptr->lolevel, 0L, 50L);
			rom_ptr->lolevel = j;
			posit(8,12); getnums(&j, (long)rom_ptr->hilevel, 0L, 50L);
			rom_ptr->hilevel = j;
			break;
		case 5:			/* flags */
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
					str[j] = (rom_ptr->flags[j/8] & 1<<(j%8)) ? '*':'.';
				str[64] = 0;
				posit(4,1); printf("%s",str);
				posit(7,9); getnum(&j, 0, 64);
				if(!j) break;
				if(rom_ptr->flags[(j-1)/8] & 1<<((j-1)%8))
					rom_ptr->flags[(j-1)/8] &= ~(1<<((j-1)%8));
				else
					rom_ptr->flags[(j-1)/8] |= 1<<((j-1)%8);
			}
			break;
		case 6:			/* random monsters */
			clearscreen();
			posit(2,1); printf("Random monsters:");
			for(x=0; x<10; x++) {
				posit(x+3,x==9 ? 2:3); 
				printf("%d:%-5d",x+1,rom_ptr->random[x]);
			}
			posit(14,2); printf("Traffic: %-3d", rom_ptr->traffic);
			for(x=0; x<10; x++) {
				posit(x+3,5);
				getnums(&j, (long)rom_ptr->random[x], 0L, 32000L);
				rom_ptr->random[x] = j;
			}
			posit(14,11); getnums(&j, (long)rom_ptr->traffic, -127L, 127L);
			rom_ptr->traffic = j;
			break;
		case 7:			/* Perm monsters */
			clearscreen();
			posit(2,1); printf("Permanent monsters:");
			posit(4,10); printf("Interval");
			posit(4,20); printf("Lastreset");
			posit(4,30); printf("Monster");
			for(x=0; x<10; x++) {
				posit(x+5, 2); printf("%2d:",x+1);
				posit(x+5, 10); printf("%lu",rom_ptr->perm_mon[x].interval);
				posit(x+5, 20); printf("%lu",rom_ptr->perm_mon[x].ltime);
				posit(x+5, 30); printf("%-5d",rom_ptr->perm_mon[x].misc);
			}
			posit(17,1); printf("Monster #:");
			while(1) {
				posit(17,12); getnum(&x, 0, 10);
				if(!x) break;
				posit(x+4, 10); getnums(&j, (long)rom_ptr->perm_mon[x-1].interval, 0L, 10000000L);
				rom_ptr->perm_mon[x-1].interval = j;
				posit(x+4, 20); getnums(&j, (long)rom_ptr->perm_mon[x-1].ltime, 0L, 10000000L);
				rom_ptr->perm_mon[x-1].ltime = j;
				posit(x+4, 30); getnums(&j, (long)rom_ptr->perm_mon[x-1].misc, 0L, 32000L);
				rom_ptr->perm_mon[x-1].misc = j;
			}
			break;
		case 8:			/* Perm objects */
			clearscreen();
			posit(2,1); printf("Permanent objects:");
			posit(4,10); printf("Interval");
			posit(4,20); printf("Lastreset");
			posit(4,30); printf("Object");
			for(x=0; x<10; x++) {
				posit(x+5, 2); printf("%2d:",x+1);
				posit(x+5, 10); printf("%lu",rom_ptr->perm_obj[x].interval);
				posit(x+5, 20); printf("%lu",rom_ptr->perm_obj[x].ltime);
				posit(x+5, 30); printf("%-5d",rom_ptr->perm_obj[x].misc);
			}
			posit(17,1); printf("Object #:");
			while(1) {
				posit(17,11); getnum(&x, 0, 10);
				if(!x) break;
				posit(x+4, 10); getnums(&j, (long)rom_ptr->perm_obj[x-1].interval, 0L, 10000000L);
				rom_ptr->perm_obj[x-1].interval = j;
				posit(x+4, 20); getnums(&j, (long)rom_ptr->perm_obj[x-1].ltime, 0L, 10000000L);
				rom_ptr->perm_obj[x-1].ltime = j;
				posit(x+4, 30); getnums(&j, (long)rom_ptr->perm_obj[x-1].misc, 0L, 32000L);
				rom_ptr->perm_obj[x-1].misc = j;
			}
			break;
		case 9:			/* Statistics */
			clearscreen();
			posit(2,1); printf("     Visits: %lu", (unsigned long)rom_ptr->beenhere);
			posit(3,1); printf("Established: %lu", (unsigned long)rom_ptr->established);
			posit(2,14); getnums(&j, (long)rom_ptr->beenhere, 0L, 10000000L);
			posit(3,14); getnums(&j, (long)rom_ptr->established, 0L, 10000000L);
			break;
		case 10:		/* Exits */
			clearscreen();
			edit_exits(rom_ptr);
			break;
		case 11:		/* Clear */
			clearscreen();
			posit(1,1);
			printf("Are you sure you want to clear it? ");
			getstr(str, "n", 2);
			if(str[0]=='y' || str[0]=='Y') {
				free_rom(rom_ptr);
				rom_ptr = (room *)malloc(sizeof(room));
				zero(rom_ptr, sizeof(room));
			}
			break;
		case 12:		/* abort */
			free_rom(rom_ptr);
			return;
		case 13:		/* save */
			save_rom_to_file(num, rom_ptr);
			free_rom(rom_ptr);
			return;
		}
	} 
}

void edit_exits(rom_ptr)
room	*rom_ptr;
{
	int 	x, i, error;
	long	j;
	xtag 	*xt, *prev;
	char	str[256];

	while(1) {
		clearscreen();
		posit(5,25); printf("Room: (%d) %s",rom_ptr->rom_num, rom_ptr->name);
		posit(6,30); printf("1. Add exit");
		posit(7,30); printf("2. Modify exit");
		posit(8,30); printf("3. Delete exit");
		posit(9,30); printf("4. List exits");
		posit(10,30); printf("5. Back to room menu");
		posit(12,30); printf("Choice: ");

		getnum(&i, 1, 5);

		switch(i) {
		case 1:			/* Add exit */
			xt = rom_ptr->first_ext; x = 1;
			if(xt) {
				while(xt->next_tag) {
					x++;
					xt = xt->next_tag;
				}
				x++;
				xt->next_tag = (xtag *)malloc(sizeof(xtag));
				xt = xt->next_tag;
			}
			else {
				rom_ptr->first_ext = (xtag *)malloc(sizeof(xtag));
				xt = rom_ptr->first_ext;
			}
			xt->ext = (exit_ *)malloc(sizeof(exit_));
			zero(xt->ext, sizeof(exit_));
			xt->next_tag = 0;
			clearscreen();
			posit(2,1); printf("Exit #%d:",x);
			posit(4,10); printf("Name: %s", xt->ext->name);
			posit(5,10); printf("Room: %d", xt->ext->room);
			posit(6,11); printf("Key: %d", xt->ext->key);
			posit(7,1); printf("Time Interval: %lu", xt->ext->ltime.interval);
			posit(8,5); printf("Last Time: %lu", xt->ext->ltime.ltime);
			posit(9,10); printf("Misc: %d", xt->ext->ltime.misc);
			posit(11,10); printf("1");
			posit(11,20); printf("2");
			posit(11,30); printf("3");
			posit(12,1); printf("12345678901234567890123456789012");
			posit(15,1); printf("Flag #: ");
			posit(16,1); printf("(0 to exit)");
			for(j=0;j<32;j++)
				str[j] = (xt->ext->flags[j/8] & 1<<(j%8)) ? '*':'.';
			str[32] = 0;
			posit(13,1); printf("%s",str);
			
			posit(4,16); getstr(str, xt->ext->name, 20);
			strcpy(xt->ext->name, str);
			posit(5,16); getnums(&j, (long)xt->ext->room, 0L, 32000L);
			xt->ext->room = j;
			posit(6,16); getnums(&j, (long)xt->ext->key, -127L, 127L);
			xt->ext->key = j;
			posit(7,16); getnums(&j, (long)xt->ext->ltime.interval, 0L, 10000000L);
			xt->ext->ltime.interval = j;
			posit(8,16); getnums(&j, (long)xt->ext->ltime.ltime, 0L, 10000000L);
			xt->ext->ltime.ltime = j;
			posit(9,16); getnums(&j, (long)xt->ext->ltime.misc, 0L, 32000L);
			xt->ext->ltime.misc = j;
			while(1) {
				posit(15,9); getnum(&j, 0, 32);
				if(!j) break;
				if(xt->ext->flags[(j-1)/8] & 1<<((j-1)%8))
					xt->ext->flags[(j-1)/8] &= ~(1<<((j-1)%8));
				else
					xt->ext->flags[(j-1)/8] |= 1<<((j-1)%8);
				for(j=0;j<32;j++)
					str[j] = (xt->ext->flags[j/8] & 1<<(j%8)) ? '*':'.';
				str[32] = 0;
				posit(13,1); printf("%s",str);
			}
			break;
		case 2:			/* modify exit */
			clearscreen();
			posit(2,1); printf("Exit #"); 
			getnum(&i, 0, 50);
			if(!i) break;
			xt = rom_ptr->first_ext; x = 1; error = 0;
			while(x <= i) {
				if(!xt) {
					error = 1;
					break;
				}
				if(x == i)
					break;
				x++;
				xt = xt->next_tag;
			}
			if(error) break;
			clearscreen();
			posit(2,1); printf("Exit #%d:",x);
			posit(4,10); printf("Name: %s", xt->ext->name);
			posit(5,10); printf("Room: %d", xt->ext->room);
			posit(6,11); printf("Key: %d", xt->ext->key);
			posit(7,1); printf("Time Interval: %lu", xt->ext->ltime.interval);
			posit(8,5); printf("Last Time: %lu", xt->ext->ltime.ltime);
			posit(9,10); printf("Misc: %d", xt->ext->ltime.misc);
			posit(11,10); printf("1");
			posit(11,20); printf("2");
			posit(11,30); printf("3");
			posit(12,1); printf("12345678901234567890123456789012");
			posit(15,1); printf("Flag #: ");
			posit(16,1); printf("(0 to exit)");
			for(j=0;j<32;j++)
				str[j] = (xt->ext->flags[j/8] & 1<<(j%8)) ? '*':'.';
			str[32] = 0;
			posit(13,1); printf("%s",str);
			
			posit(4,16); getstr(str, xt->ext->name, 20);
			strcpy(xt->ext->name, str);
			posit(5,16); getnums(&j, (long)xt->ext->room, 0L, 32000L);
			xt->ext->room = j;
			posit(6,16); getnums(&j, (long)xt->ext->key, -127L, 127L);
			xt->ext->key = j;
			posit(7,16); getnums(&j, (long)xt->ext->ltime.interval, 0L, 10000000L);
			xt->ext->ltime.interval = j;
			posit(8,16); getnums(&j, (long)xt->ext->ltime.ltime, 0L, 10000000L);
			xt->ext->ltime.ltime = j;
			posit(9,16); getnums(&j, (long)xt->ext->ltime.misc, 0L, 32000L);
			xt->ext->ltime.misc = j;
			while(1) {
				posit(15,9); getnum(&j, 0, 32);
				if(!j) break;
				if(xt->ext->flags[(j-1)/8] & 1<<((j-1)%8))
					xt->ext->flags[(j-1)/8] &= ~(1<<((j-1)%8));
				else
					xt->ext->flags[(j-1)/8] |= 1<<((j-1)%8);
				for(j=0;j<32;j++)
					str[j] = (xt->ext->flags[j/8] & 1<<(j%8)) ? '*':'.';
				str[32] = 0;
				posit(13,1); printf("%s",str);
			}
			break;
		case 3: 		/* delete */
			clearscreen();
			posit(2,1); printf("Delete exit #");
			getnum(&i, 0, 50);
			if(!i) break;
			xt = rom_ptr->first_ext; x = 1; error = 0;
			while(x <= i) {
				if(!xt) {
					error = 1;
					break;
				}
				if(x == i)
					break;
				x++;
				prev = xt;
				xt = xt->next_tag;
			}
			if(error) break;
			clearscreen();
			if(xt == rom_ptr->first_ext) {
				rom_ptr->first_ext = xt->next_tag;
				free(xt->ext);
				free(xt);
			}
			else {
				prev->next_tag = xt->next_tag;
				free(xt->ext);
				free(xt);
			}
			break;
		case 4:			/* list */
			clearscreen();
			xt = rom_ptr->first_ext; x=0;
			while(xt && x<48) {
			  posit(x%24+1, x/24>0?40:1);
			  printf("%d: (%d) %s", x+1, xt->ext->room, xt->ext->name);
			  xt = xt->next_tag;
			  x++;
			}
			posit(24,40); printf("Hit Enter:");
			getnum(&i,0,0);
			break;
		case 5:
			return;
		}
	}
}

int load_rom_from_file(num, rom_ptr)
int 	num;
room	**rom_ptr;
{
	int fd;
	long n;
	char file[80];

	*rom_ptr = (room *)malloc(sizeof(room));
	if(!*rom_ptr)
		merror("load_from_file", FATAL);
	zero(*rom_ptr, sizeof(room));

#ifdef TC
	sprintf(file, "%s\\r%05d", ROOMPATH, num);
#else
	sprintf(file, "%s/r%02d/r%05d", ROOMPATH, num/1000, num);
#endif
	fd = open(file, O_RDONLY | O_BINARY, 0);
	if(fd < 0) 
		return(-1);
	n = read_rom(fd, *rom_ptr);
	if(n < 0) {
		close(fd);
		return(-1);
	}
	close(fd);

	return(0);
}

int save_rom_to_file(num, rom_ptr)
int 	num;
room	*rom_ptr;
{
	int fd;
	long n;
	char file[80];

#ifdef TC
	sprintf(file, "%s\\r%05d", ROOMPATH, num);
#else
	sprintf(file, "%s/r%02d/r%05d", ROOMPATH, num/1000, num);
#endif
	unlink(file);
#ifdef TC
	fd = open(file, O_RDWR | O_CREAT | O_BINARY, S_IWRITE | S_IREAD);
#else
	fd = open(file, O_RDWR | O_CREAT, ACC);
#endif
	if(fd < 0)
		return(-1);

	n = write_rom(fd, rom_ptr);
	if(n < 0) {
		close(fd);
		return(-1);
	}
	close(fd);

	return(0);
}
	
void clearscreen()
{
        printf("%c[2J",27);
}

void posit(x, y)
int x,y;
{
        printf("%c[%d;%df", 27, x, y);
}

void getnum(i, min, max)
int	*i;
int	min, max;
{
	char str[80];

	savepos();
	do {
		restorepos();
		gets(str);
		*i = atoi(str);
	} while(*i < min || *i > max);
}

void getnums(i, orig, min, max)
long	*i;
long	orig;
long	min, max;
{
	char str[80];

	savepos();
	do {
		restorepos();
		gets(str);
		if(!str[0]) break;
		*i = atol(str);
	} while(*i < min || *i > max);
	if(!str[0])
		*i = orig;
}
	
void getstr(str, origstr, maxlen)
char 	*str, *origstr;
int	maxlen;
{
	char tempstr[256];

	gets(tempstr);
	if(!tempstr[0])
		strcpy(str, origstr);
	else if(!strcmp(tempstr, "@"))
		str[0] = 0;
	else
		strncpy(str, tempstr, maxlen-1);
}

void savepos()
{
	printf("%c[s",27);
}

void restorepos()
{
	printf("%c[u",27);
}

void zero(ptr, size)
void 	*ptr;
int	size;
{
	char 	*chptr;
	int	i;

        chptr = (char *)ptr;
	for(i=0; i<size; i++) 
		chptr[i] = 0;
}
	
void merror(str, errtype)
char 	*str;
int 	errtype;
{
	printf("Error: %s\n", str);
	if(errtype == FATAL)
		exit(-1);
}
