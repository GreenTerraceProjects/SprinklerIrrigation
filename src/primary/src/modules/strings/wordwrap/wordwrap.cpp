#include "wordwrap.hpp"
#include <avr/pgmspace.h>
#include <string.h>

const char *wordWrap(const char *src, char *buffer, const int bufferSize) {
  memset(buffer, '\0', bufferSize);
  const int bufferMaxNumCharacters = bufferSize - 1;

  int currentLength = 0;
  while (true) {
    const char *startOfWord = NULL;
    const char *endOfWord = NULL;
    startOfWord = findStartOfWord(src);
    if (startOfWord == NULL) {
      return NULL;
    }
    endOfWord = findEndOfWord(startOfWord);
    if (endOfWord == NULL) {
      return NULL;
    }

    int wordLength = endOfWord - startOfWord + 1;

    bool paddingSpace = (currentLength != 0);
    if (currentLength + (int)paddingSpace + wordLength <= bufferMaxNumCharacters) {
      if (paddingSpace) {
        buffer[currentLength] = ' ';
        ++currentLength;
      }

      strncat_P(buffer, startOfWord, wordLength);
      currentLength += wordLength;
      src = startOfWord + wordLength;
    } else {
      if (wordLength > bufferMaxNumCharacters) {
        if (currentLength == 0) {
          const int numChars = bufferMaxNumCharacters;
          strncat_P(buffer, startOfWord, numChars);
          src = startOfWord + numChars;
        }
      } else {
        src = startOfWord;
      }
      break;
    }
  }

  return src;
}

const char *findStartOfWord(const char *src) {
  if (src == NULL) return NULL;

  while (true) {
    unsigned char character = pgm_read_byte(src);
    
    if (character == '\0') {
      return NULL;
    }

    if (character != ' ') {
      return src;
    }

    ++src;
  }
}

const char *findEndOfWord(const char *src) {
  if (src == NULL) return NULL;

  unsigned char character = pgm_read_byte(src);
  if (character == '\0') {
    return NULL;
  }

  if (character == ' ') {
    return src;
  }

  while (true) {
    ++src;

    character = pgm_read_byte(src);
    if (character == ' ' || character == '\0') {
      return src - 1;
    }
  }
}

const char *wordWrapReversed(const char *origin, const char *src, char *buffer, const int bufferSize) {
  memset(buffer, '\0', bufferSize);
  const int bufferMaxNumCharacters = bufferSize - 1;
  buffer[bufferMaxNumCharacters] = '\0';

  if (src == NULL) return NULL;

  int currentLength = 0;
  int bufferCaret = bufferMaxNumCharacters - 1;

  while (true) {
    const char *endOfWord = findPreviousWordEnd(origin, src - 1);
    if (endOfWord == NULL) break;
    const char *startOfWord = findPreviousWordStart(origin, endOfWord);
    if (startOfWord == NULL) break;

    int wordLength = (endOfWord - startOfWord) + 1;

    bool paddingSpace = (currentLength != 0 && currentLength < bufferMaxNumCharacters);
    if (paddingSpace) {
      --bufferCaret;
      buffer[bufferCaret] = ' ';
      --bufferCaret;
      ++currentLength;
    }

    if (currentLength + wordLength <= bufferMaxNumCharacters) {
      bufferCaret -= (wordLength - 1);
      memcpy_P(buffer + bufferCaret, startOfWord, wordLength);
      currentLength += wordLength;
      src = endOfWord - (wordLength - 1);
    } else {
      if (wordLength > bufferMaxNumCharacters) {
        int maxNumChars = bufferMaxNumCharacters - currentLength;
        int numChars = wordLength % bufferMaxNumCharacters;
        if (numChars > maxNumChars || numChars <= 0) {
          numChars = maxNumChars;
        }

        bufferCaret -= numChars - 1;        
        memcpy_P(buffer + bufferCaret, endOfWord - (numChars - 1), numChars);
        src = endOfWord - (numChars - 1);
      } else {
        src = findStartOfWord(endOfWord + 1);
      }
      break;
    }
  }

  const char *firstWord = buffer;
  for (int i = 0; i < bufferSize; ++i) {
    if (buffer[i] != ' ' && buffer[i] != '\0') {
      firstWord = buffer + i;
      break;
    }
  }

  int numChars = strlen(firstWord);
  memmove(buffer, firstWord, numChars);
  memset(buffer + numChars, '\0', bufferMaxNumCharacters - numChars + 1);

  if (src == origin) {
    src = NULL;
  }

  return src;
}

