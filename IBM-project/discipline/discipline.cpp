#include <iostream>
#include <httplib.h>
#include <jwt-cpp/jwt.h>
#include <nlohmann/json.hpp>
#include <postgresql/libpq-fe.h>
#include "../reg/reg.h" 
#include <vector>

using json = nlohmann::json;
using ordered_json = nlohmann::ordered_json;

PGconn* connect_db() {
    PGconn* conn = PQconnectdb("dbname=mydb user=postgres password=yourpassword host=localhost port=5432");
    if (PQstatus(conn) != CONNECTION_OK) {
        std::cerr << "Database connection failed: " << PQerrorMessage(conn) << std::endl;
        PQfinish(conn);
        return nullptr;
    }
    
    std::cout << "Connected to database successfully!" << std::endl; // Отладочный вывод

    return conn;
}

json get_discipline_info(int discipline_id) {
    PGconn* conn = connect_db();
    if (!conn) {
        return json(); // Ошибка подключения
    }

        std::cout << "Зашло в get_discipline_info" << std::endl;

    const char* query = R"(
        SELECT
            t.teacher_id AS teacher_id,
            d.discipline_name AS discipline_name
        FROM
            teachers t
        JOIN
            teacher_disciplines td ON t.teacher_id = td.teacher_id
        JOIN
            disciplines d ON td.discipline_id = d.discipline_id
        WHERE
            d.discipline_id = $1
    )";
        const char* params[1] = { std::to_string(discipline_id).c_str() };

    PGresult* res = PQexecParams(conn, query, 1, nullptr, params, nullptr, nullptr, 0);

    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        std::cerr << "SQL error: " << PQerrorMessage(conn) << std::endl;
        PQclear(res);
        PQfinish(conn);
        return json();
    }

    json discipline_get;
    //result["discipline_id"] = discipline_id;

    if (PQntuples(res) > 0) {
        // Извлекаем данные из результата
        discipline_get["discipline_name"] = PQgetvalue(res, 0, 1);
        discipline_get["teacher_id"] = PQgetvalue(res, 0, 0);
        //std::string teacher_name = PQgetvalue(res, 0, 2);
    }

    PQclear(res);
    PQfinish(conn);
    return discipline_get;
}

json get_participants_for_discipline (int discipline_id){
    PGconn* conn = connect_db();
    if (!conn) {
        return json(); // Ошибка подключения
    }

    std::cout << "Зашло в get_participants_for_discipline" << std::endl;

    const char* query = R"(
        SELECT 
            s.student_id
        FROM 
            students s
        JOIN 
            student_disciplines sd ON s.student_id = sd.student_id
        WHERE 
            sd.discipline_id = $1
    )";

    const char* params[1] = { std::to_string(discipline_id).c_str() };

    PGresult* res = PQexecParams(conn, query, 1, nullptr, params, nullptr, nullptr, 0);

    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        std::cerr << "SQL error: " << PQerrorMessage(conn) << std::endl;
        PQclear(res);
        PQfinish(conn);
        return json();
    }

    json participants = json::array();

    for (int i = 0;  i < PQntuples(res); ++i) {
        json participant;
        // Извлекаем данные из результата
        participant["student_id"] = PQgetvalue(res, i, 0);
        participants.push_back(participant);

        std::cout << "Студенты: " << participant.dump() << std::endl; // Выводим информацию о студенте

    }

    PQclear(res);
    PQfinish(conn);
    return participants;
}

json getTestsForDiscipline(int discipline_id) {

    std::cout << "getTestsForDiscipline" << std::endl;

    json response;
    response["questions"] = json::array(); // Инициализируем пустой массив для вопросов

    // Подключение к базе данных
    PGconn* conn = connect_db();
    if (!conn) {
        return json(); // Ошибка подключения
    }

    // SQL-запрос
    std::string query = R"(
        SELECT test_id, test_name, status
        FROM tests
        WHERE discipline_id = $1
    )";

    const char* paramValues[1] = { std::to_string(discipline_id).c_str() };
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
    for (int i = 0; i < rows; ++i) {
        json question_obj;
        question_obj["id"] = std::stoi(PQgetvalue(res, i, 0)); // Извлекаем test_id
        question_obj["text"] = PQgetvalue(res, i, 1);          // Извлекаем test_name
        question_obj["status"] = PQgetvalue(res, i, 2); // Здесь можно оставить пустым или установить значение по умолчанию
        response["questions"].push_back(question_obj); // Добавляем в массив
    }

    std::cout << response.dump(4) << std::endl;

    // Освобождаем ресурсы
    PQclear(res);
    PQfinish(conn);

    return response; // Возвращаем ответ с вопросами
}

