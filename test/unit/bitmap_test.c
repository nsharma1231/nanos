#include <runtime.h>
#include <stdlib.h>
#define EXIT_FAILURE 1
#define EXIT_SUCCESS 0


/**
 *  Tests allocation of bitmap using allocate_bitmap 
 *  function.
 */
bitmap test_alloc(heap h) {
    bitmap b = allocate_bitmap(h, h, 4096);
    bitmap_foreach_set(b, i) {
        if (i) {
            msg_err("!!! allocation failed for bitmap\n");
            return NULL; 
        }
    }
    return b;
}

/**
 *  Tests cloning of bitmap using bitmap_clone 
 *  function.
 *  Also implicity tests bitmap_foreach_set
 */  
boolean test_clone(bitmap b) { 
    for (int i = 0; i < 20; i++) 
        bitmap_set(b, rand(), 1);
    bitmap b_cpy = bitmap_clone(b);
    // tests bits match in original bitmap
    bitmap_foreach_set(b, j) {
        // implicit test for bitmap_foreach_set
        if (!bitmap_get(b, j)) {
            msg_err("!!! foreach_set failed for bitmap\n");
            deallocate_bitmap(b_cpy);
            return false;
        }
        if (!bitmap_get(b_cpy, j)) {
            msg_err("!!! cloning failed for bitmap\n");
            deallocate_bitmap(b_cpy);
            return false;
        }
    }
    // tests bits match in clone of bitmap
    bitmap_foreach_set(b_cpy, j) {
        // implicit test for bitmap_foreach_set
        if (!bitmap_get(b_cpy, j)) {
            msg_err("!!! foreach_set failed for bitmap\n");
            deallocate_bitmap(b_cpy);
            return false;
        }
        if (!bitmap_get(b, j)) {
            msg_err("!!! cloning failed for bitmap\n");
            deallocate_bitmap(b_cpy);
            return false;
        }
    }
    deallocate_bitmap(b_cpy);
    return true;
}

/**
 *  Tests copying of bitmap using bitmap_copy
 *  function.
 *  Also implicity tests bitmap_foreach_set
 */ 
boolean test_copy(heap h, bitmap b) { 
    bitmap b_cpy = allocate_bitmap(h, h, 4096);
    for (int i = 0; i < 20; i++) {
        bitmap_set(b_cpy, rand(), 1);
        bitmap_set(b, rand(), 1);
    }
    bitmap_copy(b, b_cpy);
    // tests bits match in original bitmap
    bitmap_foreach_set(b, j) {
        // implicit test for bitmap_foreach_set
        if (!bitmap_get(b, j)) {
            msg_err("!!! foreach_set failed for bitmap\n");
            deallocate_bitmap(b_cpy);
            return false;
        }
        if (!bitmap_get(b_cpy, j)) {
            msg_err("!!! cloning failed for bitmap\n");
            deallocate_bitmap(b_cpy);
            return false;
        }
    }
    // tests bits match in copy of bitmap
    bitmap_foreach_set(b_cpy, j) {
        // implicit test for bitmap_foreach_set
        if (!bitmap_get(b_cpy, j)) {
            msg_err("!!! foreach_set failed for bitmap\n");
            deallocate_bitmap(b_cpy);
            return false;
        }
        if (!bitmap_get(b, j)) {
            msg_err("!!! cloning failed for bitmap\n");
            deallocate_bitmap(b_cpy);
            return false;
        }
    }
    deallocate_bitmap(b_cpy);
    return true;
}

/**
 *  Tests getting and setting bits of a bitmap using 
 *  bitmap_get and bitmap_set functions.
 */ 
boolean test_set_and_get(bitmap b) {
    u64 i = rand();
    bitmap_set(b, i, 1);
    if (!bitmap_get(b, i)) {
        msg_err("!!! set and get failed for bitmap\n");
        return false;
    }
    return true;
}

/**
 *  Tests wrapping and unwrapping of bitmap using 
 *  bitmap_wrap and bitmap_unwrap functions.
 */ 
boolean test_wrap(heap h) { 
    u64 map = rand(); // how to generate random 64 bit 
    bitmap b = bitmap_wrap(h, &map, 32);
    for (int i = 0; i < 32; i++) {
        if (((map & (1 << i)) && !bitmap_get(b, i)) || (!(map & (1 << i)) && bitmap_get(b, i))) {
            msg_err("!!! wrap failed for bitmap at %d : %d %d\n", i, (map & (1 << i)), bitmap_get(b, i));
            bitmap_unwrap(b);
            return false;
        }
    }
    bitmap_unwrap(b);
    return true;
}

/**
 *  Tests bitmap alloc and dealloc of bitmap using 
 *  bitmap_alloc_within_range and bitmap_dealloc functions.
 */ 
boolean test_bitmap_alloc(bitmap b, u64 start, u64 end, u64 nbits) {
    u64 first = bitmap_alloc_within_range(b, nbits, start, end);
    if ((first == INVALID_PHYSICAL) || !bitmap_dealloc(b, first, nbits)) {
        msg_err("!!! alloc range failed for bitmap\n");
        return false;
    }
    return true;
}

boolean basic_test()
{
    heap h = init_process_runtime();
    // tests bitmap allocate
    bitmap b = test_alloc(h);
    if (b == NULL) return false;
    
    // tests bitmap clone
    if (!test_clone(b)) return false;

    // tests bitmap copy
    if (!test_copy(h, b)) return false;

    // tests bitmap set then get
    if (!test_set_and_get(b)) return false;

    // tests bitmap wrap then unwrap
    if (!test_wrap(h)) return false;

    // tests bitmap alloc then bitmap alloc within range
    u64 nbits = rand() % (4096 + 1);
    // if (!test_bitmap_alloc(b, 0, 4096, nbits)) return false; 
    u64 start = rand();
    u64 end = rand() % (4096 + 1 - start) + start;
    nbits = rand() % ((end - start) + 1);
    if(!test_bitmap_alloc(b, start, end, nbits)) return false; // bitmap_dealloc error: bitmap 0x----------------, bit -1 is not aligned to order 25

    // deallocate bitmap
    deallocate_bitmap(b);
    return true;
}

int main(int argc, char **argv)
{
    if (!basic_test()) 
        goto fail;

    msg_debug("test passed\n");
    exit(EXIT_SUCCESS);
  fail:
    msg_err("test failed\n");
    exit(EXIT_FAILURE);
}



