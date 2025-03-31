#ifndef __WIREFRAME_H__
#define __WIREFRAME_H__



template<typename T>
struct Array {
    T *lst = NULL;
    u32 len = 0;
    u32 max = 0;

    inline
    void Add(T *element) {
        assert(len < max);

        lst[len++] = *element;
    }
    inline
    T *Add(T element) {
        assert(len < max);

        lst[len++] = element;
        return LastPtr();
    }
    inline
    T *AddUnique(T element) {
        assert(len < max);

        for (u32 i = 0; i < len; ++i) {
            if (lst[i] == element) {
                return NULL;
            }
        }
        return Add(element);
    }
    inline
    void Push(T element) {
        assert(len < max);

        lst[len++] = element;
    }
    inline
    T Pop() {
        if (len) {
            return lst[--len];
        }
        else {
            return {};
        }
    }
    inline
    T Last() {
        if (len) {
            return lst[len - 1];
        }
        else {
            return {};
        }
    }
    inline
    T *LastPtr() {
        if (len) {
            return lst + len - 1;
        }
        else {
            return NULL;
        }
    }
    inline
    T First() {
        if (len) {
            return lst[0];
        }
        else {
            return {};
        }
    }
    inline
    void Delete(u32 idx) {
        assert(idx < len);

        T swap = Last();
        len--;
        lst[len] = lst[idx];
        lst[idx] = swap;
    }
};
template<class T>
Array<T> InitList(MArena *a, u32 max_len) {
    Array<T> _lst;
    _lst.len = 0;
    _lst.max = max_len;
    _lst.lst = (T*) ArenaAlloc(a, sizeof(T) * max_len);
    return _lst;
}


#endif