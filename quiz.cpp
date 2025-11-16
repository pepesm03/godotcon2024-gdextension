#include "quiz.h"

using namespace godot;

Quiz::Quiz() {}
Quiz::~Quiz() {}

void Quiz::_bind_methods() {
    ClassDB::bind_method(D_METHOD("load_questions", "difficulty", "day"), &Quiz::load_questions);
    ClassDB::bind_method(D_METHOD("get_question"), &Quiz::get_question);
    ClassDB::bind_method(D_METHOD("answer_question", "answer"), &Quiz::answer_question);
}

void Quiz::load_questions(String difficulty, int day) {
    questions.clear();
    current_index = 0;

    String path = "res://questions/" + difficulty + "/day" + String::num(day) + ".txt";

    Ref<FileAccess> file = FileAccess::open(path, FileAccess::ModeFlags::READ);

    if (file.is_null()) {
        UtilityFunctions::push_error("Could not load questions file: " + path);
        return;
    }

    while (!file->eof_reached()) {
        String line = file->get_line().strip_edges();
        if (line.length() > 0) {
            questions.push_back(line);
        }
    }

    file->close();
}

String Quiz::get_question() {
    if (current_index >= questions.size()) {
        return "NO_MORE_QUESTIONS";
    }
    return questions[current_index];
}

bool Quiz::answer_question(String answer) {
    if (current_index >= questions.size()) {
        return false;
    }

    String q = questions[current_index];

    // If question format = "QUESTION | correct_answer"
    PackedStringArray parts = q.split("|");

    if (parts.size() < 2) {
        UtilityFunctions::push_error("Invalid question format");
        return false;
    }

    String correct = parts[1].strip_edges();

    current_index++;

    return answer.strip_edges().to_lower() == correct.to_lower();
}
