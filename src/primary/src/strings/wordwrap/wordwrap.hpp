#ifndef _WORDWRAP_HPP_
#define _WORDWRAP_HPP_
#include <new>

const char *wordWrap(const char *src, char *buffer, const int bufferSize);

#define MAX_WORDWRAP_LINE_MARKS 20

class WordWrap {
protected:
  const char *lineMarks[MAX_WORDWRAP_LINE_MARKS];
  int totalLines;
  const char *src;
  char *buffer;
  int bufferSize;
  const char *prefix;
  int prefixLength;
  int prefixStep;

public:
  WordWrap(const char *src, char *buffer, const int bufferSize);
  WordWrap(const char *src, char *buffer, const int bufferSize, const char *prefix, const int prefixStep);

public:
  WordWrap& operator=(const WordWrap &wordWrap);

public:
  WordWrap(const WordWrap &wordWrap);
  ~WordWrap();

protected:
  void resetLineMarks();
  void wordWrap();

public:
  const char *getLine(int lineIndex);
  int getTotalLines();
};

#endif
