#include <kernel/fs.h>
#include <kernel/memory_addr.h>
#include <kernel/string.h>
#include <kernel/memory_func.h>
#include <kernel/fat32.h>
#include <kernel/sdhost.h>

static int total_file_counts = 0;
static int first_free_block = -1;
static int first_untouched_block = 0;
static int first_free_entry = -1;
static int first_untouched_entry = 0;

struct file_block {
    char content[512];
};

static char fat32_buffer[512];
static struct fat32 fat32;

void fs_init() {
    get_fat(&fat32, fat32_buffer);
}

static bool valid_fat_name(char c) {
    if (c >= 'A' && c <= 'Z') return true;
    if (c >= '0' && c <= '9') return true;
    if (c >= '#' && c <= ')') return true;
    if (c >= 128 && c <= 228) return true;
    if (c >= 230 && c <= 255) return true;
    if (c == ' ') return true;
    if (c == '!') return true;
    if (c == '-') return true;
    if (c == '@') return true;
    if (c == '^') return true;
    if (c == '_') return true;
    return false;
}

static char* to_fat_filename(char *dst, const char *filename) {
    int filename_ptr = 0;
    // main filename
    for (int i = 0; i < 8; i++, filename_ptr++) {
        char c = filename[filename_ptr];
        if (c >= 'a' && c <= 'z') c -= ('a' - 'A');
        else if (c == '.') { memset(&dst[i], ' ', 8 - i); break; }
        else if (c == 0) { memset(&dst[i], ' ', 11 - i); return dst; }
        else if (!valid_fat_name(c)) return nullptr;
        dst[i] = c;
    }
    // check main filename length
    char c = filename[filename_ptr];
    if (c == '.') filename_ptr++;
    else if (c == 0) { memset(&dst[8], ' ', 3); return dst; }
    else { return nullptr; }

    // sub filename
    for (int i = 8; i < 11; i++, filename_ptr++) {
        char c = filename[filename_ptr];
        if (c >= 'a' && c <= 'z') c -= ('a' - 'A');
        else if (c == 0) { memset(&dst[i], ' ', 11 - i); return dst; }
        else if (!valid_fat_name(c)) return nullptr;
        dst[i] = c;
    }

    // check ending
    if (filename[filename_ptr] != 0) {
        return nullptr;
    }
    return dst;
}

static bool fat32_filename_cmp(char *fat32_filename, char *filename) {
    char filename_buffer[11];
    char *dst = to_fat_filename(filename_buffer, filename);
    if (dst == nullptr) return false;
    return memcmp(filename_buffer, fat32_filename, 11) == 0;
}

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

static void write_fat(int fat_block_index, char * buffer) {
    for (int i = 0; i < fat32.fat_count; i++) {
        writeblock(fat32.fat_start_block + i * fat32.fat_size + fat_block_index , buffer);
    }
}

static void creat(file_entry *filename) {
    // for (int i = 0; i < fat32.root_dir_size; i++) {
    //     readblock(fat32.data_start_block + i, fat32_buffer);
    //     for (int j = 0; j < 16; j++) {
    //         if (fat32_buffer[j * 16 + 11] == 0x20) {
    //             file_entry->fat32_entry = i * 16 + j;
    //             return file_entry;
    //         }
    //     }
    // }
}

