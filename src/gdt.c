#include "lib-header/stdtype.h"
#include "lib-header/gdt.h"

/**
 * global_descriptor_table, predefined GDT.
 * Initial SegmentDescriptor already set properly according to GDT definition in Intel Manual & OSDev.
 * Table entry : [{Null Descriptor}, {Kernel Code}, {Kernel Data (variable, etc)}, ...].
 */
struct GlobalDescriptorTable global_descriptor_table = {
    .table = {
        {
            // TODO : Implement
            // NULL Descriptor
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
        },
        {
            // TODO : Implement
            // Kernel Mode Code
            // segment_low, base_low, base_mid, type_bit, non_system, dpl, p, limit, avl, l, db, g, base_high
            1, 0, 0, 0xA, 0, 0, 1, 0xFFFFF, 0, 0, 1, 1, 0
        },
        {
            // TODO : Implement
            // Kernel Mode Data
            // segment_low, base_low, base_mid, type_bit, non_system, dpl, p, limit, avl, l, db, g, base_high
            1, 0, 0, 0x2, 0, 0, 1, 0xFFFFF, 0, 0, 1, 1, 0
        },
        {
            // User Mode Code
            
        },
        {
            // User Mode data
        }
        
    }
};

/**
 * _gdt_gdtr, predefined system GDTR. 
 * GDT pointed by this variable is already set properly.
 * From: https://wiki.osdev.org/Global_Descriptor_Table, GDTR.size is GDT size minus 1.
 */
struct GDTR _gdt_gdtr = {
    // TODO : Implement, this GDTR will point to global_descriptor_table. 
    //        Use sizeof operator
    sizeof(global_descriptor_table)-1,
    &global_descriptor_table
};

