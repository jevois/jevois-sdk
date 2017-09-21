#include"stack.h"
#include"cryptlib.h"
#include"objects.h"

#undef MIN_NODES
#define MIN_NODES	4


STACK *sk_new_null(void)
{
	
	return sk_new((int (*)(const char * const *, const char * const *))0);
}

STACK *sk_new(int (*c)(const char * const *, const char * const *))
{
	STACK *ret;
	int i;
	
	if ((ret=(STACK *)OPENSSL_malloc(sizeof(STACK))) == NULL)
		goto err;
	if ((ret->data=(char **)OPENSSL_malloc(sizeof(char *)*MIN_NODES)) == NULL)
		goto err;
	for (i=0; i<MIN_NODES; i++)
		ret->data[i]=NULL;
	ret->comp=c;
	ret->num_alloc=MIN_NODES;
	ret->num=0;
	ret->sorted=0;
	return(ret);
err:
	if(ret)
		OPENSSL_free(ret);
	return(NULL);
}


int sk_insert(STACK *st, char *data, int loc)
	{
	char **s;

	if(st == NULL) return 0;
	if (st->num_alloc <= st->num+1)
		{
		s=(char **)OPENSSL_realloc((char *)st->data,
			(unsigned int)sizeof(char *)*st->num_alloc*2);
		if (s == NULL)
			return(0);
		st->data=s;
		st->num_alloc*=2;
		}
	if ((loc >= (int)st->num) || (loc < 0))
		st->data[st->num]=data;
	else
		{
		int i;
		char **f,**t;

		f=(char **)st->data;
		t=(char **)&(st->data[1]);
		for (i=st->num; i>=loc; i--)
			t[i]=f[i];

		st->data[loc]=data;
		}
	st->num++;
	st->sorted=0;
	return(st->num);
	}


int sk_push(STACK *st, char *data)
	{
		
	return(sk_insert(st,data,st->num));
	}


void sk_free(STACK *st)
	{
		
	if (st == NULL) return;
	if (st->data != NULL) OPENSSL_free(st->data);
	OPENSSL_free(st);
	}


void sk_pop_free(STACK *st, void (*func)(void *))
	{
	/*int i;
	if (st == NULL) return;
	for (i=0; i<st->num; i++)
		if (st->data[i] != NULL)
			func(st->data[i]);
	sk_free(st);*/
	}