json get_grades_for_discipline(int discipline_id, const std::string& student_id) {
    PGconn* conn = connect_db();
    if (!conn) {
        return json(); // Ошибка подключения
    }

    json result;
    result["questions"] = json::array(); // Инициализируем массив вопросов

    // Запрос для получения тестов, их лучших результатов и дат последней попытки
    const char* test_query = R"(
        SELECT 
            t.test_id AS id,
            t.test_name AS text,
            MAX(sa.contribution) AS best_contribution,
            MAX(sa.score) AS best_score
        FROM 
            tests t
        LEFT JOIN 
            student_attempts sa ON t.test_id = sa.test_id
        WHERE 
            t.discipline_id = $1
        AND 
            (t.student_id = $2 OR t.student_id IS NULL)
        GROUP BY 
            t.test_id, t.test_name
        ORDER BY 
            t.test_id ASC;

    )";

    const char* params[2] = { std::to_string(discipline_id).c_str(), student_id.c_str() };
    PGresult* test_res = PQexecParams(conn, test_query, 2, nullptr, params, nullptr, nullptr, 0);

    if (PQresultStatus(test_res) != PGRES_TUPLES_OK) {
        std::cerr << "Ошибка выполнения запроса: " << PQerrorMessage(conn) << std::endl;
        PQclear(test_res);
        PQfinish(conn);
        return result; // Возвращаем пустой JSON
    }

    // Формируем JSON-объекты для каждого теста
    for (int i = 0; i < PQntuples(test_res); ++i) {
        json question;
        question["id"] = std::stoi(PQgetvalue(test_res, i, 0)); // ID теста
        question["text"] = PQgetvalue(test_res, i, 1);          // Название теста

        // Баллы и вклад
        const char* score = PQgetvalue(test_res, i, 3);
        const char* contribution = PQgetvalue(test_res, i, 2);

        // Если результат отсутствует (NULL), ставим "-"
        question["score"] = (score && *score) ? score : "-";
        question["contribution"] = (contribution && *contribution) ? contribution : "-";

        result["questions"].push_back(question);
    }

    std::cout << result.dump(4) << std::endl;

    PQclear(test_res);
    PQfinish(conn);
    return result;
}


