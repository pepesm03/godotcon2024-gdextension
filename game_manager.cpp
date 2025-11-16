// game_manager.cpp
#include "game_manager.h"

using namespace godot;

void GameManager::_bind_methods() {
    ClassDB::bind_method(D_METHOD("set_difficulty", "difficulty"),
                         &GameManager::set_difficulty);
    ClassDB::bind_method(D_METHOD("get_difficulty"),
                         &GameManager::get_difficulty);

    ClassDB::bind_method(D_METHOD("get_tokens"),
                         &GameManager::get_tokens);
    ClassDB::bind_method(D_METHOD("get_day"),
                         &GameManager::get_day);
    ClassDB::bind_method(D_METHOD("is_player_alive"),
                         &GameManager::is_player_alive);

    ClassDB::bind_method(D_METHOD("start_new_game"),
                         &GameManager::start_new_game);
    ClassDB::bind_method(D_METHOD("start_night"),
                         &GameManager::start_night);
    ClassDB::bind_method(D_METHOD("finish_night"),
                         &GameManager::finish_night);
    ClassDB::bind_method(D_METHOD("start_day"),
                         &GameManager::start_day);

    ClassDB::bind_method(D_METHOD("let_visitor_in", "visitor"),
                         &GameManager::let_visitor_in);
    ClassDB::bind_method(D_METHOD("reject_visitor", "visitor"),
                         &GameManager::reject_visitor);

    ClassDB::bind_method(D_METHOD("start_quiz"),
                         &GameManager::start_quiz);
    ClassDB::bind_method(D_METHOD("answer_quiz_question", "choice_index"),
                         &GameManager::answer_quiz_question);
    ClassDB::bind_method(D_METHOD("quiz_has_more_questions"),
                         &GameManager::quiz_has_more_questions);

    ClassDB::bind_method(D_METHOD("inspect_resident", "idx", "token_cost"),
                         &GameManager::inspect_resident);
    ClassDB::bind_method(D_METHOD("decide_resident_fate", "idx", "kill"),
                         &GameManager::decide_resident_fate);

    ClassDB::bind_method(D_METHOD("resolve_night_danger"),
                         &GameManager::resolve_night_danger);
}

GameManager::GameManager() {
    rng.randomize();
}

void GameManager::_ready() {
    // Adjust paths to match your actual scene tree
    day_manager = get_node<DayManager>("DayManager");
    quiz_manager = get_node<QuizManager>("QuizManager");
    player = get_node<Player>("Player");

    if (!day_manager) {
        UtilityFunctions::push_error("GameManager: DayManager node not found.");
    }
    if (!quiz_manager) {
        UtilityFunctions::push_error("GameManager: QuizManager node not found.");
    }
    if (!player) {
        UtilityFunctions::push_error("GameManager: Player node not found.");
    }

    start_new_game();
}

// Difficulty
void GameManager::set_difficulty(int d) {
    if (d < 0) d = 0;
    if (d > 2) d = 2;
    difficulty = (Difficulty)d;
}

int GameManager::get_difficulty() const {
    return (int)difficulty;
}

// Tokens
int GameManager::get_tokens() const {
    return tokens;
}

void GameManager::earn_tokens(int amount) {
    if (amount < 0) return;
    tokens += amount;
    UtilityFunctions::print("You earned ", amount, " tokens. Total tokens: ", tokens);
}

bool GameManager::spend_tokens(int amount) {
    if (amount <= 0) return true;
    if (tokens < amount) {
        UtilityFunctions::print("Not enough tokens. You have: ", tokens);
        return false;
    }
    tokens -= amount;
    UtilityFunctions::print("Spent ", amount, " tokens. Remaining: ", tokens);
    return true;
}

// Main flow
void GameManager::start_new_game() {
    day = 1;
    tokens = 5;
    player_alive = true;
    residents.clear();

    UtilityFunctions::print("New game started. Day 1 begins.");

    // Show rules & difficulty choice via UI.
    // Once difficulty chosen (call set_difficulty from UI), you can start night:
    start_night();
}

