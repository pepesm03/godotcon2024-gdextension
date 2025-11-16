#include "quiz.h"
#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;

void QuizManager::_bind_methods() {
    ClassDB::bind_method(D_METHOD("load_questions_for_day", "difficulty", "day"),
                         &QuizManager::load_questions_for_day);
    ClassDB::bind_method(D_METHOD("get_next_question"),
                         &QuizManager::get_next_question);
    ClassDB::bind_method(D_METHOD("answer_current_question", "choice_index"),
                         &QuizManager::answer_current_question);
    ClassDB::bind_method(D_METHOD("has_more_questions"),
                         &QuizManager::has_more_questions);
    ClassDB::bind_method(D_METHOD("reset"),
                         &QuizManager::reset);
}

QuizManager::QuizManager() {
    reset();
}

void QuizManager::reset() {
    questions.clear();
    current_index = -1;
}

void QuizManager::add_question(const String &text,
                               const PackedStringArray &choices,
                               int correct_index) {
    Dictionary q;
    q["text"] = text;
    q["choices"] = choices;
    q["correct_index"] = correct_index;
    questions.append(q);
}

void QuizManager::fill_questions_easy(int day) {
    // Example: 2 easy questions per day
    if (day == 1) {
        add_question("What is 2 + 2?",
                     PackedStringArray({"3", "4", "5", "6"}), 1);
        add_question("Which one is a color?",
                     PackedStringArray({"Dog", "Blue", "House", "Car"}), 1);
    } else if (day == 2) {
        add_question("How many days are in a week?",
                     PackedStringArray({"5", "6", "7", "8"}), 2);
        add_question("Which is larger?",
                     PackedStringArray({"10", "3", "1", "0"}), 0);
    } else {
        // Fallback / default
        add_question("Which number is even?",
                     PackedStringArray({"1", "3", "4", "5"}), 2);
        add_question("Which is a fruit?",
                     PackedStringArray({"Apple", "Chair", "Rock", "Car"}), 0);
    }
}

void QuizManager::fill_questions_mid(int day) {
    if (day == 1) {
        add_question("Which gas do humans primarily breathe in?",
                     PackedStringArray({"Oxygen", "Carbon dioxide", "Nitrogen", "Helium"}), 0);
        add_question("Which planet is known as the Red Planet?",
                     PackedStringArray({"Venus", "Mars", "Jupiter", "Saturn"}), 1);
    } else if (day == 2) {
        add_question("What is the capital of France?",
                     PackedStringArray({"Berlin", "Madrid", "Paris", "Rome"}), 2);
        add_question("How many minutes are in one hour?",
                     PackedStringArray({"30", "60", "90", "120"}), 1);
    } else {
        add_question("Which of these is a mammal?",
                     PackedStringArray({"Shark", "Frog", "Eagle", "Dolphin"}), 3);
        add_question("Water freezes at what temperature (Celsius)?",
                     PackedStringArray({"0", "10", "50", "100"}), 0);
    }
}

void QuizManager::fill_questions_hard(int day) {
    if (day == 1) {
        add_question("What is the powerhouse of the cell?",
                     PackedStringArray({"Nucleus", "Mitochondrion", "Ribosome", "Golgi apparatus"}), 1);
        add_question("In math, what is the derivative of x^2?",
                     PackedStringArray({"x", "2x", "x^3", "2"}), 1);
    } else if (day == 2) {
        add_question("Which of these is NOT a prime number?",
                     PackedStringArray({"2", "3", "9", "11"}), 2);
        add_question("What is the chemical symbol for Sodium?",
                     PackedStringArray({"So", "Na", "N", "S"}), 1);
    } else {
        add_question("Which data structure uses FIFO (First In First Out)?",
                     PackedStringArray({"Stack", "Queue", "Tree", "Graph"}), 1);
        add_question("What is the square root of 144?",
                     PackedStringArray({"10", "11", "12", "13"}), 2);
    }
}

void QuizManager::load_questions_for_day(int difficulty, int day) {
    reset();

    switch (difficulty) {
        case 0:
            fill_questions_easy(day);
            break;
        case 1:
            fill_questions_mid(day);
            break;
        case 2:
        default:
            fill_questions_hard(day);
            break;
    }

    if (questions.size() > 0) {
        current_index = 0;
    } else {
        current_index = -1;
    }
}

Dictionary QuizManager::get_next_question() {
    Dictionary result;

    if (current_index < 0 || current_index >= questions.size()) {
        return result; // empty = no more questions
    }

    Dictionary q = questions[current_index];
    result["text"] = q["text"];
    result["choices"] = q["choices"];
    result["index"] = current_index;

    return result;
}

bool QuizManager::answer_current_question(int choice_index) {
    if (current_index < 0 || current_index >= questions.size()) {
        return false;
    }

    Dictionary q = questions[current_index];
    int correct_index = (int)q["correct_index"];

    bool correct = (choice_index == correct_index);

    // Move to next question
    current_index++;
    if (current_index >= questions.size()) {
        current_index = questions.size(); // locked at end
    }

    return correct;
}

bool QuizManager::has_more_questions() const {
    return (current_index >= 0) && (current_index < questions.size());
}
