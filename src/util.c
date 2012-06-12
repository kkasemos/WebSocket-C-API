#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "util.h"

void print_hex(const unsigned char *str, size_t size)
{
	size_t c = 0;
	while(c < size) {
		printf("0x%02x ", *(str + c));

		c++;

		if(0 == (c % 8))
			printf("\n");
	}

	/* print one more new line if the last line
	   has a number of hex not equal to 8 */
	if(c % 8)
		printf("\n");
}

const char *trim_space(char *str)
{
	char *start;
	char *cp = str;

	/* skip all leading spaces */
	while(*cp != '\0' && *cp == ' ')
		cp++;
  
	start = cp;

	/* copy the rest of string */
	while(*cp != '\0') {
		*str = *cp;
		str++; cp++;
	}
	*str = *cp;

 
	/* skip all spaces from the end */
	cp = str - 1;
	while(*cp == ' ')
		cp--;

	/* set null terminate next to the last non-space char */
	*(cp + 1) = '\0';

	return start;
}

const char *reduce_mulspace(char *str)
{
  
}

struct linkiter *linkiter_create(struct linklist *lnk)
{
	struct linkiter *iter = (struct linkiter*)malloc(sizeof(struct linkiter));
	iter->curr = lnk->head;
	
	return iter;
}

void linkiter_destory(struct linkiter **iter)
{
	free(iter);
	*iter = NULL;
}

void *linkiter_next(struct linkiter *iter)
{
	void *data = iter->curr->data;
	
	iter->curr = iter->curr->next;

	return data;
}

int linkiter_eol(struct linkiter *iter)
{
	return iter->curr == NULL;
}

struct linklist* linklist_create()
{
	struct linklist *lnk = (struct linklist*)malloc(sizeof(struct linklist));
	
	lnk->size = 0;
	lnk->head = (struct linknode*)0;
	lnk->tail = (struct linknode*)0;

	return lnk;
}

void linklist_add(struct linklist *lnk, void *data)
{
	struct linknode *node = 
		(struct linknode*)malloc(sizeof(struct linknode));

	linknode_init(node);

	node->data = data;
        
	/* if the linklist is empty, set the new node as head and tail */
	if(linklist_is_empty(lnk))
		lnk->head = lnk->tail = node;
	else
	{
		/* add the new node to the tail */
		node->prev = lnk->tail;
		lnk->tail->next = node;
		lnk->tail = node;
	}
	
	lnk->size++;
}

void linknode_init(struct linknode *node)
{
	node->data = (void*)0;
	node->next = (struct linknode*)0;
	node->prev = (struct linknode*)0;
}

int linklist_is_empty(struct linklist *lnk)
{
	return lnk->size == 0;
}
		