void GameManager::start_night() {
    if (!player_alive) return;

    UtilityFunctions::print("Night ", day, " begins.");

    if (!day_manager) {
        UtilityFunctions::push_error("GameManager: No DayManager for start_night.");
        return;
    }

    Node2D *door_parent = get_node<Node2D>("Door"); // adjust path as needed
    if (!door_parent) {
        UtilityFunctions::push_error("GameManager: Door node not found.");
        return;
    }

    day_manager->start_new_day(door_parent);

    // Your UI shows each visitor from day_manager's daily list and
    // calls let_visitor_in / reject_visitor accordingly.
}

void GameManager::finish_night() {
    if (!player_alive) return;

    UtilityFunctions::print("You go to sleep...");

    if (day_manager) {
        day_manager->end_day();
    }

    // Resolve zombie break-in / killings
    resolve_night_danger();

    if (!player_alive) {
        UtilityFunctions::print("Game over. You died during the night.");
        return;
    }

    // Morning
    start_day();
}

void GameManager::start_day() {
    if (!player_alive) return;

    // If player has reached beyond final day, they win.
    check_win_condition();
    if (!player_alive) {
        // In this design, player_alive false can also represent game finished; adjust as you like.
        return;
    }

    UtilityFunctions::print("Morning of day ", day, ".");

    // Update how long humans have stayed & remove those who leave
    increment_resident_days_and_handle_departures();

    // Show news for this day in UI (zombie trait).
    // Use 'day' to index your trait list from Godot script.

    UtilityFunctions::print("News: new zombie trait revealed for day ", day, ".");

    // Start quiz for tokens.
    start_quiz();
}

// Visitors
void GameManager::let_visitor_in(Character *visitor) {
    if (!visitor) return;

    if ((int)residents.size() >= MAX_RESIDENTS) {
        UtilityFunctions::print("House is full. You cannot let more people in.");
        // Optionally auto-reject or force player to kick someone first.
        return;
    }

    Resident r;
    r.character = visitor;
    r.days_in_house = 0;
    r.alive = true;

    residents.push_back(r);

    // Move visitor into house node if you want to visually separate inside vs door.
    Node *house_node = get_node<Node>("House"); // adjust
    if (house_node) {
        visitor->reparent(house_node);
    }

    UtilityFunctions::print("You let someone into the house. Total residents: ",
                            (int)residents.size());
}

void GameManager::reject_visitor(Character *visitor) {
    if (!visitor) return;
    UtilityFunctions::print("You rejected a visitor at the door.");
    visitor->queue_free();
}

// Quiz
void GameManager::start_quiz() {
    if (!quiz_manager) {
        UtilityFunctions::push_error("GameManager: No QuizManager for start_quiz.");
        return;
    }

    quiz_manager->reset();
    quiz_manager->load_questions_for_day((int)difficulty, day);

    if (!quiz_manager->has_more_questions()) {
        UtilityFunctions::print("No quiz questions loaded for today. Skipping straight to inspection.");
        return;
    }

    UtilityFunctions::print("Quiz started for day ", day, ".");
    // Your UI pulls the first question using quiz_manager.get_next_question()
    // and then calls answer_quiz_question(...) when the player chooses an answer.
}

bool GameManager::answer_quiz_question(int choice_index) {
    if (!quiz_manager) return false;

    bool correct = quiz_manager->answer_current_question(choice_index);
    if (correct) {
        int reward = 1; // or scale based on difficulty
        earn_tokens(reward);
    } else {
        UtilityFunctions::print("Wrong answer.");
    }

    if (!quiz_manager->has_more_questions()) {
        UtilityFunctions::print("Quiz finished. You have ", tokens, " tokens.");

        // Now player can inspect residents & decide who to kill / keep.
        // Trigger UI transition into inspection.
    }

    return correct;
}

bool GameManager::quiz_has_more_questions() const {
    if (!quiz_manager) return false;
    return quiz_manager->has_more_questions();
}

