/*
 * POST.C:
 *
 *	These routines are for the game post office.
 *
 *	Copyright (C) 1991, 1992, 1993 Brett J. Vickers
 *
 */

#include "mstruct.h"
#include "mextern.h"
#include <sys/stat.h>
#include <sys/types.h>

/************************************************************************/
/*				postsend				*/
/************************************************************************/

/* This function allows a player to send a letter to another player if	*/
/* he/she is in a post office.						*/

int postsend(ply_ptr, cmnd)
creature	*ply_ptr;
cmd		*cmnd;
{
	room	*rom_ptr;
	char	file[80];
	int	fd, ff;

	fd = ply_ptr->fd;
	rom_ptr = ply_ptr->parent_rom;

	if(!F_ISSET(rom_ptr, RPOSTO)) {
		print(fd, "¿©±â´Â ¿ìÃ¼±¹ÀÌ ¾Æ´Õ´Ï´Ù.\n");
		return(0);
	}

	if(cmnd->num < 2) {
		print(fd, "´©±¸ÇÑÅ× ÆíÁö¸¦ º¸³»½Ã·Á±¸¿ä?\n");
		return(0);
	}

	cmnd->str[1][0] = up(cmnd->str[1][0]);
	sprintf(file, "%s/%s/%s", PLAYERPATH, first_han(cmnd->str[1]), cmnd->str[1]);
	ff = open(file, O_RDONLY, 0);

	if(ff < 0) {
		print(fd, "±×·± »ç¿ëÀÚ´Â ¾ø½À´Ï´Ù.\n");
		return(0);
	}

	close(ff);

	print(fd, "ÆíÁö ³»¿ëÀ» ÀÔ·ÂÇÏ½Ê½Ã¿ä. ¹®ÀåÃ³À½¿¡ [.]À» Ä¡½Ã¸é ÆíÁö¾²±â¸¦ Á¾·áÇÕ´Ï´Ùä.\n°¢ ÇàÀº 80ÀÚ¸¦ ³Ñ±æ ¼ö ¾ø½À´Ï´Ù.\n-: ");

	sprintf(Ply[fd].extr->tempstr[0], "%s/%s", POSTPATH, cmnd->str[1]);

	F_SET(Ply[fd].ply, PREADI);
	output_buf();
	Ply[fd].io->intrpt &= ~1;
	Ply[fd].io->fn = postedit;
	Ply[fd].io->fnparam = 1;
	return(DOPROMPT);

}

/************************************************************************/
/*				postedit				*/
/************************************************************************/
 
/* This function is called when a player is editing a message to send	*/
/* to another player.  Its major flaw is that it sends the line just	*/
/* typed to the target's mail file immediately.  So if two people were	*/
/* simultaneously mailing the same person, it could get messy.		*/

void postedit(fd, param, str)
int	fd;
int	param;
char	*str;
{
	char	outstr[85], datestr[40];
	long	t;
	int	ff;

	if(str[0]=='.') {
		F_CLR(Ply[fd].ply, PREADI);
		print(fd, "ÆíÁö¸¦ º¸³Â½À´Ï´Ù.\n");
		RETURN(fd, command, 1);
	}

	ff = open(Ply[fd].extr->tempstr[0], O_CREAT | O_APPEND | O_RDWR, ACC);
	if(ff < 0)
		merror("postedit", FATAL);

	if(param == 1) {
		time(&t);
		strcpy(datestr, (char *)ctime(&t));
		datestr[strlen(datestr)-1] = 0;
		sprintf(outstr, "\n---\n%s (%s)´Ô¿¡°Ô¼­ÀÇ ÆíÁö:\n\n",
		    Ply[fd].ply->name, datestr);
		write(ff, outstr, strlen(outstr));
	}

	strncpy(outstr, str, 79);
	outstr[79] = 0;
	strcat(outstr, "\n");

	write(ff, outstr, strlen(outstr));

	close(ff);

	print(fd, ": ");

	output_buf();
	Ply[fd].io->intrpt &= ~1;
	RETURN(fd, postedit, 2);
}

/************************************************************************/
/*				postread				*/
/************************************************************************/

/* This function allows a player to read his or her mail.		*/

