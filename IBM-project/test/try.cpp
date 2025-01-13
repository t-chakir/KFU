#include <iostream>
#include <httplib.h>
#include <nlohmann/json.hpp>
#include <postgresql/libpq-fe.h>
#include "../reg/reg.h" 
#include "../discipline/discipline.h"
#include "test.h"

void update_student_trying(const std::string& student_id, int question_id, int new_selected_answer_id) {
    PGconn* conn = connect_db();
    if (!conn) {
        std::cerr << "Ошибка подключения к базе данных" << std::endl;
        return;
    }

    // Получаем test_id из последней попытки студента
    std::string attempt_query = R"(
        SELECT test_id
        FROM student_attempts
        WHERE student_id = $1
        ORDER BY data DESC
        LIMIT 1
    )";

    const char* attempt_params[1] = { student_id.c_str() };
    PGresult* attempt_res = PQexecParams(conn, attempt_query.c_str(), 1, nullptr, attempt_params, nullptr, nullptr, 0);

    if (PQresultStatus(attempt_res) != PGRES_TUPLES_OK || PQntuples(attempt_res) == 0) {
        std::cerr << "Ошибка выполнения SQL-запроса: " << PQerrorMessage(conn) << std::endl;
        PQclear(attempt_res);
        PQfinish(conn);
        return;
    }

    int test_id = std::stoi(PQgetvalue(attempt_res, 0, 0));
    PQclear(attempt_res);

    // Получаем правильный ответ для проверки
    std::string correct_query = R"(
        SELECT correct_answer_id
        FROM questions
        WHERE test_id = $1 AND question_id = $2
    )";

    const char* correct_params[2] = {
        std::to_string(test_id).c_str(),
        std::to_string(question_id).c_str()
    };
    PGresult* correct_res = PQexecParams(conn, correct_query.c_str(), 2, nullptr, correct_params, nullptr, nullptr, 0);

    if (PQresultStatus(correct_res) != PGRES_TUPLES_OK || PQntuples(correct_res) == 0) {
        std::cerr << "Ошибка выполнения SQL-запроса: " << PQerrorMessage(conn) << std::endl;
        PQclear(correct_res);
        PQfinish(conn);
        return;
    }

    int correct_answer_id = std::stoi(PQgetvalue(correct_res, 0, 0));
    PQclear(correct_res);

    // Получаем последнюю попытку студента
    std::string select_query = R"(
        SELECT attempt_id
        FROM student_attempts
        WHERE student_id = $1 AND test_id = $2
        ORDER BY data DESC
        LIMIT 1
    )";

    const char* select_params[2] = {
        student_id.c_str(),
        std::to_string(test_id).c_str()
    };

    PGresult* select_res = PQexecParams(conn, select_query.c_str(), 2, nullptr, select_params, nullptr, nullptr, 0);

    if (PQresultStatus(select_res) != PGRES_TUPLES_OK || PQntuples(select_res) == 0) {
        std::cerr << "Ошибка выполнения SQL-запроса: " << PQerrorMessage(conn) << std::endl;
        PQclear(select_res);
        PQfinish(conn);
        return;
    }

    int attempt_id = std::stoi(PQgetvalue(select_res, 0, 0));
    PQclear(select_res);

    // Обновляем `selected_answer_id` и `is_correct`
    std::string update_query = R"(
        UPDATE student_answers
        SET selected_answer_id = $1, is_correct = $2, timestamp = CURRENT_TIMESTAMP
        WHERE question_id = $3 AND student_id = $4 AND attempt_id = $5
    )";

    bool is_correct = (new_selected_answer_id == correct_answer_id);
    const char* update_params[5] = {
        std::to_string(new_selected_answer_id).c_str(),
        is_correct ? "true" : "false",
        std::to_string(question_id).c_str(),
        student_id.c_str(),
        std::to_string(attempt_id).c_str()
    };

    PGresult* update_res = PQexecParams(conn, update_query.c_str(), 5, nullptr, update_params, nullptr, nullptr, 0);
    if (PQresultStatus(update_res) != PGRES_COMMAND_OK) {
        std::cerr << "Ошибка выполнения SQL-запроса при обновлении: " << PQerrorMessage(conn) << std::endl;
    } else {
        std::cout << "Обновление выполнено успешно. Ответ " << (is_correct ? "верный" : "неверный") << "." << std::endl;
    }

    PQclear(update_res);
    PQfinish(conn);
}

