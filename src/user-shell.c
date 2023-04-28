#include "lib-header/stdtype.h"
#include "lib-header/fat32.h"

void syscall(uint32_t eax, uint32_t ebx, uint32_t ecx, uint32_t edx) {
    __asm__ volatile("mov %0, %%ebx" : /* <Empty> */ : "r"(ebx));
    __asm__ volatile("mov %0, %%ecx" : /* <Empty> */ : "r"(ecx));
    __asm__ volatile("mov %0, %%edx" : /* <Empty> */ : "r"(edx));
    __asm__ volatile("mov %0, %%eax" : /* <Empty> */ : "r"(eax));
    // Note : gcc usually use %eax as intermediate register,
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

struct ClusterBuffer cl           = {0};
void parseCommand(uint32_t buf){
    if (memcmp((char *) buf, "cd", 2) == 0)
    {
        syscall(5, buf + 3, 16, 0xF);
    } 
    else if (memcmp((char *) buf, "ls", 2) == 0)
    {
        // puts(buf + 3, cpu.ecx, cpu.edx);
    } 
    else if (memcmp((char *) buf, "mkdir", 5) == 0)
    {
        struct FAT32DriverRequest request = {
            .parent_cluster_number = ROOT_CLUSTER_NUMBER,
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
        // puts(buf + 4, cpu.ecx, cpu.edx);
    } 
    else if (memcmp((char * ) buf, "cp", 2) == 0)
    {
        // puts(buf + 3, cpu.ecx, cpu.edx);
        struct FAT32DriverRequest request = {
            .buf                   = &cl,
            .parent_cluster_number = ROOT_CLUSTER_NUMBER,
            .buffer_size           = 0,
        };
        request.buffer_size = 5*CLUSTER_SIZE;
        // memcpy(request.name, (void *) (buf + 3), 8);
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
        syscall(0, (uint32_t) &request, (uint32_t) &retcode, 0);

        if(retcode == 0)
            puts("Sukses", 0x2);
        else
            puts("Gagal", 0x4);

    } 
    else if (memcmp((char *) buf, "rm", 2) == 0)
    {
        struct ClusterBuffer cbuf[5];
        for (uint32_t i = 0; i < 5; i++)
            for (uint32_t j = 0; j < CLUSTER_SIZE; j++)
                cbuf[i].buf[j] = i + 'a';

        struct FAT32DriverRequest request = {
            .buf                   = cbuf,
            .name                  = "ikanaide",
            .ext                   = "uwu",
            .parent_cluster_number = ROOT_CLUSTER_NUMBER,
            .buffer_size           = 0,
        };
        request.buffer_size = 5*CLUSTER_SIZE;
        memcpy(request.name, (void *) (buf + 3), 8);
        memcpy(request.ext, "\0\0\0", 3);
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
    } 
    else if (memcmp((char *) buf, "whereis", 7) == 0)
    {
        // puts(buf + 8, cpu.ecx, cpu.edx);
    } 
    else if (memcmp((char *) buf, "cls", 3) == 0)
    {
        puts("cls", 0xF);
    }
    else
    {
        puts("Command not found", 0x4);
    }
}

int main(void) {
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

    char buf[16];
    while (TRUE) {
        puts("Nadil@RedStarOSKW ", 0x2);
        puts(":",0x8);
        puts("/",0x1);
        puts("$ ", 0xF);
        syscall(4, (uint32_t) buf, 16, 0);
        parseCommand((uint32_t) buf);
    }
    return 0;
}
