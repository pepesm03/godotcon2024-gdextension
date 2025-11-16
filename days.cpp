#include "days.h"

using namespace godot;

void DayManager::_bind_methods() {
    // Optionally bind methods to Godot if needed
}

DayManager::DayManager() {
}

Character* DayManager::spawn_random_character(Node2D* parent) {
    RandomNumberGenerator rng;
    rng.randomize();
    bool is_zombie = rng.randi_range(0,3) == 1;

    Character* character;
    if (is_zombie) {
        character = memnew(ZombieCharacter());
    } else {
        character = memnew(HumanCharacter());
    }

    parent->add_child(character);
    character->assign_traits_randomly();
    daily_characters.append(character);

    return character;
}

void DayManager::start_new_day(Node2D* parent) {
    for (int i = 0; i < 3; i++) {
        spawn_random_character(parent);
    }
}

void DayManager::show_character_features(Character* c, Node2D* ui_panel) {
    if (!c) return;
    ui_panel->add_child(c->get_node<Sprite2D>("Eyes"));
    ui_panel->add_child(c->get_node<Sprite2D>("Hair"));
    ui_panel->add_child(c->get_node<Sprite2D>("Teeth"));
    ui_panel->add_child(c->get_node<Sprite2D>("Hands"));
    ui_panel->add_child(c->get_node<Sprite2D>("Feet"));

}

void DayManager::end_day() {
    for (int i = 0; i < daily_characters.size(); i++) {
        Character* c = Object::cast_to<Character>(daily_characters[i]);
        if (c) {
            c->queue_free();
        }
    }
    daily_characters.clear();
}
