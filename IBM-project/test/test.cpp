#include <iostream>
#include <httplib.h>
#include <nlohmann/json.hpp>
#include <postgresql/libpq-fe.h>
#include "../reg/reg.h" 
#include "../discipline/discipline.h"

using json = nlohmann::json;

json get_test_info(int test_id) {
    PGconn* conn = connect_db();
    if (!conn) {
        return json(); // Ошибка подключения
    }

    std::cout << "Зашло в get_test_info" << std::endl;

    const char* query = R"(
        SELECT
            tt.test_id AS test_id,
            tt.test_name AS test_name,
            tt.discipline_id AS discipline_id
        FROM
            teacher_tests_with_disciplines tt
        WHERE
            tt.test_id = $1
    )";
    const char* params[1] = { std::to_string(test_id).c_str() };

    PGresult* res = PQexecParams(conn, query, 1, nullptr, params, nullptr, nullptr, 0);

    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        std::cerr << "SQL error: " << PQerrorMessage(conn) << std::endl;
        PQclear(res);
        PQfinish(conn);
        return json();
    }

    json test_info;

    if (PQntuples(res) > 0) {
        // Извлекаем данные из результата
        test_info["test_id"] = PQgetvalue(res, 0, 0);
        test_info["test_name"] = PQgetvalue(res, 0, 1);
        test_info["discipline_id"] = PQgetvalue(res, 0, 2);
    }

    PQclear(res);
    PQfinish(conn);
    return test_info;
}

int create_test(const std::string& discipline_id, const std::string& test_name, const std::string& teacher_id) {
    PGconn* conn = connect_db();
    if (!conn) {
        return -1; // Ошибка подключения
    }

    const char* check_discipline_query = "SELECT discipline_id FROM disciplines WHERE discipline_id = $1";
    const char* check_discipline_params[1] = { discipline_id.c_str() };
    PGresult* check_discipline_res = PQexecParams(conn, check_discipline_query, 1, nullptr, check_discipline_params, nullptr, nullptr, 0);

    if (PQntuples(check_discipline_res) == 0) {
        std::cerr << "Teacher with ID " << discipline_id << " does not exist." << std::endl;
        PQclear(check_discipline_res);
        PQfinish(conn);
        return -1; // Преподаватель не найден
    }
    PQclear(check_discipline_res);

    // Проверка существования teacher_id
    const char* check_teacher_query = "SELECT teacher_id FROM teachers WHERE teacher_id = $1";
    const char* check_teacher_params[1] = { teacher_id.c_str() };
    PGresult* check_teacher_res = PQexecParams(conn, check_teacher_query, 1, nullptr, check_teacher_params, nullptr, nullptr, 0);

    if (PQntuples(check_teacher_res) == 0) {
        std::cerr << "Teacher with ID " << teacher_id << " does not exist." << std::endl;
        PQclear(check_teacher_res);
        PQfinish(conn);
        return -1; // Преподаватель не найден
    }
    PQclear(check_teacher_res);

    // Вставляем тест
    const char* query = "INSERT INTO tests (test_name, discipline_id) VALUES ($1, $2) RETURNING test_id";
    const char* params[2] = { test_name.c_str(), discipline_id.c_str() };

    PGresult* res = PQexecParams(conn, query, 2, nullptr, params, nullptr, nullptr, 0);

    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        std::cerr << "SQL error: " << PQerrorMessage(conn) << std::endl;
        PQclear(res);
        PQfinish(conn);
        return -1;
    }

    int test_id = std::stoi(PQgetvalue(res, 0, 0));
    PQclear(res);

    // Связываем тест с учителем и дисциплиной
    const char* link_query = "INSERT INTO teacher_tests_with_disciplines (teacher_id, test_id, discipline_id, test_name) VALUES ($1, $2, $3, $4)";
    const char* link_params[4] = { teacher_id.c_str(), std::to_string(test_id).c_str(), discipline_id.c_str(), test_name.c_str() };

    res = PQexecParams(conn, link_query, 4, nullptr, link_params, nullptr, nullptr, 0);

    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        std::cerr << "SQL error: " << PQerrorMessage(conn) << std::endl;
        std::cout << "в create_test" << std::endl;
        PQclear(res);
        PQfinish(conn);
        return -1;
    }

    PQclear(res);
    PQfinish(conn);
    return test_id;
}

