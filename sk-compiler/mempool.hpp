/* -*- coding:shift_jis -*- */

#ifndef MEMPOOL_HPP
#define MEMPOOL_HPP

#include <memory>

namespace copy_kawaii {

enum mem_type {
#define DEFINE_MEMTYPE(name) MEM_##name,
#include "memtype.def"
#undef DEFINE_MEMTYPE
    NUM_MEM_TYPE
};

extern const char *memtype_name[];

#ifdef GATHER_STAT
#define MEMPOOL_MT_ARG     , enum mem_type mt
#else
#define MEMPOOL_MT_ARG
#endif


/**
 * @file
 * @brief メモリアロケータ
 *
 * 関連するオブジェクトをまとめて解放したい場合に使うアロケータ
 */

/**
 * @brief メモリアロケータ
 */
struct mempool {
    unsigned int entry_num;
    unsigned int entry_index;
    unsigned int entry_byte_size;
    unsigned int entry_byte_remain;
    unsigned int entry_byte_pos;
    unsigned char **data_entry;

    unsigned int large_num;
    unsigned int large_index;
    unsigned char **large;

#ifdef GATHER_STAT
    int alloc_total;
    int unused_mem;
    int align_padding;
    int size_each_type[NUM_MEM_TYPE];
#endif
};

/**
 * @brief create mempool
 */
extern void mempool_init(struct mempool *p,
                         unsigned int size_hint);

/**
 * @brief delete mempool
 */
extern void mempool_destroy(struct mempool *p);

/**
 * @brief allocate memory from pool.
 *        memory is aligend to `2^align'.
 * 
 * @param p pool object
 * @param align_shift align
 * @param size object size
 * @return allocated memory
 */
extern void *mempool_alloc_align_stat(struct mempool *p,
                                      unsigned int align_shift,
                                      unsigned int size
                                      MEMPOOL_MT_ARG
);

#ifdef GATHER_STAT
#define mempool_alloc_align(p,a,s,t) mempool_alloc_align_stat(p,a,s,t)
#else
#define mempool_alloc_align(p,a,s,t) ((void)(t),mempool_alloc_align_stat(p,a,s))
#endif

/**
 * @brief allocate memory from pool.
 *        memory is aligned to 8.
 * @param p pool
 * @param s size
 * @return allocated memory
 */
#define mempool_alloc(p,s,t) (mempool_alloc_align(p, 3, s, t))

extern char *mempool_strdup(struct mempool *p,
                            const char *str
                            MEMPOOL_MT_ARG
);

extern void *mempool_copy(struct mempool *p,
                          void *data,
                          int data_len);

struct MemPool {
    struct mempool pool;
    MemPool(int hint = 1024) {
        mempool_init(&pool, hint);
    }
    ~MemPool();
    void reset(int hint = 1024);

    operator mempool *() {
        return &pool;
    }

    template <typename T>
    T *alloc_no_ctor(int num_elem, enum mem_type mt = MEM_OTHER, unsigned int align_shift = 3) {
        void *p = mempool_alloc_align(&this->pool, align_shift, sizeof(T) * num_elem, mt);
        return reinterpret_cast<T*>(p);
    }

    void *alloc_bytes(size_t b, unsigned int alloc_shift=3, enum mem_type mt = MEM_OTHER) {
        void *p = mempool_alloc_align(&this->pool, alloc_shift, b, mt);
        return p;
    }

    /* warning : Destructor of object that allocated by mempool is not called */
    template <typename T>
    T *create(int num_elem, enum mem_type mt = MEM_OTHER, unsigned int align_shift = 3) {
        void *p = mempool_alloc_align(&this->pool, align_shift, sizeof(T) * num_elem, mt);
        int i;

        T* ret = reinterpret_cast<T*>(p);
        for (i=0; i<num_elem; i++) {
            new ((void*) &ret[i]) T();
        }
        return ret;
    }


    template <typename T>
    T *alloc1(enum mem_type mt = MEM_OTHER) {
        void *p = mempool_alloc(&this->pool, sizeof(T), mt);
        new (p) T();
        return (T*)p;
    }

    template <typename T>
    T *alloc1(T const &v, enum mem_type mt = MEM_OTHER) {
        void *p = mempool_alloc(&this->pool, sizeof(T), mt);
        new (p) T(v);
        return (T*)p;
    }

    template <typename T>
    T *allocN(T const &v, int num_elem, enum mem_type mt = MEM_OTHER) {
        T *p = (T*)mempool_alloc(&this->pool, sizeof(T)*num_elem, mt);
        for (int i=0; i<num_elem; i++) {
            new ((void*)&p[i]) T(v);
        }
        return p;
    }


    template <typename T>
    typename T::value_type *copy(T const &vec, enum mem_type mt = MEM_OTHER, unsigned int align_shift = 3) {
        typedef typename T::value_type VT;
        int n = vec.size();
        VT *ret = alloc_no_ctor<VT>(n, mt, align_shift);
        for (int i=0; i<n; i++) {
            new ((void*)&ret[i]) VT(vec[i]);
        }
        return ret;
    }

    template <typename InputIterator>
    typename InputIterator::value_type *copy(InputIterator start,
                                             InputIterator end,
                                             enum mem_type mt = MEM_OTHER,
                                             unsigned int align_shift = 3)
    {
        typedef typename InputIterator::value_type VT;
        int n = end - start;
        VT *ret = alloc_no_ctor<VT>(n, mt, align_shift);
        int i=0;

        while (start != end) {
            new ((void*)&ret[i]) VT(*start);
            ++i;
            ++start;
        }
        return ret;
    }

#ifdef GATHER_STAT
    size_t alloc_total() const { return pool.alloc_total; }
    size_t unused_mem() const { return pool.unused_mem; }
    size_t align_padding() const { return pool.align_padding; }
    size_t size_each_type(int idx) const { return pool.size_each_type[idx]; }
#else
    size_t alloc_total() const { return 0; }
    size_t unused_mem() const { return 0; }
    size_t align_padding() const { return 0; }
    size_t size_each_type(int) const { return 0; }
#endif

private:
    MemPool(MemPool const &){}
};

void dump_pool_stat(FILE *fp, MemPool &pool);

}

#endif
