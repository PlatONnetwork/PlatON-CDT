
#ifndef    _SLINUX_RBTREE_H
#define    _SLINUX_RBTREE_H

#ifdef _MSC_VER
struct __declspec(align(uintptr_t)) rb_node
{
    uintptr_t  rb_parent_color;
#define    RB_RED        0
#define    RB_BLACK    1
    struct rb_node *rb_right;
    struct rb_node *rb_left;
};
/* The alignment might seem pointless, but allegedly CRIS needs it */
#else
struct rb_node
{
    unsigned long  rb_parent_color;
#define    RB_RED        0
#define    RB_BLACK    1
    struct rb_node *rb_right;
    struct rb_node *rb_left;
}__attribute__((aligned(sizeof(long))));
/* The alignment might seem pointless, but allegedly CRIS needs it */
#endif

struct rb_root
{
    struct rb_node *rb_node;
};


#define rb_parent(r)   ((struct rb_node *)((r)->rb_parent_color & ~3))
#define rb_color(r)   ((r)->rb_parent_color & 1)
#define rb_is_red(r)   (!rb_color(r))
#define rb_is_black(r) rb_color(r)
#define rb_set_red(r)  do { (r)->rb_parent_color &= ~1; } while (0)
#define rb_set_black(r)  do { (r)->rb_parent_color |= 1; } while (0)

static inline void rb_set_parent(struct rb_node *rb, struct rb_node *p)
{
#ifdef _MSC_VER
    rb->rb_parent_color = (rb->rb_parent_color & 3) | (uintptr_t)p;
#else
    rb->rb_parent_color = (rb->rb_parent_color & 3) | (unsigned long)p;
#endif
}
static inline void rb_set_color(struct rb_node *rb, int color)
{
    rb->rb_parent_color = (rb->rb_parent_color & ~1) | color;
}

#define offsetof(TYPE, MEMBER) ((unsigned long long) &((TYPE *)0)->MEMBER)

#define container_of(ptr, type, member) ((type *)( (char *)ptr - offsetof(type,member) ))


#ifdef _MSC_VER
#define RB_ROOT {0}
#else
#define RB_ROOT (struct rb_root) { 0, }
#endif
#define    rb_entry(ptr, type, member) container_of(ptr, type, member)

#define RB_EMPTY_ROOT(root)    ((root)->rb_node == 0)
#define RB_EMPTY_NODE(node)    (rb_parent(node) == node)
#define RB_CLEAR_NODE(node)    (rb_set_parent(node, node))

static inline void rb_init_node(struct rb_node *rb)
{
    rb->rb_parent_color = 0;
    rb->rb_right = 0;
    rb->rb_left = 0;
    RB_CLEAR_NODE(rb);
}

void rb_insert_color(struct rb_node *, struct rb_root *);
void rb_erase(struct rb_node *, struct rb_root *);

typedef void(*rb_augment_f)(struct rb_node *node, void *data);

extern void rb_augment_insert(struct rb_node *node,
    rb_augment_f func, void *data);
extern struct rb_node *rb_augment_erase_begin(struct rb_node *node);
extern void rb_augment_erase_end(struct rb_node *node,
    rb_augment_f func, void *data);

/* Find logical next and previous nodes in a tree */
extern struct rb_node *rb_next(const struct rb_node *);
extern struct rb_node *rb_prev(const struct rb_node *);
extern struct rb_node *rb_first(const struct rb_root *);
extern struct rb_node *rb_last(const struct rb_root *);

/* Fast replacement of a single node without remove/rebalance/add/rebalance */
extern void rb_replace_node(struct rb_node *victim, struct rb_node *news,
    struct rb_root *root);

static inline void rb_link_node(struct rb_node * node, struct rb_node * parent,
    struct rb_node ** rb_link)
{
#if _MSC_VER
    node->rb_parent_color = (uintptr_t)parent;
#else
    node->rb_parent_color = (unsigned long)parent;
#endif
    node->rb_left = node->rb_right = 0;

    *rb_link = node;
}

#endif    /* _LINUX_RBTREE_H */