int insertDiscipline(const std::string& name, const std::string& teacher_id) {
    PGconn* conn = connect_db();
    if (!conn) {
        return -1; // Ошибка подключения
    }

    
    // Добавление преподавателя, если его нет
    const char* insert_teacher_query = "INSERT INTO teachers (teacher_id) VALUES ($1) ON CONFLICT (teacher_id) DO NOTHING";
    const char* insert_teacher_params[1] = { teacher_id.c_str() };

    PGresult* insert_res = PQexecParams(conn, insert_teacher_query, 1, nullptr, insert_teacher_params, nullptr, nullptr, 0);

    if (PQresultStatus(insert_res) != PGRES_COMMAND_OK) {
        std::cerr << "Failed to insert teacher: " << PQerrorMessage(conn) << std::endl;
        PQclear(insert_res);
        PQfinish(conn);
        return -1;
    }
    PQclear(insert_res);

    // Проверка существования teacher_id
    const char* check_teacher_query = "SELECT teacher_id FROM teachers WHERE teacher_id = $1";
    const char* check_params[1] = { teacher_id.c_str() };
    PGresult* check_res = PQexecParams(conn, check_teacher_query, 1, nullptr, check_params, nullptr, nullptr, 0);

    if (PQntuples(check_res) == 0) {
        std::cerr << "Teacher with ID " << teacher_id << " does not exist." << std::endl;
        PQclear(check_res);
        PQfinish(conn);
        return -1; // Преподаватель не найден
    }
    PQclear(check_res);

    // Вставляем дисциплину
    const char* query = "INSERT INTO disciplines (discipline_name) VALUES ($1) RETURNING discipline_id";
    const char* params[1] = { name.c_str() };

    PGresult* res = PQexecParams(conn, query, 1, nullptr, params, nullptr, nullptr, 0);

    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        std::cerr << "SQL error: " << PQerrorMessage(conn) << std::endl;
        PQclear(res);
        PQfinish(conn);
        return -1;
    }

    int discipline_id = std::stoi(PQgetvalue(res, 0, 0));
    PQclear(res);

    // Связываем дисциплину с учителем
    const char* link_query = "INSERT INTO teacher_disciplines (teacher_id, discipline_id) VALUES ($1, $2)";
    const char* link_params[2] = { teacher_id.c_str(), std::to_string(discipline_id).c_str() };

    res = PQexecParams(conn, link_query, 2, nullptr, link_params, nullptr, nullptr, 0);

    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        std::cerr << "SQL error: " << PQerrorMessage(conn) << std::endl;
        PQclear(res);
        PQfinish(conn);
        return -1;
    }

    PQclear(res);
    PQfinish(conn);
    return discipline_id;
}

void giveAcc (std::string teacher_token, std::string chang_test = ""){
    std::cout << "Зашло в giveAcc" << std::endl;

    // Пример использования httplib::Client для отправки внутреннего запроса
    httplib::Client cli("http://localhost:8080");
    json user_data;
    // Формируем URL для запроса данных пользователя
    std::string url = "/func/giveAcc?AccessToken=" + teacher_token + "&NewAccess=" + chang_test;

    std::cout << "Тест: " << chang_test << std::endl;

    if (auto res = cli.Get(url.c_str())) {

        if (res->status == 401){
            std::cout << "401" << std::endl;
        }
    }
}

std::string token_h (const std::string id){
    auto tokeExpiresAt = std::chrono::system_clock::now() + std::chrono::minutes(1);

    //std::vector<std::string> user = { id };
    picojson::array userArray;
    
    userArray.push_back(picojson::value(id));


    // Создаём токен с методом шифрования HS256
    auto token = jwt::create()
        // Тип токена
        .set_type("JWS")
        // Заполняем данными полезную нагрузку
        .set_payload_claim("accec", jwt::claim(userArray))
        .set_payload_claim("expires_at", jwt::claim(tokeExpiresAt))
        // Подписываем токен ключом SECRET
        .sign(jwt::algorithm::hs256{ "Wkb5e69a95d783e6a08e3Hl" });


    std::cout << "Token: " << token << std::endl;
    return token;
}

void DelitAcc (std::string acc_token, std::string chang_test = ""){
    std::cout << "Зашло в DelitAcc" << std::endl;

    // Пример использования httplib::Client для отправки внутреннего запроса
    httplib::Client cli("http://localhost:8080");
    json user_data;
    // Формируем URL для запроса данных пользователя
    std::string url = "/func/DelitAcc?AccessToken=" + token_h(acc_token) + "&NewAccess=" + chang_test;

    std::cout << "Тест: " << chang_test << std::endl;

    if (auto res = cli.Get(url.c_str())) {

        if (res->status == 401){
            std::cout << "401" << std::endl;
        }
    }
}

bool find_resolution(const std::vector<std::string>& id, const std::string& creat){
    for (auto creat_id : id){

        if (creat_id == creat){
            return true;
        }
    }
    return false;
}

