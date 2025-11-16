#ifndef PLAYER_H
#define PLAYER_H

#include <godot_cpp/classes/node.hpp>
using namespace godot;

class Player : public Node {
    GDCLASS(Player, Node);

protected:
    static void _bind_methods();

public:
    int tokens = 5;

    Player() {}
    bool spend_tokens(int amount);
    void earn_tokens(int amount);
};

#endif
