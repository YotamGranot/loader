#include "loader/loader_status.h"
#include "loader/common.h"
#include "elf/elf_internal.h"
#include <elf.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/auxv.h>

extern char **environ;

typedef int (*entry_point_t)(void);
typedef struct { uint64_t a_type; uint64_t a_val; } Auxv;
void *__libc_stack_end = NULL;
void *__dl_random = NULL;

// Allocate 16 bytes for dl_random
static uint8_t fake_dl_random[16] = { 
    1, 2, 3, 4, 5, 6, 7, 8,
    9, 10, 11, 12, 13, 14,
    15, 16
};

enum loader_status ELF_load(const char *filename)
{
    enum loader_status status = LOADER_STATUS_UNINITIALIZED;
    Elf64_Ehdr *ehdr = NULL;
    Elf64_Phdr *phdrs = NULL;
    struct stat st = {0};
    int fd = INVALID_FD;
    int libc_retval = C_STANDARD_FAILURE_VALUE;

    if (NULL == filename)
    {
        return LOADER_STATUS_INVALID_PARAMETER;
    }

    fd = open(filename, O_RDONLY);
    if (fd <= INVALID_FD)
    {
        DEBUG_PRINT("Failed to open file: %s\n", filename);
        status = LOADER_STATUS_NOT_FOUND;
        goto lbl_cleanup;
    }

    libc_retval = fstat(fd, &st);
    if (libc_retval < 0)
    {
        DEBUG_PRINT("Failed to get file status: %s\n", filename);
        status = LOADER_STATUS_ERROR;
        goto lbl_cleanup;
    }

    void *file = mmap(NULL, st.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (file == MAP_FAILED)
    {
        DEBUG_PRINT("Failed to map file: %s\n", filename);
        status = LOADER_STATUS_ERROR;
        goto lbl_cleanup;
    }

    ehdr = (Elf64_Ehdr *)file;

    if (!elf_veirfy_is_elf_file(ehdr))
    {
        DEBUG_PRINT0("Invalid ELF file format.\n");
        status = LOADER_STATUS_INVALID_FORMAT;
        goto lbl_cleanup;
    }

    if (!elf_veirfy_is_supported_elf_file(ehdr))
    {
        DEBUG_PRINT0("Unsupported ELF file format.\n");
        status = LOADER_STATUS_UNSUPPORTED;
        goto lbl_cleanup;
    }

    phdrs = (Elf64_Phdr *)((char *)file + ehdr->e_phoff);
    for (int i = 0; i < ehdr->e_phnum; ++i)
    {
        Elf64_Phdr *ph = &phdrs[i];
        if (!ELF_IS_LOADABLE_SEGMENT(ph))
        {
            continue;
        }

        status = elf_map_segment(ph, file, ehdr->e_entry);
        if (LOADER_STATUS_IS_ERROR(status))
        {
            DEBUG_PRINT("Failed to map segment %d of ELF file: %s\n", i, filename);
            goto lbl_cleanup;
        }
    }

    elf_jump_to_entry(ehdr->e_entry);
lbl_cleanup:
    munmap(file, st.st_size);
    CLOSE(fd);
    return status;
}

bool elf_veirfy_is_elf_file(Elf64_Ehdr *ehdr)
{
    bool is_elf = false;
    if (ehdr->e_ident[EI_MAG0] == ELFMAG0 &&
        ehdr->e_ident[EI_MAG1] == ELFMAG1 &&
        ehdr->e_ident[EI_MAG2] == ELFMAG2 &&
        ehdr->e_ident[EI_MAG3] == ELFMAG3)
    {
        is_elf = true;
    }
    return is_elf;
}

bool elf_veirfy_is_supported_elf_file(Elf64_Ehdr *ehdr)
{
    bool is_elf = false;

    if (ehdr->e_ident[EI_CLASS] != ELFCLASS64 || ehdr->e_ident[EI_DATA] != ELFDATA2LSB)
    {
        is_elf = false;
    }

    is_elf = true;
    return is_elf;
}

enum loader_status elf_map_segment(Elf64_Phdr *ph, void *elf_file, size_t entry_point)
{
    enum loader_status status = LOADER_STATUS_UNINITIALIZED;

    ASSERT(ph != NULL);
    ASSERT(elf_file != NULL);
    size_t page_size = sysconf(_SC_PAGE_SIZE);