void insert_initial_student_answer(int question_id, int selected_answer_id, const std::string& student_id_token, int attempt_id) {
    
    std::cout << attempt_id << std::endl;
    
    PGconn* conn = connect_db();
    if (!conn) {
        std::cerr << "Ошибка подключения к базе данных" << std::endl;
        return;
    }

    std::string check_query = R"(
        SELECT 1
        FROM student_answers
        WHERE attempt_id = $1 AND question_id = $2
    )";

    const char* check_params[2] = {
        std::to_string(attempt_id).c_str(),
        std::to_string(question_id).c_str()
    };
    PGresult* res_check = PQexecParams(conn, check_query.c_str(), 2, nullptr, check_params, nullptr, nullptr, 0);

    if (PQresultStatus(res_check) != PGRES_TUPLES_OK) {
        std::cerr << "Ошибка проверки записи: " << PQerrorMessage(conn) << std::endl;
        PQclear(res_check);
        PQfinish(conn);
        return;
    }

    bool record_exists = PQntuples(res_check) > 0;
    PQclear(res_check);

    if (!record_exists) {
        // Вставляем новую запись
        std::string insert_query = R"(
            INSERT INTO student_answers (student_id, question_id, selected_answer_id, attempt_id)
            VALUES ($1, $2, $3, $4)
        )";

        const char* insert_params[4] = {
            student_id_token.c_str(),
            std::to_string(question_id).c_str(),
            std::to_string(selected_answer_id).c_str(),
            std::to_string(attempt_id).c_str()
        };

        PGresult* res_insert = PQexecParams(conn, insert_query.c_str(), 4, nullptr, insert_params, nullptr, nullptr, 0);

        if (PQresultStatus(res_insert) != PGRES_COMMAND_OK) {
            std::cerr << "Ошибка вставки записи: " << PQerrorMessage(conn) << std::endl;
        }

        PQclear(res_insert);
    }

    PQfinish(conn);
}

