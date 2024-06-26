/* crypto/bn/bn_print.c */
/* Copyright (C) 1995-1998 Eric Young (eay@cryptsoft.com)
 * All rights reserved.
 *
 * This package is an SSL implementation written
 * by Eric Young (eay@cryptsoft.com).
 * The implementation was written so as to conform with Netscapes SSL.
 *
 * This library is free for commercial and non-commercial use as long as
 * the following conditions are aheared to.  The following conditions
 * apply to all code found in this distribution, be it the RC4, RSA,
 * lhash, DES, etc., code; not just the SSL code.  The SSL documentation
 * included with this distribution is covered by the same copyright terms
 * except that the holder is Tim Hudson (tjh@cryptsoft.com).
 *
 * Copyright remains Eric Young's, and as such any Copyright notices in
 * the code are not to be removed.
 * If this package is used in a product, Eric Young should be given attribution
 * as the author of the parts of the library used.
 * This can be in the form of a textual message at program startup or
 * in documentation (online or textual) provided with the package.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *    "This product includes cryptographic software written by
 *     Eric Young (eay@cryptsoft.com)"
 *    The word 'cryptographic' can be left out if the rouines from the library
 *    being used are not cryptographic related :-).
 * 4. If you include any Windows specific code (or a derivative thereof) from
 *    the apps directory (application code) you must include an acknowledgement:
 *    "This product includes software written by Tim Hudson (tjh@cryptsoft.com)"
 *
 * THIS SOFTWARE IS PROVIDED BY ERIC YOUNG ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * The licence and distribution terms for any publically available version or
 * derivative of this code cannot be changed.  i.e. this code cannot simply be
 * copied and put under another distribution licence
 * [including the GNU Public Licence.]
 */

#include "cryptlib.h"
#include <openssl/buffer.h>
#include "bn_lcl.h"

extern int BIO_snprintf(char *buf, size_t n, const char *format, ...);

static const char Hex[]="0123456789ABCDEF";

/* Must 'OPENSSL_free' the returned data */

/* Must 'OPENSSL_free' the returned data */
char *BN_bn2dec(const BIGNUM *a)
	{
	int i=0,num, ok = 0;
	char *buf=NULL;
	char *p;
	BIGNUM *t=NULL;
	BN_ULONG *bn_data=NULL,*lp;

	/* get an upper bound for the length of the decimal integer
	 * num <= (BN_num_bits(a) + 1) * log(2)
	 *     <= 3 * BN_num_bits(a) * 0.1001 + log(2) + 1     (rounding error)
	 *     <= BN_num_bits(a)/10 + BN_num_bits/1000 + 1 + 1
	 */
	i=BN_num_bits(a)*3;
	num=(i/10+i/1000+1)+1;
	bn_data=(BN_ULONG *)OPENSSL_malloc((num/BN_DEC_NUM+1)*sizeof(BN_ULONG));
	buf=(char *)OPENSSL_malloc(num+3);
	if ((buf == NULL) || (bn_data == NULL))
		{
		goto err;
		}
	if ((t=BN_dup(a)) == NULL) goto err;

#define BUF_REMAIN (num+3 - (size_t)(p - buf))
	p=buf;
	lp=bn_data;
	if (BN_is_zero(t))
		{
		*(p++)='0';
		*(p++)='\0';
		}
	else
		{
		if (BN_is_negative(t))
			*p++ = '-';

		i=0;
		while (!BN_is_zero(t))
			{
			*lp=BN_div_word(t,BN_DEC_CONV);
			lp++;
			}
		lp--;
		/* We now have a series of blocks, BN_DEC_NUM chars
		 * in length, where the last one needs truncation.
		 * The blocks need to be reversed in order. */
		BIO_snprintf(p,BUF_REMAIN,BN_DEC_FMT1,*lp);
		while (*p) p++;
		while (lp != bn_data)
			{
			lp--;
			BIO_snprintf(p,BUF_REMAIN,BN_DEC_FMT2,*lp);
			while (*p) p++;
			}
		}
	ok = 1;
err:
	if (bn_data != NULL) OPENSSL_free(bn_data);
	if (t != NULL) BN_free(t);
	if (!ok && buf)
		{
		OPENSSL_free(buf);
		buf = NULL;
		}

	return(buf);
	}





