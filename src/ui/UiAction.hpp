#ifndef UI_ACTION_HPP
#define UI_ACTION_HPP

enum class Action {
    NONE=0,
    SHOW_TARGET_LAYOUT=1,
    HIDE_TARGET_LAYOUT=2,
    TOGGLE_TARGET_LAYOUT=3,
    SWITCH_TO_TARGET_LAYOUT=4,
};

enum class ActionSource {
    MAIN_MENU_EXIT=7116,
};

typedef struct {
    Action action;
    int target;
    int source;
} UiAction;

#endif // UI_ACTION_HPP