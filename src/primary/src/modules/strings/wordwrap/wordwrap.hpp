#ifndef _WORDWRAP_HPP_
#define _WORDWRAP_HPP_

const char *wordWrap(const char *src, char *buffer, const int bufferSize);
const char *findStartOfWord(const char *src);
const char *findEndOfWord(const char *src);

const char *wordWrapReversed(const char *origin, const char *src, char *buffer, const int bufferSize);
const char *findPreviousWordEnd(const char* origin, const char *src);
const char *findPreviousWordStart(const char *origin, const char *src);

enum WordWrapMovingDirection {
    NotChange,
    Forward,
    Backward
};

class WordWrap {
protected:
  int currentLine;

protected:
  const char *src;
  char *const buffer;
  const int bufferSize;

public:
  WordWrap (
    const char *src,
    char *const buffer,
    const int bufferSize);

public:
  bool goNext();
  bool goBack();
  bool goTo(int line);
  void refill();
  int getCurrentLineNumber();

protected:
  virtual bool getLine(int expectedLine);
};

class PrefixWordWrap : public WordWrap {

protected:
    const char *prefix;
    const int prefixLength;
    const int prefixStep;

public:
  PrefixWordWrap(
    const char *src,
    char *const buffer,
    const int bufferSize,
    const char *prefix,
    const int prefixStep);  

protected:
  bool getLine(int expectedLine) override;
  
};

#endif