json get_test_info_try(int question_id_info, const std::string& student_id_token) {
    std::cout << "get_test_info_try: " << question_id_info << std::endl;
    std::cout << "student_id: " << student_id_token << std::endl;

    PGconn* conn = connect_db();
    if (!conn) {
        return json{{"error", "Ошибка подключения к базе данных"}};
    }

    std::string test_id_query = R"(
        SELECT test_id
        FROM student_attempts
        WHERE student_id = $1
        ORDER BY data DESC
        LIMIT 1
    )";

    const char* test_id_params[1] = { student_id_token.c_str() };
    PGresult* res_test_id = PQexecParams(conn, test_id_query.c_str(), 1, nullptr, test_id_params, nullptr, nullptr, 0);

    if (PQresultStatus(res_test_id) != PGRES_TUPLES_OK || PQntuples(res_test_id) == 0) {
        std::cerr << "SQL error: " << PQerrorMessage(conn) << std::endl;
        PQclear(res_test_id);
        PQfinish(conn);
        return json{{"error", "Ошибка выполнения SQL-запроса или test_id не найден"}};
    }

    int test_id = std::stoi(PQgetvalue(res_test_id, 0, 0));
    PQclear(res_test_id);

    std::cout << "test_id: " << test_id << std::endl;

    // Получаем attempt_id
    std::string attempt_id_query = R"(
        SELECT attempt_id
        FROM student_attempts
        WHERE student_id = $1
        ORDER BY data DESC
        LIMIT 1
    )";

    PGresult* res_attempt_id = PQexecParams(conn, attempt_id_query.c_str(), 1, nullptr, test_id_params, nullptr, nullptr, 0);

    if (PQresultStatus(res_attempt_id) != PGRES_TUPLES_OK || PQntuples(res_attempt_id) == 0) {
        std::cerr << "SQL error: " << PQerrorMessage(conn) << std::endl;
        PQclear(res_attempt_id);
        PQfinish(conn);
        return json{{"error", "Ошибка выполнения SQL-запроса или attempt_id не найден"}};
    }

    int attempt_id = std::stoi(PQgetvalue(res_attempt_id, 0, 0));
    PQclear(res_attempt_id);

    std::cout << "attempt_id: " << attempt_id << std::endl;

    // Получаем количество вопросов
    std::string count_query = R"(
        SELECT COUNT(*) AS quantity
        FROM questions
        WHERE test_id = $1
    )";

    const char* count_params[1] = { std::to_string(test_id).c_str() };
    PGresult* count_res = PQexecParams(conn, count_query.c_str(), 1, nullptr, count_params, nullptr, nullptr, 0);

    if (PQresultStatus(count_res) != PGRES_TUPLES_OK) {
        std::cerr << "SQL error: " << PQerrorMessage(conn) << std::endl;
        PQclear(count_res);
        PQfinish(conn);
        return json{{"error", "Ошибка выполнения SQL-запроса при подсчёте количества вопросов"}};
    }

    int question_quantity = std::stoi(PQgetvalue(count_res, 0, 0));
    PQclear(count_res);

    std::cout << "question_quantity: " << question_quantity << std::endl;

    using ordered_json = nlohmann::ordered_json;

    std::string question_query = R"(
        SELECT
            q.question_id AS id,
            q.text_question AS text,
            q.correct_answer_id AS res,
            COALESCE(sa.selected_answer_id, 0) AS now
        FROM
            questions q
        LEFT JOIN
            student_answers sa ON q.question_id = sa.question_id AND sa.student_id = $1 AND sa.attempt_id = $2
        WHERE
            q.test_id = $3 AND q.question_id = $4
    )";
    const char* question_params[4] = {
        student_id_token.c_str(),
        std::to_string(attempt_id).c_str(),
        std::to_string(test_id).c_str(),
        std::to_string(question_id_info).c_str()
    };

    PGresult* res_question = PQexecParams(conn, question_query.c_str(), 4, nullptr, question_params, nullptr, nullptr, 0);

    if (PQresultStatus(res_question) != PGRES_TUPLES_OK || PQntuples(res_question) == 0) {
        std::cerr << "SQL error: " << PQerrorMessage(conn) << std::endl;
        PQclear(res_question);
        PQfinish(conn);
        return json{{"error", "Ошибка выполнения SQL-запроса или вопрос не найден"}};
    }

    int question_id = std::stoi(PQgetvalue(res_question, 0, 0));
    std::string question_text = PQgetvalue(res_question, 0, 1);
    int correct_answer_id = std::stoi(PQgetvalue(res_question, 0, 2));
    int selected_answer_id = std::stoi(PQgetvalue(res_question, 0, 3));

    ordered_json result_json = {
        {"id", question_id},
        {"quantity", question_quantity},
        {"text", question_text},
        {"res", correct_answer_id},
        {"now", selected_answer_id},
        {"answers", ordered_json::array()}
    };

    PQclear(res_question);

    std::string answers_query = R"(
        SELECT
            answer_index AS id,
            text_answer AS text
        FROM
            answers
        WHERE
            test_id = $1 AND question_id = $2
    )";
    const char* answers_params[2] = {
        std::to_string(test_id).c_str(),
        std::to_string(question_id_info).c_str()
    };

    PGresult* res_answers = PQexecParams(conn, answers_query.c_str(), 2, nullptr, answers_params, nullptr, nullptr, 0);

    if (PQresultStatus(res_answers) != PGRES_TUPLES_OK) {
        std::cerr << "SQL error: " << PQerrorMessage(conn) << std::endl;
        PQclear(res_answers);
        PQfinish(conn);
        return json{{"error", "Ошибка выполнения SQL-запроса для получения ответов"}};
    }

    for (int i = 0; i < PQntuples(res_answers); ++i) {
        int answer_id = std::stoi(PQgetvalue(res_answers, i, 0));
        std::string answer_text = PQgetvalue(res_answers, i, 1);

        ordered_json answer_json = {
            {"id", answer_id},
            {"text", answer_text}
        };

        result_json["answers"].push_back(answer_json);
    }

    PQclear(res_answers);

    insert_initial_student_answer(question_id_info, selected_answer_id, student_id_token, attempt_id);

    PQfinish(conn);
    return result_json;
}