json get_questions_for_test(int test_id) {
    PGconn* conn = connect_db();
    if (!conn) {
        return json(); // Ошибка подключения
    }

    std::cout << "Зашло в get_questions_for_test" << std::endl;

    const char* query = R"(
        SELECT
            q.question_id AS id,
            q.text_question AS text,
            q.correct_answer_id AS res,
            a.answer_index AS answer_id,
            a.text_answer AS answer_text
        FROM
            questions q
        LEFT JOIN
            answers a ON q.question_id = a.question_id AND a.test_id = $1
        WHERE
            q.test_id = $1
    )";
    const char* params[1] = { std::to_string(test_id).c_str() };

    PGresult* res = PQexecParams(conn, query, 1, nullptr, params, nullptr, nullptr, 0);

    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        std::cerr << "SQL error: " << PQerrorMessage(conn) << std::endl;
        PQclear(res);
        PQfinish(conn);
        return json();
    }

    json questions_json = json::array();
    std::map<int, json> questions_map; // Для хранения вопросов по ID
    // std::map<int, int> question_id_mapping; // Для хранения нового ID для каждого question_id
    // int new_question_id = 1;

    for (int i = 0; i < PQntuples(res); ++i) {
        int question_id = std::stoi(PQgetvalue(res, i, 0));
        std::string question_text = PQgetvalue(res, i, 1);
        int correct_answer_id = PQgetvalue(res, i, 2) ? std::stoi(PQgetvalue(res, i, 2)) : 0; // Проверка на NULL
        int answer_id = PQgetvalue(res, i, 3) ? std::stoi(PQgetvalue(res, i, 3)) : 0; // Проверка на NULL
        std::string answer_text = PQgetvalue(res, i, 4) ? PQgetvalue(res, i, 4) : "";

        // Если вопрос еще не добавлен, создаем его
        if (questions_map.find(question_id) == questions_map.end()) {
            //question_id_mapping[question_id] = new_question_id++;

            json question_json;
            question_json["id"] = question_id;
            question_json["res"] = correct_answer_id;
            question_json["text"] = question_text;
            question_json["answers"] = json::array(); // Инициализация массива ответов
            questions_map[question_id] = question_json;
        }

        // Добавляем ответ к соответствующему вопросу
        if (!answer_text.empty()) {
            json answer_json;
            answer_json["id"] = answer_id;
            answer_json["text"] = answer_text;

            if (questions_map.find(question_id) != questions_map.end()) {
                questions_map.at(question_id)["answers"].push_back(answer_json); // Используем безопасный доступ
            } else {
                std::cerr << "Ошибка: вопрос с ID " << question_id << " не найден!" << std::endl;
            }
        }
    }

    // Переносим вопросы из map в массив
    for (const auto& pair : questions_map) {
        questions_json.push_back(pair.second);
    }

    PQclear(res);
    PQfinish(conn);
    return {{"questions", questions_json}};
}

int insert_question(const std::string& test_id, std::string discipline_id, const std::string& text, int correct_answer_id) {

    std::cout << "insert_question" << std::endl;
    PGconn* conn = connect_db();
    if (!conn) {
        return -1; // Ошибка подключения
    }

    const char* max_id_query = "SELECT COALESCE(MAX(question_id), 0) + 1 FROM questions WHERE test_id = $1";
    const char* max_id_params[1] = { test_id.c_str() };

    PGresult* max_id_res = PQexecParams(conn, max_id_query, 1, nullptr, max_id_params, nullptr, nullptr, 0);

    if (PQresultStatus(max_id_res) != PGRES_TUPLES_OK) {
        std::cerr << "SQL error (max_id_query): " << PQerrorMessage(conn) << std::endl;
        PQclear(max_id_res);
        PQfinish(conn);
        return -1; // Ошибка при поиске следующего question_id
    }

    int next_question_id = std::stoi(PQgetvalue(max_id_res, 0, 0));
    PQclear(max_id_res);

    const char* query = "INSERT INTO questions ( test_id, question_id, discipline_id, text_question, correct_answer_id) VALUES ($1, $2, $3, $4, $5) RETURNING question_id";
    const char* params[5] = { test_id.c_str(), std::to_string(next_question_id).c_str(),  std::to_string(std::stoi(discipline_id)).c_str(), text.c_str(), std::to_string(correct_answer_id).c_str() };

    PGresult* res = PQexecParams(conn, query, 5, nullptr, params, nullptr, nullptr, 0);

    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        std::cerr << "SQL error: " << PQerrorMessage(conn) << std::endl;
        std::cout << "insert_question" << std::endl;
        PQclear(res);
        PQfinish(conn);
        return -1; // Ошибка вставки
    }

    int question_id = std::stoi(PQgetvalue(res, 0, 0));
    PQclear(res);
    PQfinish(conn);
    return question_id; // Возвращаем ID вставленного вопроса
}

int insert_answer(const std::string& test_id, int question_id, const std::string& text_answer, int answer_index) {
    PGconn* conn = connect_db();
    if (!conn) {
        return -1; // Ошибка подключения
    }

    // Определяем следующий answer_id для указанного question_id
    const char* max_id_query = "SELECT COALESCE(MAX(answer_id), 0) + 1 FROM answers WHERE question_id = $1 AND test_id = $2";
    const char* max_id_params[2] = { std::to_string(question_id).c_str(), test_id.c_str() };

    PGresult* max_id_res = PQexecParams(conn, max_id_query, 2, nullptr, max_id_params, nullptr, nullptr, 0);

    if (PQresultStatus(max_id_res) != PGRES_TUPLES_OK) {
        std::cerr << "SQL error (max_id_query): " << PQerrorMessage(conn) << std::endl;
        PQclear(max_id_res);
        PQfinish(conn);
        return -1; // Ошибка при поиске следующего answer_id
    }

    int next_answer_id = std::stoi(PQgetvalue(max_id_res, 0, 0));
    PQclear(max_id_res);

    // Вставляем новый ответ
    const char* query = "INSERT INTO answers (test_id, question_id, answer_index, text_answer) "
                        "VALUES ($1, $2, $3, $4) RETURNING answer_id";
    const char* params[4] = {
        test_id.c_str(),
        std::to_string(question_id).c_str(),
        //std::to_string(next_answer_id).c_str(),
        std::to_string(answer_index).c_str(),
        text_answer.c_str()
    };

    PGresult* res = PQexecParams(conn, query, 4, nullptr, params, nullptr, nullptr, 0);

    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        std::cerr << "SQL error: " << PQerrorMessage(conn) << std::endl;
        std::cout << "insert_answer" << std::endl;
        PQclear(res);
        PQfinish(conn);
        return -1; // Ошибка вставки
    }

    int answer_id = std::stoi(PQgetvalue(res, 0, 0));
    PQclear(res);
    PQfinish(conn);
    return answer_id; // Возвращаем ID вставленного ответа
}

