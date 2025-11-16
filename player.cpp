#include "player.h"
using namespace godot;

void Player::_bind_methods() {
    ClassDB::bind_method(D_METHOD("spend_tokens", "amount"), &Player::spend_tokens);
    ClassDB::bind_method(D_METHOD("earn_tokens", "amount"), &Player::earn_tokens);
}

bool Player::spend_tokens(int amount) {
    if (tokens >= amount) {
        tokens -= amount;
        return true;
    }
    return false;
}

void Player::earn_tokens(int amount) {
    tokens += amount;
}