// Обработчик создания дисциплины
void handle_create_discipline(const httplib::Request& req, httplib::Response& res) {

    static int last_id = 0;

    std::string token = req.has_param("AccessToken") ? req.get_param_value("AccessToken") : "";

    std::cout << "Токен" << std::endl;

    std::vector<std::string> ids = id_vec(token);

    if (!find_resolution(ids, "create_discipline")){
        res.status = 403;
        res.set_content("Unauthorized", "text/plain");
        return;
    }
    
    std::string name = req.has_param("name") ? req.get_param_value("name") : "";
    
    if (name.empty()) {
        //AccessToken доступа ids если 
        res.status = 200;
        res.set_content("Missing name", "text/plain");
        return;
    }

    int id = insertDiscipline(name, ids[0]);

    if (id == -1) {
        res.status = 500;
        res.set_content("Failed to create discipline", "text/plain");
        return;
    }

    last_id = id;

    std::string last_id_test = "chang_test:" + std::to_string(last_id);

    giveAcc(token, last_id_test);

    json response;
    response["id"] = last_id;

    res.status = 200;
    std::cout << "200" << std::endl;
    res.set_content(response.dump(), "application/json");

    return;
}


json toggle_test_status(std::string test_id) {

    std::cout << "test_id_toggle_test_status" << test_id << std::endl;
    PGconn* conn = connect_db();
    if (!conn) {
        return json{{"error", "Database connection failed"}};
    }

    // SQL-запрос для получения текущего статуса
    const char* select_query = R"(
        SELECT status 
        FROM tests 
        WHERE test_id = $1
    )";

    const char* params_select[1] = { test_id.c_str() };
    PGresult* res_select = PQexecParams(conn, select_query, 1, nullptr, params_select, nullptr, nullptr, 0);

    if (PQresultStatus(res_select) != PGRES_TUPLES_OK) {
        std::cerr << "Select error: " << PQerrorMessage(conn) << std::endl;
        PQclear(res_select);
        PQfinish(conn);
        return json{{"error", "Select failed"}};
    }

    // Проверяем, есть ли результат
    if (PQntuples(res_select) == 0) {
        PQclear(res_select);
        PQfinish(conn);
        return json{{"error", "Test not found"}};
    }

    // Получаем текущий статус
    std::string current_status = PQgetvalue(res_select, 0, 0);
    std::string new_status;

    // Переключаем статус
    if (current_status == "-1") {
        new_status = "0"; // Если текущий статус -1, меняем на 0
    } else if (current_status == "0") {
        new_status = "-1"; // Если текущий статус 0, меняем на -1
    } else {
        PQclear(res_select);
        PQfinish(conn);
        return json{{"error", "Invalid status"}};
    }

    // SQL-запрос для обновления статуса
    const char* update_query = R"(
        UPDATE tests 
        SET status = $1 
        WHERE test_id = $2
    )";

    const char* params_update[2] = { new_status.c_str(), test_id.c_str() };
    PGresult* res_update = PQexecParams(conn, update_query, 2, nullptr, params_update, nullptr, nullptr, 0);

    if (PQresultStatus(res_update) != PGRES_COMMAND_OK) {
        std::cerr << "Update error: " << PQerrorMessage(conn) << std::endl;
        PQclear(res_select);
        PQclear(res_update);
        PQfinish(conn);
        return json{{"error", "Update failed"}};
    }

    // Освобождаем ресурсы
    PQclear(res_select);
    PQclear(res_update);
    PQfinish(conn);

    return json{{"success", "Test status toggled", {"new_status", new_status}}};
}

void addGradesToDiscipline(json& discipline, const json& grade_id) {
    discipline["questions"] = json::array(); // Инициализируем массив для оценок

    int scoreSum = 0;
    int scoreMax = 0;

        for (const auto& grade_info : grade_id["questions"]) {
            json grade;
            grade["id"] = grade_info["id"];
            grade["text"] = grade_info["text"];
            grade["score"] = grade_info["score"];
            grade["contribution"] = grade_info["contribution"];

            discipline["questions"].push_back(grade);

            // Безопасная обработка баллов
            // std::string score_str = grade_info["score"].get<std::string>();
            // if (score_str != "-") {
            //     int score = std::stoi(score_str);
            // }
            std::string contribution_str = grade_info["contribution"].get<std::string>();
            if (contribution_str != "-") {
                int contribution = std::stoi(contribution_str);
                scoreSum += contribution; // Добавляем вклад к сумме
            }

            scoreMax += 100; // Предполагаем максимальный вклад 100
        }


    double scoreRes = (scoreMax > 0) ? (static_cast<double>(scoreSum) / scoreMax) * 100 : 0;

    // Дополнительная информация о сумме и максимальной оценке
    json score_info;
    score_info["scoreSum"] = scoreSum; 
    score_info["scoreMax"] = scoreMax; 
    score_info["scoreRes"] = scoreRes;
    discipline["score"] = score_info; // Добавляем информацию о оценках
}

