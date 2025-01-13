// discipline.h
#ifndef DISCIPLINE_H
#define DISCIPLINE_H

#include <httplib.h>
#include <httplib.h>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

PGconn* connect_db();
bool find_resolution(const std::vector<std::string>& id, const std::string& creat);
void handle_create_discipline(const httplib::Request& req, httplib::Response& res);
void handle_get_discipline(const httplib::Request& req, httplib::Response& res, const std::vector<std::string>& id_user);
void giveAcc (std::string teacher_token, std::string chang_test = "");
json toggle_test_status(std::string test_id);
//json get_AllDisciplines();
void DelitAcc (std::string acc_token, std::string chang_test = "");
nlohmann::json get_discipline_from_db(int discipline_id);

#endif // DISCIPLINE_H