const char *findPreviousWordEnd(const char* origin, const char *src) {
  if (src == NULL) return NULL;

  while (true) {
    if (src < origin) {
      return NULL;
    }

    unsigned char character = pgm_read_byte(src);

    if (character != ' ') {
      return src;
    }

    --src;
  }
}

const char *findPreviousWordStart(const char *origin, const char *src) {
  if (src == NULL) return NULL;

  while (true) {
    if (src < origin) {
      return origin;
    }

    unsigned char character = pgm_read_byte(src);

    if (character == ' ') {
      return src + 1;
    }

    --src;
  }
}

/*
0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2
D e f a u l t   I r r i g a t i o n   M o d e

0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5
D e f a u l t     D e f a u l t

0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6
                        M o d e \0

Default Irrigation Mode

index 2 - index 1 + 1 = numChars
numChars - 1 + indexx

AAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA

AAAAAAAAAAAAAAAA

QRSTUVWXYZABC

1000000. AAAAAAA
AAAAAAAA AAAAAAA

ABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIKL Default Irrigation Mode AA
*/

WordWrap::WordWrap(
  const char *src,
  char *const buffer,
  const int bufferSize)
  :
  src(src),
  buffer(buffer), bufferSize(bufferSize) {
  
  this->currentLine = 0;
}

PrefixWordWrap::PrefixWordWrap(
  const char *src,
  char *const buffer,
  const int bufferSize,
  const char *prefix,
  const int prefixStep)
  :
  prefix(prefix),
  prefixStep(prefixStep),
  prefixLength(strlen(prefix)),
  WordWrap(src, buffer, bufferSize) {
  
}

bool WordWrap::getLine(int expectedLine) {
  if (expectedLine <= 0) return false;

  const char *tmpSrc = this->src;
  
  int line = 0;
  while (line < expectedLine) {
    ++line;
    
    const char *next = wordWrap(tmpSrc, this->buffer, this->bufferSize);
    if (next == NULL) {
      break;
    }

    tmpSrc = next;
  }

  return (line == expectedLine);
}

bool WordWrap::goNext() {
  int expectedLine = this->currentLine + 1;
  if (this->getLine(expectedLine)) {
    ++this->currentLine;
    return true;
  }

  return false;
}

bool WordWrap::goBack() {
  int expectedLine = this->currentLine - 1;
  if (this->getLine(expectedLine)) {
    --this->currentLine;
    return true;
  }

  return false;
}

bool WordWrap::goTo(int line) {
  if (this->getLine(line)) {
    this->currentLine = line;
    return true;
  }

  return false;
}

void WordWrap::refill() {
  if (this->currentLine == 0) {
    this->currentLine = 1;
  }

  this->getLine(this->currentLine);
}

int WordWrap::getCurrentLineNumber() {
  return this->currentLine;
}

bool PrefixWordWrap::getLine(int expectedLine) {
  if (expectedLine <= 0) return false;

  const char *tmpSrc = this->src;
  
  int line = 0;
  while (line < expectedLine) {
    ++line;

    bool shift = (this->prefixStep == 1) ||
                 ((line % this->prefixStep) == 1);
    if (shift) {
      strcpy(this->buffer, this->prefix);
    }

    int tmpBufferSize = this->bufferSize - (shift ? this->prefixLength : 0);
    char *tmpBuffer = this->buffer + (shift ? this->prefixLength : 0);
    const char *next = wordWrap(tmpSrc, tmpBuffer, tmpBufferSize);
    if (next == NULL) {
      break;
    }

    tmpSrc = next;
  }

  return (line == expectedLine);
}
