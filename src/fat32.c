#include "lib-header/stdtype.h"
#include "lib-header/fat32.h"
#include "lib-header/stdmem.h"

// struct FAT32DriverState fat32_driver_state = {
//     .fat_table = {
//         .cluster_map = {0},
//     },
//     .dir_table_buf = {
//         .table = {0},
//     },
//     .cluster_buf = {
//         .buf = {0},
//     },
// };

static struct FAT32DriverState fat32_driver_state;
static struct FAT32DirectoryTable dir_table;
const uint8_t fs_signature[BLOCK_SIZE] = {
    'R', 'e', 'd', 'S', 't', 'a', 'r', 'O', 'S', 'K', 'W', ' ', ' ', ' ', ' ',  ' ',
    'D', 'e', 's', 'i', 'g', 'n', 'e', 'd', ' ', 'b', 'y', ' ', ' ', ' ', ' ',  ' ',
    'O', 'S', ' ', 'E', 'Z', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',  ' ',
    'M', 'a', 'd', 'e', ' ', 'w', 'i', 't', 'h', ' ', '<', '3', ' ', ' ', ' ',  ' ',
    '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '2', '0', '2', '3', '\n',
    [BLOCK_SIZE-2] = 'O',
    [BLOCK_SIZE-1] = 'k',
};

/**
 * Initialize file system driver state, if is_empty_storage() then create_fat32()
 * Else, read and cache entire FileAllocationTable (located at cluster number 1) into driver state
 */
void initialize_filesystem_fat32(void) {
    if (is_empty_storage()) {
        create_fat32();
    } else {
        read_clusters(fat32_driver_state.fat_table.cluster_map, 1, 1);
    }
}

/**
 * Checking whether filesystem signature is missing or not in boot sector
 * 
 * @return True if memcmp(boot_sector, fs_signature) returning inequality
 */
bool is_empty_storage(void){
    uint8_t boot_sector[BLOCK_SIZE];
    read_blocks(boot_sector, 0, 1);
    return memcmp(boot_sector, fs_signature, BLOCK_SIZE);
}

/**
 * Create new FAT32 file system. Will write fs_signature into boot sector and 
 * proper FileAllocationTable (contain CLUSTER_0_VALUE, CLUSTER_1_VALUE, 
 * and initialized root directory) into cluster number 1
 */
void create_fat32(void){
    uint8_t boot_sector[BLOCK_SIZE];
    memset(boot_sector, 0, BLOCK_SIZE);
    memcpy(boot_sector, fs_signature, BLOCK_SIZE);
    write_blocks(boot_sector, 0, 1);

    fat32_driver_state.fat_table.cluster_map[0] = CLUSTER_0_VALUE;
    fat32_driver_state.fat_table.cluster_map[1] = CLUSTER_1_VALUE;
    write_clusters(&fat32_driver_state.fat_table.cluster_map, 1, 1);

    struct FAT32DirectoryTable root_dir_table = {0};
    init_directory_table(&root_dir_table, "ROOT", 1);
    write_clusters(&root_dir_table, 2, 1);
}

/**
 * Read cluster operation, wrapper for read_blocks().
 * Recommended to use struct ClusterBuffer
 * 
 * @param ptr            Pointer to buffer for reading
 * @param cluster_number Cluster number to read
 * @param cluster_count  Cluster count to read, due limitation of read_blocks block_count 255 => max cluster_count = 63
 */
void read_clusters(void *ptr, uint32_t cluster_number, uint8_t cluster_count){
    read_blocks(ptr, cluster_number * 4, cluster_count * CLUSTER_BLOCK_COUNT);
}

/**
 * Write cluster operation, wrapper for write_blocks().
 * Recommended to use struct ClusterBuffer
 * 
 * @param ptr            Pointer to source data
 * @param cluster_number Cluster number to write
 * @param cluster_count  Cluster count to write, due limitation of write_blocks block_count 255 => max cluster_count = 63
 */
void write_clusters(const void *ptr, uint32_t cluster_number, uint8_t cluster_count){
    write_blocks(ptr, cluster_number * 4, cluster_count * CLUSTER_BLOCK_COUNT);
}

/**
 * Initialize DirectoryTable value with parent DirectoryEntry and directory name
 * 
 * @param dir_table          Pointer to directory table
 * @param name               8-byte char for directory name
 * @param parent_dir_cluster Parent directory cluster number
 */
void init_directory_table(struct FAT32DirectoryTable *dir_table, char *name, uint32_t parent_dir_cluster) {
    dir_table->table[0].name[0] = '.';
    dir_table->table[0].cluster_high = (uint16_t)(parent_dir_cluster >> 16);
    dir_table->table[0].cluster_low = (uint16_t)(parent_dir_cluster & 0xFFFF);

    dir_table->table[1].name[0] = '.';
    dir_table->table[1].name[1] = '.';
    dir_table->table[1].cluster_high = (uint16_t)(parent_dir_cluster >> 16);
    dir_table->table[1].cluster_low = (uint16_t)(parent_dir_cluster & 0xFFFF);

    for (uint8_t i = 0; i < 8; i++) {
        dir_table->table[2].name[i] = name[i];
    }
    dir_table->table[2].cluster_high = 0;
    dir_table->table[2].cluster_low = 2;
}

/**
 * FAT32 read, read a file from file system.
 *
 * @param request All attribute will be used for read, buffer_size will limit reading count
 * @return Error code: 0 success - 1 not a file - 2 not enough buffer - 3 not found - -1 unknown
 */
int8_t read(struct FAT32DriverRequest request)
{
    int8_t err;
    struct FAT32DirectoryEntry *t = request.buf;
    if (request.buffer_size<t->filesize)
    {
        err = 1; // ini gatau return apa antara 1/-1
    }
    else if (t->attribute == 1)
    {
        err = -1; // ini juga
    }
    else
    {
        if (request.buf == FAT32_FAT_END_OF_FILE)
        {
            return 0;
        }
        for (int i = 0 ; i < 128 ; i ++)
        {
            if (dir_table.table[i].name == t->name && dir_table.table[i].ext == t->ext)
            {
                request.buf = &dir_table.table[i];
                read(request);
                return 0;
            }
        }
        return 2;
    }
}
