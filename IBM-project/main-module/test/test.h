#ifndef TEST_H
#define TEST_H

#include <string>
#include <vector>
#include <httplib.h>
#include <nlohmann/json.hpp>

int create_test(const std::string& discipline_id, const std::string& test_name, const std::string& description, const std::string& teacher_id);
void handle_create_test(const httplib::Request& req, httplib::Response& res);
void handle_get_test(const httplib::Request& req, httplib::Response&, std::vector<std::string> id_user);
json getStudentData(int student_id);
json generate_test_json(int test_id, int student_id);
void post_test (const httplib::Request& req, httplib::Response& res);
void calculate_and_update_score(const std::string& student_id);

#endif // TEST_H