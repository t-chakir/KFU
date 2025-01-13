#include <iostream>
#include <jwt-cpp/jwt.h>
#include <string>
#include <httplib.h>
#include <nlohmann/json.hpp>
#include <vector>
#include <postgresql/libpq-fe.h>
#include "../discipline/discipline.h"
#include "../test/test.h"
#include "../test/try.h"

using json = nlohmann::json;

	const std::string SECRET = "Wkb5e69a95d783e6a08e3Hl";

//Проверяет JWT токен
bool validate_jwt (std::string& token){
    try {
        // Декодируем токен
        auto decoded = jwt::decode(token);

        auto verifier = jwt::verify().allow_algorithm(jwt::algorithm::hs256{ SECRET });

        verifier.verify(decoded);

        // Получаем поле expires_at (expiration time)
        auto exp_claim = decoded.get_payload_claim("expires_at");

        // Преобразуем значение exp в std::chrono::system_clock::time_point
        auto exp_time = exp_claim.as_date();

        // Получаем текущее время
        auto now = std::chrono::system_clock::now(); 

        // Сравниваем текущее время с временем истечения токена
        
        return now < exp_time;
    } catch (const std::exception& e) {
        // Обработка исключения
        std::cerr << "JWT не прошел проверку потому что: " << e.what() << std::endl;
        return false;
    }
}

std::vector<std::string> id_vec(const std::string& token) {
    std::vector<std::string> access;

    try {
        // Декодируем токен
        auto decoded_token = jwt::decode(token);

        try{
        // Получаем claim "access"
            auto id_get = decoded_token.get_payload_claim("access");

            try {
                    // Попробуем извлечь данные как массив
                auto access_array = id_get.as_array(); // Это может выбросить исключение, если не массив
                    
                    // Добавляем все элементы массива в вектор
                for (const auto& id : access_array) {
                    access.push_back(id.get<std::string>());
                }
            } catch (const std::exception& e) {
                    std::cerr << "Error: The 'access' claim is not an array: " << e.what() << std::endl;
                    }
        }catch (const std::exception& e){
            std::cerr << "Error: Failed to retrieve 'access' claim: " << e.what() << std::endl;
        }
    } catch (const std::exception& e) {
        std::cerr << "Error JWT: " << e.what() << std::endl;
    }  

    return access;
}

// Разрешения из payload
bool has_permission (std::string& token, const std::string& req_permission){
    try{
        auto decoded_token = jwt::decode(token);

         // Извлекаем разрешения пользователя 
        auto permissions = decoded_token.get_payload_claim("permission").as_array();

        for(const auto& permission : permissions){
            if(permission.get<std::string>() == req_permission){
                return true;
            }
        }

        return false;
    }catch (const std::exception& e) {
        std::cerr << "JWT не прошел проверку потому что: " << e.what() << std::endl;
        return false;
    }
}

bool is_admin(const std::string& token){
    // try{
    //     auto decoded_token = jwt::decode(token);

    //     auto role = decoded_token.get_payload_claim("role").as_string();

    //     return role == "admin";
    // }
    // catch(const std::exception& e){
    //     std::cerr << "Error role: " << e.what() << std::endl;
    //     return false;
    // }

    return true;
}

void insertUser(const std::string& email, const std::string& name) {
    PGconn* conn = PQconnectdb("dbname=mydb user=postgres password=yourpassword host=localhost port=5432");

    if (PQstatus(conn) != CONNECTION_OK) {
        std::cerr << "Не удалось подключиться к базе данных: " << PQerrorMessage(conn) << std::endl;

        PQfinish(conn);
        return;
    }

    // Проверка существования пользователя по email
    const char* chek_sql = "SELECT email FROM users WHERE email = $1";
    const char* chek_params[1] = {email.c_str()};

    PGresult* chek_res = PQexecParams(conn, chek_sql, 1, nullptr, chek_params, nullptr, nullptr, 0);

    if (PQresultStatus(chek_res) != PGRES_TUPLES_OK){
        std::cerr << "Ошибка в проверке: " << PQerrorMessage(conn) << std::endl;
        PQclear(chek_res);
        PQfinish(conn);
        return;
    }

    if (PQntuples(chek_res) > 0){
        std::cerr << "Такой пользователь существует: " << email << std::endl;
        PQclear(chek_res);
        PQfinish(conn);
        return;
    }

    PQclear(chek_res);

    // SQL-запрос для вставки данных
    const char* insert_sql = "INSERT INTO users (username, email) VALUES ($1, $2)";
    
    // Параметры для запроса
    const char* insert_params[2] = {name.c_str(), email.c_str()};

    // Выполнение запроса
    PGresult* insert_res  = PQexecParams(conn, insert_sql, 2, nullptr, insert_params, nullptr, nullptr, 0);

    // Проверка результата
    if (PQresultStatus(insert_res ) != PGRES_COMMAND_OK) {
        std::cerr << "Ошибка при вставке: " << PQerrorMessage(conn) << std::endl;
    } else {
        std::cout << "Пользователь успешно вставлен!" << std::endl;
    }

    PQclear(insert_res);
    PQfinish(conn);
}