    size_t memsz = ph->p_memsz;
    size_t filesz = ph->p_filesz;
    size_t offset = ph->p_offset;
    size_t vaddr = ph->p_vaddr;

    size_t aligned_vaddr = vaddr & ~(page_size - 1);
    size_t vaddr_offset = vaddr - aligned_vaddr;
    size_t map_size = memsz + vaddr_offset;

    void *segment = mmap((void *)aligned_vaddr, map_size,
                         PROT_READ | PROT_WRITE | PROT_EXEC,
                         MAP_PRIVATE | MAP_ANONYMOUS | MAP_FIXED,
                         -1, 0);
    if (MAP_FAILED == segment)
    {
        DEBUG_PRINT("Failed to mmap segment at 0x%lx, size %zu\n", (unsigned long)aligned_vaddr, map_size);
        status = LOADER_STATUS_ERROR;
        goto lbl_cleanup;
    }

    if (aligned_vaddr <= entry_point && entry_point < aligned_vaddr + map_size)
    {
        DEBUG_PRINT0("Entry point is within the mapped segment.\n");
    }

    memcpy((char *)segment + vaddr_offset, (char *)elf_file + offset, filesz);
    memset((char *)segment + vaddr_offset + filesz, 0, memsz - filesz);
    status = LOADER_STATUS_OK;
lbl_cleanup:
    return status;
}


void elf_jump_to_entry(uint64_t entry)
{
    const char *argv[] = { "program", "arg1", "arg2", NULL };
    const char *envp[] = { "PATH=/usr/bin", "USER=tester", NULL };
    Auxv auxv[] = {
        { AT_PAGESZ, 4096 },
        { AT_UID, getuid() },
        { AT_EUID, geteuid() },
        { AT_SECURE, 0 },
        { AT_RANDOM, (uintptr_t)fake_dl_random },
        { AT_NULL, 0 },  

    };

    int argc = 0;
    while (argv[argc]) argc++;
    int envc = 0;
    while (envp[envc]) envc++;
    int auxc = sizeof(auxv) / sizeof(auxv[0]);

    size_t stack_size = 0x10000;
    char *stack = mmap(NULL, stack_size, PROT_READ | PROT_WRITE,
                       MAP_PRIVATE | MAP_ANONYMOUS | MAP_STACK, -1, 0);
    if (stack == MAP_FAILED) {
        perror("mmap stack");
        exit(1);
    }

    char *sp = stack + stack_size;

    // Copy strings to stack
    char **argv_on_stack = alloca((argc + 1) * sizeof(char *));
    char **envp_on_stack = alloca((envc + 1) * sizeof(char *));

    for (int i = argc - 1; i >= 0; --i) {
        size_t len = strlen(argv[i]) + 1;
        sp -= len;
        memcpy(sp, argv[i], len);
        argv_on_stack[i] = sp;
    }
    argv_on_stack[argc] = NULL;

    for (int i = envc - 1; i >= 0; --i) {
        size_t len = strlen(envp[i]) + 1;
        sp -= len;
        memcpy(sp, envp[i], len);
        envp_on_stack[i] = sp;
    }
    envp_on_stack[envc] = NULL;

    sp = (char *)((uintptr_t)sp & ~0xF);

    // Push auxv[]
    for (int i = auxc - 1; i >= 0; --i) {
        sp -= sizeof(Auxv);
        memcpy(sp, &auxv[i], sizeof(Auxv));
    }

    // Push envp[]
    sp -= sizeof(char *);
    *(char **)sp = NULL;
    for (int i = envc - 1; i >= 0; --i) {
        sp -= sizeof(char *);
        memcpy(sp, &envp_on_stack[i], sizeof(char *));
    }

    // Push argv[]
    sp -= sizeof(char *);
    *(char **)sp = NULL;
    for (int i = argc - 1; i >= 0; --i) {
        sp -= sizeof(char *);
        memcpy(sp, &argv_on_stack[i], sizeof(char *));
    }

    // Push argc
    sp -= sizeof(uint64_t);
    *(uint64_t *)sp = argc;

    sp = (char *)((uintptr_t)sp & ~0xF);
    __libc_stack_end = sp;

    /* This is needed because glibc checks this to see the stack is leagal */
    __dl_random = (void *)(&fake_dl_random[0]);

    __asm__ volatile(
        "mov %0, %%rsp\n"
        "xor %%rbp, %%rbp\n"
        "jmp *%1\n"
        :
        : "r"(sp), "r"(entry)
        : "memory");
}