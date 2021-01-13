/*
 * LIST.C:
 *
 *	This program is a utility that lists out monsters, objects and
 *	rooms.
 *
 *	Copyright (C) 1991 Brett J. Vickers
 *
 */

#include "mtype.h"
#include "mstruct.h"
#include <stdio.h>
#include <sys/file.h>

#define PRINTF(a) write(Fd, a, strlen(a))
#define MISSMAX 500 

extern void list_crt(), list_obj(), list_rom(), handle_args(), abort();
extern int  load_obj(), load_crt(), load_rom();

char	Progtitle[80];
char	Str[240];
int	Rlo = 0, Rhi = 2000, Type = -1, Wear = -1,
	Levlo = 1, Levhi = 127, Quests, Obj = -1, Mon = -1;
int	Class[15], Notclass[15], Flag[15], Notflag[15], Spell[15], Spellnum=0,
	Classnum=0, Notclassnum=0, Flagnum=0, Notflagnum=0, Fd=1;
int Dmglo = 0, Dmghi = 1000;
int Dmg=0;
int check_dmg;

main(argc, argv)
int	argc;
char	*argv[];
{
	strcpy(Progtitle, argv[0]);

	if(argc < 2)
		abort();

	if(argc > 2) 
		handle_args(argc-2, &argv[2]);

	switch(argv[1][0]) {
	case 'm':
		list_crt();
		break;
	case 'o':
		list_obj();
		break;
	case 'r':
		list_rom();
		break;
	case 'a':
		list_obj();
		list_crt();
		list_rom();
		break;
	default:
		abort();
	}
}

void handle_args(argc, argv)
int 	argc;
char 	*argv[];
{
	char	rangestr[80];
	int	i, j, n;

	Rlo = 0;
	Rhi = 32000;

	for(i=0; i<argc; i++) {
		if(argv[i][0] == 0) continue;

		if(argv[i][0] != '-')
			abort();

		switch(argv[i][1]) {

		case 'r':

		strcpy(rangestr, &argv[i][2]);
		n = strlen(rangestr);
		Rlo = -1; Rhi = -1;
		for(j=1; j<n; j++) {
			if(rangestr[j] == ':') {
				rangestr[j] = 0;
				Rlo = atoi(rangestr);
				Rhi = atoi(&rangestr[j+1]);
			}
		}
		if(Rlo == -1 || Rhi == -1 || Rlo > Rhi)
			abort();
		break;

		case 'l':

		strcpy(rangestr, &argv[i][2]);
		n = strlen(rangestr);
		Levlo = -1; Levhi = -1;
		for(j=1; j<n; j++) {
			if(rangestr[j] == ':') {
				rangestr[j] = 0;
				Levlo = atoi(rangestr);
				Levhi = atoi(&rangestr[j+1]);
			}
		}
		if(Levlo == -1 || Levhi == -1 || Levlo > Levhi)
			abort();
		break;

		case 'd':

		strcpy(rangestr, &argv[i][2]);
		n = strlen(rangestr);
		Dmglo = -1; Dmghi = -1;
		for(j=1; j<n; j++) {
			if(rangestr[j] == ':') {
				rangestr[j] = 0;
				Dmglo = atoi(rangestr);
				Dmghi = atoi(&rangestr[j+1]);
			}
		}
		if(Dmglo == -1 || Dmghi == -1 || Dmglo > Dmghi)
			abort();
		break;

		case 't':

		Type = atoi(&argv[i][2]);
		break;

		case 'w':

		Wear = atoi(&argv[i][2]);
		break;

		case 'f':

		Flag[Flagnum++] = atoi(&argv[i][2]);
		break;

		case 'F':

		Notflag[Notflagnum++] = atoi(&argv[i][2]);
		break;

		case 'D':
			Dmg = atoi(&argv[i][2]);
			break;

		case 'c':
			Class[Classnum++] = atoi(&argv[i][2]);
			break;

		case 'C':
			Notclass[Notclassnum++] = atoi(&argv[i][2]);
			break;

		case 'q':

		Quests = 1;
		break;

		case 'o':

		Obj = atoi(&argv[i][2]);
		break;

		case 'm':

		Mon = atoi(&argv[i][2]);
		break;

		case 'S':

		Spell[Spellnum++] = atoi(&argv[i][2]);
		break;

		case 's':

		Fd = atoi(&argv[i][2]);
		break;

		default:

		break;

		}
	}

}

