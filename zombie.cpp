#include "zombie.h"

using namespace godot;

void ZombieCharacter::_bind_methods() {
    // Inherits everything from Character
}

ZombieCharacter::ZombieCharacter() {
    is_zombie = true;
}

ZombieCharacter::~ZombieCharacter() {
}

void ZombieCharacter::assign_traits_randomly() {
    randomize_features();
}