json get_UserData(const std::string& id) {

    std::cout << "Зашло в get_UserData" << std::endl;

    
    // Пример использования httplib::Client для отправки внутреннего запроса
    httplib::Client cli("http://localhost:8080");
    json user_data;
    // Формируем URL для запроса данных пользователя
    std::string url = "/func/getinfouser?id=" + id;
    
    // Выполняем GET-запрос
    if (auto res = cli.Get(url.c_str())) {

        if (res->status == 200) {
            try {
                user_data["user"] = json::parse(res->body);
            } catch (const json::parse_error& e) {
                std::cerr << "Ошибка парсинга JSON: " << e.what() << std::endl;
                return json();
            }
        } else {
            std::cerr << "Ошибка: HTTP статус " << res->status << std::endl;
            return json();
        }
    } else {
        std::cerr << "Ошибка: не удалось выполнить запрос." << std::endl;
        return json();
    }


    return user_data;
}

json getStudentDisciplines(const std::string& student_id) {
    json response;
    response["disciplines"] = json::array(); // Инициализируем пустой массив для дисциплин

    // Подключение к базе данных
    PGconn* conn = connect_db();
    if (!conn) {
        return json(); // Ошибка подключения
    }

    // SQL-запрос
    std::string query = R"(
        SELECT DISTINCT d.discipline_id, d.discipline_name
        FROM disciplines d
        LEFT JOIN teacher_disciplines td ON d.discipline_id = td.discipline_id
        LEFT JOIN student_disciplines sd ON d.discipline_id = sd.discipline_id
        WHERE td.teacher_id = $1 OR sd.student_id = $1;
    )";

    const char* paramValues[1] = { student_id.c_str() };
    PGresult* res = PQexecParams(
        conn,
        query.c_str(),
        1,          // Количество параметров
        nullptr,    // Типы параметров
        paramValues,// Значения параметров
        nullptr,    // Длины параметров
        nullptr,    // Форматы параметров
        0           // Формат результата: текстовый
    );

    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        std::cerr << "Ошибка выполнения запроса: " << PQerrorMessage(conn) << std::endl;
        PQclear(res);
        PQfinish(conn);
        return response;
    }

    // Обрабатываем строки результата
    int rows = PQntuples(res);
    std::cout << "Количество дисциплин: " << rows << std::endl;
    for (int i = 0; i < rows; ++i) {
        json discipline_obj;
        discipline_obj["id"] = std::stoi(PQgetvalue(res, i, 0)); // Извлекаем discipline_id
        discipline_obj["name"] = PQgetvalue(res, i, 1);          // Извлекаем name
        response["disciplines"].push_back(discipline_obj);
    }

    // Освобождаем ресурсы
    PQclear(res);
    PQfinish(conn);

    return response;
}

json get_AllDisciplines() {
    json response;
    response["discipline"] = json::array(); // Инициализируем пустой массив для дисциплин

    // Подключение к базе данных
    PGconn* conn = connect_db();
    if (!conn) {
        std::cerr << "Ошибка подключения к базе данных." << std::endl;
        return response; // Возвращаем пустой JSON
    }

    // SQL-запрос для получения всех дисциплин
    std::string query = "SELECT discipline_id, discipline_name FROM disciplines";

    PGresult* res = PQexec(conn, query.c_str());

    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        std::cerr << "Ошибка выполнения запроса: " << PQerrorMessage(conn) << std::endl;
        PQclear(res);
        PQfinish(conn);
        return response;
    }

    // Обрабатываем строки результата
    int rows = PQntuples(res);
    std::cout << "Количество дисциплин: " << rows << std::endl;

    for (int i = 0; i < rows; ++i) {
        json discipline_obj;
        discipline_obj["id"] = std::stoi(PQgetvalue(res, i, 0)); // Извлекаем discipline_id
        discipline_obj["text"] = PQgetvalue(res, i, 1);          // Извлекаем discipline_name
        response["discipline"].push_back(discipline_obj);       // Добавляем в массив
        //std::cout << "Дисциплина ID: " << discipline_obj["id"] << ", Название: " << discipline_obj["text"] << std::endl;
    }


    // Освобождаем ресурсы
    PQclear(res);
    PQfinish(conn);

    return response;
}