void abort()
{
	PRINTF("list <m|o|r> [options]\n");
	PRINTF("[options]:  -r#:#     index range\n");
	PRINTF("            -s#       descriptor for output\n");
	PRINTF("            -l#:#     level range\n");
	PRINTF("            -t#       object type\n");
	PRINTF("            -w#       object wearflag\n");
 	PRINTF("            -f#       flag set\n");
 	PRINTF("            -F#       flag NOT set\n");
	PRINTF("            -S#       spell set\n");
	PRINTF("			-d#:#	  damage range\n");
	PRINTF("			-D#		  damage set\n");
	PRINTF("			-c#		  class\n");
	PRINTF("			-C#		  not class\n");
	PRINTF("            -q        quest objects only\n");
	PRINTF("	    -o#       monsters/rooms carrying object\n");
	PRINTF("            -m#       rooms with monster\n");
	exit(0);
}

#define ISSET(a,b)  ((a).flags[(b)/8] & (1<<((b)%8)))
#define SP_ISSET(a,b)  ((a).spells[(b)/8] & (1<<((b)%8)))

void list_crt()
{
	creature	crt;
	int		i, j, missing=0, skipped=0, breakout=0, found=0;
	
	sprintf(Str, "%c\n%4s %-20s %-2s %4s %-14s %-3s %-3s %-2s %-5s %-4s %-8s\n", 12, "  # ", "Name", "Lv", "Algn", "Stats", "HP", "AC", "T0", "Exp", "Gold", " Dice");
	write(Fd, Str, strlen(Str));
	sprintf(Str, "------------------------------------------------------------------------------\n");
	write(Fd, Str, strlen(Str));
	for(i=Rlo; i<=Rhi; i++) {
		if(load_crt(i, &crt) < 0) {
			missing++;
			if(missing > MISSMAX) break;
			continue;
		}
		if(crt.level < Levlo || crt.level > Levhi) {
			skipped++;
			continue;
		}
		check_dmg = crt.ndice * crt.sdice + crt.pdice;
		if(check_dmg < Dmglo || check_dmg > Dmghi) {
			skipped++;
			continue;
		}
		if(check_dmg < Dmg) {
			skipped++;
			continue;
		}

		if(Obj != -1) {
			for(j=0; j<10; j++)
				if(crt.carry[j] == Obj) {
					found = 1;
					break;
				}
			if(!found) {
				skipped++;
				continue;
			}
			else found = 0;
		}
		for(j=0; j<Flagnum; j++) {
			if(!ISSET(crt, Flag[j]-1)) {
				skipped++;
				breakout = 1;
				break;
			}
		}
		if(breakout) {
			breakout = 0;
			continue;
		}
		for(j=0; j<Spellnum; j++) {
			if(!SP_ISSET(crt, Spell[j]-1)) {
				skipped++;
				breakout = 1;
				break;
			}
		}
		if(breakout) {
			breakout = 0;
			continue;
		}
		for(j=0; j<Notflagnum; j++) {
			if(ISSET(crt, Notflag[j]-1)) {
				skipped++;
				breakout = 1;
				break;
			}
		}
		if(breakout) {
			breakout = 0;
			continue;
		}
		for(j=0; j<Classnum; j++) {
			if(crt.class != Class[j]) {
				skipped++;
				breakout = 1;
				break;
			}
		}
		if(breakout) {
			breakout = 0;
			continue;
		}
		for(j=0; j<Notclassnum; j++) {
			if(crt.class == Notclass[j]) {
				skipped++;
				breakout = 1;
				break;
			}
		}
		if(breakout) {
			breakout = 0;
			continue;
		}
		sprintf(Str, "%3d. %-20.20s %02d %4d %02d/%02d/%02d/%02d/%02d %03d %03d %02d %05d %04d %2dd%-2d+%-2d\n", i, crt.name, crt.level, crt.alignment, crt.strength, crt.dexterity, crt.constitution, crt.intelligence, crt.piety, crt.hpmax, crt.armor, crt.thaco, crt.experience, crt.gold, crt.ndice, crt.sdice, crt.pdice);
		write(Fd, Str, strlen(Str));
		if(!((i-(missing+skipped)-Rlo+1)%50)) {
			sprintf(Str, "%c\n%4s %-20s %-2s %4s %-14s %-3s %-3s %-2s %-5s %-4s %-8s\n", 12, "  # ", "Name", "Lv", "Algn", "Stats", "HP", "AC", "T0", "Exp", "Gold", " Dice");
			write(Fd, Str, strlen(Str));
			sprintf(Str, "------------------------------------------------------------------------------\n");
			write(Fd, Str, strlen(Str));
		}
	}

}

