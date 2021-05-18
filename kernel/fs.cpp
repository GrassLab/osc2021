#include <kernel/fs.h>
#include <kernel/memory_addr.h>
#include <kernel/string.h>
#include <kernel/memory_func.h>

 int total_file_counts = 0;
int first_free_block = -1;
int first_untouched_block = 0;
int first_free_entry = -1;
int first_untouched_entry = 0;

struct file_block {
    char content[512];
};


static int get_free_block() {
    if (first_free_block != -1) {
        int result = first_free_block;
        first_free_block = ((uint32_t*)FILE_BLOCK_ENTRY)[result];
        return result;
    }
    else {
        ((uint32_t*)FILE_BLOCK_ENTRY)[first_untouched_block] = -1;
        return first_untouched_block++;
    }
}

static file_entry * creat(char *filename) {
    file_entry *file_entries = (file_entry*) FILE_ENTRY_BASE;
    int tpidr = get_tpidr_el1();
    file_entry *new_file_entry = nullptr;
    int32_t *block_entries = (int32_t*) FILE_BLOCK_ENTRY;
    if (first_free_entry != -1) {
        new_file_entry = &file_entries[first_free_entry];
        first_free_entry = new_file_entry->next_free_entry;
    }
    else {
        new_file_entry = &file_entries[first_untouched_entry++];
    }
    total_file_counts++;
    new_file_entry->block = get_free_block();
    block_entries[new_file_entry->block] = -1;
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
    int32_t * block_entry_root = (int32_t*) FILE_BLOCK_ENTRY;
    uint32_t block_index = target_fd_entry->entry->block;
    uint32_t block_offset = target_fd_entry->pos;
    while (block_offset > sizeof(file_block)) {
        block_index = block_entry_root[block_index];
        block_offset -= sizeof(file_block);
    }
    file_block * target_file_block = file_block_root + block_index;

    uint32_t read_count = target_fd_entry->entry->file_size - target_fd_entry->pos > count ? count : target_fd_entry->entry->file_size - target_fd_entry->pos;
    uint32_t read_bytes_left = read_count;

    while (read_bytes_left > 0) {
        uint32_t current_read = (read_bytes_left + block_offset > sizeof(file_block)) ? (sizeof(file_block) - block_offset) : read_bytes_left;
        memcpy(buf, file_block_root[block_index].content + block_offset, current_read);
        buf += current_read;
        read_bytes_left -= current_read;
        if (read_bytes_left > 0) {
            block_index = block_entry_root[block_index];
            block_offset = 0;
        }
    }

    target_fd_entry->pos += read_count;

    return read_count;
}

size_t write(int fd, char *buf, size_t count) {
    fd_entry * fd_entry_root = (fd_entry*)tasks[get_tpidr_el1()].fd_entries;
    file_block * file_block_root = (file_block*) FILE_CONTENT_BASE;
    fd_entry * target_fd_entry = fd_entry_root + fd;
    int32_t * block_entry_root = (int32_t*) FILE_BLOCK_ENTRY;
    uint32_t block_index = target_fd_entry->entry->block;
    uint32_t block_offset = target_fd_entry->pos;
    while (block_offset > sizeof(file_block)) {
        block_index = block_entry_root[block_index];
        block_offset -= sizeof(file_block);
    }
    file_block * target_file_block = file_block_root + block_index;
    uint32_t write_bytes_left = count;
    while (write_bytes_left > 0) {
        uint32_t current_write = (write_bytes_left + block_offset > sizeof(file_block)) ? (sizeof(file_block) - block_offset) : write_bytes_left;
        memcpy(file_block_root[block_index].content + block_offset, buf, current_write);
        buf += current_write;
        write_bytes_left -= current_write;
        if (write_bytes_left > 0) {
            if (block_entry_root[block_index] < 0) {
                int32_t new_block_index = get_free_block();
                block_entry_root[new_block_index] = -1;
                block_entry_root[block_index] = new_block_index;
            }
            block_index = block_entry_root[block_index];
            block_offset = 0;
        }
    }
    target_fd_entry->pos += count;
    if (target_fd_entry->entry->file_size < target_fd_entry->pos) {
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