void delete_discipline_data(std::string id, std::string discipline_id) {
    std::cout << "delete_discipline_data" << std::endl;
    PGconn* conn = connect_db();
    if (!conn) {
        return; // Ошибка подключения
    }
    if (PQstatus(conn) != CONNECTION_OK) {
        std::cerr << "Ошибка соединения с базой данных: " << PQerrorMessage(conn) << std::endl;
        return;
    }

    std::string discipline_id_token = "SELECT student_id FROM student_disciplines WHERE discipline_id = " + discipline_id + ";";

    //DelitAcc(id, "chang_test:" +  discipline_id_token);

    std::cout << discipline_id << std::endl;

    std::string sql = "SELECT delete_discipline_data(" + discipline_id + ");";
    PGresult* res = PQexec(conn, sql.c_str());

    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        std::cerr << "Ошибка выполнения запроса: " << PQerrorMessage(conn) << std::endl;
    } else {
        std::cout << "Данные дисциплины с ID " << discipline_id << " успешно удалены." << std::endl;
    }

    PQclear(res);
}

void delete_student_data_from_discipline(const std::string& student_id, const std::string& discipline_id) {
    std::cout << "delete_student_data_from_discipline" << std::endl;
    PGconn* conn = connect_db();
    if (!conn) {
        std::cerr << "Ошибка подключения к базе данных." << std::endl;
        return; // Ошибка подключения
    }
    if (PQstatus(conn) != CONNECTION_OK) {
        std::cerr << "Ошибка соединения с базой данных: " << PQerrorMessage(conn) << std::endl;
        PQfinish(conn);
        return;
    }

    // Формируем SQL-запрос для вызова функции
    std::string sql = "SELECT delete_student_data_from_discipline('" + student_id + "', " + discipline_id + ");";
    
    // Выполняем SQL-запрос
    PGresult* res = PQexec(conn, sql.c_str());

    // Проверяем результат выполнения запроса
    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        std::cerr << "Ошибка выполнения запроса: " << PQerrorMessage(conn) << std::endl;
    } else {
        std::cout << "Данные студента с ID " << student_id << " успешно удалены из дисциплины с ID " << discipline_id << "." << std::endl;
    }

    // Освобождаем ресурсы
    PQclear(res);
    PQfinish(conn); // Закрываем соединение с базой данных
}


void delete_test_data(std::string test_id) {
    PGconn* conn = connect_db();
    if (!conn) {
        return; // Ошибка подключения
    }
    if (PQstatus(conn) != CONNECTION_OK) {
        std::cerr << "Ошибка соединения с базой данных: " << PQerrorMessage(conn) << std::endl;
        return;
    }

    std::cout << test_id << std::endl;

    std::string sql = "SELECT delete_test_data(" + test_id + ");";
    PGresult* res = PQexec(conn, sql.c_str());

    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        std::cerr << "Ошибка выполнения запроса: " << PQerrorMessage(conn) << std::endl;
    } else {
        std::cout << "Данные дисциплины с ID " << test_id << " успешно удалены." << std::endl;
    }

    PQclear(res);
}

