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

const uint8_t fs_signature[BLOCK_SIZE] = {
    'R', 'e', 'd', 'S', 't', 'a', 'r', 'O', 'S', 'K', 'W', ' ', ' ', ' ', ' ',  ' ',
    'D', 'e', 's', 'i', 'g', 'n', 'e', 'd', ' ', 'b', 'y', ' ', ' ', ' ', ' ',  ' ',
    'O', 'S', ' ', 'S', 'U', 'S', 'A', 'H', ' ', ' ', ' ', ' ', ' ', ' ', ' ',  ' ',
    'M', 'a', 'd', 'e', ' ', 'w', 'i', 't', 'h', ' ', '<', '3', ' ', ' ', ' ',  ' ',
    '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '2', '0', '2', '3', '\n',
    [BLOCK_SIZE-2] = ':',
    [BLOCK_SIZE-1] = '(',
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
    init_directory_table(&root_dir_table, "", 1);
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
    dir_table->table[0].cluster_high = (uint16_t)(parent_dir_cluster >> 16);
    dir_table->table[0].cluster_low = (uint16_t)(parent_dir_cluster & 0xFFFF);

    for (int i = 0; i < 8; i++) {
        dir_table->table[0].name[i] = name[i];
    }
    
    dir_table->table[0].attribute = ATTR_SUBDIRECTORY;
    dir_table->table[0].cluster_high = 0;
    dir_table->table[0].cluster_low = 2;
}

double ceil(double x) {
    int intPart = (int)x;
    return (x > intPart) ? (double)(intPart + 1) : (double)intPart;
}

/**
 * FAT32 read, read a file from file system.
 *
 * @param request All attribute will be used for read, buffer_size will limit reading count
//  * @return Error code: 0 success - 1 not a file - 2 not enough buffer - 3 not found - -1 unknown
//  */
int8_t read(struct FAT32DriverRequest request)
{
/*
Read akan mencari file yang terletak pada folder parent request.parent_cluster_number. DirectoryTable akan di-iterasi hingga ditemukan request.name dan request.ext yang sama.
Jika request.buffer_size kurang dari entry.filesize, kembalikan error. Jika flag subdirectory pada entry.attribute menyala (bit subdirectory bernilai 1), kembalikan error.
Jika kondisi sudah valid, lakukan pembacaan hingga semua cluster terbaca dan dimasukkan ke request.buf.
Untuk read_directory memiliki cara kerja yang sama, tetapi hanya menerima target yaitu sebuah folder.
 request.ext tidak digunakan pada read_directory. read_directory akan membaca DirectoryTable yang ada pada disk dan memasukkannya pada request.buf.
*/
    struct FAT32DirectoryEntry *t = request.buf;
    if (request.buffer_size<t->filesize)
    {
        return 2;
    }
    else if (t->attribute == 1)
    {
        return 1; // ini juga
    }
    else
    {
        for (int i = 0 ; i < 128 ; i ++)
        {
            if (fat32_driver_state.dir_table_buf.table[i].name[0] == 0)
            {
                return 3;
            }
            else if (memcmp(fat32_driver_state.dir_table_buf.table[i].name, request.name, 8) == 0 && memcmp(fat32_driver_state.dir_table_buf.table[i].ext, request.ext, 3) == 0)
            {
                uint32_t cluster = (fat32_driver_state.dir_table_buf.table[i].cluster_high << 16) | fat32_driver_state.dir_table_buf.table[i].cluster_low;
                uint32_t cluster_count = ceil((double)fat32_driver_state.dir_table_buf.table[i].filesize / (double)CLUSTER_SIZE);
                read_clusters(request.buf, cluster, cluster_count);
                return 0;
            }
        }
        return 2;
    }
    return -1;
}

/**
 * FAT32 write, write a file or folder to file system.
 *
 * @param request All attribute will be used for write, buffer_size == 0 then create a folder / directory
 * @return Error code: 0 success - 1 file/folder already exist - 2 invalid parent cluster - -1 unknown
 */
