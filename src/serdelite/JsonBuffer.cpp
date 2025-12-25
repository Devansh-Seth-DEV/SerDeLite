#include "serdelite/JsonBuffer.h"

#include <cstdio>

namespace serdelite {

JsonBuffer::JsonBuffer(const char* _data, size_t len)
    : data(_data), length(len)
{

}

void JsonBuffer::printPretty(int tabWidth) const  {
    if (!this->data || this->length == 0) return;

    int indentLevel = 0;
    bool inQuotes = false;

    for (size_t i = 0; i < this->length; i++) {
        char c = this->data[i];

        // Handle string literals: don't format characters inside " "
        // Also handles escaped quotes \" so they don't toggle the inQuotes flag
        if (c == '\"' &&
            (i == 0 || this->data[i - 1] != '\\')) {
            inQuotes = !inQuotes;
            putchar(c);
            continue;
        }

        if (inQuotes) {
            putchar(c);
            continue;
        }

        switch (c) {
        case '{':
        case '[':
            putchar(c);
            putchar('\n');
            printIndent(++indentLevel, tabWidth);
            break;

        case '}':
        case ']':
            putchar('\n');
            printIndent(--indentLevel, tabWidth);
            putchar(c);
            break;

        case ',':
            putchar(c);
            putchar('\n');
            printIndent(indentLevel, tabWidth);
            break;

        case ':':
            putchar(c);
            putchar(' '); // Space for readability after key
            break;

        // Ignore existing whitespace to re-format cleanly
        case ' ':
        case '\t':
        case '\n':
        case '\r':
            break;

        default:
            putchar(c);
            break;
        }
    }

    putchar('\n');
}

void JsonBuffer::printIndent(int level, int tabWidth) const {
    if (level < 0) return;
    int totalSpaces = level * tabWidth;
    for (int i = 0; i < totalSpaces; i++) putchar(' ');
}

}