#ifndef __SCENEGRAPH_H__
#define __SCENEGRAPH_H__


struct Transform;


struct SceneGraphHandle {
    MPool pool;
    Transform *root;
};


struct Transform {
    Matrix4f t_loc;
    Matrix4f t_world;
    u16 next;
    u16 first;
    u16 parent;
    u16 index;

    inline
    Transform *Next(SceneGraphHandle *sg) {
        return (Transform*) PoolIdx2Ptr(&sg->pool, next);
    }

    inline
    Transform *First(SceneGraphHandle *sg) {
        return (Transform*) PoolIdx2Ptr(&sg->pool, first);
    }

    inline
    Transform *Parent(SceneGraphHandle *sg) {
        if (parent) {
            return (Transform*) PoolIdx2Ptr(&sg->pool, parent);
        }
        else{
            return sg->root;
        }
    }

    void AppendChild(SceneGraphHandle *sg, Transform *c) {
        if (first == 0) {
            first = c->index;
        }
        else {
            Transform *n = First(sg);
            while (n->next) {
                n = n->Next(sg);
            }
            n->next = c->index;
        }
        c->parent = index;
    }

    void RemoveChild(SceneGraphHandle *sg, Transform *t) {
        if (first == t->index) {
            first = t->next;
        }
        else {
            // find prev
            Transform *prev = NULL;
            Transform *c = First(sg);

            while (c) {
                if (c->index == t->index && prev) {
                    assert(c == t);

                    prev->next = t->next;
                }
                else {
                    prev = c;
                    c = c->Next(sg);
                }
            }
        }
    }
};


SceneGraphHandle SceneGraphInit(MArena *a_dest, s32 cap = 256) {
    SceneGraphHandle sg = {};

    sg.pool = PoolCreate(a_dest, sizeof(Transform), cap + 1);
    // root at index-0
    sg.root = (Transform*) PoolAlloc(&sg.pool);
    sg.root->t_loc = Matrix4f_Identity();
    sg.root->t_world = Matrix4f_Identity();

    return sg;
}

Transform *SceneGraphAlloc(SceneGraphHandle *sg, Transform *parent = NULL) {
    Transform *t = (Transform*) PoolAlloc(&sg->pool);
    t->index = (u16) PoolPtr2Idx(&sg->pool, t);
    t->t_loc = Matrix4f_Identity();
    t->t_world = Matrix4f_Identity();

    assert(t->index != 0);

    if (!parent) {
        parent = sg->root;
    }
    parent->AppendChild(sg, t);

    return t;
}

void SceneGraphFree(SceneGraphHandle *sg, Transform *t) {
    t->Parent(sg)->RemoveChild(sg, t);

    // relinquish child branches -> to root
    Transform *c = t->First(sg);
    Transform *nxt = c;
    while (nxt) {
        c = nxt;
        nxt = c->Next(sg);

        c->next = 0;
        sg->root->AppendChild(sg, c);
    }

    PoolFree(&sg->pool, t);
}

void SGUpdateRec(SceneGraphHandle *sg, Transform *t, Transform *p) {
    while (t) {
        t->t_world = p->t_world * t->t_loc;

        // iterate children
        if (t->first) {
            SGUpdateRec(sg, t->First(sg), t);
        }

        // iterate siblings
        t = t->Next(sg);
    }
}

void SceneGraphUpdate(SceneGraphHandle *sg) {
    Transform *r = sg->root;

    // initialize the starting point
    r->t_world = r->t_loc;

    // walk the tree
    if (r->first) {
        SGUpdateRec(sg, r->First(sg), r);
    }
}

void SceneGraphSetRotParent(SceneGraphHandle *sg, Transform *t, Transform *p_rot) {
    // p_rot is the rotational parent
    // t's parent pointer is the proper parent, whose translation is to be applied

    // we need the world matrices of p_rot and p:
    // (Because p_rot has an accumulated rotation above it, which we need to bake into our local matrix)
    // NOTE: Possibly, the rot-parent could be baked into the SceneGraphUpdate call, possibly.
    SceneGraphUpdate(sg);

    // our world translation matrix
    Vector3f our_w_transl_v3 = TransformGetTranslation(t->t_world);
    Matrix4f our_w_transl = TransformBuildTranslation(our_w_transl_v3);

    // our local rotation matrix
    Matrix4f our_l_rot = TransformSetTranslation(t->t_loc, { 0, 0, 0 });

    // the rotational parent's world (accumulated) rotation: Its world matrix with translation set to zero.
    Matrix4f prot_w_rot = p_rot->t_world;
    prot_w_rot = TransformSetTranslation(prot_w_rot, {0, 0, 0} );

    // our world rotation matrix
    Matrix4f our_w_rot = prot_w_rot * our_l_rot;

    // our world matrix is found by combining our translation and rotation matrices: 
    t->t_world = our_w_transl * our_w_rot;

    // recover our local matrix wrt. the primary "at-rel" parent (p, not p_rot)
    Matrix4f w_to_p = TransformGetInverse( t->Parent(sg)->t_world );
    t->t_loc = w_to_p * t->t_world;
}


#endif
