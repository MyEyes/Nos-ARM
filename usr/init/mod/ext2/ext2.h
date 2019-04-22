#include <types.h>

typedef struct {
    uint32_t    inodes_count;
    uint32_t    blocks_count;
    uint32_t    r_blocks_count;
    uint32_t    free_blocks_count;
    uint32_t    free_inodes_count;
    uint32_t    first_data_block;
    uint32_t    log_block_size;
    uint32_t    log_frag_size;
    uint32_t    blocks_per_group;
    uint32_t    frags_per_group;
    uint32_t    inodes_per_group;
    uint32_t    mtime;
    uint32_t    wtime;
    uint32_t    mnt_count;
    uint32_t    max_mnt_count;
    uint32_t    magic;
    uint32_t    state;
    uint32_t    errors;
    uint32_t    minor_rev_level;
    uint32_t    lastcheck;
    uint32_t    checkinterval;
    uint32_t    creator_os;
    uint32_t    rev_level;
    uint32_t    def_resuid;
    uint32_t    def_resgid;
} ext2_super_block_t;


