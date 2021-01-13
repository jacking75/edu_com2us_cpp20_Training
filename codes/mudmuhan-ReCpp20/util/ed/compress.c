/*
 * COMPRESS.C:
 *
 *	Routines to do LZW compression on the player data.
 *
 *	Copyright (C) 1992 Brett J. Vickers
 *
 */

#ifdef COMPRESS

#include <stdio.h>
#include "mstruct.h"
#include "mextern.h"

#define FALSE    	0
#define TRUE     	!FALSE
#define TABSIZE  	4096
#define STACKSIZE	TABSIZE
#define NO_PRED  	0xFFFF
#define EMPTY    	0xFFFF
#define NOT_FND  	0xFFFF
#define UEOF		((unsigned)EOF)
#define UPDATE		TRUE
#define NO_UPDATE	FALSE

static struct entry {
	char used;
	unsigned int next;
	unsigned int predecessor;
	unsigned char follower;
} string_tab[TABSIZE];

static char		*start_inbuf, *start_outbuf;
static char		*inbuf, *outbuf;
static int		sizeof_inbuf, sizeof_outbuf;
static unsigned int	temp_inbuf;
static unsigned int	temp_outbuf;
static char 		stack[STACKSIZE];
static int		sp;

unsigned int	h(), eolist(), hash(), unhash(), getcode(), readc(), writec();
void		init_tab(), upd_tab(), flushout(), putcode(), push();
int		pop(), compress(), uncompress();

/* h uses the 'mid-square' algorithm. I.E. for a hash val of n bits     */
/* hash = middle binary digits of (key * key).  Upon collision, hash    */
/* searches down linked list of keys that hashed to that key already.   */
/* It will NOT notice if the table is full. This must be handled        */
/* elsewhere                                                            */

unsigned int h(pred,foll)
unsigned int pred; 
unsigned char foll;
{
	long temp, local;

	local = (pred + foll) | 0x0800;
	temp = local * local;
	local = (temp >> 6) & 0x0FFF;
	return local;
}

/* return last element in a collision list                              */
unsigned int eolist(index)
unsigned int index;
{
	register int temp;

	while((temp = string_tab[index].next) != 0)
		index = temp;
	return index;
}

unsigned int hash(pred, foll, update)
unsigned int pred; 
unsigned char foll; 
int update;
{
	unsigned int local, tempnext;
	struct entry *ep;

	local = h(pred,foll);
	if(!string_tab[local].used)
		return local;
	else {
		local = eolist(local);
  		tempnext = (local + 101) & 0x0FFF; 
		ep = &string_tab[tempnext];
		while (ep->used) {
			++tempnext;
			if(tempnext == TABSIZE) {
				tempnext = 0;
				ep = string_tab;
			}
			else
				ep++;
		}
    
		if (update)
			string_tab[local].next = tempnext;
		return tempnext;
	} 
}

/* unhash uses the 'next' field to go down the collision tree to find   */
/* the entry corresponding to the passed key                            */
/* passed key and returns either the matching entry # or NOT_FND        */

unsigned int unhash(pred, foll)
unsigned int pred;
unsigned char foll;
{
	unsigned int local, offset;
	struct entry *ep;

	local = h(pred,foll);
    loop:
	ep = &string_tab[local];
	if((ep->predecessor == pred) && (ep->follower == foll)) 
		return local;
	if(!ep->next)
		return NOT_FND;
	local = ep->next;
	goto loop;
}

void init_tab()
{
	int i;

	temp_outbuf = temp_inbuf = EMPTY;
	sp = 0;
	memset((char *)string_tab, 0, sizeof(string_tab));
	for(i=0; i<256; i++)
		upd_tab(NO_PRED,i);
}

void upd_tab(pred, foll)
unsigned int pred, foll;
{
	struct entry *ep;

	ep = &string_tab[hash(pred,foll,UPDATE)];
	ep->used = TRUE;
	ep->next = 0;
	ep->predecessor = pred;
	ep->follower = foll;
}

/* getcode and putcode 'gallop' through input and output - they either  */
/* output two bytes or one depending on the contents of the buffer.     */