json get_test_results(int discipline_id) {
    PGconn* conn = connect_db();  // Подключаемся к базе данных
    if (!conn) {
        return json();  // Если не удается подключиться, возвращаем пустой JSON
    }

    // SQL-запрос для получения данных
    const char* query = R"(
        SELECT 
            sd.student_id,
            t.score,
            t.test_name,
            t.test_id,
            t.contribution,
            t.discipline_id
        FROM 
            tests t
        JOIN 
            student_disciplines sd ON t.student_id = sd.student_id
        WHERE 
            t.discipline_id = $1
    )";

    // Параметры запроса
    std::string discipline_id_str = std::to_string(discipline_id);
    const char* params[1] = { std::to_string(discipline_id).c_str() };

    // Выполняем запрос
    PGresult* res = PQexecParams(conn, query, 1, nullptr, params, nullptr, nullptr, 0);

    // Проверяем успешность выполнения запроса
    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        std::cerr << "Ошибка SQL: " << PQerrorMessage(conn) << std::endl;
        PQclear(res);
        PQfinish(conn);
        return json();  // В случае ошибки возвращаем пустой JSON
    }

      // Выводим количество полученных строк для отладки
    int num_rows = PQntuples(res);
    std::cout << "Количество участников: " << num_rows << std::endl;

    json test_results = json::array();  // Массив для хранения результатов тестов

    // Проходим по всем строкам результата
    for (int i = 0; i < PQntuples(res); ++i) {
        json test_result;
        test_result["student_id"] = PQgetvalue(res, i, 0);  // student_id
        test_result["score"] = PQgetvalue(res, i, 1);        // score (оценка)
        test_result["test_name"] = PQgetvalue(res, i, 2);    // test_name (название теста)
        test_result["test_id"] = PQgetvalue(res, i, 3);      // test_id (идентификатор теста)
        test_result["contribution"] = PQgetvalue(res, i, 4); // contribution (вклад в итог)
        test_result["discipline_id"] = PQgetvalue(res, i, 5); // discipline_id (идентификатор дисциплины)
        
        test_results.push_back(test_result);  // Добавляем результат в массив
    }

    PQclear(res);  // Освобождаем память
    PQfinish(conn);  // Закрываем соединение с базой
    return test_results;  // Возвращаем массив с результатами
}

int insert_test_result(int test_id, const std::string& student_id, const std::string& score, const std::string& contribution) {
    PGconn* conn = connect_db();
    if (!conn) {
        std::cerr << "Ошибка подключения к базе данных." << std::endl;
        return -1; // Ошибка подключения
    }

    const char* query = "INSERT INTO test_results (test_id, student_id, score, contribution) VALUES ($1, $2, $3, $4) RETURNING result_id";
    const char* params[4] = { std::to_string(test_id).c_str(), student_id.c_str(), score.c_str(), contribution.c_str() };

    PGresult* res = PQexecParams(conn, query, 4, nullptr, params, nullptr, nullptr, 0);

    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        std::cerr << "SQL error: " << PQerrorMessage(conn) << std::endl;
        PQclear(res);
        PQfinish(conn);
        return -1; // Ошибка вставки
    }

    int result_id = std::stoi(PQgetvalue(res, 0, 0));
    PQclear(res);
    PQfinish(conn);
    return result_id; // Возвращаем ID вставленного результата
}

json get_all_students_test_results(const std::string& test_id) {
    PGconn* conn = connect_db();  // Подключаемся к базе данных
    if (!conn) {
        return json();  // Если не удается подключиться, возвращаем пустой JSON
    }

    // SQL-запрос для получения всех студентов, прошедших тест
    const char* query = R"(
        SELECT 
            sa.student_id,
            sa.score,
            sa.contribution
        FROM 
            student_attempts sa
        WHERE 
            sa.test_id = $1
    )";

    const char* params[1] = { test_id.c_str() }; // Передаем только один параметр

    // Выполняем запрос
    PGresult* res = PQexecParams(conn, query, 1, nullptr, params, nullptr, nullptr, 0);

    // Проверяем успешность выполнения запроса
    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        std::cerr << "Ошибка SQL: " << PQerrorMessage(conn) << std::endl;
        PQclear(res);
        PQfinish(conn);
        return json();  // В случае ошибки возвращаем пустой JSON
    }

    json results = json::array();  // Массив для хранения результатов

    // Проходим по всем строкам результата
    for (int i = 0; i < PQntuples(res); ++i) {
        json result;
        result["student_id"] = PQgetvalue(res, i, 0);  // student_id
        result["contribution"] = PQgetvalue(res, i, 2);        // score
        //result["contribution"] = PQgetvalue(res, i, 2); // contribution
        
        results.push_back(result);  // Добавляем результат в массив
    }

    PQclear(res);  // Освобождаем память
    PQfinish(conn);  // Закрываем соединение с базой
    return results;  // Возвращаем массив с результатами
}

void delete_questions_and_answers(int test_id) {
    PGconn* conn = connect_db();
    if (!conn) {
        std::cerr << "Ошибка подключения к базе данных." << std::endl;
        return;
    }

    std::string sql = "SELECT delete_test_data_post(" + std::to_string(test_id) + ");";

    // Выполняем запрос
    PGresult *res = PQexec(conn, sql.c_str());

    // Проверяем результат выполнения
    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        std::cerr << "Error executing query: " << PQerrorMessage(conn) << std::endl;
    } else {
        std::cout << "Data for test_id " << test_id << " deleted successfully." << std::endl;
    }

    PQclear(res);
    PQfinish(conn);
}