bool addStudentDiscipline(const std::string &studentId, int disciplineId) {
    std::cout << "addStudentDiscipline" << std::endl;
    PGconn* conn = connect_db();
    if (!conn) {
        return false; // Ошибка подключения
    }
    if (PQstatus(conn) != CONNECTION_OK) {
        std::cerr << "Ошибка соединения с базой данных: " << PQerrorMessage(conn) << std::endl;
        return false;
    }

    // Проверяем, существует ли студент в таблице students
    std::string checkStudentQuery = "SELECT COUNT(*) FROM students WHERE student_id = $1;";
    const char* checkParams[1] = { studentId.c_str() };
    PGresult* checkRes = PQexecParams(conn, checkStudentQuery.c_str(), 1, nullptr, checkParams, nullptr, nullptr, 0);

    if (PQresultStatus(checkRes) != PGRES_TUPLES_OK) {
        std::cerr << "Ошибка проверки существования студента: " << PQerrorMessage(conn) << std::endl;
        PQclear(checkRes);
        return false;
    }

    // Проверяем, есть ли студент в базе
    bool studentExists = std::stoi(PQgetvalue(checkRes, 0, 0)) > 0;
    PQclear(checkRes);

    // Если студент не существует, добавляем его
    if (!studentExists) {
        std::string insertStudentQuery = "INSERT INTO students (student_id) VALUES ($1);";
        PGresult* insertRes = PQexecParams(conn, insertStudentQuery.c_str(), 1, nullptr, checkParams, nullptr, nullptr, 0);

        if (PQresultStatus(insertRes) != PGRES_COMMAND_OK) {
            std::cerr << "Ошибка добавления студента: " << PQerrorMessage(conn) << std::endl;
            PQclear(insertRes);
            return false;
        }
        PQclear(insertRes);
        std::cout << "Студент добавлен: " << studentId << std::endl;
    }

    // Добавляем связь студента с дисциплиной
    std::string insertDisciplineQuery =
        "INSERT INTO student_disciplines (student_id, discipline_id) "
        "VALUES ($1, $2) ON CONFLICT DO NOTHING;";
    const char* disciplineParams[2] = { studentId.c_str(), std::to_string(disciplineId).c_str() };
    PGresult* disciplineRes = PQexecParams(
        conn, insertDisciplineQuery.c_str(), 2, nullptr, disciplineParams, nullptr, nullptr, 0);

    if (PQresultStatus(disciplineRes) != PGRES_COMMAND_OK) {
        std::cerr << "Ошибка добавления дисциплины студенту: " << PQerrorMessage(conn) << std::endl;
        PQclear(disciplineRes);
        return false;
    }

    PQclear(disciplineRes);
    std::cout << "Связь добавлена: студент " << studentId << ", дисциплина " << disciplineId << std::endl;

    return true;
}

