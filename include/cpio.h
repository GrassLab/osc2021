//#define CPIO_ARCHIVE_LOCATION 0x8000000
#define CPIO_ARCHIVE_LOCATION 0x20000000
/* Magic identifiers for the "cpio" file format. */
#define CPIO_HEADER_MAGIC "070701"
#define CPIO_FOOTER_MAGIC "TRAILER!!!"
#define CPIO_ALIGNMENT 4

struct cpio_header {
    char c_magic[6];      /* Magic header '070701'. */
    char c_ino[8];        /* "i-node" number. */
    char c_mode[8];       /* Permisions. */
    char c_uid[8];        /* User ID. */
    char c_gid[8];        /* Group ID. */
    char c_nlink[8];      /* Number of hard links. */
    char c_mtime[8];      /* Modification time. */
    char c_filesize[8];   /* File size. */
    char c_devmajor[8];   /* Major dev number. */
    char c_devminor[8];   /* Minor dev number. */
    char c_rdevmajor[8];
    char c_rdevminor[8];
    char c_namesize[8];   /* Length of filename in bytes. */
    char c_check[8];      /* Checksum. */
};

/**
 * Stores information about the underlying implementation.
 */
struct cpio_info {
    /// The number of files in the CPIO archive
    unsigned int file_count;
    /// The maximum size of a file name
    unsigned int max_path_sz;
};


/**
 * Retrieves information about the provided CPIO archive
 * @param[in] archive  The location of the CPIO archive
 * @param[out] info    A CPIO info structure to populate
 * @return             Non-zero on error.
 */
int cpio_info(void *archive, struct cpio_info *info);

/**
 * Writes the list of file names contained within a CPIO archive into
 * a provided buffer
 * @param[in] archive  The location of the CPIO archive
 * @param[in] buf      A memory location to store the CPIO file list to
 * @param[in] buf_len  The length of the provided buf
 */
void cpio_ls(void *archive, char buf[100][100], unsigned long buf_len);

void cpio_get_file(void *archive, char name[], char content[]);