static file_entry * load(char *filename) {
    file_entry *file_entries = (file_entry*) FILE_ENTRY_BASE;
    int tpidr = get_tpidr_el1();
    file_entry *file_entry = nullptr;
    int32_t *block_entries = (int32_t*) FILE_BLOCK_ENTRY;
    if (first_free_entry != -1) {
        file_entry = &file_entries[first_free_entry];
        first_free_entry = file_entry->next_free_entry;
    }
    else {
        file_entry = &file_entries[first_untouched_entry++];
    }
    total_file_counts++;
    file_entry->block = get_free_block();
    block_entries[file_entry->block] = -1;
    strcpy_size(file_entry->filename, filename);
    file_entry->file_size = 0;
    file_entry->fat32_entry = -1;
    
    file_block * file_block_root = (file_block*) FILE_CONTENT_BASE;

    readblock(fat32.data_start_block, fat32_buffer);
    for (int j = 0; j < 16; j++) {
        if (fat32_buffer[j * 32 + 11] == 0x20 && fat32_filename_cmp(&fat32_buffer[j * 32], file_entry->filename)) {
            file_entry->fat32_entry = j;
            memcpy(&file_entry->file_size, &fat32_buffer[j * 32 + 28], 4);
            int data_block_index;
            memcpy(&data_block_index, &fat32_buffer[j * 32 + 26], 2);
            memcpy(((char*)&data_block_index) + 2, &fat32_buffer[j * 32 + 20], 2);
            file_entry->fat32_block = data_block_index;
            uint32_t block_index = file_entry->block;

            while (true) {
                readblock(fat32.data_start_block + data_block_index - fat32.root_dir_start, &file_block_root[block_index]);
                int fat_block_index = data_block_index / 128;
                readblock(fat_block_index + fat32.fat_start_block, fat32_buffer);
                data_block_index = ((int*)fat32_buffer)[data_block_index % 128];
                if ((data_block_index & 0xffffff8) != 0xffffff8) {
                    int new_block_index = get_free_block();
                    block_entries[block_index] = new_block_index;
                    block_index = new_block_index;
                }
                else {
                    break;
                }
            }

            return file_entry;
        }
    }
    return file_entry;
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
        target_file_entry = load(filename);
    }
    if (target_file_entry->fat32_entry == -1) {
        // if (mode & O_CREAT != 0) {
        //     creat(target_file_entry);
        // }
        // else {
        //     return -1;
        // }
        return -1;
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
            readblock(fat32.data_start_block + target_file_entry->fat32_entry / 16, fat32_buffer);
            memset(fat32_buffer + target_file_entry->fat32_entry * 32 + 0x1c, 0, 4);
            writeblock(fat32.data_start_block + target_file_entry->fat32_entry / 16, fat32_buffer);
            readblock(fat32.fat_start_block + target_file_entry->fat32_block / 128, fat32_buffer);
            int block_index = ((int*)fat32_buffer)[target_file_entry->fat32_block % 128];
            ((int*)fat32_buffer)[target_file_entry->fat32_block % 128] = 0xffffffff;
            write_fat(target_file_entry->fat32_block / 128, fat32_buffer);
            while (block_index & 0xffffff8 != 0xffffff8) {
                readblock(fat32.fat_start_block + block_index / 128, fat32_buffer);
                int next_block_index = ((int*)fat32_buffer)[block_index % 128];
                ((int*)fat32_buffer)[block_index % 128] = 0;
                write_fat(block_index / 128, fat32_buffer);
                block_index = next_block_index;
            }
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
    uint32_t fs_block_index = target_fd_entry->entry->fat32_block;
    while (block_offset > sizeof(file_block)) {
        block_index = block_entry_root[block_index];
        readblock(fat32.fat_start_block + fs_block_index / 128, fat32_buffer);
        fs_block_index = ((int*)fat32_buffer)[fs_block_index % 128];
        block_offset -= sizeof(file_block);
    }
    uint32_t write_bytes_left = count;
    while (write_bytes_left > 0) {
        uint32_t current_write = (write_bytes_left + block_offset > sizeof(file_block)) ? (sizeof(file_block) - block_offset) : write_bytes_left;
        memcpy(file_block_root[block_index].content + block_offset, buf, current_write);
        writeblock(fat32.data_start_block + fs_block_index - fat32.root_dir_start, file_block_root[block_index].content);
        buf += current_write;
        write_bytes_left -= current_write;
        if (write_bytes_left > 0) {
            if (block_entry_root[block_index] < 0) {
                int32_t new_block_index = get_free_block();
                block_entry_root[new_block_index] = -1;
                block_entry_root[block_index] = new_block_index;
            }
            readblock(fat32.fat_start_block + fs_block_index / 128, fat32_buffer);
            uint32_t new_fs_block_index = ((int*)fat32_buffer)[fs_block_index % 128];
            if ((new_fs_block_index & 0xffffff8) == 0xffffff8) {
                for (int i = 0; i < fat32.fat_size; i++) {
                    readblock(fat32.fat_start_block + i, fat32_buffer);
                    for (int j = 0; j < 128; j++) {
                        if (((int*)fat32_buffer)[j] == 0) {
                            uint32_t end_block_marker = new_fs_block_index;
                            new_fs_block_index = i * 128 + j;
                            ((int*)fat32_buffer)[j] = end_block_marker;
                            write_fat(i, fat32_buffer);
                            readblock(fat32.fat_start_block + fs_block_index / 128, fat32_buffer);
                            ((int*)fat32_buffer)[fs_block_index % 128] = new_fs_block_index;
                            write_fat(fs_block_index / 128, fat32_buffer);
                            fs_block_index = new_fs_block_index;
                            goto finish_alloc_new_block;
                        }
                    }
                }
                return -1;
            }
            else {
                fs_block_index = new_fs_block_index;
            }
finish_alloc_new_block:
            block_index = block_entry_root[block_index];
            block_offset = 0;
        }
    }
    target_fd_entry->pos += count;
    if (target_fd_entry->entry->file_size < target_fd_entry->pos) {
        target_fd_entry->entry->file_size = target_fd_entry->pos;
        readblock(fat32.data_start_block + target_fd_entry->entry->fat32_entry / 16, fat32_buffer);
        *(int*)(fat32_buffer + target_fd_entry->entry->fat32_entry * 32 + 0x1c) = target_fd_entry->entry->file_size;
        writeblock(fat32.data_start_block + target_fd_entry->entry->fat32_entry / 16, fat32_buffer);
    }
    return count;
}

void close(int fd) {
    fd_entry * fd_entry_root = (fd_entry*)tasks[get_tpidr_el1()].fd_entries;
    fd_entry * target_fd_entry = fd_entry_root + fd;
    target_fd_entry->next_free_fd = tasks[get_tpidr_el1()].first_free_fd;
    tasks[get_tpidr_el1()].first_free_fd = fd;
}
