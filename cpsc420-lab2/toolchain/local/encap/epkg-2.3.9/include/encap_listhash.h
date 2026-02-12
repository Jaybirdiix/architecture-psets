/* listhash/encap_listhash.h.  Generated from listhash.h.in by configure. */

/*
**  Copyright 1998-2002 University of Illinois Board of Trustees
**  Copyright 1998-2002 Mark D. Roth
**  All rights reserved.
**
**  encap_listhash.h - header file for listhash module
**
**  Mark D. Roth <roth@uiuc.edu>
**  Campus Information Technologies and Educational Services
**  University of Illinois at Urbana-Champaign
*/

#ifndef encap_LISTHASH_H
#define encap_LISTHASH_H


/***** list.c **********************************************************/

/*
** Comparison function (used to determine order of elements in a list)
** returns less than, equal to, or greater than 0
** if data1 is less than, equal to, or greater than data2
*/
typedef int (*encap_cmpfunc_t)(void *, void *);

/*
** Free function (for freeing allocated memory in each element)
*/
typedef void (*encap_freefunc_t)(void *);

/*
** Plugin function for encap_list_iterate()
*/
typedef int (*encap_iterate_func_t)(void *, void *);

/*
** Matching function (used to find elements in a list)
** first argument is the data to search for
** second argument is the list element it's being compared to
** returns 0 if no match is found, non-zero otherwise
*/
typedef int (*encap_matchfunc_t)(void *, void *);


struct encap_node
{
	void *data;
	struct encap_node *next;
	struct encap_node *prev;
};
typedef struct encap_node *encap_listptr_t;

struct encap_list
{
	encap_listptr_t first;
	encap_listptr_t last;
	encap_cmpfunc_t cmpfunc;
	int flags;
	unsigned int nents;
};
typedef struct encap_list encap_list_t;


/* values for flags */
#define LIST_USERFUNC	0	/* use cmpfunc() to order */
#define LIST_STACK	1	/* new elements go in front */
#define LIST_QUEUE	2	/* new elements go at the end */


/* reset a list pointer */
void encap_listptr_reset(encap_listptr_t *);

/* retrieve the data being pointed to */
void *encap_listptr_data(encap_listptr_t *);

/* creates a new, empty list */
encap_list_t *encap_list_new(int, encap_cmpfunc_t);

/* call a function for every element in a list */
int encap_list_iterate(encap_list_t *,
				   encap_iterate_func_t, void *);

/* empty the list */
void encap_list_empty(encap_list_t *,
				  encap_freefunc_t);

/* remove and free() the entire list */
void encap_list_free(encap_list_t *,
				 encap_freefunc_t);

/* add elements */
int encap_list_add(encap_list_t *, void *);

/* removes an element from the list - returns -1 on error */
void encap_list_del(encap_list_t *,
				encap_listptr_t *);

/* returns 1 when valid data is returned, or 0 at end of list */
int encap_list_next(encap_list_t *,
				encap_listptr_t *);

/* returns 1 when valid data is returned, or 0 at end of list */
int encap_list_prev(encap_list_t *,
				encap_listptr_t *);

/* return 1 if the data matches a list entry, 0 otherwise */
int encap_list_search(encap_list_t *,
				  encap_listptr_t *, void *,
				  encap_matchfunc_t);

/* return number of elements from list */
unsigned int encap_list_nents(encap_list_t *);

/* adds elements from a string delimited by delim */
int encap_list_add_str(encap_list_t *, char *, char *);

/* string matching function */
int encap_str_match(char *, char *);


/***** hash.c **********************************************************/

/*
** Hashing function (determines which bucket the given key hashes into)
** first argument is the key to hash
** second argument is the total number of buckets
** returns the bucket number
*/
typedef unsigned int (*encap_hashfunc_t)(void *, unsigned int);


struct encap_hashptr
{
	int bucket;
	encap_listptr_t node;
};
typedef struct encap_hashptr encap_hashptr_t;

struct encap_hash
{
	int numbuckets;
	encap_list_t **table;
	encap_hashfunc_t hashfunc;
	unsigned int nents;
};
typedef struct encap_hash encap_hash_t;


/* reset a hash pointer */
void encap_hashptr_reset(encap_hashptr_t *);

/* retrieve the data being pointed to */
void *encap_hashptr_data(encap_hashptr_t *);

/* default hash function, optimized for 7-bit strings */
unsigned int encap_str_hashfunc(char *, unsigned int);

/* return number of elements from hash */
unsigned int encap_hash_nents(encap_hash_t *);

/* create a new hash */
encap_hash_t *encap_hash_new(int, encap_hashfunc_t);

/* empty the hash */
void encap_hash_empty(encap_hash_t *,
				  encap_freefunc_t);

/* delete all the encap_nodes of the hash and clean up */
void encap_hash_free(encap_hash_t *,
				 encap_freefunc_t);

/* returns 1 when valid data is returned, or 0 at end of list */
int encap_hash_next(encap_hash_t *,
				encap_hashptr_t *);

/* return 1 if the data matches a list entry, 0 otherwise */
int encap_hash_search(encap_hash_t *,
				  encap_hashptr_t *, void *,
				  encap_matchfunc_t);

/* return 1 if the key matches a list entry, 0 otherwise */
int encap_hash_getkey(encap_hash_t *,
				  encap_hashptr_t *, void *,
				  encap_matchfunc_t);

/* inserting data */
int encap_hash_add(encap_hash_t *, void *);

/* delete an entry */
int encap_hash_del(encap_hash_t *,
			       encap_hashptr_t *);

#endif /* ! encap_LISTHASH_H */