bool execute_query(PGconn* conn, const char* query, const char* params[], int param_count) {
    PGresult* res = PQexecParams(conn, query, param_count, nullptr, params, nullptr, nullptr, 0);
    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        std::cerr << "Ошибка SQL-запроса: " << PQerrorMessage(conn) << std::endl;
        PQclear(res);
        return false;
    }
    PQclear(res);
    return true;
}

void check_active_attempt(const httplib::Request& req, httplib::Response& res, const std::string& student_id) {
    std::cout << "check_active_attempt" << std::endl;
    std::cout << "student_id: " << student_id << std::endl;
    PGconn* conn = connect_db();
    if (!conn) {
        res.status = 500;
        res.set_content(R"({"error": "Database connection failed"})", "application/json");
        return;
    }
    // Запрос для проверки активной попытки
    std::string active_attempt_query = R"(
        SELECT attempt_id 
        FROM student_attempts 
        WHERE student_id = $1 AND contribution IS NULL 
        ORDER BY data DESC
        LIMIT 1
    )";

    const char* params[1] = { student_id.c_str() };
    PGresult* res_active_attempt = PQexecParams(conn, active_attempt_query.c_str(), 1, nullptr, params, nullptr, nullptr, 0);

    std::cout << "========ВСЕ ХОРОШО========" << std::endl;
    
    if (PQresultStatus(res_active_attempt) != PGRES_TUPLES_OK) {
        std::cerr << "Ошибка проверки активной попытки: " << PQerrorMessage(conn) << std::endl;
        PQclear(res_active_attempt);
        PQfinish(conn);
        res.status = 500;
        res.set_content(R"({"error": "Failed to check active attempt"})", "application/json");
        return; 
    }

    int num_rows = PQntuples(res_active_attempt);
    // Если есть активная попытка, возвращаем статус 208
    if (num_rows > 0) {
        const char* attempt_id = PQgetvalue(res_active_attempt, 0, 0);
        std::cout << "attempt_id: " << attempt_id << std::endl;

        PQclear(res_active_attempt);
        PQfinish(conn);
        std::cout << "208"<< std::endl;
        res.status = 208;
        res.set_content(R"({"status": 208, "message": "Active attempt exists for this student."})", "application/json");
        return;
    }

    // Если активных попыток нет, возвращаем статус 200
    PQclear(res_active_attempt);
    PQfinish(conn);
    std::cout << "200"<< std::endl;
    res.status = 200;
    res.set_content(R"({"status": 200, "message": "No active attempts for this student."})", "application/json");
}

