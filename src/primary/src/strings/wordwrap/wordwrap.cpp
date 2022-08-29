#include "wordwrap.hpp"
#include <string.h>
#include <avr/pgmspace.h>

const char *findStartOfWord(const char *src);
const char *findEndOfWord(const char *src);

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

    bool paddingSpace = currentLength != 0;
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

WordWrap::WordWrap(const char *src, char *buffer, const int bufferSize) :
  WordWrap(src, buffer, bufferSize, NULL, 0) {

}

WordWrap::WordWrap(
  const char *src,
  char *buffer,
  const int bufferSize,
  const char *prefix,
  const int prefixStep) :
    lineMarks({ NULL }),
    totalLines(0),
    src(src),
    buffer(buffer),
    bufferSize(bufferSize),
    prefix(prefix),
    prefixLength(prefix == NULL ? 0 : strlen(prefix)),
    prefixStep(prefixStep) {
  
  this->wordWrap();
}

/*
void* WordWrap::operator new(std::size_t size, void *ptr) {
  return NULL;
}
*/

WordWrap& WordWrap::operator=(const WordWrap &other) {
  if (this == &other) {
    return *this;
  }

  this->resetLineMarks();
  this->totalLines = 0;
  this->src = other.src;
  this->buffer = other.buffer;
  this->bufferSize = other.bufferSize;
  this->prefix = other.prefix;
  this->prefixLength = other.prefixLength;
  this->prefixStep = other.prefixStep;
  
  this->wordWrap();
  return *this;
}

WordWrap::WordWrap(const WordWrap &wordWrap) :
  WordWrap(
    wordWrap.src,
    wordWrap.buffer,
    wordWrap.bufferSize,
    wordWrap.prefix,
    wordWrap.prefixStep) {

}

WordWrap::~WordWrap() {
  
}

void WordWrap::resetLineMarks() {
  for (int index = 0; index < MAX_WORDWRAP_LINE_MARKS; ++index) {
    this->lineMarks[index] = NULL;
  }
}

void WordWrap::wordWrap() {
  const char *next = this->src;
  this->totalLines = 0;
  int lineIndex = 0;
  while (next != NULL) {
    ++this->totalLines;
    if (lineIndex >= MAX_WORDWRAP_LINE_MARKS) {  
      break;
    }

    this->lineMarks[lineIndex] = next;    
    
    char *tmpBuffer = this->buffer;
    int tmpBufferSize = this->bufferSize;

    if (this->prefix != NULL) {
      if (lineIndex % this->prefixStep == 0) {
        tmpBuffer = this->buffer + this->prefixLength;
        tmpBufferSize = this->bufferSize - this->prefixLength;
      }
    }

    next = ::wordWrap(next, tmpBuffer, tmpBufferSize);

    ++lineIndex;
  }
}

const char *WordWrap::getLine(int lineIndex) {
  memset(this->buffer, '\0', this->bufferSize);

  if (lineIndex >= this->totalLines) {
    return NULL;
  }

  const char *tmpSrc = this->lineMarks[lineIndex];
  char *tmpBuffer = this->buffer;
  int tmpBufferSize = this->bufferSize;
  
  if (this->prefix != NULL) {
    if (lineIndex % this->prefixStep == 0) {
      tmpBuffer = this->buffer + this->prefixLength;
      tmpBufferSize = this->bufferSize - this->prefixLength;

      strcpy(this->buffer, this->prefix);
    }
  }

  ::wordWrap(tmpSrc, tmpBuffer, tmpBufferSize);

  return tmpSrc;
}

int WordWrap::getTotalLines() {
  return this->totalLines;
}