int8_t write(struct FAT32DriverRequest request)
{
/*
    Berikut merupakan deskripsi dari interface write:
    Behavior untuk write adalah first empty location found pada FileAllocationTable. Request wajib memberikan request.parent_cluster_number yang valid (cluster tersebut berisikan directory). 
    Jika directory valid, cek apakah request.name dan request.ext sudah ada atau belum (Tips: Hati-hati dengan komparasi Null Terminated String).
    Jika request.buffer_size bernilai 0, buatlah sub-direktori pada folder parent request.parent_cluster_number dengan nama request.name.
    Jika request.buffer_size lebih dari 0, write akan menuliskan data yang ditunjuk oleh request.buf hingga jumlah cluster yang ditulis = ceil(request.buffer_size / CLUSTER_SIZE).
    Parent folder DirectoryTable akan ditambahkan DirectoryEntry baru dengan nama yang sesuai dari entry.user_attribute = UATTR_NOT_EMPTY. Sesuaikan flags pada entry.attribute.
    Pastikan juga mengupdate cache yang ada pada memory pada FAT32DriverState.
    Style penulisan data ke storage dibebaskan, beberapa contoh:
    Mark first, write later (Defer penulisan, simpan informasi cluster kosong dan tulis diakhir)
    Single iteration & direct cluster writing (Seperti pada animasi)
*/

    // Validasi parrent cluster
    if (request.parent_cluster_number == 0)
    {
        return 2;
    }

    // Mencari first empty location found pada FileAllocationTable
    int index = 0;
    while (fat32_driver_state.fat_table.cluster_map[index] != 0)
    {
        index++;
    }

    // Cek apakah request.name dan request.ext sudah ada atau belum
    // for (int i = 0 ; i < 128 ; i++)
    // {
    //     if (memcmp(fat32_driver_state.dir_table_buf.table[i].name, request.name, 8) == 0)
    //     {
    //         return 1;
    //     }
    //     else if (memcmp(fat32_driver_state.dir_table_buf.table[i].ext, request.ext, 3) == 0)
    //     {
    //         return 1;
    //     }
    // }

    if (request.buffer_size == 0)
    {
        // membuat sub-direktori pada folder parent request.parent_cluster_number dengan nama request.name.   
        write_clusters(request.buf, index, 1);

        // memberi nama request.name
        memcpy(fat32_driver_state.dir_table_buf.table[index].name, request.name, 8);

        return 0;
    }
    
    else
    {
        int loop = ceil(request.buffer_size / CLUSTER_SIZE);
        for (int i = 0 ; i < loop ; i++)
        {
            write_clusters(request.buf, index+1+i, 1);
        }


        // Parent folder DirectoryTable akan ditambahkan DirectoryEntry baru dengan nama yang sesuai dari entry.user_attribute = UATTR_NOT_EMPTY. Sesuaikan flags pada entry.attribute.
        fat32_driver_state.dir_table_buf.table[0].user_attribute = UATTR_NOT_EMPTY;
        fat32_driver_state.dir_table_buf.table[0].attribute = ATTR_SUBDIRECTORY;

        // Pastikan juga mengupdate cache yang ada pada memory pada FAT32DriverState.
        fat32_driver_state.fat_table.cluster_map[index] = 0xFFFFFFFF;

        return 0;
    }

    return -1;
}

/**
 * FAT32 delete, delete a file or empty directory (only 1 DirectoryEntry) in file system.
 *
 * @param request buf and buffer_size is unused
 * @return Error code: 0 success - 1 not found - 2 folder is not empty - -1 unknown
 */
int8_t delete(struct FAT32DriverRequest request)
{
    // Delete akan menghapus sebuah file jika dan hanya jika request.name, request.ext, dan request.parent_cluster_number menunjuk kepada DirectoryEntry file yang valid 
    // (hasil pencarian tidak kosong). DirectoryEntry pada folder parent request.parent_cluster_number akan dihapus tanpa menggeser entry yang lain. Hapus semua linked list clusters yang ada pada FileAllocationTable. 
    // Data file fisik yang ada pada cluster tidak wajib untuk dihapus (di-overwrite dengan byte 0x00 misalnya). 
    // Entry khusus (indeks ke 0, pointer ke parent folder, dengan nama direktori sekarang) yang disebutkan pada desain file system, tidak dapat dihapus. Abaikan request yang ditemukan pada indeks ke 0 pada DirectoryTable.
    // Untuk folder, delete akan menghapus DirectoryTable hanya dan hanya jika tepat memiliki 1 entry (entry khusus) dan bukan root directory. Jika masih ada entry lain, kembalikan error. 

    // entry khusus
    if (request.parent_cluster_number == 0)
    {
        return 2;
    }
    else
    {
        // mencari file (request.name, request.ext, dan request.parent_cluster_number menunjuk kepada DirectoryEntry file yang valid )
        for (int i = 0 ; i < 128 ; i++)
        {
            if (memcmp(fat32_driver_state.dir_table_buf.table[i].name, request.name, 8) == 0)
            {
                if (memcmp(fat32_driver_state.dir_table_buf.table[i].ext, request.ext, 3) == 0)
                {
                    if (fat32_driver_state.dir_table_buf.table[i].cluster_low == request.parent_cluster_number)
                    {
                        // menghapus semua linked list clusters yang ada pada FileAllocationTable
                        int index = fat32_driver_state.dir_table_buf.table[i].cluster_low;
                        while (fat32_driver_state.fat_table.cluster_map[index] != 0xFFFFFFFF)
                        {
                            fat32_driver_state.fat_table.cluster_map[index] = 0;
                            index = fat32_driver_state.fat_table.cluster_map[index];
                        }
                        fat32_driver_state.fat_table.cluster_map[index] = 0;

                        // menghapus DirectoryTable
                        fat32_driver_state.dir_table_buf.table[i].name[0] = 0xE5;
                        return 0;
                    }
                }
            }
        }
    }
    return -1;
}