#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <openssl/sha.h>
#include "acrypto.h"
#include "handshake.h"
#include "util.h"

#define WEBSCK_GUID              "258EAFA5-E914-47DA-95CA-C5AB0DC85B11"
#define SRV_HNDSHK_STATUS_LINE   "HTTP/1.1 101 Switching Protocols"
#define HNDSHK_FLD_UPGRADE       "Upgrade"
#define HNDSHK_FLD_CONNECT       "Connection"
#define HNDSHK_FLD_WEBSCK_KEY    "Sec-WebSocket-Key"
#define HNDSHK_FLD_WEBSCK_ACCEPT "Sec-WebSocket-Accept"

void hndshk_init(struct websck_hndshk *hndp)
{
	int i = 0;

	hndp->startline[0] = '\0';
	while(i < MAX_NUM_FLD) {
		hndp->fields[i].name[0] = '\0';
		hndp->fields[i].value[0] = '\0';
		i++;
	}
	hndp->numfld = 0;
}

int hndshk_cmp_fld(const struct hndshk_field *fld1, const struct hndshk_field *fld2)
{
	return (strcmp(fld1->name , fld2->name)  == 0 && 
		strcmp(fld1->value, fld2->value) == 0);  
}

int hndshk_cmp(const struct websck_hndshk *hnd1, const struct websck_hndshk *hnd2)
{
	const struct hndshk_field *fld1, *fld2;
	int equal;
	int i;

	if(strcmp(hnd1->startline, hnd2->startline) == 0 && 
	   hnd1->numfld == hnd2->numfld) {
		equal = 1;
		i = 0;

		fld1 = hnd1->fields;
		fld2 = hnd2->fields;
		while(i < hnd1->numfld && equal) {
			if(!hndshk_cmp_fld(fld1, fld2)) {
				equal = 0;
			}
			fld1++;
			fld2++;
			i++;
		}
	} else {
		equal = 0;
	}

	return equal;
} 

const unsigned char *hndshk_sprintf(unsigned char *str, struct websck_hndshk *hndp)
{
	int i = 0;
	int offset = 0;
	
	/* handshake start line */
	sprintf(str, "%s\r\n", hndp->startline);
	offset = strlen(str);

	/* handshake fields */
	while(i < hndp->numfld) {
		hndshk_fld_sprintf(str + offset, &hndp->fields[i]);
		offset = strlen(str);
		i++;
	}

	/* end of handshake */
	sprintf(str + offset, "\r\n");

	return str;
}

const unsigned char *hndshk_fld_sprintf(unsigned char *str, const struct hndshk_field *fld)
{
	sprintf(str, "%s: %s\r\n", fld->name, fld->value);
}

void hndshk_gen_srv(const struct websck_hndshk *clihnd, struct websck_hndshk *srvhnd)
{
	const struct hndshk_field *fld;
	unsigned char accept[MAX_FLD_LEN];
	int asize = 0;

	hndshk_init(srvhnd);

	strcpy(srvhnd->startline, SRV_HNDSHK_STATUS_LINE);
	
	fld = hndshk_get_fld(clihnd, HNDSHK_FLD_UPGRADE);
	if(fld) {
		hndshk_add_fld(srvhnd, fld->name, strlen(fld->name), 
			       fld->value, strlen(fld->value));
	}
	
	fld = hndshk_get_fld(clihnd, HNDSHK_FLD_CONNECT);
	if(fld) {
		hndshk_add_fld(srvhnd, fld->name, strlen(fld->name), 
			       fld->value, strlen(fld->value));	
	}

	fld = hndshk_get_fld(clihnd, HNDSHK_FLD_WEBSCK_KEY);
	if(fld) { 
		hndshk_gen_websck_accept(fld->value, strlen(fld->value), 
					 accept, &asize);
		hndshk_add_fld(srvhnd, HNDSHK_FLD_WEBSCK_ACCEPT, 
			       strlen(HNDSHK_FLD_WEBSCK_ACCEPT), 
			       accept, asize);
	}
}