void handle_create_test(const httplib::Request& req, httplib::Response& res) {
    static int last_id = 0;

    std::string token = req.has_param("AccessToken") ? req.get_param_value("AccessToken") : "";

    std::cout << "Токен: " << token << std::endl;

    std::vector<std::string> ids = id_vec(token);
    
    std::string discipline_id = req.has_param("id") ? req.get_param_value("id") : "";

    if (!find_resolution(ids, "chang_test:" + discipline_id)) {
        res.status = 403;
        res.set_content("Unauthorized", "text/plain");
        return;
    }

    std::string test_name = req.has_param("name") ? req.get_param_value("name") : "";

    std::cout << test_name << std::endl;
    std::cout << discipline_id << std::endl;

    if (test_name.empty() || discipline_id.empty()) {
        res.status = 400; // Ошибка 400: отсутствует имя теста или ID дисциплины
        std::cout << "400" << std::endl;
        res.set_content("Missing name or discipline ID", "text/plain");
        return;
    }

    // Вставляем тест в базу данных
    int test_id = create_test(discipline_id, test_name, ids[0]); // Параметр description можно оставить пустым, если он не нужен

    if (test_id == -1) {
        res.status = 500;
        res.set_content("Failed to create test", "text/plain");
        return;
    }

    last_id = test_id;

    json response;
    response["id"] = last_id;

    res.status = 200;
    std::cout << "200" << std::endl;
    res.set_content(response.dump(), "application/json");

    return;
}

bool is_test_attempted(int test_id) {
    PGconn* conn = connect_db();
    if (!conn) {
        std::cerr << "Ошибка подключения к базе данных." << std::endl;
        return false;  // Невозможно подключиться к базе данных
    }

    // SQL-запрос для проверки существующих попыток для данного test_id
    std::string query = R"(
        SELECT COUNT(*) 
        FROM student_attempts 
        WHERE test_id = $1
    )";

    const char* params[1] = { std::to_string(test_id).c_str() };

    PGresult* res = PQexecParams(conn, query.c_str(), 1, nullptr, params, nullptr, nullptr, 0);
    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        std::cerr << "Ошибка выполнения запроса: " << PQerrorMessage(conn) << std::endl;
        PQclear(res);
        PQfinish(conn);
        return false;
    }

    int attempt_count = std::stoi(PQgetvalue(res, 0, 0));
    PQclear(res);
    PQfinish(conn);

    return attempt_count > 0;  // Если попытки существуют, вернем true
}

void post_test (const httplib::Request& req, httplib::Response& res){

    std::cout << "ЗАШЛЕ В post_test" <<std::endl;

    std::string token = req.has_param("AccessToken") ? req.get_param_value("AccessToken") : "";

    if (token.empty() || !validate_jwt(token)){
        res.status = 401;
        res.set_content("401", "text/plain");
        return;
    }

    // Получаем тело запроса (JSON)
    json request_data;
    try {
        request_data = json::parse(req.body);
    } catch (const std::exception& e) {
        res.status = 400; // Неверный JSON
        res.set_content("Invalid JSON", "text/plain");
        return;
    }

    std::string test_id = req.has_param("id") ? req.get_param_value("id") : "";

    std::cout << "test_id: " << test_id << std::endl;
    std::string discipline_id = request_data["discipline"];

    json questions = request_data["questions"];

    // Проверка на наличие вопросов
    if (questions.empty()) {
        res.status = 400; // Неверный запрос
        res.set_content("No questions provided", "text/plain");
        return;
    }

    if (is_test_attempted(std::stoi(test_id))) {  // Псевдокод для проверки попыток
        res.status = 400; // Ошибка
        res.set_content("Test cannot be edited because attempts already exist.", "text/plain");
        return;
    }
    delete_questions_and_answers(std::stoi(test_id));

    // Вставляем вопросы в базу данных
    for (const auto& question : questions) {
        // Извлекаем текст вопроса и правильный ответ
        std::string question_text = question["text"];
        int correct_answer_id = question["res"];

        //int question_id = new_question_id++;

        // Вставляем вопрос в базу данных
        int question_id = insert_question(test_id, discipline_id, question_text, correct_answer_id);
        if (question_id == -1) {
            res.status = 500; // Ошибка вставки
            res.set_content("Failed to insert question", "text/plain");
            return;
        }

        // Вставляем ответы в базу данных
        json answers = question["answers"];
        int answer_index = 1;
        for (const auto& answer : answers) {
            // Вставляем ответ без передачи correct_answer_id
            int answer_id = insert_answer(test_id, question_id, answer["text"], answer_index); 
            if (answer_id == -1) {
                res.status = 500; // Ошибка вставки
                res.set_content("Failed to insert answer", "text/plain");
                return;
            }
            answer_index++;
        }
    }
    // Устанавливаем успешный ответ
    res.status = 200; // Успешно
    res.set_content("Test data processed successfully", "text/plain");
}

json populate_test_questions(int test_id) {

    std::cout << "ЗАШЛО В populate_test_questions" << std::endl;
    json test_json;
    json questions = get_questions_for_test(test_id); // Получаем вопросы для теста

    test_json["questions"] = json::array();

    for (const auto& question : questions["questions"]) {
        json question_json;
        question_json["id"] = question["id"];
        question_json["res"] = question["res"];
        question_json["text"] = question["text"];
        question_json["answers"] = json::array();

        for (const auto& answer : question["answers"]) {
            json answer_json;
            answer_json["id"] = answer["id"];
            answer_json["text"] = answer["text"];
            question_json["answers"].push_back(answer_json);
        }

        test_json["questions"].push_back(question_json);
    }

    return test_json; // Возвращаем сформированный JSON
}