void handle_get_user_data (const httplib::Request& req, httplib::Response& res){

    // Получаем параметры из запроса
    std::string token = req.has_param("AccessToken") ? req.get_param_value("AccessToken") : "";

    if (token.empty() || !validate_jwt(token)){
        res.status = 401;
        res.set_content("401", "text/plain");
        return;
    }

    // Проверка разрешений (например, для получения данных требуется "read_data" разрешение)
    // if(!has_permission(token, "read_data")){
    //     res.status = 403;
    //     res.set_content("403", "text/plain");
    //     return;
    // }

    std::vector<std::string> ids = id_vec(token);

    std::string type = req.has_param("type") ? req.get_param_value("type") : ""; // Тип запроса

    json response;

    if (type == "UserDats") {
        std::string id = req.has_param("id") ? req.get_param_value("id") : ""; // ID запроса
        if(id == "null" || id == ""){
            response = get_UserData(ids[0]);
            response["disciplines"] = getStudentDisciplines(ids[0])["disciplines"];
        }
        else{
            response = get_UserData(id);
            response["disciplines"] = getStudentDisciplines(id)["disciplines"];
        }
    } else if (type == "createDiscipline") {
        handle_create_discipline(req, res);
        return;
    } else if (type == "disciplineGet"){
        handle_get_discipline(req, res, ids);
        return;
    } else if (type == "listDiscipline"){
        json result = get_AllDisciplines();
        //std::cout << "Disciplines found: " << result.dump(4) << std::endl;
        if (result["discipline"].empty()) {
            res.status = 404;
            res.set_content("Дисциплины не найдены.", "text/plain");
        } else {
            res.status = 200;
            res.set_content(result.dump(), "application/json");
        }
        return;
    } else if (type == "createTest"){
        handle_create_test(req, res);
        return;
    } else if (type == "testGet"){
        handle_get_test(req, res, ids);
        return;
    } else if (type == "regUserOnDiscipline"){
        std::string id = req.has_param("id") ? req.get_param_value("id") : ""; // ID запроса
        giveAcc(token, "regDiscipline:" + id);
        std::string student_id = ids[0];
        addStudentDiscipline (student_id, std::stoi(id));
        res.status = 200;
        return;
    } else if (type == "ExcludeMe"){
        std::string id = req.has_param("id") ? req.get_param_value("id") : ""; // ID запроса
        DelitAcc(ids[0], "regDiscipline:" + id);
        delete_student_data_from_discipline (ids[0], id);
        res.status = 200;
        return;
    } else if (type == "DelitDiscipline"){
        std::string id = req.has_param("id") ? req.get_param_value("id") : ""; // ID запроса
        DelitAcc(ids[0], "chang_test:" + id);
        delete_discipline_data (ids[0], id);
        res.status = 200;
        return;
    } else if (type == "Trying"){
        handle_save_test_attempt(req, res, ids);
        res.status = 200;
        return;
    } else if (type == "DelitTest") {
        std::string test_id = req.has_param("id") ? req.get_param_value("id") : ""; // ID запроса
        DelitAcc(ids[0], "regDiscipline:" + test_id);
        delete_test_data(test_id);
        res.status = 200;
        return;
    } else if (type == "StatusTest"){
        std::string test_id = req.has_param("id") ? req.get_param_value("id") : ""; // ID запроса
        std::cout << "status_id: " << std::stoi(test_id) << std::endl;
        toggle_test_status(test_id);
        res.status = 200;
        return;
    } else if (type == "CheckTrying"){

        check_active_attempt(req, res, ids[0]);

        if (res.status == 208) {
            // Если активная попытка существует, устанавливаем статус 208 и завершаем ответ
            res.status = 208;
            res.set_content(R"({"message": "Active attempt exists for this student."})", "application/json");
            std::cout << "208"<< std::endl;
            return; // Завершаем выполнение
        } else if (res.status == 200) {
            // Если активных попыток нет, устанавливаем статус 200 и завершаем ответ
            calculate_and_update_score(ids[0]);
            res.status = 200;
            res.set_content(R"({"message": "No active attempts for this student."})", "application/json");
            std::cout << "200"<< std::endl;
            return; // Завершаем выполнение
        }
        //json test;
        // int student_id = std::stoi(ids[0]);
        // json getStuden = getStudentData (student_id);
        // json getAtt
        // test["questions"] = getStuden["questions"];
        // res.status = 208;
        // res.set_content(test.dump(), "application/json");

        //if ()
    } else if (type == "СreateTrying"){
        std::string test_id = req.has_param("id") ? req.get_param_value("id") : ""; // ID запроса
        std::cout << "СreateTrying: " << test_id << std::endl;
        //int student_id = std::stoi(ids[0]);
        create_student_attempt(req, res, ids[0], std::stoi(test_id));
        res.status = 200;
        std::cout << "===200==="<< std::endl;
        return;
    } else if (type == "UpdateTrying") {
        std::cout << "=======UpdateTrying=======" << std::endl;
        std::string student_id = ids[0];
        std::string question_id = req.has_param("id") ? req.get_param_value("id") : ""; // ID запроса
        std::string resu = req.has_param("res") ? req.get_param_value("res") : ""; // ID запроса

        int question_id_int = std::stoi(question_id);
        int resu_int = std::stoi(resu);
        update_student_trying(ids[0], question_id_int, resu_int);

        std::cout << "question_id: " << question_id << std::endl;
        std::cout << "resu: " << resu << std::endl;
        std::cout << "student_id: " << student_id << std::endl;
    } else if (type == "СloseTrying") {
        close_active_attempt(req, res, ids[0]);
        calculate_and_update_score(ids[0]);
        return;
    } else if (type == "updateName") {
        std::cout << "=======updateName=======" << std::endl;
        std::string student_id = ids[0];
        std::string newName = req.has_param("newName") ? req.get_param_value("newName") : ""; // ID запроса

        std::cout << "newName: " << newName << std::endl;
                // Пример использования httplib::Client для отправки внутреннего запроса 
        httplib::Client cli("http://localhost:8080"); 
        // Формируем URL для запроса данных пользователя 
        std::string url = "/func/func/rename?id=" + student_id + "&newname=" + newName; 

        // Выполняем GET-запрос 
        if (auto res_internal = cli.Get(url.c_str())) { 
            if (res_internal->status == 200) { 
                res.status = 200; 
                res.set_content("Name updated successfully", "text/plain");
                return;
            } else { 
                res.status = 200; 
                res.set_content("403 Forbidden", "text/plain");
                return;
            } 
        }
    } else {
        res.status = 200;
        res.set_content("400", "text/plain");
        return;
    }

    // else if ( type == "CheckTrying" ){
    //     std::string test_id = req.has_param("id") ? req.get_param_value("id") : ""; // ID запроса
    //     json getStudentData ()
    //     res.status = 200;
    //     return;
    // } 

    res.status = 200;
    res.set_content(response.dump(), "application/json");
}