unsigned int getcode()
{
	unsigned int localbuf, returnval;

	if(temp_inbuf == EMPTY) {
		if((localbuf = readc()) == UEOF)
			return UEOF;
		localbuf &= 0xFF;

		if((temp_inbuf = readc()) == UEOF)
			return UEOF;
		temp_inbuf &= 0xFF;

		returnval = ((localbuf << 4) & 0xFF0) | 
			    ((temp_inbuf >> 4) & 0x00F);
		temp_inbuf &= 0x000F;
	}

	else {
		if((localbuf = readc()) == UEOF)
			return UEOF;
		localbuf &= 0xFF;

		returnval = localbuf | ((temp_inbuf << 8) & 0xF00);
		temp_inbuf = EMPTY;
  	}

	return returnval;
}

void putcode(code)
unsigned int code;
{
	unsigned int localbuf;

	if(temp_outbuf == EMPTY) {
		writec((code >> 4) & 0xFF);
		temp_outbuf = code & 0x000F;
	}

	else {
		writec(((temp_outbuf << 4) & 0xFF0) | ((code >> 8) & 0x00F));
		writec(code & 0x00FF);
		temp_outbuf = EMPTY;
	}
}

unsigned int readc()
{
	if(inbuf - start_inbuf >= sizeof_inbuf)
		return(UEOF);
	else
		return((*(inbuf++) & 0xFF));
}

unsigned int writec(ch)
int ch;
{
	*outbuf = (char)ch;
	outbuf++;
	sizeof_outbuf++;
}

/* flushout makes sure fractional output buffer gets written */

void flushout()
{
	if(temp_outbuf != EMPTY) {
		*outbuf = (temp_outbuf << 4) & 0xFF0;
		outbuf++;
		sizeof_outbuf++;
	}
}

/* Compress routines */

int compress(in, out, size)
char 	*in;
char 	*out;
int 	size;
{
	unsigned int 	c, code, localcode;
	int 		code_count = TABSIZE - 256;
	
	start_inbuf  = inbuf  = in;
	start_outbuf = outbuf = out;
	sizeof_inbuf = size;
	sizeof_outbuf = 0;

	init_tab();
	c = readc();
	code = unhash(NO_PRED, c);

	while((c = readc()) != UEOF) {
		if((localcode = unhash(code, c)) != NOT_FND) {
			code = localcode;
			continue;
		}

		putcode(code);
		if(code_count) {
			upd_tab(code, c);
			code_count--;
		}

		code = unhash(NO_PRED, c);
	}

	putcode(code);
	flushout();

	return sizeof_outbuf;
}

/* Uncompression routines */

int uncompress(in, out, size)
char	*in;
char	*out;
int	size;
{
	unsigned int	c, tempc, code, oldcode, incode, finchar, lastchar;
	char		unknown = FALSE;
	int		code_count = TABSIZE - 256;
	struct entry	*ep;

	start_inbuf  = inbuf  = in;
	start_outbuf = outbuf = out;
	sizeof_inbuf = size;
	sizeof_outbuf = 0;

	init_tab();
	code = oldcode = getcode();

	c = string_tab[code].follower;
	writec(c);
	finchar = c;

	while ((code = incode = getcode()) != UEOF) {
		ep = &string_tab[code];
		if(!ep->used) {
			lastchar = finchar;
			code = oldcode;
			unknown = TRUE;
			ep = &string_tab[code];
		}

		while(ep->predecessor != NO_PRED) {
			push(ep->follower);
			code = ep->predecessor;
			ep = &string_tab[code];
		}

		finchar = ep->follower;
		writec(finchar);
		while((tempc = pop()) != EMPTY)
			writec(tempc);

		if(unknown) {
			writec(finchar = lastchar);
			unknown = FALSE;
		}
		if(code_count) {
			upd_tab(oldcode,finchar);
			code_count--;
		}
		oldcode = incode;
	}
	flushout();

	return sizeof_outbuf;
}

void push(c)
int c;
{
	stack[sp] = (char) c;
	sp++;
	if(sp >= STACKSIZE)
		merror(FATAL, "Stack overflow");
}

int pop() 
{
	if(sp > 0) {
		sp--;
		return((int)stack[sp]);
	}
	else
		return EMPTY;
}

#endif