// Inspection
bool GameManager::inspect_resident(int idx, int token_cost) {
    if (idx < 0 || idx >= (int)residents.size()) {
        UtilityFunctions::print("Invalid resident index for inspection.");
        return false;
    }

    Resident &r = residents[idx];
    if (!r.alive || !r.character) {
        UtilityFunctions::print("This resident is already dead or missing.");
        return false;
    }

    if (!spend_tokens(token_cost)) {
        return false;
    }

    bool is_zombie = r.character->get_is_zombie();

    // You can show more detailed traits in UI here.
    UtilityFunctions::print("Inspection result: resident ", idx,
                            is_zombie ? " IS a zombie." : " seems HUMAN.");

    return is_zombie;
}

void GameManager::decide_resident_fate(int idx, bool kill) {
    if (idx < 0 || idx >= (int)residents.size()) {
        UtilityFunctions::print("Invalid resident index for decision.");
        return;
    }

    Resident &r = residents[idx];
    if (!r.alive || !r.character) {
        UtilityFunctions::print("Cannot change fate of this resident.");
        return;
    }

    if (kill) {
        r.alive = false;
        UtilityFunctions::print("You killed resident ", idx, ".");
        r.character->queue_free();
    } else {
        UtilityFunctions::print("You let resident ", idx, " live.");
    }
}

// Night danger logic
void GameManager::resolve_night_danger() {
    if (residents.empty()) {
        UtilityFunctions::print("Your house is empty. Someone breaks in and kills you while you sleep.");
        player_alive = false;
        return;
    }

    std::vector<int> zombie_indices;
    std::vector<int> human_indices;

    for (int i = 0; i < (int)residents.size(); ++i) {
        Resident &r = residents[i];
        if (!r.alive || !r.character) continue;

        if (r.character->get_is_zombie()) {
            zombie_indices.push_back(i);
        } else {
            human_indices.push_back(i);
        }
    }

    if (zombie_indices.empty()) {
        UtilityFunctions::print("No zombies inside tonight. You sleep peacefully.");
        return;
    }

    // Probability that something bad happens increases with day
    float kill_prob = 0.2f + 0.1f * (day - 1); // day 1: 20%, day 7: 80%
    if (kill_prob > 0.95f) kill_prob = 0.95f;

    float roll = rng.randf();
    UtilityFunctions::print("Night danger roll: ", roll, ", threshold: ", kill_prob);

    if (roll > kill_prob) {
        UtilityFunctions::print("You got lucky. Zombies didn't attack tonight.");
        return;
    }

    if (!human_indices.empty()) {
        int victim_idx = human_indices[rng.randi_range(0, (int)human_indices.size() - 1)];
        Resident &victim = residents[victim_idx];
        victim.alive = false;

        UtilityFunctions::print("During the night, a zombie killed one of your human guests (index ",
                                victim_idx, ").");
        if (victim.character) {
            victim.character->queue_free();
        }
    } else {
        UtilityFunctions::print("No humans left… the zombies attack YOU in your sleep.");
        player_alive = false;
    }
}

// Helpers
void GameManager::increment_resident_days_and_handle_departures() {
    // Increase stay length for living humans
    for (auto &r : residents) {
        if (!r.alive || !r.character) continue;
        if (!r.character->get_is_zombie()) {
            r.days_in_house += 1;
        }
    }

    // Humans leave after 2 days
    for (int i = (int)residents.size() - 1; i >= 0; --i) {
        Resident &r = residents[i];
        if (!r.alive || !r.character) continue;
        if (!r.character->get_is_zombie() && r.days_in_house >= 2) {
            UtilityFunctions::print("A human guest (index ", i, ") leaves your house after two days.");
            r.character->queue_free();
            residents.erase(residents.begin() + i);
        }
    }
}

void GameManager::check_win_condition() {
    if (day > MAX_DAYS) {
        UtilityFunctions::print("You survived ", MAX_DAYS, " days. YOU WIN!");
        // You can set a separate "game_finished" flag if you don't want to reuse player_alive.
        player_alive = false;
        return;
    }

    // If still within valid days, continue.
    // The day increments at the end of each full cycle; you can choose where to do that.
    // Simple approach: increment here for the next cycle.
    day += 1;
}
