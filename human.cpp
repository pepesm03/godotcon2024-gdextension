#include "human.h"

using namespace godot;

void HumanCharacter::_bind_methods() {
    // Inherits everything from Character
}

HumanCharacter::HumanCharacter() {
    is_zombie = false;
}

HumanCharacter::~HumanCharacter() {
}

void HumanCharacter::assign_traits_randomly() {
    randomize_features();
}