void list_obj()
{
	object	obj;
	int	i, j, missing=0, skipped=0, breakout=0;

	sprintf(Str, "%c\n%4s %-20.20s %-6s %-3s %-2s %-3s %-3s %-8s %-2s %-2s %-2s %-3s\n", 12, "  # ", "Name", "Value", "Wgt", "Tp", "Adj", "Sht", " Dice", "AC", "Wr", "Mg", "Qst");
	write(Fd, Str, strlen(Str));
	sprintf(Str, "------------------------------------------------------------------------------\n");
	write(Fd, Str, strlen(Str));
	for(i=Rlo; i<=Rhi; i++) {
		if(load_obj(i, &obj) < 0) {
			missing++;
			if(missing > MISSMAX) break;
			continue;
		}
		if(Type != -1 && obj.type != Type) {
			skipped++;
			continue;
		}
		check_dmg = obj.ndice * obj.sdice + obj.pdice;
		if(check_dmg < Dmglo || check_dmg > Dmghi) {
			skipped++;
			continue;
		}
		if(check_dmg < Dmg) {
			skipped++;
			continue;
		}

		if(Wear != -1 && obj.wearflag != Wear) {
			skipped++;
			continue;
		}
		if(Quests && !obj.questnum) {
			skipped++;
			continue;
		}
		for(j=0; j<Flagnum; j++) {
			if(!ISSET(obj, Flag[j]-1)) {
				skipped++;
				breakout = 1;
				break;
			}
		}
		if(breakout) {
			breakout = 0;
			continue;
		}
		for(j=0; j<Notflagnum; j++) {
			if(ISSET(obj, Notflag[j]-1)) {
				skipped++;
				breakout = 1;
				break;
			}
		}
		if(breakout) {
			breakout = 0;
			continue;
		}
		sprintf(Str, "%3d. %-20.20s %06d %03d %02d +%-2d %03d %2dd%-2d+%-2d %02d %02d %02d %03d\n", i, obj.name, obj.value, obj.weight, obj.type, obj.adjustment, obj.shotsmax, obj.ndice, obj.sdice, obj.pdice, obj.armor, obj.wearflag, obj.magicpower, obj.questnum);
		write(Fd, Str, strlen(Str));
		if(!((i-(missing+skipped)-Rlo+1)%50)) {
			sprintf(Str,"%c\n%4s %-20s %-6s %-3s %-2s %-3s %-3s %-8s %-2s %-2s %-2s %-3s\n", 12, "  # ", "Name", "Value", "Wgt", "Tp", "Adj", "Sht", " Dice", "AC", "Wr", "Mg", "Qst");
			write(Fd, Str, strlen(Str));
			sprintf(Str, "------------------------------------------------------------------------------\n");
			write(Fd, Str, strlen(Str));
		}
	}

}

