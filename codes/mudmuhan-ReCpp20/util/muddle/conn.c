extern "C" {
#include <stdio.h>
#include <string.h>
#include <signal.h>
}
#include "muddle.h"

int conn::update_buf(void)
{
	static char tempbuf[1024];
	extern auton *First_auton;
	extern int Target;

	int n = read(fd, tempbuf, 1024);
	if(!n) return -1;
	if(n<0) return 0;
	tempbuf[n] = 0;

	// Check for autons
	if(fd) for(auton *a=First_auton; a; a=a->next)
		if(a->match(tempbuf, slot))
			a->out(Target, slot);
	
	// If the circular buffer's right edge is to be overstepped...
	if(head+n >= bufsize) {
		if(head < bufsize)
			strncpy(&buf[head], tempbuf, bufsize-head);
		memcpy(buf, &tempbuf[bufsize-head], n-bufsize+head);
		int temp = head;
		head = (head + n) % bufsize;
		if(tail > temp || tail <= head)
			tail = head+1;
	}

	// Otherwise, just copy it in.
	else {
		memcpy(&buf[head], tempbuf, n);
		if(tail > head && tail <= (head+n))
			tail = head+n+1;
		head += n;
	}

	return 0;
}

void conn::output_buf(void)
{
	// If we don't have to wrap-around the buffer, just output
	if(head > tail) {
		write(1, &buf[tail], head-tail);
		tail = head;
	}

	// Otherwise, output in two parts.
	else if(head < tail) {
		write(1, &buf[tail], bufsize-tail);
		write(1, buf, head);
		tail = head;
	}
}

int conn::check_command(char *str)
{
	if(head == tail) return 0;

	int j=0;
	if(head > tail) {
		for(int i=tail; i<head; i++) {
			str[j++] = buf[i];
			if(buf[i] == '\n') {
				str[j] = 0;
				tail = i+1;
				return 1;
			}
		}
	}

	else {
		for(int i=tail; i<bufsize; i++) {
			str[j++] = buf[i];
			if(buf[i] == '\n') {
				str[j] = 0;
				tail = (i+1)%bufsize;
				return 1;
			}
		}

		for(i=0; i<head; i++) {
			str[j++] = buf[i];
			if(buf[i] == '\n') {
				str[j] = 0;
				tail = i+1;
				return 1;
			}
		}
	}
}