int postread(ply_ptr, cmnd)
creature	*ply_ptr;
cmd		*cmnd;
{
	struct stat f_stat;
	room	*rom_ptr;
	char 	file[80], tmp[80];
	int	fd;
	
	fd = ply_ptr->fd;
	rom_ptr = ply_ptr->parent_rom;

	if(!F_ISSET(rom_ptr, RPOSTO)) {
		print(fd, "ÀÌ°÷Àº ¿ìÃ¼±¹ÀÌ ¾Æ´Õ´Ï´Ù.\n");
		return(0);
	}
	strcpy(file, POSTPATH);

	strcat(file, "/");
	strcat(file, ply_ptr->name);
	if(stat(file,&f_stat)){
		print(fd,"¹ÞÀº ÆíÁö°¡ ¾ø½À´Ï´Ù.\n");
		return(0);
	}
	else {
		strcpy(file, POSTPATH);

		strcat(file, "/");
		strcat(file, ply_ptr->name);
		view_file(fd, 1, file);
		return(DOPROMPT);
		}
}

/************************************************************************/
/*				postdelete				*/
/************************************************************************/

/* This function allows a player to delete his or her mail.		*/

int postdelete(ply_ptr, cmnd)
creature	*ply_ptr;
cmd		*cmnd;
{
	room	*rom_ptr;
	char	file[80];
	int	fd;

	fd = ply_ptr->fd;
	rom_ptr = ply_ptr->parent_rom;

	if(!F_ISSET(rom_ptr, RPOSTO)) {
		print(fd, "ÀÌ°÷Àº ¿ìÃ¼±¹ÀÌ ¾Æ´Õ´Ï´Ù.\n");
		return(0);
	}

	sprintf(file, "%s/%s", POSTPATH, ply_ptr->name);

	print(fd, "ÆíÁö°¡ »èÁ¦µÇ¾ú½À´Ï´Ù.\n");

	unlink(file);

	return(0);
}
/************************************************************************/
/*                              notepad                                 */
/************************************************************************/
int notepad(ply_ptr, cmnd)
creature    *ply_ptr;
cmd     *cmnd;
{
    room    *rom_ptr;
    char    file[80];
    int fd, ff;
 
    fd = ply_ptr->fd;
    rom_ptr = ply_ptr->parent_rom;
 
    if (ply_ptr->class < CARETAKER)
        return(PROMPT);
 
    sprintf(file, "%s/DM_pad", POSTPATH);
 
    if (cmnd->num == 2) {
        if( low(cmnd->str[1][0]) == 'a'){
            strcpy(Ply[fd].extr->tempstr[0],file);
            print(fd, "DM notepad:\n->");
            F_SET(Ply[fd].ply, PREADI);
            output_buf();
            Ply[fd].io->intrpt &= ~1;
            Ply[fd].io->fn = noteedit;
            Ply[fd].io->fnparam = 1;
            return(DOPROMPT);
        }
        else if (low(cmnd->str[1][0]) == 'd'){
            unlink(file);
            print(fd,"Clearing DM notepad\n");
            return(PROMPT);
        }
        else{
            print(fd,"invalid option.\n");
            return(PROMPT);
        }
    }
    else{
        view_file(fd, 1, file);
        return(DOPROMPT);
    }
}
 
/************************************************************************/
/*                              noteedit                                */
/************************************************************************/
 
void noteedit(fd, param, str)
int fd;
int param;
char    *str;
{
    char    outstr[85], tmpstr[40];
    long    t;
    int ff;
 
    if(str[0] == '.') {
        F_CLR(Ply[fd].ply, PREADI);
        print(fd, "Message appended.\n");
        RETURN(fd, command, 1); 
    }
 
    ff = open(Ply[fd].extr->tempstr[0], O_RDONLY, 0);
    if(ff < 0){
        ff = open(Ply[fd].extr->tempstr[0], O_CREAT | O_RDWR, ACC);
        sprintf(tmpstr,"            %s\n\n","=== DM Notepad ===");
        write(ff, tmpstr,strlen(tmpstr));
    }
    close(ff);
 
    ff = open(Ply[fd].extr->tempstr[0], O_CREAT | O_APPEND | O_RDWR, ACC);
    if(ff < 0)
        merror("noteedit", FATAL);
 
    strncpy(outstr, str, 79);
    outstr[79] = 0;
    strcat(outstr, "\n");
 
    write(ff, outstr, strlen(outstr));
 
    close(ff);
 
    print(fd, "->");
 
    output_buf();
    Ply[fd].io->intrpt &= ~1;
    RETURN(fd, noteedit, 2);
}