void addParticipantsToTest(json& test, const json& participants_id) {
    std::cout << "Зашло в addParticipantsToTest" << std::endl;
    test["questions"] = json::array();

    std::cout << "participants_id: " << participants_id.dump() << std::endl;


    for (const auto& participant_info : participants_id) {
        json participant;
        std::cout << "questions" << std::endl;
        participant["id"] = participant_info["student_id"];
        participant["text"] = get_UserData(participant_info["student_id"])["user"]["name"];
        participant["score"] = participant_info["contribution"];
        test["questions"].push_back(participant);
    }
}

void calculate_and_update_score(const std::string& student_id) {
    std::cout << "calculate_and_update_score" << std::endl;
    PGconn* conn = connect_db();
    if (!conn) {
        std::cerr << "Ошибка подключения к базе данных" << std::endl;
        return;
    }

    // Шаг 1: Найти текущую активную попытку
    std::string active_attempt_query = R"(
        SELECT attempt_id, test_id
        FROM student_attempts 
        WHERE student_id = $1 AND contribution IS NULL 
        ORDER BY data DESC
        LIMIT 1
    )";
    const char* params1[1] = { student_id.c_str() };
    PGresult* res_active_attempt = PQexecParams(conn, active_attempt_query.c_str(), 1, nullptr, params1, nullptr, nullptr, 0);

    if (PQresultStatus(res_active_attempt) != PGRES_TUPLES_OK || PQntuples(res_active_attempt) == 0) {
        std::cerr << "Активная попытка не найдена или ошибка запроса: " << PQerrorMessage(conn) << std::endl;
        PQclear(res_active_attempt);
        PQfinish(conn);
        return;
    }

    const char* attempt_id = PQgetvalue(res_active_attempt, 0, 0);
    const char* test_id = PQgetvalue(res_active_attempt, 0, 1);

    std::cout << attempt_id << std::endl;
    std::cout << test_id << std::endl;

    PQclear(res_active_attempt);

    // Шаг 2: Подсчитать количество правильных ответов
    std::string correct_answers_query = R"(
        SELECT COUNT(*) AS correct_answers
        FROM student_answers
        WHERE attempt_id = $1 AND is_correct = true
    )";
    const char* params2[1] = { attempt_id };
    PGresult* res_correct = PQexecParams(conn, correct_answers_query.c_str(), 1, nullptr, params2, nullptr, nullptr, 0);

    if (PQresultStatus(res_correct) != PGRES_TUPLES_OK) {
        std::cerr << "Ошибка подсчета правильных ответов: " << PQerrorMessage(conn) << std::endl;
        PQclear(res_correct);
        PQfinish(conn);
        return;
    }
    int correct_answers = 0;
    const char* correct_answers_value = PQgetvalue(res_correct, 0, 0);
    if (correct_answers_value != nullptr && correct_answers_value[0] != '\0') {
        correct_answers = std::stoi(correct_answers_value);

        std::cout << correct_answers << std::endl;
    }
    PQclear(res_correct);

    std::cout << correct_answers << std::endl;

    // Шаг 3: Подсчитать общее количество вопросов
    std::string total_questions_query = R"(
        SELECT COUNT(*) AS quantity
        FROM questions
        WHERE test_id = $1
    )";
    const char* params3[1] = { test_id };
    PGresult* res_total = PQexecParams(conn, total_questions_query.c_str(), 1, nullptr, params3, nullptr, nullptr, 0);

    if (PQresultStatus(res_total) != PGRES_TUPLES_OK) {
        std::cerr << "Ошибка подсчета общего количества вопросов: " << PQerrorMessage(conn) << std::endl;
        PQclear(res_total);
        PQfinish(conn);
        return;
    }
    int total_questions = 0;
    const char* total_questions_value = PQgetvalue(res_total, 0, 0);
    if (total_questions_value != nullptr && total_questions_value[0] != '\0') {
        total_questions = std::stoi(total_questions_value);

        std::cout << total_questions << std::endl;
    }

    std::cout << total_questions << std::endl;

    PQclear(res_total);

    // Шаг 4: Вычисление баллов и процента выполнения
    int score = correct_answers; // Количество баллов за попытку равно числу правильных ответов
    float contribution = (static_cast<float>(correct_answers) / total_questions) * 100;

    // Шаг 5: Обновление текущей попытки
    std::string update_attempt_query = R"(
        UPDATE student_attempts
        SET score = $1, contribution = $2
        WHERE attempt_id = $3
    )";
    const char* params4[3] = { std::to_string(score).c_str(), std::to_string(contribution).c_str(), attempt_id };
    PGresult* res_update_attempt = PQexecParams(conn, update_attempt_query.c_str(), 3, nullptr, params4, nullptr, nullptr, 0);

    if (PQresultStatus(res_update_attempt) != PGRES_COMMAND_OK) {
        std::cerr << "Ошибка обновления текущей попытки: " << PQerrorMessage(conn) << std::endl;
        PQclear(res_update_attempt);
        PQfinish(conn);
        return;
    }
    PQclear(res_update_attempt);

    // Шаг 6: Проверить и обновить лучший результат студента score
    std::string best_score_query = R"(
        SELECT MAX(contribution)
        FROM student_attempts
        WHERE student_id = $1
    )";
    PGresult* res_best_score = PQexecParams(conn, best_score_query.c_str(), 1, nullptr, params1, nullptr, nullptr, 0);

    if (PQresultStatus(res_best_score) != PGRES_TUPLES_OK) {
        std::cerr << "Ошибка запроса на лучший результат: " << PQerrorMessage(conn) << std::endl;
        PQclear(res_best_score);
        PQfinish(conn);
        return;
    }
    int best_score = 0;
    const char* best_score_value = PQgetvalue(res_best_score, 0, 0);
    if (best_score_value != nullptr && best_score_value[0] != '\0') {
        best_score = std::stoi(best_score_value);
    }
    PQclear(res_best_score);

    // Если текущий результат лучший, обновляем resTest
    if (contribution >= best_score) {
        std::string update_best_result_query = R"(
            UPDATE student_attempts
            SET restest = $1
            WHERE attempt_id = $2
        )";
        const char* params5[2] = { std::to_string(contribution).c_str(), attempt_id };
        PGresult* res_update_best = PQexecParams(conn, update_best_result_query.c_str(), 2, nullptr, params5, nullptr, nullptr, 0);

        if (PQresultStatus(res_update_best) != PGRES_COMMAND_OK) {
            std::cerr << "Ошибка обновления наилучшей попытки: " << PQerrorMessage(conn) << std::endl;
            PQclear(res_update_best);
            PQfinish(conn);
            return;
        }
        PQclear(res_update_best);
    }

    std::cout << "Результаты обновлены: score = " << score << ", contribution = " << contribution << "%" << std::endl;
    PQfinish(conn);
}

