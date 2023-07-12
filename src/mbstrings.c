#include "mbstrings.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include "mbstrings.h"

/* mbslen - multi-byte string length
 * - Description: returns the number of UTF-8 code points ("characters")
 * in a multibyte string. If the argument is NULL or an invalid UTF-8
 * string is passed, returns -1.
 *
 * - Arguments: A pointer to a character array (`bytes`), consisting of UTF-8
 * variable-length encoded multibyte code points.
 *
 * - Return: returns the actual number of UTF-8 code points in `src`. If an
 * invalid sequence of bytes is encountered, return -1.
 *
 * - Hints:
 * UTF-8 characters are encoded in 1 to 4 bytes. The number of leading 1s in the
 * highest order byte indicates the length (in bytes) of the character. For
 * example, a character with the encoding 1111.... is 4 bytes long, a character
 * with the encoding 1110.... is 3 bytes long, and a character with the encoding
 * 1100.... is 2 bytes long. Single-byte UTF-8 characters were designed to be
 * compatible with ASCII. As such, the first bit of a 1-byte UTF-8 character is
 * 0.......
 *
 * You will need bitwise operations for this part of the assignment!
 */
size_t mbslen(const char* bytes)  {
    if (!bytes) {
        return -1;
    }

    size_t numCodePoints = 0;
    const char* currByte = bytes;

    while (*currByte != '\0') {
        int byteCount = 0;
        unsigned char byte = (unsigned char) *currByte;

        if ((byte & 0b10000000) == 0) {
            byteCount = 1;
        } else if ((byte & 0b11100000) == 0b11000000) {
            byteCount = 2;
        } else if ((byte & 0b11110000) == 0b11100000) {
            byteCount = 3;
        } else if ((byte & 0b11111000) == 0b11110000) {
            byteCount = 4;
        } else {
            return -1;
        }

        for (int i = 1; i < byteCount; i++) {
            if ((unsigned char) currByte[i] >> 6 != 0b10) {
                return -1;
            }
        }

        currByte += byteCount;
        numCodePoints++;
    }

    return numCodePoints;
}
