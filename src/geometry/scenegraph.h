#ifndef __SCENEGRAPH_H__
#define __SCENEGRAPH_H__


struct SGNode;


static MPool g_p_sgnodes;
static void *g_sg_root_addr;


struct SGNode {
    Matrix4f t_loc;
    Matrix4f t_world;
    u16 next;
    u16 first;
    u16 parent;
    u16 index;

    inline
    SGNode *Next() {
        return (SGNode *) PoolIdx2Ptr(&g_p_sgnodes, next);
    }

    inline
    SGNode *First() {
        return (SGNode *) PoolIdx2Ptr(&g_p_sgnodes, first);
    }

    inline
    SGNode *Parent() {
        if (parent) {
            return (SGNode *) PoolIdx2Ptr(&g_p_sgnodes, parent);
        }
        else{
            return (SGNode*) g_sg_root_addr;
        }
    }

    void AppendChild(SGNode *c) {
        if (first == 0) {
            first = c->index;
        }
        else {
            SGNode *n = First();
            while (n->next) {
                n = n->Next();
            }
            n->next = c->index;
        }
        c->parent = index;
    }

    void RemoveChild(SGNode *t) {
        if (first == t->index) {
            first = t->next;
        }
        else {
            // find prev
            SGNode *prev = NULL;
            SGNode *c = First();

            while (c) {
                if (c->index == t->index && prev) {
                    assert(c == t);

                    prev->next = t->next;
                }
                else {
                    prev = c;
                    c = c->Next();
                }
            }
        }
    }
};


static SGNode g_sg_root;


void SceneGraphInit(s32 cap = 256) {
    assert(g_p_sgnodes.mem == NULL);

    g_p_sgnodes = PoolCreate(sizeof(SGNode), cap + 1);
    // lock index-0:
    PoolAlloc(&g_p_sgnodes);

    //  The root node is both the zero-stub, the tree root node, and the object of index 0.
    //  Thus every zero-initialized node is already has root as its parent

    g_sg_root.t_loc = Matrix4f_Identity();
    g_sg_root.t_world = Matrix4f_Identity();

    // the address of the root node (necessary due to laguage quirks)
    g_sg_root_addr = &g_sg_root;
}


SGNode *SceneGraphAlloc(SGNode *parent = NULL) {
    SGNode *t = (SGNode*) PoolAlloc(&g_p_sgnodes);
    t->index = (u16) PoolPtr2Idx(&g_p_sgnodes, t);
    t->t_loc = Matrix4f_Identity();
    t->t_world = Matrix4f_Identity();

    assert(t->index != 0);

    if (!parent) {
        parent = &g_sg_root;
    }
    parent->AppendChild(t);

    return t;
}


void SceneGraphFree(SGNode * t) {
    t->Parent()->RemoveChild(t);

    // relinquish child branches -> to root
    SGNode *c = t->First();
    SGNode *nxt = c;
    while (nxt) {
        c = nxt;
        nxt = c->Next();

        c->next = 0;
        g_sg_root.AppendChild(c);
    }

    PoolFree(&g_p_sgnodes, t);
}


#endif
