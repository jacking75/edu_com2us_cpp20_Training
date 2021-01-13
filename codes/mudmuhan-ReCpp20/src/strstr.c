/*Copyright (c) 1991 Xerox Corporation.  All Rights Reserved.

  Permission to use,  copy,  modify  and  distribute  without
  charge this software, documentation, images, etc. is grant-
  ed, provided that this copyright and the author's  name  is
  retained.

  A fee may be charged for this program ONLY to recover costs
  for distribution (i.e. media costs).  No profit can be made
  on this program.

  The author assumes no responsibility for disasters (natural
  or otherwise) as a consequence of use of this software.

  Adam Stein (stein.wbst129@xerox.com)
*/

#include <stdio.h>

/*This function will find the first occurrence of a midstring (s2) in a
  string (s1).  This function is here because BSD systems don't have it.

  Inputs:  s1      - string to search through
	   s2      - midstring to search for
  Outputs: pointer - start of midstring in string, NULL if midstring not found
  Locals:  pointer - pointer to go through string
	   save_s2 - save s2 since s2 changes when checking
  Globals: NULL    - 0
*/
char *strstr(s1,s2)
register char *s1,*s2;
{
	register char *pointer,*save_s2;

	save_s2 = s2;

	for(pointer = s1;*pointer;++pointer) {
	  if(*pointer == *s2)
	    while(*pointer && *s2 && *(++pointer) == *(++s2)) ;

	  /*If we matched every character from s2 then return pointer
	    (which is pointing to the end of the substring in the main
	    string) - the length of the substring.  Else, return s2 to
	    what it was originally and start looking again*/
	  if(!(*s2)) return(pointer-strlen(save_s2));
	  else s2 = save_s2;
	}

	return(NULL);
}

