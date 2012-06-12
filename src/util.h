
struct linknode {
  void *data;
  struct linknode *next;
  struct linknode *prev;
};

struct linklist {
  struct linknode *head;
  struct linknode *tail;
  unsigned int size;
};

/* linked list iterator */
struct linkiter {
  struct linknode *curr;
};

void print_hex(const unsigned char *str, size_t size);
const char *trim_space(char *str);
const char *reduce_mulspace(char *str);
void linklist_add(struct linklist *lnk, void *data);
struct linklist *linklist_create();
struct linkiter *linkiter_create(struct linklist *lnk);
int linkiter_eol(struct linkiter *iter);
void *linkiter_next(struct linkiter *iter);
void linknode_init(struct linknode *node);