// json get_student_attempts(int test_id) {
//     PGconn* conn = connect_db();
//     if (!conn) {
//         return json();
//     }

//     // Получаем попытки для всех студентов по test_id
//     const char* attempt_query = R"(
//         SELECT
//             s.student_id,
//             sa.attempt_id,
//             sa.data, -- Дата попытки
//             sa.score, -- Баллы
//             sa.contribution, -- Процент от максимального количества баллов
//             sa.restest -- Наилучший результат
//         FROM
//             student_attempts sa
//         JOIN
//             students s ON sa.student_id = s.student_id
//         WHERE
//             sa.test_id = $1
//         ORDER BY
//             sa.data ASC -- Сортируем по дате попытки
//     )";

//     const char* attempt_params[1] = { std::to_string(test_id).c_str() };
//     PGresult* attempt_res = PQexecParams(conn, attempt_query, 1, nullptr, attempt_params, nullptr, nullptr, 0);

//     if (PQresultStatus(attempt_res) != PGRES_TUPLES_OK) {
//         std::cerr << "SQL error: " << PQerrorMessage(conn) << std::endl;
//         PQclear(attempt_res);
//         PQfinish(conn);
//         return json();
//     }

//     json attempts_json = json::array();

//     // Проходим по всем попыткам
//     for (int j = 0; j < PQntuples(attempt_res); ++j) {
//         json attempt;
//         attempt["student_id"] = PQgetvalue(attempt_res, j, 0);
//         attempt["attempt_id"] = std::stoi(PQgetvalue(attempt_res, j, 1));
//         attempt["attempt_date"] = PQgetvalue(attempt_res, j, 2); // Дата попытки
//         attempt["score"] = std::stoi(PQgetvalue(attempt_res, j, 3)); // Баллы
//         attempt["contribution"] = std::stof(PQgetvalue(attempt_res, j, 4)); // Процент
//         attempt["restest"] = PQgetvalue(attempt_res, j, 5); // Наилучший результат
//         attempts_json.push_back(attempt);
//     }

//     //std::cout << attempts_json.dump(4) << std::endl;

//     PQclear(attempt_res);
//     PQfinish(conn);
//     return attempts_json;
// }

json get_student_attempts(const std::string& student_id, int test_id) {
    std::cout << "======get_student_attempts======" << std::endl;
    std::cout << "student_id: " << student_id << ", test_id: " << test_id << std::endl;

    PGconn* conn = connect_db();
    if (!conn) {
        return json(); // Ошибка подключения
    }

    json attempts = json::array();

    std::string query = R"(
        SELECT 
            attempt_id,
            data AS attempt_date,
            score,
            contribution
        FROM 
            student_attempts
        WHERE 
            student_id = $1 AND test_id = $2
        ORDER BY 
            data DESC
    )";

    const char* params[2] = { student_id.c_str(), std::to_string(test_id).c_str() };
    PGresult* res = PQexecParams(conn, query.c_str(), 2, nullptr, params, nullptr, nullptr, 0);

    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        std::cerr << "Ошибка выполнения запроса: " << PQerrorMessage(conn) << std::endl;
        PQclear(res);
        return attempts; // Пустой результат
    }

    int numRows = PQntuples(res);
    std::cout << "Количество строк: " << numRows << std::endl;

    for (int i = 0; i < numRows; ++i) {
        json attempt = {
            {"attempt_id", PQgetvalue(res, i, 0)},
            {"attempt_date", PQgetvalue(res, i, 1)},
            {"score", std::stoi(PQgetvalue(res, i, 2))},
            {"contribution", std::stof(PQgetvalue(res, i, 3))}
        };
        attempts.push_back(attempt);
    }

    PQclear(res);
    return attempts;
}