std::string close_active_attempt(const httplib::Request& req, httplib::Response& res, const std::string& student_id) {
    std::cout << "student_id: " << student_id << std::endl;
    PGconn* conn = connect_db();
    if (!conn) {
        res.status = 500;
        res.set_content(R"({"error": "Database connection failed"})", "application/json");
        return "500";
    }
    // Запрос для проверки активной попытки
    std::string active_attempt_query = R"(
        SELECT attempt_id, test_id
        FROM student_attempts 
        WHERE student_id = $1 AND contribution IS NULL 
        ORDER BY data DESC
        LIMIT 1
    )";

    const char* params[1] = { student_id.c_str() };
    PGresult* res_active_attempt = PQexecParams(conn, active_attempt_query.c_str(), 1, nullptr, params, nullptr, nullptr, 0);

    std::cout << "========ВСЕ ХОРОШО========" << std::endl;

    if (PQresultStatus(res_active_attempt) != PGRES_TUPLES_OK) {
        std::cerr << "Ошибка проверки активной попытки: " << PQerrorMessage(conn) << std::endl;
        PQclear(res_active_attempt);
        PQfinish(conn);
        res.status = 500;
        res.set_content(R"({"error": "Failed to check active attempt"})", "application/json");
        return "500"; 
    }

    int num_rows = PQntuples(res_active_attempt);
    if (num_rows == 0) {
        PQclear(res_active_attempt);
        PQfinish(conn);
        std::cout << "Нет активных попыток" << std::endl;
        res.status = 200;
        res.set_content(R"({"message": "No active attempts found"})", "application/json");
        return "{}";
    }

    const char* attempt_id = PQgetvalue(res_active_attempt, 0, 0);
    const char* test_id = PQgetvalue(res_active_attempt, 0, 1);

    std::cout << "attempt_id: " << attempt_id << std::endl;
    std::cout << "test_id: " << test_id << std::endl;

    PQclear(res_active_attempt);
    PQfinish(conn);

    std::cout << "200" << std::endl;
    res.status = 200;

    json json_response;
    json_response["idTerying"] = attempt_id;
    json_response["idTest"] = test_id;

    res.set_content(json_response.dump(), "application/json");

    return json_response.dump();
}

void create_student_attempt(const httplib::Request& req, httplib::Response& res, const std::string& student_id, int test_id) {
    std::cout << "student_id_create_student_attempt: " << student_id << std::endl;
    std::cout << "test_id_create_student_attempt: " << test_id << std::endl;

    PGconn* conn = connect_db();
    if (!conn) {
        res.status = 500;
        res.set_content(R"({"error": "Database connection failed"})", "application/json");
        return;
    }

    // Проверка существующей попытки
    std::string check_attempt_query = R"(
        SELECT COUNT(*) 
        FROM student_attempts 
        WHERE student_id = $1 AND test_id = $2
    )";

    const char* check_params[2] = { student_id.c_str(), std::to_string(test_id).c_str() };
    PGresult* res_check = PQexecParams(conn, check_attempt_query.c_str(), 2, nullptr, check_params, nullptr, nullptr, 0);

    if (PQresultStatus(res_check) != PGRES_TUPLES_OK) {
        std::cerr << "Ошибка проверки существующей попытки: " << PQerrorMessage(conn) << std::endl;
        PQclear(res_check);
        PQfinish(conn);
        res.status = 500;
        res.set_content(R"({"error": "Failed to check existing attempts"})", "application/json");
        return;
    }

    int existing_attempts = std::stoi(PQgetvalue(res_check, 0, 0));
    std::cout << "existing_attempts: " << existing_attempts << std::endl;
    PQclear(res_check);

    std::string insert_attempt_query = R"(
        INSERT INTO student_attempts (student_id, test_id) 
        VALUES ($1, $2) 
        RETURNING attempt_id
    )";

    const char* attempt_params[2] = { student_id.c_str(), std::to_string(test_id).c_str() };
    PGresult* res_attempt = PQexecParams(conn, insert_attempt_query.c_str(), 2, nullptr, attempt_params, nullptr, nullptr, 0);

    if (PQresultStatus(res_attempt) != PGRES_TUPLES_OK) {
        std::cerr << "Ошибка при создании попытки: " << PQerrorMessage(conn) << std::endl;
        PQclear(res_attempt);
        PQfinish(conn);
        res.status = 500;
        res.set_content(R"({"error": "Failed to create student attempt"})", "application/json");
        return;
    }

    // Получаем новый attempt_id
    int attempt_id = std::stoi(PQgetvalue(res_attempt, 0, 0));
    std::cout << "Created attempt_id: " << attempt_id << std::endl;
    PQclear(res_attempt);

    // Извлекаем все question_id для данного test_id
    std::string question_query = R"(
        SELECT question_id 
        FROM questions 
        WHERE test_id = $1
    )";

    const char* question_params[1] = { std::to_string(test_id).c_str() };
    PGresult* res_question = PQexecParams(conn, question_query.c_str(), 1, nullptr, question_params, nullptr, nullptr, 0);

    if (PQresultStatus(res_question) != PGRES_TUPLES_OK || PQntuples(res_question) == 0) {
        std::cerr << "Ошибка получения question_id: " << PQerrorMessage(conn) << std::endl;
        PQclear(res_question);
        PQfinish(conn);
        res.status = 500;
        res.set_content(R"({"error": "Failed to retrieve question_id"})", "application/json");
        return;
    }

    // Вставляем связи между attempt_id и всеми question_id
    int rows = PQntuples(res_question);
    for (int i = 0; i < rows; ++i) {
        int question_id = std::stoi(PQgetvalue(res_question, i, 0));
        std::string insert_question_attempt_query = R"(
            INSERT INTO question_attempts (attempt_id, test_id, question_id) 
            VALUES ($1, $2, $3 )
        )";

        const char* question_attempt_params[3] = { std::to_string(attempt_id).c_str(), std::to_string(test_id).c_str(), std::to_string(question_id).c_str() };
        PGresult* res_insert = PQexecParams(conn, insert_question_attempt_query.c_str(), 3, nullptr, question_attempt_params, nullptr, nullptr, 0);

        if (PQresultStatus(res_insert) != PGRES_COMMAND_OK) {
            std::cerr << "Ошибка при добавлении вопроса в попытку: " << PQerrorMessage(conn) << std::endl;
            PQclear(res_insert);
            PQclear(res_question);
            PQfinish(conn);
            res.status = 500;
            res.set_content(R"({"error": "Failed to associate question with attempt"})", "application/json");
            return;
        }

        PQclear(res_insert);
    }

    PQclear(res_question);
    PQfinish(conn);

    // Успешный ответ
    res.status = 200;
    res.set_content(R"({"success": "Attempt created", "attempt_id": )" + std::to_string(attempt_id) + R"(})", "application/json");
}

