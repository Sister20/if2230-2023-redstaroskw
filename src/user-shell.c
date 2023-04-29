#include "lib-header/stdtype.h"
#include "lib-header/fat32.h"

uint32_t id = 0;
uint32_t listCluster[100];
char* listName[100];

void syscall(uint32_t eax, uint32_t ebx, uint32_t ecx, uint32_t edx) {
    __asm__ volatile("mov %0, %%ebx" : /* <Empty> */ : "r"(ebx));
    __asm__ volatile("mov %0, %%ecx" : /* <Empty> */ : "r"(ecx));
    __asm__ volatile("mov %0, %%edx" : /* <Empty> */ : "r"(edx));
    __asm__ volatile("mov %0, %%eax" : /* <Empty> */ : "r"(eax));
    // Note : gcc usually use %eax as intermediate register,ROOT\0\0\0\0
    //        so it need to be the last one to mov
    __asm__ volatile("int $0x30");
}

int memcmp(const void *s1, const void *s2, size_t n) {
    const uint8_t *buf1 = (const uint8_t*) s1;
    const uint8_t *buf2 = (const uint8_t*) s2;
    for (size_t i = 0; i < n; i++) {
        if (buf1[i] < buf2[i])
            return -1;
        else if (buf1[i] > buf2[i])
            return 1;
    }

    return 0;
}

void* memcpy(void* restrict dest, const void* restrict src, size_t n) {
    uint8_t *dstbuf       = (uint8_t*) dest;
    const uint8_t *srcbuf = (const uint8_t*) src;
    for (size_t i = 0; i < n; i++)
        dstbuf[i] = srcbuf[i];
    return dstbuf;
}

int strlen(char* str){
    int len = 0;
    while (str[len] != '\0')
        len++;
    return len;
}

void puts(char* buf, uint8_t color){
    syscall(5, (uint32_t) buf, strlen(buf), color);
}

int32_t buffer_length(char* buf){
    int32_t len = 0;
    while (buf[len] != '\0')
        len++;
    return len;
}

void int_to_char(uint32_t num, char* buf){
    int32_t len = 0;
    while (num != 0)
    {
        buf[len] = num % 10 + '0';
        num /= 10;
        len++;
    }
    buf[len] = '\0';
    for (int32_t i = 0; i < len / 2; i++)
    {
        char temp = buf[i];
        buf[i] = buf[len - i - 1];
        buf[len - i - 1] = temp;
    }
}