void list_rom()
{
	int	i, j, missing=0, breakout=0, skipped=0, found=0;
	room	rom;

	sprintf(Str, "%c\n%4s %-20s %-39s %-4s\n", 12, "  # ", "Name", "Random Monsters", "Traf");
	write(Fd, Str, strlen(Str));
	sprintf(Str,"------------------------------------------------------------------------------\n");
	write(Fd, Str, strlen(Str));
	for(i=Rlo; i<=Rhi; i++) {
		if(load_rom(i, &rom) < 0) {
			missing++;
			if(missing > MISSMAX && i>2999) break;
			continue;
		}
		for(j=0; j<Flagnum; j++) {
			if(!ISSET(rom, Flag[j]-1)) {
				skipped++;
				breakout = 1;
				break;
			}
		}
		if(breakout) {
			breakout = 0;
			continue;
		}
		for(j=0; j<Notflagnum; j++) {
			if(ISSET(rom, Notflag[j]-1)) {
				skipped++;
				breakout = 1;
				break;
			}
		}
		if(breakout) {
			breakout = 0;
			continue;
		}
		if(Obj != -1) {
			for(j=0; j<10; j++)
				if(rom.perm_obj[j].misc == Obj) {
					found = 1;
					break;
				}
			if(!found) {
				skipped++;
				continue;
			}
			else found = 0;
		}
		if(Mon != -1) {
			for(j=0; j<10; j++)
				if(rom.random[j] == Mon) {
					found = 1;
					break;
				}
			for(j=0; j<10; j++)
				if(rom.perm_mon[j].misc == Mon) {
					found = 1;
					break;
				}
			if(!found) {
				skipped++;
				continue;
			}
			else found = 0;
		}
		rom.name[20]=0;
		sprintf(Str, "%3d. %-20.20s %03d/%03d/%03d/%03d/%03d/%03d/%03d/%03d/%03d/%03d %03d%%\n", i, rom.name, rom.random[0], rom.random[1], rom.random[2], rom.random[3], rom.random[4], rom.random[5], rom.random[6], rom.random[7], rom.random[8], rom.random[9], rom.traffic);
		write(Fd, Str, strlen(Str));
		if(!((i-(missing+skipped)-Rlo+1)%50)) {
			sprintf(Str, "%c\n%4s %-20s %-39s %-4s\n", 12, "  # ", "Name", "Random Monsters", "Traf");
			write(Fd, Str, strlen(Str));
			sprintf(Str, "------------------------------------------------------------------------------\n");
			write(Fd, Str, strlen(Str));
		}
	}
}

int load_obj(index, obj_ptr)
short	index;
object	*obj_ptr;
{
	int	fd;
	long	n;
	char	file[256];

	sprintf(file, "%s/o%02d", OBJPATH, index/OFILESIZE);
	fd = open(file, O_RDONLY, 0);
	if(fd < 0)
		return(-1);
	n = lseek(fd, (long)((index%OFILESIZE)*sizeof(object)), 0);
	if(n < 0L) {
		close(fd);
		return(-1);
	}
	n = read(fd, obj_ptr, sizeof(object));
	close(fd);
	if(n < sizeof(object))
		return(-1);
	return(0);
}

int load_crt(index, mon_ptr)
short		index;
creature	*mon_ptr;
{
	int	fd;
	long	n;
	char	file[256];

	sprintf(file, "%s/m%02d", MONPATH, index/MFILESIZE);
	fd = open(file, O_RDONLY, 0);
	if(fd < 0)
		return(-1);

	n = lseek(fd, (long)((index%MFILESIZE)*sizeof(creature)), 0);
	if(n < 0L) {
		close(fd);
		return(-1);
	}

	n = read(fd, mon_ptr, sizeof(creature));
	close(fd);

	if(n < sizeof(creature)) 
		return(-1);

	return(0);
}

int load_rom(index, rom_ptr)
int	index;
room	*rom_ptr;
{
	int	fd;
	long	n;
	char	file[256];

	sprintf(file, "%s/r0%d/r%05d", ROOMPATH, index/1000, index);
	fd = open(file, O_RDONLY, 0);
	if(fd < 0)
		return(-1);

	lseek(fd, 0L, 0);

	n = read(fd, rom_ptr, sizeof(room));
	close(fd);

	if(n < sizeof(room))
		return(-1);

	return(0);
}
