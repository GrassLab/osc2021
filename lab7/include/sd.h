# define SECTOR_SIZE       512

# define SECTOR_TABLE_COLS     16
# define SECTOR_TABLE_ROWS     (SECTOR_SIZE/SECTOR_TABLE_COLS)
# define SECTOR_TABLE_OFFSET   20

# define TO_HEX(n)       ((char)( (n<10) ? n+48 : n+55 ))

void listblock(int n);
