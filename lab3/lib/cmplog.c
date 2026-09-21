#include <sys/stat.h>

#include <errno.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// This bounded, single-threaded runtime is provided to students. Instrumented
// target code calls the two public hooks; this file is compiled separately.
enum {
  CMP_LOG_MAX_BYTES = 64,
  CMP_LOG_MAX_RECORDS = 4096
};

struct ComparisonRecord {
  uint32_t kind;
  uint32_t width;
  size_t lhs_size;
  size_t rhs_size;
  unsigned char lhs[CMP_LOG_MAX_BYTES];
  unsigned char rhs[CMP_LOG_MAX_BYTES];
};

static int __cmp_log_fd = -1;
static size_t __cmp_log_count = 0;
static struct ComparisonRecord __cmp_log_records[CMP_LOG_MAX_RECORDS];

__attribute__((constructor)) static void __cmp_log_init(void) {
  int saved_errno = errno;
  const char* path = getenv("LAB3_CMPLOG_PATH");
  if (path != NULL && path[0] != '\0') {
    __cmp_log_fd =
        open(path, O_WRONLY | O_CREAT | O_TRUNC | O_CLOEXEC | O_NONBLOCK, 0600);
    // A log is a regular file. Refusing pipes and devices also prevents a bad
    // destination from hanging the target or delivering SIGPIPE to it.
    struct stat status;
    if (__cmp_log_fd >= 0 &&
        (fstat(__cmp_log_fd, &status) < 0 || !S_ISREG(status.st_mode))) {
      close(__cmp_log_fd);
      __cmp_log_fd = -1;
    }
  }
  errno = saved_errno;
}

__attribute__((destructor)) static void __cmp_log_finish(void) {
  int saved_errno = errno;
  if (__cmp_log_fd >= 0) {
    close(__cmp_log_fd);
    __cmp_log_fd = -1;
  }
  errno = saved_errno;
}

static size_t __cmp_log_hex(char* output, const unsigned char* bytes, size_t size) {
  static const char digits[] = "0123456789abcdef";
  if (size == 0) {
    output[0] = '-';
    return 1;
  }
  for (size_t index = 0; index < size; ++index) {
    output[2 * index] = digits[bytes[index] >> 4];
    output[2 * index + 1] = digits[bytes[index] & 15];
  }
  return 2 * size;
}

static void __cmp_log_emit(const struct ComparisonRecord* record) {
  for (size_t index = 0; index < __cmp_log_count; ++index) {
    const struct ComparisonRecord* previous = &__cmp_log_records[index];
    if (previous->kind == record->kind && previous->width == record->width &&
        previous->lhs_size == record->lhs_size &&
        previous->rhs_size == record->rhs_size &&
        memcmp(previous->lhs, record->lhs, record->lhs_size) == 0 &&
        memcmp(previous->rhs, record->rhs, record->rhs_size) == 0) {
      return;
    }
  }

  static const char* const names[] = {"icmp", "strcmp", "strncmp", "memcmp"};
  char line[4 * CMP_LOG_MAX_BYTES + 32];
  size_t length = strlen(names[record->kind]);
  memcpy(line, names[record->kind], length);
  line[length++] = '\t';
  if (record->width >= 10) {
    line[length++] = '0' + record->width / 10;
  }
  line[length++] = '0' + record->width % 10;
  line[length++] = '\t';
  length += __cmp_log_hex(line + length, record->lhs, record->lhs_size);
  line[length++] = '\t';
  length += __cmp_log_hex(line + length, record->rhs, record->rhs_size);
  line[length++] = '\n';

  // Unbuffered writes make every complete record visible immediately, including
  // when the target later crashes or calls _exit. Disable logging on I/O errors.
  size_t written = 0;
  while (written < length) {
    ssize_t result = write(__cmp_log_fd, line + written, length - written);
    if (result < 0 && errno == EINTR) {
      continue;
    }
    if (result <= 0) {
      __cmp_log_finish();
      return;
    }
    written += (size_t)result;
  }
  __cmp_log_records[__cmp_log_count++] = *record;
}

void __cmp_log_int(uint32_t width, uint64_t lhs, uint64_t rhs) {
  if (__cmp_log_fd < 0 || __cmp_log_count == CMP_LOG_MAX_RECORDS ||
      (width != 8 && width != 16 && width != 32 && width != 64)) {
    return;
  }
  int saved_errno = errno;
  struct ComparisonRecord record = {0};
  record.width = width;
  record.lhs_size = record.rhs_size = width / 8;
  // The wire format is always little endian, independent of host byte order.
  for (size_t index = 0; index < record.lhs_size; ++index) {
    record.lhs[index] = (unsigned char)(lhs >> (8 * index));
    record.rhs[index] = (unsigned char)(rhs >> (8 * index));
  }
  __cmp_log_emit(&record);
  errno = saved_errno;
}

static size_t __cmp_log_read(
    unsigned char* output, const void* input, size_t limit, int stop_at_nul) {
  const unsigned char* bytes = input;
  size_t size = 0;
  while (size < limit) {
    unsigned char byte = bytes[size];
    if (stop_at_nul && byte == 0) {
      break;
    }
    output[size++] = byte;
  }
  return size;
}

void __cmp_log_bytes(uint32_t kind, const void* lhs, const void* rhs, uint64_t n) {
  if (__cmp_log_fd < 0 || __cmp_log_count == CMP_LOG_MAX_RECORDS || kind < 1 ||
      kind > 3) {
    return;
  }
  int saved_errno = errno;
  struct ComparisonRecord record = {0};
  record.kind = kind;
  size_t limit = (kind == 1 || n > CMP_LOG_MAX_BYTES) ? CMP_LOG_MAX_BYTES : (size_t)n;
  record.lhs_size = __cmp_log_read(record.lhs, lhs, limit, kind != 3);
  record.rhs_size = __cmp_log_read(record.rhs, rhs, limit, kind != 3);
  __cmp_log_emit(&record);
  errno = saved_errno;
}
