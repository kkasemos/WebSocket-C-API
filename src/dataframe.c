#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dataframe.h"

#define MAX_FRM_HEADER_LEN 12
#define ONE_BYTE_OFFSET    1
#define TWO_BYTES_OFFSET   (ONE_BYTE_OFFSET * 2)
#define THREE_BYTES_OFFSET (ONE_BYTE_OFFSET * 3)
#define FOUR_BYTES_OFFSET  (ONE_BYTE_OFFSET * 4)

unsigned char *datfrm_create_txt(const unsigned char *buff, size_t bsize, unsigned char mask, size_t *flen)
{
	unsigned char *frm;
	unsigned char rsv1, rsv2, rsv3;
	size_t nb; /* number of bytes used to encode the payload length */
	/* size_t flen =  */
	unsigned char mkey[MASK_KEY_LEN];
	unsigned char *pmk = mkey;

	/* allocate buffer for a frame as max length */
	*flen = (bsize + MAX_FRM_HEADER_LEN);
	frm = (unsigned char*)malloc(sizeof(unsigned char) * (*flen));
	memset(frm, 0, *flen);

	/* set FIN 1 bit, start at 0th bit */
	datfrm_set_fin(frm, FIN_ON);

	/* set RSV1, RSV2, RSV3 1 bit each, start at 1th bit */
	rsv1 = rsv2 = rsv3 = 0; /* the default must be 0 */
	datfrm_set_rsv(frm, rsv1, rsv2, rsv3);

	/* set opcode 4 bit, start at 4th bit */
	datfrm_set_opcode(frm, OPCODE_TEXT);

	/* set mask 1 bit, start at 8th bit */
	datfrm_set_mask(frm, mask);

	/* set payload length 7 bit, start at 9th bit */
	nb = datfrm_set_payload_len(frm, bsize);

	/* set mask key if enabled (4 bytes) */
	if(mask) {
		datfrm_gen_mask_key(pmk);
		datfrm_set_mask_key(frm, nb,  pmk);
		nb += FOUR_BYTES_OFFSET;
	} else {
		pmk = (void*)0;
	}

	/* set payload */
	datfrm_set_payload(frm, nb, buff, bsize, pmk);
	
	return frm;
}

const unsigned char *datfrm_gen_mask_key(unsigned char *k)
{
	*k = 0x37;
	*(k + 1) = 0xfa;
	*(k + 2) = 0x21;
	*(k + 3) = 0x3d;	
}

void datfrm_set_mask_key(unsigned char *frm, size_t offset, const unsigned char *mkey)
{
	unsigned char *p = frm + TWO_BYTES_OFFSET + offset; 
	
       	*p = 0x37;
	*(p + 1) = 0xfa;
	*(p + 2) = 0x21;
	*(p + 3) = 0x3d;
}

const unsigned char *datfrm_unmask_payload(unsigned char *pl, size_t psize, const unsigned char *mkey) {
	return datfrm_mask_payload(pl, psize, mkey);
}
const unsigned char *datfrm_mask_payload(unsigned char *pl, size_t psize, const unsigned char *mkey)
{
//	unsigned char *p = frm + offset;
	unsigned char *p = pl;
	unsigned int i, j;

	i = j = 0;
	while(i < psize) {
		j = i % MASK_KEY_LEN;
		*p = *p ^ *(mkey + j);
		i++; p++;
	}

	return p;
}

const unsigned char *datfrm_get_payload(const unsigned char *frm, unsigned char mask, size_t plen, unsigned char *buff) {
	unsigned char *bp = buff;
	const unsigned char *fp = frm;
	int i;

	if(plen < 126)
		fp += TWO_BYTES_OFFSET;
	else if(plen == 126)
		fp += FOUR_BYTES_OFFSET;
	else
		; /* not support yet */

	if(mask) {
		/* skip mask key */
		fp += MASK_KEY_LEN;
	}

	for(i = 0; i < plen; i++) {
		*bp = *fp;
		bp++; fp++;
	}
}

size_t datfrm_set_payload(unsigned char *frm, size_t offset, const unsigned char *buff, size_t bsize, const unsigned char *mkey)
{
	unsigned char *p = frm + TWO_BYTES_OFFSET + offset;
	
	memcpy(p, buff, bsize);
	if(mkey) {
		//datfrm_mask_payload(p, 0, bsize, mkey);
		datfrm_mask_payload(p, bsize, mkey);
	}

	return bsize;
}

size_t datfrm_get_payload_len(unsigned char *frm)
{
	size_t len; 

	len = (size_t)(*(frm + ONE_BYTE_OFFSET) & 0x7f); 
	if(len < 126)
		return len;
	else if(len == 126) {
		len = *(frm + TWO_BYTES_OFFSET);
		len = (len << 8) | *(frm + THREE_BYTES_OFFSET);
	} else {
		/* not yet supported */
	}

	return len;
}

size_t datfrm_set_payload_len(unsigned char *frm, size_t plen)
{
	unsigned char *p = frm + ONE_BYTE_OFFSET;
	size_t nb; /* number of bytes used to encode the payload length */

	if(plen < 126) {
		*p = (*p | (unsigned char)plen);
		nb = 0;
	}
	else if(plen > 126 && plen <= 0xffff) {
		/* the following 2 bytes interpreted as a 16-bit unsigned int */
		*p = *p | (unsigned char)126;
		//printf("size of size_t = %x\n", (plen & 0xffff) >> 8);
		*(p + ONE_BYTE_OFFSET) = (unsigned char)((plen & 0xffff) >> 8);
		*(p + TWO_BYTES_OFFSET) = (unsigned char)(plen & 0xff);
		nb = 2;
	} else {
		/* the following 8 bytes interpreted as a 64-bit unsigned int */
		*p = *p | (unsigned char)127;
		*(p + ONE_BYTE_OFFSET) = (unsigned char)0;
		nb = 8;
	}
	//printf("%d\n", datfrm_get_payload_len(frm));	
	//return datfrm_get_payload_len(frm);
	return nb;
}

unsigned char datfrm_get_mask(const unsigned char *frm)
{
	return (*(frm + ONE_BYTE_OFFSET) & 0x80) > 0;
}

void datfrm_set_mask(unsigned char *frm, unsigned char on)
{
	unsigned char *p = frm + ONE_BYTE_OFFSET;

	if(on)
		*p = *p | (on << 7);
	else
		*p = *p & ((unsigned char)~on >> 1);
}

void datfrm_get_mask_key(const unsigned char *frm, size_t plen, unsigned char *mkey) {
	const unsigned char *p = frm + TWO_BYTES_OFFSET;
	int i;

	if(plen < 126)
		;
	else if(plen == 126)
		p += TWO_BYTES_OFFSET;
	else
		; /* not support yet */

	for(i = 0; i < MASK_KEY_LEN; i++) {
		*mkey = *p;
		mkey++; p++;
	}
}

void datfrm_set_fin(unsigned char *frm, unsigned char on)
{
	if(on)
		*frm = *frm | (on << 7);
	else
		*frm = *frm & ((unsigned char)~on >> 1);
}

unsigned char datfrm_get_fin(const unsigned char *frm)
{
	return (*frm & 0x80) > 0;
}

void datfrm_set_rsv(unsigned char *frm, unsigned char rsv1, 
		    unsigned char rsv2, unsigned char rsv3)
{
	*frm &= 0x8f;
}

void datfrm_set_opcode(unsigned char *frm, unsigned char opcode)
{
	*frm |= opcode; 
}

unsigned char datfrm_get_opcode(const unsigned char *frm)
{
	return (*frm & 0x0f);
}
