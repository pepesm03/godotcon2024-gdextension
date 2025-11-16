#include "game_manager.h"
using namespace godot;

void GameManager::_bind_methods() {
    ClassDB::bind_method(D_METHOD("spend_tokens", "amount"), &GameManager::spend_tokens);
    ClassDB::bind_method(D_METHOD("earn_tokens", "amount"), &GameManager::earn_tokens);
    ClassDB::bind_method(D_METHOD("next_day"), &GameManager::next_day);
    ClassDB::bind_method(D_METHOD("set_difficulty", "difficulty"), &GameManager::set_difficulty);
    ClassDB::bind_method(D_METHOD("get_difficulty"), &GameManager::get_difficulty);
    ClassDB::bind_method(D_METHOD("get_tokens"), &GameManager::get_tokens);
    ClassDB::bind_method(D_METHOD("get_days_passed"), &GameManager::get_days_passed);
}

bool GameManager::spend_tokens(int amount) {
    if (tokens >= amount) {
        tokens -= amount;
        return true;
    }
    return false;
}

void GameManager::earn_tokens(int amount) {
    tokens += amount;
}

void GameManager::next_day() {
    days_passed++;
    if (days_passed > 7) {
        UtilityFunctions::print("🎉 Congratulations! You survived 7 days!");
    }
}
