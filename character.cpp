#include "character.h"
#include <godot_cpp/classes/dir_access.hpp>
#include <godot_cpp/classes/resource_loader.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;

void Character::_bind_methods() {
    ClassDB::bind_method(D_METHOD("set_is_zombie", "value"), &Character::set_is_zombie);
    ClassDB::bind_method(D_METHOD("get_is_zombie"), &Character::get_is_zombie);

    ADD_PROPERTY(PropertyInfo(Variant::BOOL, "is_zombie"), "set_is_zombie", "get_is_zombie");

    ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "zombie_eyes"), "set_zombie_eyes", "");
    ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "zombie_hair"), "set_zombie_hair", "");
    ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "zombie_teeth"), "set_zombie_teeth", "");
    ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "zombie_feet"), "set_zombie_feet", "");
    ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "zombie_hands"), "set_zombie_hands", "");

    ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "human_eyes"), "set_human_eyes", "");
    ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "human_hair"), "set_human_hair", "");
    ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "human_teeth"), "set_human_teeth", "");
    ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "human_feet"), "set_human_feet", "");
    ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "human_hands"), "set_human_hands", "");
}

Character::Character() {}

void Character::_ready() {
    eyes_sprite  = get_node<Sprite2D>("Eyes");
    hair_sprite  = get_node<Sprite2D>("Hair");
    teeth_sprite = get_node<Sprite2D>("Teeth");
    hands_sprite = get_node<Sprite2D>("Hands");
    feet_sprite  = get_node<Sprite2D>("Feet");

    randomize_features();
}

void Character::set_is_zombie(bool value) { is_zombie = value; }
bool Character::get_is_zombie() const { return is_zombie; }

void Character::set_zombie_eyes(const Array& arr) { zombie_eyes = arr; }
void Character::set_zombie_hair(const Array& arr) { zombie_hair = arr; }
void Character::set_zombie_teeth(const Array& arr) { zombie_teeth = arr; }
void Character::set_zombie_feet(const Array& arr) { zombie_feet = arr; }
void Character::set_zombie_hands(const Array& arr) { zombie_hands = arr; }

void Character::set_human_eyes(const Array& arr) { human_eyes = arr; }
void Character::set_human_hair(const Array& arr) { human_hair = arr; }
void Character::set_human_teeth(const Array& arr) { human_teeth = arr; }
void Character::set_human_feet(const Array& arr) { human_feet = arr; }
void Character::set_human_hands(const Array& arr) { human_hands = arr; }

Ref<Texture2D> Character::get_random_texture(const Array& arr) {
    if (arr.is_empty()) return nullptr;

    RandomNumberGenerator rng;
    rng.randomize();
    int index = rng.randi_range(0, arr.size() - 1);

    return arr[index];
}

void Character::randomize_features() {
    if (!eyes_sprite || !hair_sprite || !teeth_sprite || !hands_sprite || !feet_sprite)
        return;

    if (is_zombie) {
        eyes_sprite->set_texture(get_random_texture(zombie_eyes));
        hair_sprite->set_texture(get_random_texture(zombie_hair));
        teeth_sprite->set_texture(get_random_texture(zombie_teeth));
        hands_sprite->set_texture(get_random_texture(zombie_hands));
        feet_sprite->set_texture(get_random_texture(zombie_feet));
    } else {
        eyes_sprite->set_texture(get_random_texture(human_eyes));
        hair_sprite->set_texture(get_random_texture(human_hair));
        teeth_sprite->set_texture(get_random_texture(human_teeth));
        hands_sprite->set_texture(get_random_texture(human_hands));
        feet_sprite->set_texture(get_random_texture(human_feet));
    }
}
