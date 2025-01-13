#ifndef TRY_H
#define TRY_H

#include <httplib.h>

void handle_save_test_attempt(const httplib::Request& req, httplib::Response& res, std::vector<std::string> id_user);
json generate_test_json(int test_id, int student_id);
void check_active_attempt(const httplib::Request& req, httplib::Response& res, const std::string& student_id);
void create_student_attempt(const httplib::Request& req, httplib::Response& res, const std::string& student_id, int test_id);
void update_student_trying(const std::string& student_id, int question_id, int new_selected_answer_id);
std::string close_active_attempt(const httplib::Request& req, httplib::Response& res, const std::string& student_id);

#endif //TRY_H