const unsigned char *hndshk_gen_websck_accept(const unsigned char *key, int ksize, unsigned char *accept, int *asize)
{
	unsigned char ktmp[MAX_FVAL_LEN];
	unsigned char stmp[SHA_DIGEST_LENGTH];
	unsigned int  idlen = strlen(WEBSCK_GUID);
	const unsigned char *b64;

	/* concatenate the key with the GUID */
	strncpy(ktmp, key, ksize);
	strncpy(ktmp + ksize, WEBSCK_GUID, idlen);

	/* gen sha-1 from the concatenated key and WEBSCK_GUID */
	crypt_sha1(ktmp, ksize + idlen, stmp);

	/* gen base64 from the result of the sha-1 */
	b64 = crypt_base64(stmp, SHA_DIGEST_LENGTH, asize);
	strncpy(accept, b64, *asize);

	free((void*)b64);

	return accept;
}

int hndshk_validate_cli(struct websck_hndshk *hndp, struct hndshk_criteria *ctrp)
{
	return 1;
}		      

void hndshk_add_fld(struct websck_hndshk *hndp, const unsigned char *name, size_t nsize, const unsigned char *value, size_t vsize)
{
	int i = hndp->numfld;

	strncpy(hndp->fields[i].name, name, nsize);
	hndp->fields[i].name[nsize] = '\0';
	trim_space(hndp->fields[i].name);

	strncpy(hndp->fields[i].value, value, vsize);
	hndp->fields[i].value[vsize] = '\0';
	trim_space(hndp->fields[i].value);

	hndp->numfld++;
}

const struct hndshk_field *hndshk_get_fld(const struct websck_hndshk *hndp, const unsigned char *name)
{
	const struct hndshk_field *fld = NULL;
	int i = 0;

	while(i < hndp->numfld && fld == NULL) {
		if(strcmp(hndp->fields[i].name, name) == 0) {
			fld = &hndp->fields[i];
		}
		i++;
	}
	return fld;
}

void hndshk_set_startline(const unsigned char *line, size_t size, struct websck_hndshk *hndp)
{
	strncpy(hndp->startline, line, size);
	hndp->startline[size] = '\0';
}

void hndshk_parse(const unsigned char *buff, size_t bsize, struct websck_hndshk *hndp)
{
	const unsigned char *curr = buff;
	const unsigned char *end  = buff + bsize;
	const unsigned char *fld, *name, *val;
	size_t fsize, nsize, vsize;
	int i;

	hndshk_init(hndp);

	i = 0;
	while(curr < end) {
		/* parse field */
		fld = hndshk_parse_fld(curr, bsize, &fsize);

		/* the start line of HTTP header */
		if(0 == i) {
			hndshk_set_startline(fld, fsize, hndp);
		} else if(fsize > 0) {
			/* parse field name */
			name = hndshk_parse_fldname(fld, fsize, &nsize);
			if(nsize > 0) {
				/* parse field value */
				val = hndshk_parse_fldval(fld, fsize, &vsize);
				hndshk_add_fld(hndp, name, nsize, val, vsize);
			}
		} else {
			break;
		}
		curr = fld + fsize + 2;
		i++;
	}
}

const unsigned char *hndshk_parse_fld(const unsigned char *buff, size_t bsize, size_t *fsize)
{
	const unsigned char *curr = buff;
	const unsigned char *end  = buff + bsize;
	size_t nc;

	/* field is separated by '\r\n' */
	*fsize = nc = 0;
	while(curr < end) {
		if(*curr == '\r') {
			curr++;
			if(curr < end && *curr == '\n') {
				*fsize = nc;
				break;
			}
		}
		nc++;
		curr++;
	}

	return buff;
}

const unsigned char *hndshk_parse_fldname(const unsigned char *buff, size_t bsize, size_t *nsize)
{
	const unsigned char *curr = buff;
	const unsigned char *end  = buff + bsize;
	size_t nc;

	/* field name end with ':' */
	*nsize = nc = 0;
	while(curr < end && *curr != ':') {
		nc++;
		curr++;
	}

	if(curr < end)
		*nsize = nc;

	return buff;
} 

const unsigned char *hndshk_parse_fldval(const unsigned char *buff, size_t bsize, size_t *vsize)
{
	const unsigned char *curr = buff;
	const unsigned char *end  = buff + bsize;
	const char *pos = NULL;
	size_t nc;

	/* field value begin with ':' */
	*vsize = nc = 0;
	while(curr < end && *curr != ':')
		curr++;
	curr++;

	if(curr < end) {
		pos = curr;
  
		while(curr < end) {
			nc++;
			curr++;
		}
	}

	*vsize = nc;

	return pos;
}
