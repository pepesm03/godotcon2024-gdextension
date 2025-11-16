#ifndef QUIZ_H
#define QUIZ_H

#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/file_access.hpp>
#include <godot_cpp/classes/utility_functions.hpp>

#include <godot_cpp/variant/array.hpp>
#include <godot_cpp/variant/string.hpp>

namespace godot {

class Quiz : public Node {
    GDCLASS(Quiz, Node);

private:
    Array questions;   
    int current_index = 0;

protected:
    static void _bind_methods();

public:
    Quiz();
    ~Quiz();

    void load_questions(String difficulty, int day);   // Loads from a .txt file
    String get_question();
    bool answer_question(String answer);
};

}

#endif
