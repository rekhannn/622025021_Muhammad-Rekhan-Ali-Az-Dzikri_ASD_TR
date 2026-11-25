#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#ifdef _WIN32
#include <direct.h>
#define MKDIR(path) _mkdir(path)
#else
#include <sys/stat.h>
#include <sys/types.h>
#define MKDIR(path) mkdir(path, 0777)
#endif

#include "modules.h"

void ensure_data_dir(void) {
    MKDIR("data");
}

void clear_stdin_buffer(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {
        /* buang sisa karakter di buffer */
    }
}

void trim_newline(char *str) {
    size_t len;
    if (str == NULL) {
        return;
    }
    len = strlen(str);
    while (len > 0 && (str[len - 1] == '\n' || str[len - 1] == '\r')) {
        str[len - 1] = '\0';
        len--;
    }
}

int is_blank_or_whitespace(const char *str) {
    size_t i;
    if (str == NULL) {
        return 1;
    }
    for (i = 0; str[i] != '\0'; i++) {
        if (!isspace((unsigned char)str[i])) {
            return 0;
        }
    }
    return 1;
}

int read_line_safe(char *buffer, int size) {
    if (buffer == NULL || size <= 1) {
        return 0;
    }
    if (fgets(buffer, size, stdin) == NULL) {
        buffer[0] = '\0';
        return 0;
    }
    if (strchr(buffer, '\n') == NULL) {
        clear_stdin_buffer();
    }
    trim_newline(buffer);
    return 1;
}

int read_int_safe(const char *prompt, int *out_val) {
    char line[MAX_LINE];
    char extra;
    if (out_val == NULL) {
        return 0;
    }
    if (prompt != NULL) {
        printf("%s", prompt);
    }
    if (!read_line_safe(line, sizeof(line)) || is_blank_or_whitespace(line)) {
        return 0;
    }
    if (sscanf(line, " %d %c", out_val, &extra) != 1) {
        return 0;
    }
    return 1;
}

int read_double_safe(const char *prompt, double *out_val) {
    char line[MAX_LINE];
    char extra;
    if (out_val == NULL) {
        return 0;
    }
    if (prompt != NULL) {
        printf("%s", prompt);
    }
    if (!read_line_safe(line, sizeof(line)) || is_blank_or_whitespace(line)) {
        return 0;
    }
    if (sscanf(line, " %lf %c", out_val, &extra) != 1) {
        return 0;
    }
    return 1;
}

void str_to_lower_copy(char *dst, const char *src, size_t cap) {
    size_t i;
    for (i = 0; i + 1 < cap && src[i] != '\0'; i++) {
        dst[i] = (char)tolower((unsigned char)src[i]);
    }
    dst[i] = '\0';
}