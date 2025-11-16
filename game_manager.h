#ifndef GAMEMANAGER_HPP
#define GAMEMANAGER_HPP

#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/core/class_db.hpp>

using namespace godot;

enum Difficulty {
    EASY,
    MID,
    HARD
};

class GameManager : public Node {
    GDCLASS(GameManager, Node);

protected:
    static void _bind_methods();

public:
    int tokens = 5;
    int days_passed = 0;
    Difficulty difficulty = EASY;

    GameManager() {}

    void set_difficulty(Difficulty d) { difficulty = d; }
    Difficulty get_difficulty() const { return difficulty; }

    bool spend_tokens(int amount);
    void earn_tokens(int amount);
    void next_day();
    int get_tokens() const { return tokens; }
    int get_days_passed() const { return days_passed; }
};

#endif
