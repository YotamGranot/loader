/**
 * @file elf.h
 * @brief Header file for ELF (Executable and Linkable Format) loader.
 * @author Yotam Granot
 * @date 2025-06-16
 */
#ifndef __LOADER_ELF_H__
#define __LOADER_ELF_H__

#include "loader/loader_status.h"
/**
 * @brief Load an ELF file and prepare it for execution.
 *
 * @param elf Pointer to a struct elf pointer that will hold the loaded ELF data.
 * @param filename Path to the ELF file to load.
 * @return Status of the loading operation (e.g., LOADER_STATUS_OK, LOADER_STATUS_ERROR).
 */
enum loader_status ELF_load(const char *filename);

#endif /* __LOADER_ELF_H__ */