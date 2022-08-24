#include "../../devices/lcd/lcd.hpp"
#include "../../devices/keypad/keypad.hpp"
#include "custom-mode-entries.hpp"
#include "custom-mode-entry/custom-mode-entry-label.hpp"

CustomModeEntries::CustomModeEntries()
    : entries { 0 }, index(0), Input(&keypad) {

}

void CustomModeEntries::initialize() {
    memset(this->entries, 0, MAX_NUM_ENTRIES * sizeof(int8_t));
    this->index = 0;

    for (int8_t index = 0; index < MAX_NUM_ENTRIES; ++index) {
        this->entries[index] = index;
    }

    this->viewCurrentEntry();
}

bool CustomModeEntries::handleKeyPressed(char inputKey) {
    switch (inputKey) {
        case '2':
            --this->index;
            if (this->index < 0) {
                this->index = MAX_NUM_ENTRIES - 1;
            }
            this->readCurrentEntry();
            break;
        case '8':
            ++this->index;
            if (this->index > MAX_NUM_ENTRIES - 1) {
                this->index = 0;
            }
            this->readCurrentEntry();
            break;
        case '5':
            if (this->inputCurrentEntry()) {
                this->writeCurrentEntry();

                int8_t realIndex = this->getRealIndex();
                this->sortEntries();
                this->index = this->getSortedIndex(realIndex);
            }
            break;
        case '*':
            return false;
            break;
        default:
            break;
    }

    this->viewCurrentEntry();
    return true;
}

void CustomModeEntries::viewCurrentEntry() {
    CustomModeEntryLabelArgs customModeEntryLabelArgs {
        .index = this->getRealIndex(),
        .startAt = this->currentStartAt,
        .duration = this->currentDuration
    };
    CustomModeEntryLabel customModeEntryLabel(customModeEntryLabelArgs);
    customModeEntryLabel.view();
}

bool CustomModeEntries::inputCurrentEntry() {
    bool hasChanged = false;
    CustomModeEntryInputArgs inputArgs {
        .index = this->getRealIndex(),
        .defaultStartAt = this->currentStartAt,
        .defaultDuration = this->currentDuration
    };
    CustomModeEntryInput entryInput(inputArgs);
    entryInput.run();
    if (!entryInput.isCanceled()) {
        auto inputResult = entryInput.getResult();

        if (inputResult.startAt != this->currentStartAt ||
            inputResult.duration != this->currentDuration) {
        
            hasChanged = true;
        }

        this->currentStartAt = inputResult.startAt;
        this->currentDuration = inputResult.duration;
    }

    return hasChanged;
}

void CustomModeEntries::readCurrentEntry() {
    CustomModeEntryReaderArgs args {
        .index = this->getRealIndex()
    };
    CustomModeEntryReader entryReader(args);
    
    auto entry = entryReader.read();
    this->currentStartAt = entry.startAt;
    this->currentDuration = entry.duration;
}

void CustomModeEntries::writeCurrentEntry() {
    CustomModeEntryWriterArgs args {
        .index = this->getRealIndex(),
        .startAt = this->currentStartAt,
        .duration = this->currentDuration
    };
    CustomModeEntryWriter entryWriter(args);
    entryWriter.write();
}

int8_t CustomModeEntries::getRealIndex() {
    return this->entries[this->index];
}

int8_t CustomModeEntries::getSortedIndex(int8_t realIndex) {
    for (int8_t index = 0; index < MAX_NUM_ENTRIES; ++index) {
        if (realIndex == this->entries[index]) {
            return index;
        }
    }

    return 0;
}

int CustomModeEntries::comparator(const void *a, const void *b) {
    const int8_t *_a = (const int8_t *)a;
    const int8_t *_b = (const int8_t *)b;

    CustomModeEntryReaderArgs entryReaderArgs = {
        .index = *_a
    };
    CustomModeEntryReader entryReader(entryReaderArgs);
    CustomModeEntryReaderResult entryA = entryReader.read();

    entryReaderArgs = CustomModeEntryReaderArgs {
        .index = *_b
    };
    CustomModeEntryReaderResult entryB = entryReader.read();

    if (entryA.startAt == 0 && entryA.duration == 0) {
        return 1;
    }

    if (entryB.startAt == 0 && entryB.duration == 0) {
        return -1;
    }

    if (entryA.startAt > entryB.startAt) {
        return 1;
    } else if (entryA.startAt < entryB.startAt) {
        return -1;
    } else {
        return 0;
    }
}

void CustomModeEntries::sortEntries() {
    qsort(this->entries, MAX_NUM_ENTRIES, sizeof(int8_t), comparator);
}
