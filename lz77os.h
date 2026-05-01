int compress(const char *input_name, const char *out_name);
int decompress(const char *input_name, const char *out_name);
long filesize(const char *name);

#define WINDOW_SIZE    4096
#define MAX_LEN       18
#define MIN_LEN       3