int save_test_attempt(int question_id, const json& questions) {
    PGconn* conn = connect_db();
    if (!conn) {
        std::cerr << "Ошибка подключения к базе данных!" << std::endl;
        return -1;
    }

    PGresult* res = nullptr;
    PGresult* update_res = nullptr;

    try {
        // Транзакция
        PQexec(conn, "BEGIN");

        // Получаем test_id и student_id
        const char* query = R"(
            SELECT sa.test_id, sa.student_id
            FROM student_attempts sa
            JOIN questions q ON sa.test_id = q.test_id
            WHERE q.question_id = $1
        )";
        const char* params[1] = { std::to_string(question_id).c_str() };
        res = PQexecParams(conn, query, 1, nullptr, params, nullptr, nullptr, 0);

        if (PQresultStatus(res) != PGRES_TUPLES_OK || PQntuples(res) == 0) {
            throw std::runtime_error("Вопрос не найден");
        }

        int student_id = std::stoi(PQgetvalue(res, 0, 1));
        int test_id = std::stoi(PQgetvalue(res, 0, 0));
        PQclear(res);
        // Сохранение ответов
        int correct_count = 0, total_questions = questions.size();
        for (const auto& question : questions) {
            int question_id = question.at("id");
            int correct_answer_id = question.at("res");
            int selected_answer_id = question.at("now");
            bool is_correct = (selected_answer_id == correct_answer_id);

            correct_count += is_correct ? 1 : 0;

            const char* insert_query = R"(
                INSERT INTO student_answers(attempt_id, question_id, selected_answer_id, is_correct)
                VALUES ($1, $2, $3, $4)
            )";
            const char* answer_params[4] = {
                std::to_string(question_id).c_str(),
                std::to_string(question_id).c_str(),
                std::to_string(selected_answer_id).c_str(),
                is_correct ? "true" : "false"
            };

            if (!execute_query(conn, insert_query, answer_params, 4)) {
                throw std::runtime_error("Ошибка сохранения ответа");
            }
        }

        // Обновление итоговых результатов
        double contribution = (double(correct_count) / total_questions) * 100;
        const char* update_query = R"(
            UPDATE student_attempts
            SET resTest = $1, score = $2, contribution = $3
            WHERE test_id = $4 AND student_id = $5
        )";
        const char* update_params[5] = {
            std::to_string(correct_count).c_str(),
            std::to_string(correct_count).c_str(),
            std::to_string(contribution).c_str(),
            std::to_string(test_id).c_str(),
            std::to_string(student_id).c_str()
        };

        if (!execute_query(conn, update_query, update_params, 5)) {
            throw std::runtime_error("Ошибка обновления результатов");
        }

        // Завершаем транзакцию
        PQexec(conn, "COMMIT");
        PQfinish(conn);

        return correct_count;
    } catch (const std::exception& e) {
        std::cerr << "Ошибка: " << e.what() << std::endl;
        PQexec(conn, "ROLLBACK");
        PQfinish(conn);
        return -1;
    }
}

