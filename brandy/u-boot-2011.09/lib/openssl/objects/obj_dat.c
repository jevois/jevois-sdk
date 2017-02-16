#include"asn1.h"
#include"obj_dat.h"
#include"lhash.h"
#include"objects.h"
#include"err.h"
#include"cryptlib.h"
#include"limits.h"

#define ADDED_NID 3
#define ADDED_DATA  0


typedef struct added_obj_st
{
  int type;
  ASN1_OBJECT * obj;
} ADDED_OBJ;

static LHASH * added = NULL;

void reset_OBJ_nid2ln_reset (void)
{
  added = NULL;
}

const char * OBJ_nid2ln (int n)
{
  ADDED_OBJ ad, *adp;
  ASN1_OBJECT ob;
  
  if ( (n >= 0) && (n < NUM_NID) )
  {
    if ( (n != NID_undef) && (nid_objs[n].nid == NID_undef) )
    {
      return (NULL);
    }
    return (nid_objs[n].ln);
  }
  else
    if (added == NULL)
    { return (NULL); }
    else
    {
      ad.type = ADDED_NID;
      ad.obj = &ob;
      ob.nid = n;
      adp = (ADDED_OBJ *) lh_retrieve (added, &ad);
      if (adp != NULL)
      { return (adp->obj->ln); }
      else
      {
        return (NULL);
      }
    }
}

const char * OBJ_bsearch (const char * key, const char * base, int num, int size,
                          int (*cmp) (const void *, const void *) )
{

  return OBJ_bsearch_ex (key, base, num, size, cmp, 0);
}


const char * OBJ_bsearch_ex (const char * key, const char * base, int num,
                             int size, int (*cmp) (const void *, const void *), int flags)
{
  int l, h, i = 0, c = 0;
  const char * p = NULL;
  
  if (num == 0) { return (NULL); }
  l = 0;
  h = num;
  while (l < h)
  {
    i = (l + h) / 2;
    p = & (base[i * size]);
    c = (*cmp) (key, p);
    if (c < 0)
    { h = i; }
    else
      if (c > 0)
      { l = i + 1; }
      else
      { break; }
  }
  /* THIS IS A KLUDGE - Because the *_obj is sorted in ASCII order, and
   * I don't have perl (yet), we revert to a *LINEAR* search
   * when the object wasn't found in the binary search.
   */
  if (c != 0)
  {
    for (i = 0; i < num; ++i)
    {
      p = & (base[i * size]);
      c = (*cmp) (key, p);
      if (c == 0 || (c < 0 && (flags & OBJ_BSEARCH_VALUE_ON_NOMATCH) ) )
      { return p; }
    }
  }
  if (c != 0 && ! (flags & OBJ_BSEARCH_VALUE_ON_NOMATCH) )
  { p = NULL; }
  else
    if (c == 0 && (flags & OBJ_BSEARCH_FIRST_VALUE_ON_MATCH) )
    {
      while (i > 0 && (*cmp) (key, & (base[ (i - 1) *size]) ) == 0)
      { i--; }
      p = & (base[i * size]);
    }
  return (p);
}


static int obj_cmp (const void * ap, const void * bp)
{
  int j;
  const ASN1_OBJECT * a = * (ASN1_OBJECT * const *) ap;
  const ASN1_OBJECT * b = * (ASN1_OBJECT * const *) bp;
  
  j = (a->length - b->length);
  if (j) { return (j); }
  return (memcmp (a->data, b->data, a->length) );
}

int OBJ_obj2nid (const ASN1_OBJECT * a)
{
  ASN1_OBJECT ** op;
  ADDED_OBJ ad, *adp;
  
  if (a == NULL)
  { return (NID_undef); }
  if (a->nid != 0)
  { return (a->nid); }
  
  if (added != NULL)
  {
    ad.type = ADDED_DATA;
    ad.obj = (ASN1_OBJECT *) a; /* XXX: ugly but harmless */
    adp = (ADDED_OBJ *) lh_retrieve (added, &ad);
    if (adp != NULL) { return (adp->obj->nid); }
  }
  op = (ASN1_OBJECT **) OBJ_bsearch ( (const char *) &a, (const char *) obj_objs,
                                      NUM_OBJ, sizeof (ASN1_OBJECT *), obj_cmp);
  if (op == NULL)
  { return (NID_undef); }
  return ( (*op)->nid);
}



ASN1_OBJECT * OBJ_nid2obj (int n)
{
  ADDED_OBJ ad, *adp;
  ASN1_OBJECT ob;
  
  if ( (n >= 0) && (n < NUM_NID) )
  {
    if ( (n != NID_undef) && (nid_objs[n].nid == NID_undef) )
    {
    
      return (NULL);
    }
    return ( (ASN1_OBJECT *) & (nid_objs[n]) );
  }
  else
    if (added == NULL)
    { return (NULL); }
    else
    {
      ad.type = ADDED_NID;
      ad.obj = &ob;
      ob.nid = n;
      adp = (ADDED_OBJ *) lh_retrieve (added, &ad);
      if (adp != NULL)
      { return (adp->obj); }
      else
      {
        return (NULL);
      }
    }
}

const char * OBJ_nid2sn (int n)
{
  ADDED_OBJ ad, *adp;
  ASN1_OBJECT ob;
  
  if ( (n >= 0) && (n < NUM_NID) )
  {
    if ( (n != NID_undef) && (nid_objs[n].nid == NID_undef) )
    {
      OBJerr (OBJ_F_OBJ_NID2SN, OBJ_R_UNKNOWN_NID);
      return (NULL);
    }
    return (nid_objs[n].sn);
  }
  else
    if (added == NULL)
    { return (NULL); }
    else
    {
      ad.type = ADDED_NID;
      ad.obj = &ob;
      ob.nid = n;
      adp = lh_retrieve (added, &ad);
      if (adp != NULL)
      { return (adp->obj->sn); }
      else
      {
        return (NULL);
      }
    }
}


int OBJ_obj2txt (char * buf, int buf_len, const ASN1_OBJECT * a, int no_name)
{
  return -1;
}

int OBJ_obj2name (char * dst_buf, int buf_len, const ASN1_OBJECT * a)
{
  if (buf_len < a->length)
  {
    printf ("OBJ_obj2name err: not enough buffer to store name\n");
    
    return -1;
  }
  memcpy (dst_buf, a->data, a->length);
  
  return a->length;
}
