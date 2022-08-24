#ifndef _CUSTOM_MODE_ENTRIES_HPP_
#define _CUSTOM_MODE_ENTRIES_HPP_

#include "../../inputs/input.hpp"
#include "custom-mode-entry/custom-mode-entry-input.hpp"
#include "custom-mode-entry/custom-mode-entry-reader.hpp"
#include "custom-mode-entry/custom-mode-entry-writer.hpp"

#define MAX_NUM_ENTRIES 24

class CustomModeEntries : public Input {
public:
    CustomModeEntries();

protected:
    int8_t entries[MAX_NUM_ENTRIES];
    int8_t index;
    uint32_t currentStartAt;
    uint32_t currentDuration;

protected:
    void initialize() override;
    bool handleKeyPressed(char inputKey) override;

protected:
    void readCurrentEntry();
    void writeCurrentEntry();
    bool inputCurrentEntry();
    void viewCurrentEntry();

protected:
    int8_t getRealIndex();
    int8_t getSortedIndex(int8_t realIndex);

protected:
    static int comparator(const void *a, const void *b);
    void sortEntries();
};

#endif
