/**
 * @file elf_internal.h
 * @brief Internal header file for ELF (Executable and Linkable Format) loader.
 * @author Yotam Granot
 * @date 2025-06-16
 */
#ifndef __ELF_INTERNAL_H__
#define __ELF_INTERNAL_H__

#include "elf/elf.h"
#include <elf.h>
#define ELF_IS_LOADABLE_SEGMENT(program_header) ((program_header)->p_type == PT_LOAD)


/**
 * @brief Verify if the given ELF header is a valid ELF file.
 *
 * @param ehdr Pointer to the ELF header.
 * @return true if it is a valid ELF file, false otherwise.
 */
bool elf_veirfy_is_elf_file(Elf64_Ehdr *ehdr);


/**
 * @brief Verify if the given ELF header is a supported ELF file.
 *
 * @note This function assumes the given ELF header is already verified to be an ELF file.
 * @param ehdr Pointer to the ELF header.
 * @return true if it is a supported ELF file, false otherwise.
 */
bool elf_veirfy_is_supported_elf_file(Elf64_Ehdr *ehdr);

/**
 * @brief Map a segment of an ELF file into memory.
 * 
 * @param ph Pointer to the program header of the segment to map.
 * @param elf_file Pointer to the ELF file data in memory.
 */
enum loader_status elf_map_segment(Elf64_Phdr *ph, void *elf_file, size_t entry_point);

/**
 * @brief Jump to the entry point of the ELF file.
 * 
 * This function prepares the stack and jumps to the entry point of the ELF file.
 * It sets up the arguments, environment variables, and auxiliary vectors as required by the ELF specification.
 *
 * @param entry The entry point address of the ELF file.
 */
void elf_jump_to_entry(uint64_t entry);
#endif /* __ELF_INTERNAL_H__ */