// Получение всех вопросов теста
json get_test_questions(int test_id) {
    PGconn* conn = connect_db();
    if (!conn) {
        return json();
    }

    const char* query = R"(
        SELECT question_id, text_question, correct_answer_id
        FROM questions
        WHERE test_id = $1
    )";

    const char* params[1] = { std::to_string(test_id).c_str() };

    PGresult* res = PQexecParams(conn, query, 1, nullptr, params, nullptr, nullptr, 0);

    json questions = json::array();
    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        std::cerr << "SQL error: " << PQerrorMessage(conn) << std::endl;
        PQclear(res);
        PQfinish(conn);
        return json();
    }

    // Извлекаем вопросы
    for (int i = 0; i < PQntuples(res); ++i) {
        json question;
        question["id"] = PQgetvalue(res, i, 0);
        question["text"] = PQgetvalue(res, i, 1);
        question["res"] = PQgetvalue(res, i, 2); // Правильный ответ
        questions.push_back(question);
    }

    PQclear(res);
    PQfinish(conn);

    return questions;
}

// Получение всех ответов для вопроса
json get_answers_for_question(int question_id) {
    PGconn* conn = connect_db();
    if (!conn) {
        return json();
    }

    const char* query = R"(
        SELECT answer_id, text_answer
        FROM answers
        WHERE question_id = $1
    )";

    const char* params[1] = { std::to_string(question_id).c_str() };

    PGresult* res = PQexecParams(conn, query, 1, nullptr, params, nullptr, nullptr, 0);

    json answers = json::array();
    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        std::cerr << "SQL error: " << PQerrorMessage(conn) << std::endl;
        PQclear(res);
        PQfinish(conn);
        return json();
    }

    // Извлекаем ответы
    for (int i = 0; i < PQntuples(res); ++i) {
        json answer;
        answer["id"] = PQgetvalue(res, i, 0);
        answer["text"] = PQgetvalue(res, i, 1);
        answers.push_back(answer);
    }

    PQclear(res);
    PQfinish(conn);

    return answers;
}

// Получить ответы студента для теста
json get_student_answers(int student_id, int test_id) {
    PGconn* conn = connect_db();
    if (!conn) {
        return json();
    }

    const char* query = R"(
        SELECT question_id, selected_answer_id
        FROM student_answers
        WHERE student_id = $1 AND test_id = $2
    )";

    const char* params[2] = { std::to_string(student_id).c_str(), std::to_string(test_id).c_str() };

    PGresult* res = PQexecParams(conn, query, 2, nullptr, params, nullptr, nullptr, 0);

    json student_answers = json::array();
    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        std::cerr << "SQL error: " << PQerrorMessage(conn) << std::endl;
        PQclear(res);
        PQfinish(conn);
        return json();
    }

    // Извлекаем ответы
    for (int i = 0; i < PQntuples(res); ++i) {
        json student_answer;
        student_answer["question_id"] = PQgetvalue(res, i, 0);
        student_answer["selected_answer_id"] = PQgetvalue(res, i, 1);
        student_answers.push_back(student_answer);
    }

    PQclear(res);
    PQfinish(conn);

    return student_answers;
}

json generate_test_json(int test_id, int student_id) {
    std::cout << "!!generate_test_json!!"<< std::endl;
    json test_result;
    json questions = get_test_questions(test_id);

    test_result["id"] = test_id;
    test_result["quantity"] = questions.size();

    for (auto& question : questions) {
        int question_id = question["id"];
        json answers = get_answers_for_question(question_id);
        json student_answers = get_student_answers(student_id, test_id);

        // Находим ответ студента для текущего вопроса
        int selected_answer_id = 0;
        for (const auto& student_answer : student_answers) {
            if (student_answer["question_id"] == question_id) {
                selected_answer_id = student_answer["selected_answer_id"];
                break;
            }
        }

        // Формируем JSON для вопроса
        json question_json;
        question_json["id"] = question_id;
        question_json["text"] = question["text"];
        question_json["res"] = question["res"]; // правильный ответ
        question_json["now"] = selected_answer_id;

        // Добавляем ответы
        question_json["answers"] = answers;

        // Добавляем вопрос в итоговый результат
        test_result["questions"].push_back(question_json);
    }

    return test_result;
}

void populate_student_attempts(json& test, const std::string& student_id, int test_id) {
    std::cout << "Зашло в populate_student_attempts" << std::endl;

    // Получаем информацию о попытках теста для студента
    json attempts = get_student_attempts(student_id, test_id);

    // Проверяем, есть ли попытки
    if (attempts.empty()) {
        std::cout << "Попытки отсутствуют для test_id: " << test_id << std::endl;
        return;
    }

    // Инициализация массива для попыток
    test["attempts"] = json::array();

    double best_score = 0.0; // Переменная для хранения лучшего результата

    // Обрабатываем все попытки
    for (const auto& attempt_info : attempts) {
        json attempt;

        // Заполняем информацию о попытке
        attempt["id"] = attempt_info["attempt_id"]; // ID попытки
        attempt["date"] = attempt_info["attempt_date"]; // Дата попытки
        attempt["ball"] = std::to_string(attempt_info["contribution"].get<double>()) + "%"; // Процент
        attempt["score"] = attempt_info["score"]; // Баллы за попытку

        // Находим лучший результат
        double contribution = attempt_info["contribution"].get<double>();
        if (contribution > best_score) {
            best_score = contribution;
        }

        // Добавляем информацию о попытке в массив
        test["attempts"].push_back(attempt);
    }

    // Добавляем наилучший результат в test
    test["resTest"] = std::to_string(best_score) + "%";

    std::cout << "Результат test после заполнения: " << test.dump(4) << std::endl;
}