int family_news(ply_ptr, cmnd)
creature    *ply_ptr;
cmd     *cmnd;
{
    room    *rom_ptr;
    char    file[80];
    int fd, ff;
    int ftotal, fnum;
 
    fd = ply_ptr->fd;
    rom_ptr = ply_ptr->parent_rom;
   
	if(!F_ISSET(ply_ptr, PFAMIL)) {
		print(fd, "´ç½ÅÀº ÆÐ°Å¸®¿¡ °¡ÀÔµÇ¾î ÀÖÁö ¾Ê½À´Ï´Ù.");
		return(0);
	}
	fnum = ply_ptr->daily[DL_EXPND].max;
#ifdef CHECKFAMILY 
	ftotal = load_family(); 
	if(!strcmp(ply_ptr->name, fmboss_str[fnum])) {
		F_SET(ply_ptr, PFMBOS);
	}
#endif
 
    sprintf(file, "%s/family/family_news_%d", PLAYERPATH, fnum);
 
    if(cmnd->num == 2) {
        if( low(cmnd->str[1][0]) == 'a'){
            strcpy(Ply[fd].extr->tempstr[0],file);
            print(fd, "ÆÐ°Å¸® °øÁö:\n->");
            F_SET(Ply[fd].ply, PREADI);
            output_buf();
            Ply[fd].io->intrpt &= ~1;
            Ply[fd].io->fn = newsedit;
            Ply[fd].io->fnparam = 1;
            return(DOPROMPT);
        }
        else if (low(cmnd->str[1][0]) == 'd'){
        	if(!F_ISSET(ply_ptr, PFMBOS)) {
        		print(fd, "»èÁ¦´Â µÎ¸ñ¸¸ °¡´ÉÇÕ´Ï´Ù.");
        		return(PROMPT);
        	}
            unlink(file);
            print(fd,"°øÁö ³»¿ëÀ» Áö¿ü½À´Ï´Ù.\n");
            return(PROMPT);
        }
        else{
            print(fd,"Àß¸øµÈ ¿É¼ÇÀÔ´Ï´Ù.\n");
            return(PROMPT);
        }
    }
    else{
    	if(file_exists(file)) {
        	view_file(fd, 1, file);
        }
        else {
        	print(fd, "ÆÐ°Å¸®ÀÇ °øÁö»çÇ×ÀÌ ¾ø½À´Ï´Ù.");
        }
        return(DOPROMPT);
    }
}
 
/************************************************************************/
/*                              noteedit                                */
/************************************************************************/
 
void newsedit(fd, param, str)
int fd;
int param;
char    *str;
{
    char    outstr[85], tmpstr[40];
    long    t;
    int ff;
 
    if(str[0] == '.') {
        F_CLR(Ply[fd].ply, PREADI);
        print(fd, "°øÁö¸¦ ³²°å½À´Ï´Ù.\n");
        RETURN(fd, command, 1); 
    }
 
    ff = open(Ply[fd].extr->tempstr[0], O_RDONLY, 0);
    if(ff < 0){
        ff = open(Ply[fd].extr->tempstr[0], O_CREAT | O_RDWR, ACC);
        sprintf(tmpstr,"                      %s\n\n","=== ÆÐ°Å¸® °øÁö ===");
        write(ff, tmpstr,strlen(tmpstr));
    }
    close(ff);
 
    ff = open(Ply[fd].extr->tempstr[0], O_CREAT | O_APPEND | O_RDWR, ACC);
    if(ff < 0)
        merror("newsedit", FATAL);
 
    strncpy(outstr, str, 79);
    outstr[79] = 0;
    strcat(outstr, "\n");
 
    write(ff, outstr, strlen(outstr));
 
    close(ff);
 
    print(fd, "->");
 
    output_buf();
    Ply[fd].io->intrpt &= ~1;
    RETURN(fd, newsedit, 2);
}

