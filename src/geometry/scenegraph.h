#ifndef __SCENEGRAPH_H__
#define __SCENEGRAPH_H__


struct Transform;


static MPool g_p_sgnodes;
static void *g_sg_root_addr;


struct Transform {
    Matrix4f t_loc;
    Matrix4f t_world;
    u16 next;
    u16 first;
    u16 parent;
    u16 index;

    inline
    Transform *Next() {
        return (Transform*) PoolIdx2Ptr(&g_p_sgnodes, next);
    }

    inline
    Transform *First() {
        return (Transform*) PoolIdx2Ptr(&g_p_sgnodes, first);
    }

    inline
    Transform *Parent() {
        if (parent) {
            return (Transform*) PoolIdx2Ptr(&g_p_sgnodes, parent);
        }
        else{
            return (Transform*) g_sg_root_addr;
        }
    }

    void AppendChild(Transform *c) {
        if (first == 0) {
            first = c->index;
        }
        else {
            Transform *n = First();
            while (n->next) {
                n = n->Next();
            }
            n->next = c->index;
        }
        c->parent = index;
    }

    void RemoveChild(Transform *t) {
        if (first == t->index) {
            first = t->next;
        }
        else {
            // find prev
            Transform *prev = NULL;
            Transform *c = First();

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


static Transform g_sg_root;


void SceneGraphInit(s32 cap = 256) {
    assert(g_p_sgnodes.mem == NULL);

    g_p_sgnodes = PoolCreate(sizeof(Transform), cap + 1);
    // lock index-0:
    PoolAlloc(&g_p_sgnodes);

    //  The root node is both the zero-stub, the tree root node, and the object of index 0.
    //  Thus every zero-initialized node is already has root as its parent

    g_sg_root.t_loc = Matrix4f_Identity();
    g_sg_root.t_world = Matrix4f_Identity();

    // the address of the root node (necessary due to laguage quirks)
    g_sg_root_addr = &g_sg_root;
}

Transform *SceneGraphAlloc(Transform *parent = NULL) {
    assert(g_p_sgnodes.mem && "Initialize first");

    Transform *t = (Transform*) PoolAlloc(&g_p_sgnodes);
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

void SceneGraphFree(Transform *t) {
    assert(g_p_sgnodes.mem && "Initialize first");

    t->Parent()->RemoveChild(t);

    // relinquish child branches -> to root
    Transform *c = t->First();
    Transform *nxt = c;
    while (nxt) {
        c = nxt;
        nxt = c->Next();

        c->next = 0;
        g_sg_root.AppendChild(c);
    }

    PoolFree(&g_p_sgnodes, t);
}

void SGUpdateRec(Transform *t, Transform *p) {
    while (t) {
        t->t_world = p->t_world * t->t_loc;

        // iterate children
        if (t->first) {
            SGUpdateRec(t->First(), t);
        }

        // iterate siblings
        t = t->Next();
    }
}

void SceneGraphUpdate() {
    assert(g_p_sgnodes.mem && "Initialize first");

    Transform *r = &g_sg_root;

    // initialize the starting point
    r->t_world = r->t_loc;

    // walk the tree
    if (r->first) {
        SGUpdateRec(r->First(), r);
    }
}


#endif