json save_student_answer(int question_id, const std::string& student_id, int selected_answer_id) {
    PGconn* conn = connect_db();
    if (!conn) {
        return json{{"error", "Ошибка подключения к базе данных"}};
    }

    // Получаем test_id для данного студента
    const char* test_id_query = R"(
        SELECT test_id
        FROM student_attempts
        WHERE student_id = $1
        ORDER BY data DESC
        LIMIT 1;
    )";
    const char* params[1] = { student_id.c_str() };
    PGresult* res = PQexecParams(conn, test_id_query, 1, nullptr, params, nullptr, nullptr, 0);
    if (PQresultStatus(res) != PGRES_TUPLES_OK || PQntuples(res) == 0) {
        std::cerr << "SQL error: " << PQerrorMessage(conn) << std::endl;
        PQclear(res);
        PQfinish(conn);
        return json{{"error", "Ошибка получения test_id или нет попыток"}};
    }
    int test_id = std::stoi(PQgetvalue(res, 0, 0)); // Получаем test_id
    PQclear(res);

    // Получаем attempt_id для этого студента и test_id
    const char* attempt_query = R"(
        SELECT attempt_id
        FROM student_attempts
        WHERE student_id = $1 AND test_id = $2
        ORDER BY data DESC
        LIMIT 1;
    )";
    const char* attempt_params[2] = { student_id.c_str(), std::to_string(test_id).c_str() };
    PGresult* attempt_res = PQexecParams(conn, attempt_query, 2, nullptr, attempt_params, nullptr, nullptr, 0);
    if (PQresultStatus(attempt_res) != PGRES_TUPLES_OK || PQntuples(attempt_res) == 0) {
        std::cerr << "SQL error: " << PQerrorMessage(conn) << std::endl;
        PQclear(attempt_res);
        PQfinish(conn);
        return json{{"error", "Ошибка получения attempt_id"}};
    }
    int attempt_id = std::stoi(PQgetvalue(attempt_res, 0, 0)); // Получаем attempt_id
    PQclear(attempt_res);

    // Проверяем, существует ли уже ответ для данного вопроса и студента
    const char* check_answer_query = R"(
        SELECT answer_id, selected_answer_id
        FROM student_answers
        WHERE attempt_id = $1 AND question_id = $2
        LIMIT 1;
    )";
    const char* check_answer_params[2] = { std::to_string(attempt_id).c_str(), std::to_string(question_id).c_str() };
    PGresult* check_res = PQexecParams(conn, check_answer_query, 2, nullptr, check_answer_params, nullptr, nullptr, 0);
    if (PQresultStatus(check_res) != PGRES_TUPLES_OK) {
        std::cerr << "SQL error: " << PQerrorMessage(conn) << std::endl;
        PQclear(check_res);
        PQfinish(conn);
        return json{{"error", "Ошибка выполнения запроса при проверке существующего ответа"}};
    }

    bool answer_changed = false; // Флаг изменения ответа

    if (PQntuples(check_res) > 0) {
        // Если ответ существует, проверяем, изменился ли выбранный ответ
        int existing_answer_id = std::stoi(PQgetvalue(check_res, 0, 0)); // Получаем старый answer_id
        int existing_selected_answer = std::stoi(PQgetvalue(check_res, 0, 1)); // Получаем старый selected_answer_id

        if (existing_selected_answer != selected_answer_id) {
            // Если выбранный ответ изменился, удаляем старый и записываем новый
            const char* delete_query = R"(
                DELETE FROM student_answers
                WHERE answer_id = $1;
            )";
            const char* delete_params[1] = { std::to_string(existing_answer_id).c_str() };
            PGresult* delete_res = PQexecParams(conn, delete_query, 1, nullptr, delete_params, nullptr, nullptr, 0);
            if (PQresultStatus(delete_res) != PGRES_COMMAND_OK) {
                std::cerr << "SQL error: " << PQerrorMessage(conn) << std::endl;
                PQclear(delete_res);
                PQclear(check_res);
                PQfinish(conn);
                return json{{"error", "Ошибка удаления старого ответа"}};
            }
            PQclear(delete_res);
            answer_changed = true; // Ответ был изменен
        }
    }

    PQclear(check_res);

    // Вставляем новый ответ, если он изменился или не был записан ранее
    if (answer_changed || PQntuples(check_res) == 0) {
        const char* insert_query = R"(
            INSERT INTO student_answers (attempt_id, question_id, selected_answer_id, student_id, is_correct)
            VALUES ($1, $2, $3, $4, $5);
        )";
        const char* insert_params[5] = {
            std::to_string(attempt_id).c_str(),
            std::to_string(question_id).c_str(),
            std::to_string(selected_answer_id).c_str(),
            student_id.c_str(),
            "false"  // Проставляем false, так как не проверяем здесь правильность
        };

        PGresult* insert_res = PQexecParams(conn, insert_query, 5, nullptr, insert_params, nullptr, nullptr, 0);
        if (PQresultStatus(insert_res) != PGRES_COMMAND_OK) {
            std::cerr << "SQL error: " << PQerrorMessage(conn) << std::endl;
            PQclear(insert_res);
            PQfinish(conn);
            return json{{"error", "Ошибка вставки нового ответа"}};
        }

        PQclear(insert_res);
    }

    PQfinish(conn);
    return json{{"status", "success", "message", "Ответ успешно сохранен"}};
}


