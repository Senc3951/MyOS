#include <kernel/dev/display/vga.h>
#include <kernel/dev/keyboard/translator.h>

#define BASIC_KEY_START 0x0D
#define BASIC_KEY_END   0x5E

const char g_LETTERS_NUMBERS[] = {
    '`',
    NoKey, NoKey, NoKey, NoKey, NoKey, NoKey,
    'q',
    '1',
    NoKey, NoKey, NoKey,
    'z',
    's',
    'a',
    'w',
    '2',
    NoKey, NoKey,
    'c',
    'x',
    'd',
    'e',
    '4',
    '3',
    NoKey, NoKey,
    ' ',
    'v',
    'f',
    't',
    'r',
    '5',
    NoKey, NoKey,
    'n',
    'b',
    'h',
    'g',
    'y',
    '6',
    NoKey, NoKey, NoKey,
    'm',
    'j',
    'u',
    '7',
    '8',
    NoKey, NoKey,
    ',',
    'k',
    'i',
    'o',
    '0',
    '9',
    NoKey, NoKey,
    '.',
    '/',
    'l',
    ';',
    'p',
    '-',
    NoKey, NoKey, NoKey,
    '\'',
    NoKey,
    '[',
    '=',
    NoKey, NoKey, NoKey, NoKey,
    '\n',
    ']',
    NoKey,
    '\\'
};
const char g_SYMBOLS[] = {
    ')',
    '!',
    '@',
    '#',
    '$',
    '%',
    '^',
    '&',
    '*',
    '('
};

char translate(const uint8_t code, const bool isCaps, const bool isShift)
{
    if (code > BASIC_KEY_START && code < BASIC_KEY_END)
    {
        char c = g_LETTERS_NUMBERS[code - BASIC_KEY_START - 1];
        if (c != NoKey)
        {
            if (isShift)
            {
                switch (c)
                {
                    case '0':
                    case '1':
                    case '2':
                    case '3':
                    case '4':
                    case '5':
                    case '6':
                    case '7':
                    case '8':
                    case '9':
                        return g_SYMBOLS[c - '0'];
                    case '-':
                        return '_';
                    case '=':
                        return '+';
                    case '\n':
                        return '\n';
                    case ' ':
                        return ' ';
                    case '\\':
                        return '|';
                    case '`':
                        return '~';
                    case ';':
                        return ':';
                    case '\'':
                        return '"';
                    case ',':
                        return '<';
                    case '.':
                        return '>';
                    case '/':
                        return '?';
                    case '[':
                        return '{';
                    case ']':
                        return '}';
                    default:
                        return c - 32;
                }
            }
            if (isCaps)
            {
                if (c >= 'a' && c <= 'z')
                    return c - 32;
            }
            
            return c;
        }
    }
    else
    {
        if (isShift)
        {
            if (code == TAB)
            {
                for (size_t i = 0; i < TAB_LENGTH; i++)
                    delc();
                
                return NoKey;
            }
        }
        else
        {
            if (code == TAB)
                return '\t';
        }
    }
    
    return NoKey;   
}