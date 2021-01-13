extern "C" {
#include <string.h>
#include <stdio.h>
#include <ctype.h>
}
#include "muddle.h"

int macro::match(char *str, char **array, int *len)
{
	int i = 0, j = 0, last, n;
	int l = strlen(str);
	int l2 = strlen(key);

	if(!l || !l2) return 0;

	// strip header white space
	while(i < l && str[i] == ' ') i++;
	while(j < l2 && str[j] == ' ') j++;

	// interpret macro including %wildcards
	while(i < l && j < l2) {
		// Handle non-wildcard comparisons, and %%->% translation
		while(i < l && j < l2 && str[i] == key[j]) { 
			if(key[j] == '%' && key[j+1] == '%') j++;
			i++; j++;
		}

		// Do we already have a match?
		if(i == l && j == l2) return 1;

		// No match, so it better be a % wildcard symbol.
		if(key[j++] != '%') return 0;

		// Make sure the %n symbol is such that n is a number
		// from 1 to 9 or a *.
		if((key[j+1] != ' ' && key[j+1] != 0) || 
		    ((key[j] < '1' || key[j] > '9') && key[j] != '*'))
			return 0;

		// If it's a *, use array location 0 to store wildcard info
		// and return success.
		if(key[j] == '*') {
			array[0] = &str[i];
			len[0] = l-i;
			return 1;
		}

		// Get macro # from ascii value and store wildcard info
		// in that #'s wildcard spot.
		n = key[j] - '0';
		array[n] = &str[i];
		j++; last = i;
		while(str[i] != ' ' && str[i] != 0) i++;
		len[n] = i-last;
	}

	// If there's nothing left, we have a match.
	return(i == l && j == l2);
}

void macro::out(int cur, char **array, int *len)
{
	int i=0, j=0, n=cur;
	int l = strlen(output);
	char outstr[512];
	extern conn *Conn[];

	// Interpret the macro, and output it to the appropriate descriptor.
	while(i < l) {

		// Check for %%, $$, and ^^ combinations.  Output a single
		// one if it is found.
		if((output[i] == '%' || output[i] == '$' ||
		    output[i] == '^') && output[i+1] == output[i]) {
			outstr[j++] = output[i];
			i+=2;
			continue;
		}

		// Handle control characters.
		if(output[i] == '^') {
			outstr[j++] = output[++i] - 'A' + 1;
			i++;
			continue;
		}

		// Handle wildcards.
		else if(output[i] == '%') {
			i++;
			if(isdigit(output[i])) {
				int d = output[i] - '0';
				if(d < 1 || !array[d])
					{ i++; continue; }
				for(int k=0; k<len[d]; k++)
					outstr[j++] = array[d][k];
			}
			if(output[i] == '*') {
				if(!array[0]) { i++; continue; }
				for(int k=0; k<len[0]; k++)
					outstr[j++] = array[0][k];
			}
		}

		// Handle multi-descriptor outputs.
		else if(output[i] == '$') {
			i++;
			if((output[i] == '0' || !isdigit(output[i])) &&
			    output[i] != '*')
				{ i++; continue; }
			int d;
			if(output[i] == '*') d = 0;
			else d = output[i] - '0';
			if(!Conn[d]) { i++; continue; }
			outstr[j] = 0;
			if(!n) for(int k=9; k>0; k--) {
				if(Conn[k]) Conn[k]->send(outstr);
				if(k == cur) printf(outstr);
			}
			else {
				Conn[n]->send(outstr);
				if(n == cur) printf(outstr);
			}
			j=0; n = d;
		}

		// Just a regular character
		else outstr[j++] = output[i];
		i++;
	}

	// Wrap things up by outputting the last string.
	if(j) {
		outstr[j] = 0;
		if(!n) for(int k=9; k>0; k--) {
			if(Conn[k]) Conn[k]->send(outstr);
			if(k==cur) printf(outstr);
		}
		else {
			Conn[n]->send(outstr);
			if(n == cur) printf(outstr);
		}
	}
}

int auton::match(char *str, int slot)
{
	int matchslot = -1;
	char *matchstr = key;

	if(matchstr[0] == '$') {
		if(matchstr[1] == '$') matchstr++;
		else {
			matchslot = matchstr[1] - '0';
			matchstr += 2;
		}
	}

	if(matchslot > -1 && matchslot != slot)
		return 0;

	if(strstr(str, matchstr))
		return 1;

	return 0;
}

void auton::out(int cur, int slot)
{
	int i=0, j=0, n=slot;
	int l = strlen(output);
	char outstr[512];
	extern conn *Conn[];

	// Interpret the macro, and output it to the appropriate descriptor.
	while(i < l) {

		// Check for $$ and ^^ combinations.  Output a single
		// one if it is found.
		if((output[i] == '$' || output[i] == '^') &&
		   output[i+1] == output[i]) {
			outstr[j++] = output[i];
			i+=2;
			continue;
		}

		// Handle control characters.
		if(output[i] == '^') {
			outstr[j++] = output[++i] - 'A' + 1;
			i++;
			continue;
		}

		// Handle multi-descriptor outputs.
		else if(output[i] == '$') {
			i++;
			if((output[i] == '0' || !isdigit(output[i])) &&
			    output[i] != '*')
				{ i++; continue; }
			int d;
			if(output[i] == '*') d = 0;
			else d = output[i] - '0';
			if(!Conn[d]) { i++; continue; }
			outstr[j] = 0;
			if(!n) for(int k=9; k>0; k--) {
				if(Conn[k]) Conn[k]->send(outstr);
				if(k == cur) printf(outstr);
			}
			else {
				Conn[n]->send(outstr);
				if(n == cur) printf(outstr);
			}
			j=0; n = d;
		}

		// Just a regular character
		else outstr[j++] = output[i];
		i++;
	}

	// Wrap things up by outputting the last string.
	if(j) {
		outstr[j] = 0;
		if(!n) for(int k=9; k>0; k--) {
			if(Conn[k]) Conn[k]->send(outstr);
			if(k==cur) printf(outstr);
		}
		else {
			Conn[n]->send(outstr);
			if(n == cur) printf(outstr);
		}
	}
}

