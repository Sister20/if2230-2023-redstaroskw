#include "lib-header/paging.h"
#include "lib-header/stdmem.h"

__attribute__((aligned(0x1000))) struct PageDirectory _paging_kernel_page_directory = {
    .table = {
        [0] = {
            .flag.present_bit       = 1,
            .flag.write_bit         = 1,
            .lower_address          = 0,
            .flag.use_pagesize_4_mb = 1,

        },
        [0x300] = {
            .flag.present_bit       = 1,
            .flag.write_bit         = 1,
            .lower_address          = 0,
            .flag.use_pagesize_4_mb = 1,
        },
    }
};

static struct PageDriverState page_driver_state = {
    .last_available_physical_addr = (uint8_t*) 0 + PAGE_FRAME_SIZE,
};

void update_page_directory_entry(void *physical_addr, void *virtual_addr, struct PageDirectoryEntryFlag flag) {
    uint32_t page_index = ((uint32_t) virtual_addr >> 22) & 0x3FF;

    _paging_kernel_page_directory.table[page_index].flag          = flag;
    _paging_kernel_page_directory.table[page_index].lower_address = ((uint32_t)physical_addr >> 22) & 0x3FF;
    flush_single_tlb(virtual_addr);
}

/*
selalu masuk interrupt pagefault - gatau knp
HIKS - ahmad
*/
int8_t allocate_single_user_page_frame(void *virtual_addr) {
    // Using default QEMU config (128 MiB max memory)
    uint32_t last_physical_addr = (uint32_t) page_driver_state.last_available_physical_addr;

    if (last_physical_addr < 0x8000000) {
        // Allocate a new page frame
        void *new_physical_addr = (void *) last_physical_addr;
        memset(new_physical_addr, 0, PAGE_FRAME_SIZE);

        // Update the page directory entry for the virtual address
        struct PageDirectoryEntryFlag flag = {
            .present_bit = 1,
            .write_bit = 1,
            .user_bit = 1,
            .use_pagesize_4_mb = 1,
        };
        update_page_directory_entry(new_physical_addr, virtual_addr, flag);

        // Update the state to the next available physical address
        page_driver_state.last_available_physical_addr += PAGE_FRAME_SIZE;
        return 0;
    }
    return -1;
}

void update_page_directory(void *physical_addr, void *virtual_addr, struct PageDirectoryEntryFlag flag) {
    uint32_t page_index = ((uint32_t) virtual_addr >> 22) & 0x3FF;

    _paging_kernel_page_directory.table[page_index].flag          = flag;
    _paging_kernel_page_directory.table[page_index].lower_address = ((uint32_t)physical_addr >> 22) & 0x3FF;
    flush_single_tlb(virtual_addr);
}

void flush_single_tlb(void *virtual_addr) {
    asm volatile("invlpg (%0)" : /* <Empty> */ : "b"(virtual_addr): "memory");
}