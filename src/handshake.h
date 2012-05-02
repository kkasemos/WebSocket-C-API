#define MAX_FLD_LEN   512
#define MAX_FNAME_LEN 256
#define MAX_FVAL_LEN  256
#define MAX_NUM_FLD   32

struct hndshk_criteria {
  char websck_protocol[MAX_FVAL_LEN];
  char websck_version[MAX_FVAL_LEN];
  char host[MAX_FVAL_LEN];
  char origin[MAX_FVAL_LEN];
};

struct hndshk_field {
  unsigned char name[MAX_FNAME_LEN];
  unsigned char value[MAX_FVAL_LEN];
};

struct websck_hndshk {
  unsigned char       startline[MAX_FNAME_LEN + MAX_FVAL_LEN];
  struct hndshk_field fields[MAX_NUM_FLD];
  unsigned int        numfld;
};

const unsigned char *hndshk_fld_sprintf(unsigned char *str, const struct hndshk_field *fld);
const unsigned char *hndshk_sprintf(unsigned char *str, struct websck_hndshk *hndp);
void hndshk_add_fld(struct websck_hndshk *hndp, const unsigned char *name, size_t nsize, const unsigned char *value, size_t vsize);
void hndshk_gen_srv(const struct websck_hndshk *clihnd, struct websck_hndshk *srvhnd);
int hndshk_cmp(const struct websck_hndshk *hnd1, const struct websck_hndshk *hnd2);
const unsigned char *hndshk_gen_websck_accept(const unsigned char *key, int ksize, unsigned char *accept, int *asize);
void hndshk_init(struct websck_hndshk *hndp);
int hndshk_validate_cli(struct websck_hndshk *hndp, struct hndshk_criteria *ctrp);
const struct hndshk_field *hndshk_get_fld(const struct websck_hndshk *hndp, const unsigned char *name);
const unsigned char *hndshk_parse_fldname(const unsigned char *buff, size_t bsize, size_t *nsize);
const unsigned char *hndshk_parse_fldval(const unsigned char *buff, size_t bsize, size_t *vsize);
const unsigned char *hndshk_parse_fld(const unsigned char *buff, size_t bsize, size_t *fsize);
void hndshk_parse(const unsigned char *buff, size_t bsize, struct websck_hndshk *hndp);