void delete_discipline(int discipline_id) {
    PGconn* conn = connect_db();
    if (!conn) {
        return;
    }

    PGresult *res;

    // Удаляем все вопросы, связанные с дисциплиной
    res = PQexec(conn, ("DELETE FROM questions WHERE discipline_id = " + std::to_string(discipline_id) + ";").c_str());
    PQclear(res);

    // Удаляем все тесты, связанные с дисциплиной
    res = PQexec(conn, ("DELETE FROM tests WHERE discipline_id = " + std::to_string(discipline_id) + ";").c_str());
    PQclear(res);

    // Удаляем все связи учителей с дисциплиной
    res = PQexec(conn, ("DELETE FROM teacher_disciplines WHERE discipline_id = " + std::to_string(discipline_id) + ";").c_str());
    PQclear(res);

    // Удаляем все связи студентов с дисциплиной
    res = PQexec(conn, ("DELETE FROM student_disciplines WHERE discipline_id = " + std::to_string(discipline_id) + ";").c_str());
    PQclear(res);

    // Удаляем саму дисциплину
    res = PQexec(conn, ("DELETE FROM disciplines WHERE discipline_id = " + std::to_string(discipline_id) + ";").c_str());
    PQclear(res);

    std::cout << "Discipline deleted successfully." << std::endl;
}

void exclude_student_from_discipline(const std::string &student_id, int discipline_id) {
    PGconn* conn = connect_db();
    if (!conn) {
        return;
    }

    PGresult *res;

    // Удаляем все попытки студента по тестам, связанным с дисциплиной
    res = PQexec(conn, ("DELETE FROM student_attempts "
                        "WHERE student_id = '" + student_id + "' "
                        "AND test_id IN (SELECT test_id FROM tests WHERE discipline_id = " + std::to_string(discipline_id) + ");").c_str());
    PQclear(res);

    // Удаляем все ответы студента на вопросы, связанные с тестами этой дисциплины
    res = PQexec(conn, ("DELETE FROM student_answers "
                        "WHERE student_id = '" + student_id + "' "
                        "AND attempt_id IN (SELECT attempt_id FROM student_attempts "
                        "WHERE student_id = '" + student_id + "' "
                        "AND test_id IN (SELECT test_id FROM tests WHERE discipline_id = " + std::to_string(discipline_id) + "));").c_str());
    PQclear(res);

    // Удаляем связь студента с дисциплиной
    res = PQexec(conn, ("DELETE FROM student_disciplines "
                        "WHERE student_id = '" + student_id + "' "
                        "AND discipline_id = " + std::to_string(discipline_id) + ";").c_str());
    PQclear(res);

    std::cout << "Student excluded from discipline successfully." << std::endl;
}

json getChangeAndRecording(const std::vector<std::string>& id_user, const std::string& id_str) {
    std::cout << "ЗАШЕЛ getChangeAndRecording" << std::endl;
    json result;
    result["change"] = find_resolution(id_user, "chang_test:" + id_str) ? "true" : "false";
    result["recording"] = (result["change"] == "true" || find_resolution(id_user, "regDiscipline:" + id_str)) ? "true" : "false";

    std::cout << result.dump(4) << std::endl;
    return result;
}