json getStudentData(std::string trest_id, std::string student_id) {
    std::cout << "======getStudentData======" << std::endl;
    std::cout << "student_id:" << student_id << std::endl;
    PGconn* conn = connect_db();
    if (!conn) {
        return json(); // Ошибка подключения
    }

    json result;
    result["questions"] = json::array(); // Инициализируем поле для вопросов

    std::string query = R"(
        SELECT 
            d.discipline_id,
            q.question_id, 
            q.text_question, 
            q.correct_answer_id, -- Добавляем правильный ответ
            sa.selected_answer_id,
            a.answer_index, 
            a.text_answer,
            st.contribution -- Добавляем contribution из student_attempts
        FROM 
            student_answers sa 
        JOIN 
            questions q ON sa.question_id = q.question_id 
        JOIN 
            answers a ON sa.question_id = a.question_id 
        JOIN 
            student_disciplines d ON sa.student_id = d.student_id 
        JOIN 
            student_attempts st ON sa.student_id = st.student_id -- Добавляем JOIN с student_attempts
        WHERE 
            sa.student_id = $1
        ORDER BY 
            q.question_id, a.answer_id
    )";

    const char* params[1] = { student_id.c_str() };
    PGresult* res = PQexecParams(conn, query.c_str(), 1, nullptr, params, nullptr, nullptr, 0);

    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        std::cerr << "Ошибка выполнения запроса: " << PQerrorMessage(conn) << std::endl;
        PQclear(res);
        return result;
    }

    int numRows = PQntuples(res);
    std::cout << "Количество строк: " << numRows << std::endl;

    if (numRows == 0) {
        PQclear(res);
        return result; // Пустой результат
    }

    // Используем map для группировки ответов по вопросам
    std::map<int, json> questions;

    for (int i = 0; i < numRows; ++i) {
        if (i == 0) {
            result["resTest"] = std::stof(PQgetvalue(res, i, 7));
        }

        int question_id = std::stoi(PQgetvalue(res, i, 1));
        std::string question_text = PQgetvalue(res, i, 2);
        int correct_answer_id = std::stoi(PQgetvalue(res, i, 3)); // Правильный ответ
        int selected_answer_id = std::stoi(PQgetvalue(res, i, 4));
        int answer_index = std::stoi(PQgetvalue(res, i, 5));
        std::string answer_text = PQgetvalue(res, i, 6);

        // Если вопрос ещё не добавлен в map, добавляем его
        if (questions.find(question_id) == questions.end()) {
            questions[question_id] = {
                {"id", question_id},
                {"res", correct_answer_id}, // Правильный ответ
                {"now", selected_answer_id}, // Выбранный ответ
                {"text", question_text},
                {"answers", json::array()}
            };
        }
        bool answer_exists = false;
        for (const auto& answer : questions[question_id]["answers"]) {
            if (answer["id"] == answer_index) {
                answer_exists = true;
                break;
            }
        }

        if (!answer_exists) {
        // Добавляем текущий ответ к списку ответов вопроса
        questions[question_id]["answers"].push_back({
            {"id", answer_index},
            {"text", answer_text}
        });
        }
    }

    // Переносим данные из map в JSON
    for (const auto& [question_id, question_data] : questions) {
        result["questions"].push_back(question_data);
    }

    PQclear(res);

    std::cout << result.dump(4) << std::endl;
    return result;
}

void handle_get_test(const httplib::Request& req, httplib::Response& res, std::vector<std::string> id_user) {

    std::string status_str = req.has_param("status") ? req.get_param_value("status") : "";
    std::string id_str = req.has_param("id") ? req.get_param_value("id") : "";

    int id = std::stoi(id_str);
    if (id <= 0) {
        res.status = 400;
        res.set_content("Invalid ID", "text/plain");
        return;
    }

    json test;
    json test_info = get_test_info(id);

    if (test_info.is_null()) {
        res.status = 404;
        res.set_content("Test not found", "text/plain");
        return;
    }

    test["discipline"] = test_info["discipline_id"];

    // Проверка прав на изменение
    if (find_resolution(id_user, "chang_test:" + test_info["discipline_id"].get<std::string>())) {
        test["change"] = "true"; // ТОЛЬКО ЕСЛИ СОЗДАТЕЛЬ
    } else {
        test["change"] = "false";
    }

    // Проверка на запись
    if (test["change"] == "true") {
        test["recording"] = "true"; // ЗАРЕГАН
    } else if (find_resolution(id_user, "regDiscipline:" + test_info["discipline_id"].get<std::string>())) {
        test["recording"] = "true";
    } else {
        test["recording"] = "false";
    }

    if (status_str == "undefined") {
        test["text"] = test_info["test_name"];
        json questions = get_student_attempts(id_user[0], id); 
        test["attempts"] = json::array();
        populate_student_attempts(test, id_user[0], id);
    } else if (status_str == "changeTest"){
        json questions_json = populate_test_questions(id);
        test["questions"] = questions_json["questions"];
    } else if (status_str == "participants"){
        json participants = get_all_students_test_results( std::to_string(id));
        addParticipantsToTest(test, participants);
    } else if (status_str == "CheckTrying"){
        std::cout << "CheckTrying " << id_str << std::endl;
        //test["resTest"] = 45;
        json getStuden = getStudentData(id_str, id_user[0]);
        test["questions"] = getStuden["questions"];
        if (getStuden.contains("resTest")) {
            test["resTest"] = getStuden["resTest"]; // Копируем resTest
        } else {
            std::cerr << "resTest не найден в getStudentData" << std::endl;
        }
    }

    std::cout << test << std::endl;

    // Устанавливаем ответ
    res.status = 200;
    res.set_content(test.dump(), "application/json");
}