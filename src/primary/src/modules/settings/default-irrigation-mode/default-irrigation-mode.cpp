#include "default-irrigation-mode.hpp"
#include "default-irrigation-mode-reader.hpp"
#include "default-irrigation-mode-writer.hpp"
#include "../../inputs/menu/menu.hpp"
#include <avr/pgmspace.h>

DefaultIrrigationMode::DefaultIrrigationMode(DefaultIrrigationModeArgs &args)
  : Input(args) {

}

static const char menu_item_0[] PROGMEM = "[A] Turn Off";
static const char menu_item_1[] PROGMEM = "[B] Always On";
static const char menu_item_2[] PROGMEM = "[C] Cyclic";
static const char menu_item_3[] PROGMEM = "[D] Custom";
static const char* const menu[] PROGMEM = {
  menu_item_0,
  menu_item_1,
  menu_item_2,
  menu_item_3
};

void DefaultIrrigationMode::initialize() {
  const int numModes = sizeof(menu) / sizeof(menu[0]);

  uint8_t defaultMode = this->readDefaultIrrigationMode();
  if (defaultMode > numModes - 1) {
    defaultMode = 0;
  }

  MenuArgs menuArgs {
    .items = menu,
    .numItems = numModes,
    .defaultItemIndex = defaultMode
  };
  Menu menu(menuArgs);
  menu.run();
  if (menu.isCanceled()) return;
  
  MenuResult menuResult = menu.getResult();
  this->writeDefaultIrriationMode(menuResult.selectedIndex);
}

bool DefaultIrrigationMode::handleInput(char inputKey) {
  return false;
}

DefaultIrrigationModeResult DefaultIrrigationMode::createResult() {
  return DefaultIrrigationModeResult();
}

uint8_t DefaultIrrigationMode::readDefaultIrrigationMode() {
  DefaultIrrigationModeReaderArgs args;
  DefaultIrrigationModeReader reader(args);
  reader.run();
  DefaultIrrigationModeReaderResult result = reader.getResult();
  return result.value;
}

void DefaultIrrigationMode::writeDefaultIrriationMode(uint8_t value) {
  DefaultIrrigationModeWriterArgs args {
    .value = value
  };
  DefaultIrrigationModeWriter writer(args);
  writer.run();
}