json giveStudentData(const std::string& student_token) {

    std::cout << student_token << std::endl;
    std::cout << "Зашло в giveStudentData" << std::endl;

    // Пример использования httplib::Client для отправки внутреннего запроса
    httplib::Client cli("http://localhost:8080");
    json student_data;

    // Формируем URL для запроса данных студента
    std::string url = "/func/giveStudentData?AccessToken=" + student_token;

    if (auto res = cli.Get(url.c_str())) {
        if (res->status == 200) {
            // Предполагаем, что ответ в формате JSON
            student_data = json::parse(res->body);
        } else if (res->status == 401) {
            std::cout << "401 Unauthorized" << std::endl;
        }
    } else {
        std::cerr << "Ошибка при выполнении запроса" << std::endl;
    }

    return student_data; // Возвращаем данные студента
}

// Обработчик получения дисциплины
void handle_get_discipline(const httplib::Request& req, httplib::Response& res, const std::vector<std::string>& id_user) {
    
    std::string status_str = req.has_param("status") ? req.get_param_value("status") : "";
    std::string id_str = req.has_param("id") ? req.get_param_value("id") : "";

    std::string student_id = id_user[0]; 
    
    int id = std::stoi(id_str);
    if (id <= 0) {
        res.status = 400;
        res.set_content("Invalid ID", "text/plain");
        return;
    }

    json discipline;
    json discipline_id = get_discipline_info(id);

    std::cout << discipline_id << std::endl;

    if (discipline_id.is_null()) {
        res.status = 404;
        res.set_content("Discipline not found", "text/plain");
        return;
    }

    std::string teacher_id = discipline_id["teacher_id"];
    // Формируем JSON
    discipline["name"] = discipline_id["discipline_name"];
    discipline["teacher"] = {
        {"id", discipline_id["teacher_id"]},
        {"text", get_UserData(teacher_id)["user"]["name"]}
    };

    if (find_resolution(id_user, "chang_test:" + id_str)) {
        discipline["change"] = "true"; // ТОЛЬКО ЕСЛИ СОЗДАТЕЛЬ

    } else{
        discipline["change"] = "false";
    }

    if (discipline["change"] == "true" /*|| ID СТУДЕНДА*/){
        discipline["recording"] = "true"; //ЗАРЕГАН
    } else if (find_resolution(id_user, "regDiscipline:" + id_str)){
        discipline["recording"] = "true";
    } else {
        discipline["recording"] = "false";
    }

        if (status_str == "additional") {
        json response;
        response["change"] = discipline["change"];
        response["recording"] = discipline["recording"];


        // Устанавливаем ответ
        res.status = 200;
        res.set_content(response.dump(), "application/json");
        return; // Завершаем выполнение функции
    }

    discipline["questions"] = json::array();

    json participants_id = get_participants_for_discipline(id);

    std::cout << "Статус: '" << status_str << "'" << std::endl;

    std::cout << participants_id << std::endl;

    if (status_str == "undefined") {
        json tests = getTestsForDiscipline(id); // Получаем тесты для дисциплины по ID
        std::cout << tests.dump(4) << std::endl;

        // Проверяем, есть ли тесты
        if (tests["questions"].empty()) {
            std::cout << "Тесты не найдены для дисциплины с ID: " << id << std::endl;
        } else {
            std::cout << "Найденные тесты:" << std::endl;
            // Добавляем тесты в массив questions
            for (const auto& question : tests["questions"]) {
                discipline["questions"].push_back(question);
            }
        }
    }
    //json student_data = giveStudentData(student_id);
    if (status_str == "participants") {
        json participants = json::array();
        for (const auto& participant_info : participants_id) {
            json participant;
            participant["id"] = participant_info["student_id"];
            participant["text"] = get_UserData(participant_info["student_id"])["user"]["name"];
            participant["roles"] = "Студент";
            participants.push_back(participant);
        }
        discipline["questions"] = participants;
        res.status = 200;
        res.set_content(discipline.dump(), "application/json");
        return;
    }


    if (status_str == "grades") {
        json grade_id = get_grades_for_discipline(id, id_user[0]);
        if (!grade_id.empty()) { // Проверяем, что данные не пусты
            addGradesToDiscipline(discipline, grade_id);
        } else {
            std::cerr << "get_grades_for_discipline вернул пустой JSON" << std::endl;
        }
    }
    
    // Устанавливаем ответ
    res.status = 200;
    res.set_content(discipline.dump(), "application/json");
}