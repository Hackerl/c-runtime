#include <z_std.h>
#include <limits.h>

size_t z_strlen(const char * str) {
    int n = 0;

    while (*str++)
        n++;

    return n;
}

int z_strncmp(const char * s1, const char * s2, size_t n) {
    for (size_t i = 0; i < n && *s1 && *s2; i++)
        if (s1[i] != s2[i])
            return -1;

    return 0;
}

int z_strcmp(const char * s1, const char * s2) {
    size_t l1 = z_strlen(s1);
    size_t l2 = z_strlen(s2);

    if (l1 == l2 && !z_strncmp(s1, s2, l1))
        return 0;

    return -1;
}

void *z_memcpy(void *dst, const void *src, size_t n) {
    unsigned char *d = (unsigned char *) dst;
    unsigned char *s = (unsigned char *) src;

    while (n--)
        *d++ = *s++;

    return dst;
}

int z_memcmp(const void *s1, const void *s2, size_t n) {
    unsigned char *d = (unsigned char *) s1;
    unsigned char *s = (unsigned char *) s2;

    while (n-- > 0)
        if (*d++ != *s++)
            return 1;

    return 0;
}

void *z_memset(void *s, unsigned char c, size_t n) {
    unsigned char *p = (unsigned char *)s;

    while (n--)
        *p++ = c;

    return s;
}

void *z_memmem(void *haystack, size_t n, void *needle, size_t m) {
    for (int i = 0; i < n - m; i++) {
        if (!z_memcmp((char *)haystack + i, needle, m))
            return (char *)haystack + i;
    }

    return NULL;
}

int z_isspace(int c) {
    return c == ' ' || (unsigned)c - '\t' < 5;
}

int z_isdigit(int c) {
    return (unsigned)c - '0' < 10;
}

int z_isalpha(int c) {
    return ((unsigned)c | 32U) - 'a' < 26;
}

int z_isupper(int c) {
    return (unsigned)c - 'A' < 26;
}

long int z_strtol(const char *str, char **endptr, int base) {
    const unsigned char *s = (const unsigned char *) str;
    unsigned long acc;
    int c;
    unsigned long cutoff;
    int neg = 0;
    int any;
    int cutlim;

    if (base < 0 || base > 36) {
        if (endptr) {
            *endptr = (char *) str;
        }

        return 0;
    }

    do {
        c = *s++;
    } while (z_isspace(c));

    if (c == '-') {
        neg = 1;
        c = *s++;
    } else if (c == '+') {
        c = *s++;
    }

    if ((base == 0 || base == 16) && c == '0' && (*s == 'x' || *s == 'X')) {
        c = s[1];
        s += 2;
        base = 16;
    } else if ((base == 0 || base == 2) && c == '0' && (*s == 'b' || *s == 'B')) {
        c = s[1];
        s += 2;
        base = 2;
    }

    if (base == 0) {
        base = c == '0' ? 8 : 10;
    }

    cutoff = neg ? -(unsigned long) LONG_MIN : LONG_MAX;
    cutlim = (int) (cutoff % (unsigned long) base);
    cutoff /= (unsigned long) base;

    for (acc = 0, any = 0;; c = *s++) {
        if (z_isdigit(c)) {
            c -= '0';
        } else if (z_isalpha(c)) {
            c -= z_isupper(c) ? 'A' - 10 : 'a' - 10;
        } else {
            break;
        }

        if (c >= base) {
            break;
        }

        if (any < 0 || acc > cutoff || (acc == cutoff && c > cutlim)) {
            any = -1;
        } else {
            any = 1;
            acc *= (unsigned long) base;
            acc += (unsigned long) c;
        }
    }

    if (any < 0) {
        acc = neg ? (unsigned long) LONG_MIN : (unsigned long) LONG_MAX;
    } else if (neg) {
        acc = -acc;
    }

    if (endptr != 0) {
        *endptr = (char *) (any ? (const char *) s - 1 : str);
    }

    return (long) (acc);
}

unsigned long int z_strtoul(const char *str, char **endptr, int base) {
    const unsigned char *s = (const unsigned char *) str;
    unsigned long acc;
    int c;
    unsigned long cutoff;
    int neg = 0;
    int any;
    int cutlim;

    do {
        c = *s++;
    } while (z_isspace(c));

    if (c == '-') {
        neg = 1;
        c = *s++;
    } else if (c == '+') {
        c = *s++;
    }

    if ((base == 0 || base == 16) && c == '0' && (*s == 'x' || *s == 'X')) {
        c = s[1];
        s += 2;
        base = 16;
    } else if ((base == 0 || base == 2) && c == '0' && (*s == 'b' || *s == 'B')) {
        c = s[1];
        s += 2;
        base = 2;
    }

    if (base == 0) {
        base = c == '0' ? 8 : 10;
    }

    cutoff = (unsigned long) ULONG_MAX / (unsigned long) base;
    cutlim = (int) ((unsigned long) ULONG_MAX % (unsigned long) base);

    for (acc = 0, any = 0;; c = *s++) {
        if (z_isdigit(c)) {
            c -= '0';
        } else if (z_isalpha(c)) {
            c -= z_isupper(c) ? 'A' - 10 : 'a' - 10;
        } else {
            break;
        }

        if (c >= base) {
            break;
        }

        if (any < 0 || acc > cutoff || (acc == cutoff && c > cutlim)) {
            any = -1;
        } else {
            any = 1;
            acc *= (unsigned long) base;
            acc += (unsigned long) c;
        }
    }

    if (any < 0) {
        acc = ULONG_MAX;
    } else if (neg) {
        acc = -acc;
    }

    if (endptr != NULL) {
        *endptr = (char *) (any ? (const char *) s - 1 : str);
    }

    return acc;
}