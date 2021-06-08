enum log_type{
  SEVERE = 0,
  WARNING = 1,
  INFO = 2,
  FINE = 3,
};

# define LOG(level) if(level <= get_log_level())

void log_puts(const char *s, enum log_type level);
void set_log_level(enum log_type level);
enum log_type get_log_level();