void handle_save_test_attempt(const httplib::Request& req, httplib::Response& res, std::vector<std::string> id_user) {
    std::string question_id_str = req.has_param("id") ? req.get_param_value("id") : "";

    std::cout << "id_user: " << id_user[0] << std::endl;
    std::cout << "question_id_str: " << question_id_str << std::endl;

    if (question_id_str.empty()) {
        res.status = 400;
        res.set_content("Question ID is missing", "text/plain");
        return;
    }

    int question_id = std::stoi(question_id_str);
    if (question_id <= 0) {
        res.status = 400;
        res.set_content("Invalid Question ID", "text/plain");
        return;
    }

    // // Получение информации о вопросе
    json question_info = get_test_info_try(question_id, id_user[0]);
    if (question_info.is_null()) {
        res.status = 404;
        res.set_content("Question not found", "text/plain");
        return;
    }


    // Формируем ответ
    json response = {
        {"id", question_info["id"]},
        {"quantity", question_info["quantity"]},
        {"text", question_info["text"]},
        {"res", question_info["res"]},
        {"now", question_info["now"]}
    };

    // Формирование массива ответов
    json answers = json::array();
    for (const auto& answer : question_info["answers"]) {
        answers.push_back({
            {"id", answer["id"]},
            {"text", answer["text"]}
        });
    }
    response["answers"] = answers;

    std::cout << response.dump(4) << std::endl;

    // Отправка ответа
    res.status = 200;
    res.set_content(response.dump(), "application/json");
}


