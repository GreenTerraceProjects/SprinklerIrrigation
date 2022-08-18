#ifndef _MENU_HPP_
#define _MENU_HPP_

#include "../input.hpp"
#include "../../strings/wordwrap/wordwrap.hpp"

class MenuResult {
public:
    int selectedIndex = -1;
};

class MenuArgs {
public:
    const char *const *items;
    int numItems;
    int defaultItemIndex;
};

class Menu : public Input<MenuResult, MenuArgs> {

private:
    int menuItemIndex;
    bool menuItemChanged;
    int menuItemPageStartingLine;
    enum WordWrapMovingDirection menuItemWordWrapMovingDirection;

public:
    Menu(MenuArgs& args);

private:
    void initialize() override;
    bool handleInput(char inputKey) override;
    MenuResult createResult() override;

private:
    void refreshDisplay();

};

#endif
