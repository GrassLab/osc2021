#include "cpio.h"
#include "config.h"
#include "log.h"
#include "string.h"
#include "uart.h"

#ifdef CFG_LOG_CPIO
static const int _DO_LOG = 1;
#else
static const int _DO_LOG = 0;
#endif

int _hexChar2Int(char c);
int64_t _parseHexStr(const char *buf, int len);
uintptr_t _alignUp(unsigned long n, unsigned long align);
int _cpioParseHeader(CpioNewcHeader *header, const char **filename,
                     uint64_t *_filesize, void **data, CpioNewcHeader **next);

int cpioLs(void *archive) {
  CpioNewcHeader *header, *next;
  const char *filename;
  void *fileContent;
  int err;
  for (header = archive;; header = next) {
    err = _cpioParseHeader(header, &filename, NULL, &fileContent, &next);
    if (err) {
      return 1;
    }
    uart_println(" %s", filename);
  }
  return 0;
}

void *cpioGetFile(void *archive, const char *name, unsigned long *size) {
  CpioNewcHeader *header, *next;
  const char *filename;
  void *fileContent;
  int err;
  for (header = archive;; header = next) {
    err = _cpioParseHeader(header, &filename, size, &fileContent, &next);
    if (err) {
      return NULL;
    }
    if (strcmp(filename, name) == 0) {
      return fileContent;
    }
  }
  return NULL;
}

int cpioInfo(void *archive, CpioSummaryInfo *info) {
  CpioNewcHeader *header, *next;
  const char *filename;
  void *data;
  int err;
  uint64_t filesize, curPathSize;

  if (info == NULL)
    return 1;
  info->numFiles = 0;
  info->mxPathSize = 0;

  header = archive;
  for (uint32_t i = 0;; i++, info->numFiles++, header = next) {
    err = _cpioParseHeader(header, &filename, &filesize, &data, &next);
    if (err == -1) {
      return err;
    } else if (err == 1) {
      // EOF
      return 0;
    }

    // Check if this is the maximum file path size.
    curPathSize = strlen(filename);
    if (curPathSize > info->mxPathSize) {
      info->mxPathSize = curPathSize;
    }
  }

  return 0;
}

int _hexChar2Int(char c) {
  switch (c) {
  case '0' ... '9':
    return c - '0';
  case 'a' ... 'f':
    return c - 'a' + 10;
  case 'A' ... 'F':
    return c - 'A' + 10;
  default:
    return -1;
  }
}

/**
 * Parse hex string into it's corresponding value
 * return -1 if the given str is not valid
 */
int64_t _parseHexStr(const char *buf, int len) {
  int64_t out = 0;
  int val = 0;
  for (int i = 0; i < len; ++i) {
    out <<= 4;
    val = _hexChar2Int(buf[i]);
    if (val == -1) {
      return -1;
    }
    out += (int64_t)val;
  }
  return out;
}

/* Align 'n' up to the value 'align', which must be a power of two.
  Reference:
  https://github.com/SEL4PROJ/libcpio/blob/master/src/cpio.c
*/
uintptr_t _alignUp(uintptr_t n, unsigned long align) {
  return (n + align - 1) & (~(align - 1));
}

int _cpioParseHeader(CpioNewcHeader *header, const char **filename,
                     uint64_t *_filesize, void **data, CpioNewcHeader **next) {
  if (strncmp(header->magic, CPIO_HEADER_MAGIC, 6)) {
    log_println("  [parseHeader] uncorrect header magic field");
    return 1;
  }

  int64_t filesize, namesize;
  if (((filesize = _parseHexStr(header->filesize, 8)) == -1) ||
      ((namesize = _parseHexStr(header->namesize, 8)) == -1)) {
    return -1;
  }
  log_println("  [parseHeader] found entry: size:%d, namesize:%d", filesize,
              namesize);

  // Get filename && filesize
  *filename = ((char *)header) + sizeof(CpioNewcHeader);

  // Ensure this file is not the trailer in CPIO indicating EOF.
  if (strncmp(*filename, CPIO_FOOTER_MAGIC, sizeof(CPIO_FOOTER_MAGIC)) == 0) {
    log_println("  [parseHeader] hit cpio footer");
    return -1;
  }

  // Find offset to data.
  *data = (void *)_alignUp(
      ((uintptr_t)header) + sizeof(CpioNewcHeader) + namesize, 4);
  *next = (CpioNewcHeader *)_alignUp(((uintptr_t)*data) + filesize, 4);

  // Return filesize if requested
  if (_filesize) {
    *_filesize = filesize;
  }
  return 0;
}
