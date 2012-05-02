#define FIN_ON  1
#define FIN_OFF 0

#define MASK_ON      1
#define MASK_OFF     0
#define MASK_KEY_LEN 4

#define OPCODE_CONT  0x0
#define OPCODE_TEXT  0x1
#define OPCODE_BIN   0x2
#define OPCODE_CLOSE 0x3
#define OPCODE_PING  0x8
#define OPCODE_PONG  0xA

unsigned char *datfrm_create_txt(const unsigned char *buff, size_t bsize, unsigned char mask, size_t *flen);
const unsigned char *datfrm_create_bin(const unsigned char *buff, size_t bsize);
void datfrm_set_fin(unsigned char *frm, unsigned char on);
unsigned char datfrm_get_fin(const unsigned char *frm);
size_t datfrm_get_payload_len(unsigned char *frm);
size_t datfrm_set_payload_len(unsigned char *frm, size_t plen);
//size_t datfrm_set_payload(unsigned char *frm, size_t offset, const unsigned char *buff, size_t bsize);
unsigned char datfrm_get_mask(const unsigned char *frm);
void datfrm_set_mask(unsigned char *frm, unsigned char on);
void datfrm_set_rsv(unsigned char *frm, unsigned char rsv1, 
		    unsigned char rsv2, unsigned char rsv3);
void datfrm_set_opcode(unsigned char *frm, unsigned char opcode); 
unsigned char datfrm_get_opcode(const unsigned char *frm);
void datfrm_set_mask_key(unsigned char *frm, size_t offset, const unsigned char *mkey);
void datfrm_get_mask_key(const unsigned char *frm, size_t psize, unsigned char *mkey);
size_t datfrm_set_payload(unsigned char *frm, size_t offset, const unsigned char *buff, size_t bsize, const unsigned char *mkey);
const unsigned char *datfrm_get_payload(const unsigned char *frm, unsigned char mask, size_t plen, unsigned char *buff);
const unsigned char *datfrm_gen_mask_key(unsigned char *key);
//const unsigned char *datfrm_mask_payload(unsigned char *frm, size_t offset, size_t psize, const unsigned char *mkey);
const unsigned char *datfrm_mask_payload(unsigned char *pl, size_t psize, const unsigned char *mkey);
const unsigned char *datfrm_unmask_payload(unsigned char *pl, size_t psize, const unsigned char *mkey);

