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
    'R',
    'e',
    'd',
    'S',
    't',
    'a',
    'r',
    'O',
    'S',
    'K',
    'W',
    ' ',
    ' ',
    ' ',
    ' ',
    ' ',
    'D',
    'e',
    's',
    'i',
    'g',
    'n',
    'e',
    'd',
    ' ',
    'b',
    'y',
    ' ',
    ' ',
    ' ',
    ' ',
    ' ',
    'O',
    'S',
    ' ',
    'S',
    'U',
    'S',
    'A',
    'H',
    ' ',
    ' ',
    ' ',
    ' ',
    ' ',
    ' ',
    ' ',
    ' ',
    'M',
    'a',
    'd',
    'e',
    ' ',
    'w',
    'i',
    't',
    'h',
    ' ',
    '<',
    '3',
    ' ',
    ' ',
    ' ',
    ' ',
    '-',
    '-',
    '-',
    '-',
    '-',
    '-',
    '-',
    '-',
    '-',
    '-',
    '-',
    '2',
    '0',
    '2',
    '3',
    '\n',
    [BLOCK_SIZE - 2] = ':',
    [BLOCK_SIZE - 1] = '(',
};

/**
 * Initialize file system driver state, if is_empty_storage() then create_fat32()
 * Else, read and cache entire FileAllocationTable (located at cluster number 1) into driver state
 */
void initialize_filesystem_fat32(void)
{
    if (is_empty_storage())
    {
        create_fat32();
    }
    else
    {
        read_clusters(&fat32_driver_state.fat_table.cluster_map, 1, 1);
    }
}

/**
 * Checking whether filesystem signature is missing or not in boot sector
 *
 * @return True if memcmp(boot_sector, fs_signature) returning inequality
 */
bool is_empty_storage(void)
{
    uint8_t boot_sector[BLOCK_SIZE];
    read_blocks(boot_sector, 0, 1);
    return memcmp(boot_sector, fs_signature, BLOCK_SIZE);
}

/**
 * Create new FAT32 file system. Will write fs_signature into boot sector and
 * proper FileAllocationTable (contain CLUSTER_0_VALUE, CLUSTER_1_VALUE,
 * and initialized root directory) into cluster number 1
 */
