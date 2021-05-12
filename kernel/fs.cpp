#include <kernel/fs.h>
#include <kernel/memory_addr.h>
#include <kernel/string.h>
#include <kernel/memory_func.h>

static int total_file_counts = 0;
static int first_free_block = -1;
static int first_untouched_block = 0;
static int first_free_entry = -1;
static int first_untouched_entry = 0;

struct file_block {
    char content[4096];
};


static int get_free_block() {
    if (first_free_block != -1) {
        int result = first_free_block;
        first_free_block = *((uint32_t*)&((file_block *)FILE_CONTENT_BASE)[result]);
        return result;
    }
    else {
        return first_untouched_block++;
    }
}

static file_entry * creat(char *filename) {
    file_entry *file_entries = (file_entry*) FILE_ENTRY_BASE;
    int tpidr = get_tpidr_el1();
    file_entry *new_file_entry = nullptr;
    if (first_free_entry != -1) {
        new_file_entry = &file_entries[first_free_entry];
        first_free_entry = new_file_entry->next_free_entry;
    }
    else {
        new_file_entry = &file_entries[first_untouched_entry++];
    }
    total_file_counts++;
    new_file_entry->block = get_free_block();
    strcpy_size(new_file_entry->filename, filename);
    new_file_entry->file_size = 0;
    return new_file_entry;
}

int open(char *filename, int mode) {
    file_entry *file_entries = (file_entry*) FILE_ENTRY_BASE;
    int tpidr = get_tpidr_el1();
    file_entry * target_file_entry = nullptr;
    for (int i = 0; i < total_file_counts; i++) {
        if (strcmp(filename, file_entries[i].filename) == 0) {
            target_file_entry = &file_entries[i];
            break;
        }
    }
    if (target_file_entry == nullptr) {
        if (mode & O_CREAT != 0) {
            target_file_entry = creat(filename);
        }
        else {
            return -1;
        }
    }
    fd_entry * fd_entry_root = (fd_entry*)tasks[tpidr].fd_entries;
    fd_entry * new_fd_entry = nullptr;
    int new_fd;
    if (tasks[tpidr].first_free_fd >= 0) {
        new_fd = tasks[tpidr].first_free_fd;
        new_fd_entry = &fd_entry_root[new_fd];
        tasks[tpidr].first_free_fd = new_fd_entry->next_free_fd;
    }
    else {
        new_fd = tasks[tpidr].first_untouched_fd++;
        new_fd_entry = &fd_entry_root[new_fd];
    }
    new_fd_entry->entry = target_file_entry;
    new_fd_entry->mode = mode;
    new_fd_entry->pos = 0;
    if (new_fd_entry->mode & O_WRONLY) {
        if (new_fd_entry->mode & O_APPEND) {
            new_fd_entry->pos = target_file_entry->file_size;
        }
        else {
            target_file_entry->file_size = 0;
        }
    }
    return new_fd;
}

size_t read(int fd, char *buf, size_t count) {
    fd_entry * fd_entry_root = (fd_entry*)tasks[get_tpidr_el1()].fd_entries;
    file_block * file_block_root = (file_block*) FILE_CONTENT_BASE;
    fd_entry * target_fd_entry = fd_entry_root + fd;
    file_block * target_file_block = file_block_root + target_fd_entry->entry->block;

    uint32_t read_count = target_fd_entry->entry->file_size - target_fd_entry->pos > count ? count : target_fd_entry->entry->file_size - target_fd_entry->pos;

    memcpy(buf, target_file_block->content + target_fd_entry->pos, read_count);
    target_fd_entry->pos += read_count;
    return read_count;
}

size_t write(int fd, char *buf, size_t count) {
    fd_entry * fd_entry_root = (fd_entry*)tasks[get_tpidr_el1()].fd_entries;
    file_block * file_block_root = (file_block*) FILE_CONTENT_BASE;
    fd_entry * target_fd_entry = fd_entry_root + fd;
    file_block * target_file_block = file_block_root + target_fd_entry->entry->block;

    memcpy(target_file_block->content + target_fd_entry->pos, buf, count);
    target_fd_entry->pos += count;
    if (target_fd_entry->pos > target_fd_entry->entry->file_size) {
        target_fd_entry->entry->file_size = target_fd_entry->pos;
    }

    return count;
}

void close(int fd) {
    fd_entry * fd_entry_root = (fd_entry*)tasks[get_tpidr_el1()].fd_entries;
    fd_entry * target_fd_entry = fd_entry_root + fd;
    target_fd_entry->next_free_fd = tasks[get_tpidr_el1()].first_free_fd;
    tasks[get_tpidr_el1()].first_free_fd = fd;
}