struct ClusterBuffer cl           = {0};
void parseCommand(uint32_t buf){
    if (memcmp((char *) buf, "cd", 2) == 0)
    {
        if (memcmp("..", (void *) buf + 3, 2) == 0)
        {
            if (id == 0)
            {
                puts("Already in root", 0x4);
                return;
            }
            id--;
            puts("Change directory success", 0x2);
            puts(listName[id], 0xF);
            return;
        }
        else
        {
            struct FAT32DriverRequest request = {
                .buf = &cl,
                .parent_cluster_number = listCluster[id],
                .buffer_size = 0
            };
            memcpy(request.name, listName[id], 8);
            int32_t retcode = listCluster[id];
            struct FAT32DirectoryTable table = {};
            request.buf = &table;
            syscall(1, (uint32_t) &request, (uint32_t) &retcode, 0);
            for (int i = 0 ; i < 64 ; i++)
            {
                if (memcmp(table.table[i].name, (char *) buf + 3, 8) == 0)
                {
                    id++;
                    listCluster[id] = table.table[i].cluster_low | (table.table[i].cluster_high << 16);
                    // memcpy(listName[id],table.table[i].name,8);
                    listName[id] = table.table[i].name;
                    // memcpy(listName[id],cek2, 8);
                    puts("Change directory success", 0x2);
                    return;
                }
            }
            puts("No such directory", 0x4);
        }
    } 
    else if (memcmp((char *) buf, "ls", 2) == 0)
    {
        struct FAT32DriverRequest request = {
            .buf = &cl,
            .buffer_size = 0
        };
        if (id!=0){
            request.parent_cluster_number = listCluster[id-1];
            uint32_t cek = listCluster[id-1];
            listCluster[id-1] = cek;
        }
        else
        {
            request.parent_cluster_number = listCluster[id];
        }
        memcpy(request.name, listName[id],8);
        int32_t retcode;
        struct FAT32DirectoryTable table = {};
        request.buf = &table;
        syscall(1, (uint32_t) &request, (uint32_t) &retcode, 0);
        if (retcode == 0){
            for (int i = 0 ; i < 64 ; i++)
            {
                puts(table.table[i].name, 0xF);
                if (table.table[i].name[0] == '\0')
                {   
                    memcpy(listName[id], request.name,8);
                    break;
                }
            }
        }
        else if (retcode == 1)
            puts("Not a folder", 0x4);
        else if (retcode == 2)
            puts("Not enough buffer", 0x4);
        else if (retcode == 3)
            puts("Not found", 0x4);
        else
            puts("Unknown error", 0x4);
    } 
    else if (memcmp((char *) buf, "mkdir", 5) == 0)
    {
        struct FAT32DriverRequest request = {
            .parent_cluster_number = listCluster[id],
            .buffer_size           = 0,
        };
        memcpy(request.name, (void *) (buf + 6), 8);
        int32_t retcode;
        syscall(2, (uint32_t) &request, (uint32_t) &retcode, 0);
        if (retcode == 0)
            puts("Write success", 0x2);
        else if (retcode == 1)
            puts("File/Folder already exist", 0x4);
        else if (retcode == 2)
            puts("Invalid parent cluster", 0x4);
        else
            puts("Unknown error", 0x4);
    }
    else if (memcmp((char * ) buf, "cat", 3) == 0)
    {
        struct FAT32DriverRequest request = {
            .buf                   = &cl,
            .parent_cluster_number = listCluster[id],
            .buffer_size           = 0,
        };
        request.buffer_size = 5*CLUSTER_SIZE;
        int nameLen = 0;
        char* itr = (char * ) buf + 4;
        for(int i = 0; i < strlen(itr) ; i++){
            if(itr[i] == '.'){
                request.ext[0] = itr[i+1];
                request.ext[1] = itr[i+2];
                request.ext[2] = itr[i+3];
                break;
            }else{
                nameLen++;
            }
        }
        memcpy(request.name, (void *) (buf + 4), nameLen);
        int32_t retcode;
        syscall(0, (uint32_t) &request, (uint32_t) &retcode, 0);
        if(retcode == 0){
            puts(request.buf, 0xF);
            puts("", 0xF);
        }
        else if (retcode == 1)
            puts("Not a file", 0x4);
        else if (retcode == 2)
            puts("Not enough buffer", 0x4);
        else if (retcode == 3)
            puts("Not found", 0x4);
        else
            puts("Unknown error", 0x4);

    } 
    else if (memcmp((char * ) buf, "cp", 2) == 0)
    {
        struct FAT32DriverRequest request = {
            .buf                   = &cl,
            .parent_cluster_number = ROOT_CLUSTER_NUMBER,
            .buffer_size           = 0,
        };
        request.buffer_size = 5*CLUSTER_SIZE;
        int nameLen = 0;
        char* itr = (char * ) buf + 3;
        for(int i = 0; i < strlen(itr) ; i++){
            if(itr[i] == '.'){
                request.ext[0] = itr[i+1];
                request.ext[1] = itr[i+2];
                request.ext[2] = itr[i+3];
                break;
            }else{
                nameLen++;
            }
        }

        memcpy(request.name, (void *) (buf + 3), nameLen);
        int32_t retcode;

        struct FAT32DriverRequest request2 = {
            .buf                   = &cl,
            .parent_cluster_number = ROOT_CLUSTER_NUMBER,
            .buffer_size           = 0,
        };
        
        memcpy(request2.name, listName[id], 8);
        syscall(0, (uint32_t) &request, (uint32_t) &retcode, 0);

        /* Read the Destination Folder to paste */
        if (retcode == 0)
            puts("Read success", 0x2);
        else if (retcode == 1){
            puts("Not a file", 0x4);
            return;
        }
        else if (retcode == 2){
            puts("Not enough buffer", 0x4);
            return;
        }
        else if (retcode == 3){
            puts("File Not found", 0x4);
            return;
        }
        else{
            puts("Unknown error", 0x4);
            return;
        }

        int32_t retcode2;
        struct FAT32DirectoryTable table = {};
        request2.buf = &table;
        syscall(1, (uint32_t) &request2, (uint32_t) &retcode, 0);
        // char* tes = (char *)buf + 3 + nameLen + 5;
        // char* its = tes;
        // its += 1;
        for(int i = 0; i < 64; i++){
            if(memcmp(table.table[i].name, (void * )(buf + 3 + nameLen + 5), strlen((char *) buf + 3 + nameLen + 5)) == 0){
                request.parent_cluster_number = (table.table[i].cluster_high << 16) | table.table[i].cluster_low;
                syscall(2, (uint32_t) &request, (uint32_t) &retcode2, 0);
                break;
            }
        }

        if (retcode2 == 0)
            puts("Write success", 0x2);
        else if (retcode2 == 1)
            puts("File/Folder already exist", 0x4);
        else if (retcode2 == 2)
            puts("Invalid parent cluster", 0x4);
        else
            puts("Target folder not found", 0x4);
    }
    else if (memcmp((char *) buf, "rm", 2) == 0)
    {
        struct FAT32DriverRequest request = {
            .buf                   = &cl,
            .parent_cluster_number = listCluster[id],
            .buffer_size           = 0,
        };
        request.buffer_size = 5*CLUSTER_SIZE;
        int nameLen = 0;
        char* itr = (char * ) buf + 3;
        for(int i = 0; i < strlen(itr) ; i++){
            if(itr[i] == '.'){
                request.ext[0] = itr[i+1];
                request.ext[1] = itr[i+2];
                request.ext[2] = itr[i+3];
                break;
            }else{
                nameLen++;
            }
        }
        memcpy(request.name, (void *) (buf + 3), 8);
        // memcpy(request.ext, "\0\0\0", 3);
        int32_t retcode;
        syscall(3, (uint32_t) &request, (uint32_t) &retcode, 0);
        if (retcode == 0)
            puts("Delete Success", 0x2);
        else if (retcode == 1)
            puts("File/Folder Not Found", 0x4);
        else if (retcode == 2)
            puts("Folder is empty", 0x4);
        else
            puts("Unknown Error", 0x4);
    } 
    else if (memcmp((char *) buf, "mv", 2) == 0)
    {
        // puts(buf + 3, cpu.ecx, cpu.edx);
        struct FAT32DriverRequest request = {
            .buf                   = &cl,
            .parent_cluster_number = ROOT_CLUSTER_NUMBER,
            .buffer_size           = 0,
        };
        request.buffer_size = 5*CLUSTER_SIZE;
        int nameLen = 0;
        char* itr = (char * ) buf + 3;
        for(int i = 0; i < strlen(itr) ; i++){
            if(itr[i] == '.'){
                request.ext[0] = itr[i+1];
                request.ext[1] = itr[i+2];
                request.ext[2] = itr[i+3];
                break;
            }else{
                nameLen++;
            }
        }

        memcpy(request.name, (void *) (buf + 3), nameLen);
        int32_t retcode;

        struct FAT32DriverRequest request2 = {
            .buf                   = &cl,
            .parent_cluster_number = ROOT_CLUSTER_NUMBER,
            .buffer_size           = 0,
        };
        
        memcpy(request2.name, listName[id], 8);
        syscall(0, (uint32_t) &request, (uint32_t) &retcode, 0);

        /* Read the Destination Folder to paste */
        if (retcode == 0)
            puts("Read success", 0x2);
        else if (retcode == 1){
            puts("Not a file", 0x4);
            return;
        }
        else if (retcode == 2){
            puts("Not enough buffer", 0x4);
            return;
        }
        else if (retcode == 3){
            puts("File Not found", 0x4);
            return;
        }
        else{
            puts("Unknown error", 0x4);
            return;
        }

        int32_t retcode2;
        struct FAT32DirectoryTable table = {};
        request2.buf = &table;
        syscall(1, (uint32_t) &request2, (uint32_t) &retcode, 0);
        // char* tes = (char *)buf + 3 + nameLen + 5;
        // char* its = tes;
        // its += 1;
        for(int i = 0; i < 64; i++){
            if(memcmp(table.table[i].name, (void * )(buf + 3 + nameLen + 5), strlen((char *) buf + 3 + nameLen + 5)) == 0){
                request.parent_cluster_number = (table.table[i].cluster_high << 16) | table.table[i].cluster_low;
                syscall(2, (uint32_t) &request, (uint32_t) &retcode2, 0);
                break;
            }
        }

        if (retcode2 == 0)
            puts("Write success", 0x2);
        else if (retcode2 == 1)
            puts("File/Folder already exist", 0x4);
        else if (retcode2 == 2)
            puts("Invalid parent cluster", 0x4);
        else
            puts("Target folder not found", 0x4);

        struct FAT32DriverRequest request3 = {
            .buf                   = &cl,
            .parent_cluster_number = listCluster[id],
            .buffer_size           = 0,
        };
        request3.buffer_size = 5*CLUSTER_SIZE;
        int nameLen1 = 0;
        char* itr1 = (char * ) buf + 3;
        for(int i = 0; i < strlen(itr) ; i++){
            if(itr[i] == '.'){
                request3.ext[0] = itr1[i+1];
                request3.ext[1] = itr1[i+2];
                request3.ext[2] = itr1[i+3];
                break;
            }else{
                nameLen1++;
            }
        }
        memcpy(request3.name, (void *) (buf + 3), 8);
        // memcpy(request.ext, "\0\0\0", 3);
        int32_t retcode3;
        syscall(3, (uint32_t) &request3, (uint32_t) &retcode3, 0);
        if (retcode3 == 0)
            puts("Move Success", 0x2);
        else if (retcode3 == 1)
            puts("File/Folder Not Found", 0x4);
        else if (retcode3 == 2)
            puts("Folder is empty", 0x4);
        else
            puts("Unknown Error", 0x4);
    } 
    else if (memcmp((char *) buf, "whereis", 7) == 0)
    {
        // puts(buf + 8, cpu.ecx, cpu.edx);
    } 
    else if (memcmp((char *) buf, "cls", 3) == 0)
    {
        puts("cls", 0xF);
    }
    else if (memcmp((char *) buf, "touch", 5) == 0)
    {
        struct FAT32DriverRequest request = {
            .buf                   = &cl,
            .parent_cluster_number = listCluster[id],
            .buffer_size           = 0,
        };
        request.buffer_size = 5*CLUSTER_SIZE;

        /* get the name and ext of the file */
        int nameLen1 = 0;
        char* itr = (char * ) buf + 6;
        for(int i = 0; i < strlen(itr) ; i++){
            if(itr[i] == '.'){
                request.ext[0] = itr[i+1];
                request.ext[1] = itr[i+2];
                request.ext[2] = itr[i+3];
                break;
            }else{
                nameLen1++;
            }
        }

        memcpy(request.name, (void * ) buf + 6, nameLen1);
        uint32_t retcode;
        struct ClusterBuffer cbuf[5];
        
        /* Dapetin isi filenya */
        char* isi = "";

        memcpy(isi, (void *) buf + 6 + nameLen1 + 1 + 3, sizeof(buf + 6 + nameLen1 + 1 + 3));

        for (uint32_t i = 0; i < 5; i++)
            for (uint32_t j = 0; j < CLUSTER_SIZE; j++)
                cbuf[i].buf[j] = isi[j];
        /* Write to the file */
        request.buf = cbuf;
        syscall(2, (uint32_t) &request, (uint32_t) &retcode, 0);
        if(retcode == 0){
            puts("Write success", 0x2);
        }else{
            puts("Write unsuccessful", 0x4);
        }

    }
    else
    {
        puts("Command not found", 0x4);
    }
}

int main(void) {
    listCluster[0] = (uint32_t) ROOT_CLUSTER_NUMBER;
    listName[0] = (char *) "ROOT\0\0\0\0";
    struct ClusterBuffer cl           = {0};
    struct FAT32DriverRequest request = {
        .buf                   = &cl,
        .name                  = "ikanaide",
        .ext                   = "\0\0\0",
        .parent_cluster_number = ROOT_CLUSTER_NUMBER,
        .buffer_size           = CLUSTER_SIZE,
    };
    int32_t retcode;
    syscall(0, (uint32_t) &request, (uint32_t) &retcode, 0);

    // if (retcode == 0)
        // syscall(5, (uint32_t) "owo", 3, 0xF);

    char buf[80];
    char* terminal = "root@redStarOSKW ";
    while (TRUE) {
        syscall(6, (uint32_t) terminal, strlen(terminal), 0x2);
        syscall(7, (uint32_t) listName[id], strlen(listName[id]), 0x1);
        syscall(4, (uint32_t) buf, 80, 0);
        parseCommand((uint32_t) buf);
    }
    return 0;
}