void create_fat32(void)
{
    uint8_t boot_sector[BLOCK_SIZE];
    memset(boot_sector, 0, BLOCK_SIZE);
    memcpy(boot_sector, fs_signature, BLOCK_SIZE);
    write_blocks(boot_sector, 0, 1);

    fat32_driver_state.fat_table.cluster_map[0] = CLUSTER_0_VALUE;
    fat32_driver_state.fat_table.cluster_map[1] = CLUSTER_1_VALUE;
    fat32_driver_state.fat_table.cluster_map[2] = FAT32_FAT_END_OF_FILE;
    write_clusters(&fat32_driver_state.fat_table.cluster_map, 1, 1);

    struct FAT32DirectoryTable root_dir_table = {0};
    init_directory_table(&root_dir_table, "ROOT\0\0\0", 2);
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
void read_clusters(void *ptr, uint32_t cluster_number, uint8_t cluster_count)
{
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
void write_clusters(const void *ptr, uint32_t cluster_number, uint8_t cluster_count)
{
    write_blocks(ptr, cluster_number * 4, cluster_count * CLUSTER_BLOCK_COUNT);
}

/**
 * Initialize DirectoryTable value with parent DirectoryEntry and directory name
 *
 * @param dir_table          Pointer to directory table
 * @param name               8-byte char for directory name
 * @param parent_dir_cluster Parent directory cluster number
 */
void init_directory_table(struct FAT32DirectoryTable *dir_table, char *name, uint32_t parent_dir_cluster)
{
    dir_table->table[0].cluster_high = (uint16_t)(parent_dir_cluster >> 16);
    dir_table->table[0].cluster_low = (uint16_t)(parent_dir_cluster & 0xFFFF);

    // name[8] = '\0';
    for (int i = 0; i < 8; i++)
    {
        dir_table->table[0].name[i] = name[i];
    }

    dir_table->table[0].attribute = ATTR_SUBDIRECTORY;
    dir_table->table[0].user_attribute = UATTR_NOT_EMPTY;
    dir_table->table[0].cluster_high = 0;
    dir_table->table[0].cluster_low = 2;
}

/* CRUD Helper */
double ceil(double x)
{
    int intPart = (int)x;
    return (x > intPart) ? (double)(intPart + 1) : (double)intPart;
}

bool is_dir_table_valid(void)
{
    return fat32_driver_state.dir_table_buf.table[0].user_attribute == UATTR_NOT_EMPTY;
}


/* -- CRUD Operation -- */


/**
 *  FAT32 Folder / Directory read
 *
 * @param request buf point to struct FAT32DirectoryTable,
 *                name is directory name,
 *                ext is unused,
 *                parent_cluster_number is target directory table to read,
 *                buffer_size must be exactly sizeof(struct FAT32DirectoryTable)
 * @return Error code: 0 success - 1 not a folder - 2 not found - -1 unknown
 */
int8_t read_directory(struct FAT32DriverRequest request)
{
    if (request.buffer_size != sizeof(struct FAT32DirectoryTable))
    {
        return 2;
    }
    else
    {

    }
    return -1;
}


/**
 * FAT32 read, read a file from file system.
 *
 * @param request All attribute will be used for read, buffer_size will limit reading count
 * @return Error code: 0 success - 1 not a file - 2 not enough buffer - 3 not found - -1 unknown
 */
int8_t read(struct FAT32DriverRequest request)
{
    /*
    Read akan mencari file yang terletak pada folder parent request.parent_cluster_number. DirectoryTable akan di-iterasi hingga ditemukan request.name dan request.ext yang sama.
    Jika request.buffer_size kurang dari entry.filesize, kembalikan error. Jika flag subdirectory pada entry.attribute menyala (bit subdirectory bernilai 1), kembalikan error.
    Jika kondisi sudah valid, lakukan pembacaan hingga semua cluster terbaca dan dimasukkan ke request.buf.
    Untuk read_directory memiliki cara kerja yang sama, tetapi hanya menerima target yaitu sebuah folder.
     request.ext tidak digunakan pada read_directory. read_directory akan membaca DirectoryTable yang ada pada disk dan memasukkannya pada request.buf.
    */

    // Membaca DirectoryTable
    read_clusters(&fat32_driver_state.dir_table_buf, request.parent_cluster_number, 1);
    
    // Mencari name dan ext yang sama
    int idx = -1;
    for (uint32_t i = 0; i < 64; i++)
    {
        if (memcmp(fat32_driver_state.dir_table_buf.table[i].name, request.name, 8) == 0 && memcmp(fat32_driver_state.dir_table_buf.table[i].ext, request.ext, 3) == 0)
        {
            idx = i;
            break;
        }
    }
    
    // Not Found
    if (idx == -1)
    {
        return 3;
    }

    // Inisialisasi entry
    // fat32_driver_state.dir_table_buf.table[idx];

    // Error jika buffer size < filesize
    if (request.buffer_size < fat32_driver_state.dir_table_buf.table[idx].filesize)
    {
        return 2;
    }

    // Error jika flag subdirectory menyala
    else if (fat32_driver_state.dir_table_buf.table[idx].attribute == 1)
    {
        return 1;
    }

    // Kondisi Valid
    else
    {
        uint32_t cluster_count = fat32_driver_state.dir_table_buf.table[idx].filesize / CLUSTER_SIZE;
        if (cluster_count * CLUSTER_SIZE < fat32_driver_state.dir_table_buf.table[idx].filesize)
        {
            cluster_count++;
        }
        else
        {
            uint32_t cluster_number = (fat32_driver_state.dir_table_buf.table[idx].cluster_high << 16) | fat32_driver_state.dir_table_buf.table[idx].cluster_low;
            for (uint32_t j = 0 ; j < cluster_count; j++)
            {
                if (j == 0)
                {
                    read_clusters(request.buf, cluster_number, 1);
                    cluster_number = fat32_driver_state.fat_table.cluster_map[cluster_number];
                }
                else
                {
                    read_clusters(request.buf + (j * CLUSTER_SIZE), cluster_number, 1);
                    cluster_number = fat32_driver_state.fat_table.cluster_map[cluster_number];
                }
            }
        }
        return 0;
    }
    return -1; // Unknown
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
    // Read directory at parent_cluster_number
    read_clusters(&fat32_driver_state.dir_table_buf, request.parent_cluster_number, 1);
    // read_clusters(&fat32_driver_state.fat_table, 1, 1);
    // Mencari first empty location found pada FileAllocationTable
    int index = 0;
    while (fat32_driver_state.fat_table.cluster_map[index] != 0)
    {
        index++;
    }
    if (is_dir_table_valid())
    {
        // Cek apakah request.name dan request.ext sudah ada atau belum
        for (uint32_t i = 0; i < 64; i++)
        {
            if (memcmp(fat32_driver_state.dir_table_buf.table[i].name, request.name, 8) == 0 && memcmp(fat32_driver_state.dir_table_buf.table[i].ext, request.ext, 3) == 0)
            {
                return 1;
            }
        }

        if (request.buffer_size == 0)
        {
            // membuat sub-direktori pada folder parent request.parent_cluster_number dengan nama request.name.
            struct FAT32DirectoryEntry new_entry = {
                .attribute = ATTR_SUBDIRECTORY,
                .user_attribute = UATTR_NOT_EMPTY,
                .filesize = 0,
                .cluster_high = (index >> 16) & 0xFFFF,
                .cluster_low = index & 0xFFFF,
            };

            for (int i = 0; i < 8; i++)
            {
                new_entry.name[i] = request.name[i];
            }
            int dirindex = 0;
            while (fat32_driver_state.dir_table_buf.table[dirindex].user_attribute == UATTR_NOT_EMPTY)
            {
                dirindex++;
            }
            //Membuat suatu cluster baru untuk membuat suatu direktori
            fat32_driver_state.dir_table_buf.table[dirindex] = new_entry;
            //buat suatu dir table baru di suatu cluster
            struct FAT32DirectoryTable temp = 
            {
                .table = 
                {
                    {
                        //input new_entry to table
                        .attribute = ATTR_SUBDIRECTORY,
                        .user_attribute = UATTR_NOT_EMPTY,
                        .filesize = request.buffer_size,
                        .cluster_high = request.parent_cluster_number >> 16 ,
                        .cluster_low = request.parent_cluster_number & 0xFFFF,
                    }
                }
            };
            for (int i = 0; i < 8; i++)
            {
                temp.table[0].name[i] = request.name[i];
            }
            fat32_driver_state.fat_table.cluster_map[index] = FAT32_FAT_END_OF_FILE;
            write_clusters(&temp, index, 1);
            write_clusters(&fat32_driver_state.dir_table_buf, request.parent_cluster_number, 1);
            write_clusters(&fat32_driver_state.fat_table, 1, 1);
            return 0;
        }
        else
        {
            //YANG BERHUBUNGAN DENGAN DIRECTORY TABLE
            // cari yang kosong pertama
            int dirindex = 0;
            while (fat32_driver_state.dir_table_buf.table[dirindex].user_attribute == UATTR_NOT_EMPTY)
            {
                dirindex++;
            }
            int tempindex = index;
            int nextIndex = index;
            for (int i = 0; i < ceil(request.buffer_size / CLUSTER_SIZE); i++)
            {
                //YANG BERHUBUNGAN DENGAN FAT TABLE
                // cari yang kosong pertama
                while (fat32_driver_state.fat_table.cluster_map[tempindex] != 0)
                {
                    tempindex++;
                }
                //Apabila sudah ditemukan,maka masukkan ke dalam directory tablenya
                if (i == 0)
                {
                    //buat entry baru
                    struct FAT32DirectoryEntry new_entry = {
                    .attribute = ATTR_SUBDIRECTORY,
                    .user_attribute = UATTR_NOT_EMPTY,
                    .filesize = request.buffer_size,
                    .cluster_high = (tempindex >> 16) & 0xFFFF,
                    .cluster_low = tempindex & 0xFFFF,
                    };

                    for (int i = 0; i < 8; i++)
                    {
                        new_entry.name[i] = request.name[i];
                    }
                    for (int i = 0 ; i < 3 ; i ++)
                    {
                        new_entry.ext[i] = request.ext[i];
                    }
                    fat32_driver_state.dir_table_buf.table[dirindex] = new_entry;
                    write_clusters(&fat32_driver_state.dir_table_buf, request.parent_cluster_number, 1);
                }
                nextIndex = tempindex+1;
                //cari yang kosong kedua
                while (fat32_driver_state.fat_table.cluster_map[nextIndex] != 0 && nextIndex == tempindex)
                {
                    nextIndex++;
                }
                //buat entry baru
                if (i == ceil(request.buffer_size / CLUSTER_SIZE - 1))
                {
                    fat32_driver_state.fat_table.cluster_map[tempindex] = FAT32_FAT_END_OF_FILE;
                }
                else
                {
                    fat32_driver_state.fat_table.cluster_map[tempindex] = nextIndex;
                }
                write_clusters(request.buf + i * CLUSTER_SIZE, tempindex, 1);
                write_clusters(&fat32_driver_state.fat_table, 1, 1);
            }
            return 0;
        }
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

    if (request.parent_cluster_number == 0)
    {
        return 2;
    }
    else
    {
        // mencari file (request.name, request.ext, dan request.parent_cluster_number menunjuk kepada DirectoryEntry file yang valid )
        read_clusters(&fat32_driver_state.dir_table_buf, request.parent_cluster_number, 1);

        uint16_t hi, lo, idxDir;
        for(uint32_t i = 0; i < (CLUSTER_SIZE/sizeof(struct FAT32DirectoryEntry)); i++)
        {
            /* Cari di dir_table_buf, untuk c*/
            if( memcmp(fat32_driver_state.dir_table_buf.table[i].name, request.name, 8) == 0 )
            {
                hi = fat32_driver_state.dir_table_buf.table[i].cluster_high;
                lo = fat32_driver_state.dir_table_buf.table[i].cluster_low;
                idxDir = i;
                break;
            }
        }

        struct FAT32DriverState check;
        read_clusters(&check.dir_table_buf, idxDir, 1);

        int counter = 0;
        for(int i = 0 ; i < 64; i++)
        {
            if(check.dir_table_buf.table[i].user_attribute == UATTR_NOT_EMPTY)
            {
                counter++;
            }
        }

        if(counter <= 1)
        {

        /* Cari di FAT32 */
        /* Ambil dulu idx-nya */
        uint32_t idx = hi << 16 | lo;

        if(request.buffer_size == 0){
            /* Folder */
            fat32_driver_state.fat_table.cluster_map[idx] = 0;
            write_clusters(&fat32_driver_state.fat_table, 1, 1);
        }else{
            /* Buat file */
            while(fat32_driver_state.fat_table.cluster_map[idx] != FAT32_FAT_END_OF_FILE)
            {
                uint32_t temp = idx;
                idx = fat32_driver_state.fat_table.cluster_map[idx];
                fat32_driver_state.fat_table.cluster_map[temp] = 0;
            }

            fat32_driver_state.fat_table.cluster_map[idx] = 0;
            write_clusters(&fat32_driver_state.fat_table, 1, 1);
        }

        struct FAT32DirectoryTable new;

        for (uint32_t i = 0; i < (CLUSTER_SIZE/sizeof(struct FAT32DirectoryEntry)); i++)
        {
            if(i != idxDir){
                memcpy(&new.table[i],&fat32_driver_state.dir_table_buf.table[i], sizeof(fat32_driver_state.dir_table_buf.table[i]));
            }
        }

        memcpy(&fat32_driver_state.dir_table_buf, &new, sizeof(new));
        
        write_clusters(&fat32_driver_state.dir_table_buf, request.parent_cluster_number, 1);
        }else{
            return 2;
        }
    }
    return -1;
}
