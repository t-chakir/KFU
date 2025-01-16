-- Учителя
CREATE TABLE teachers (
    teacher_id TEXT PRIMARY KEY
);
-- Студенты
CREATE TABLE students (
    student_id TEXT PRIMARY KEY,
    teacher_id TEXT REFERENCES teachers(teacher_id) ON DELETE SET NULL
);
-- Дисциплины
CREATE TABLE disciplines (
    discipline_id SERIAL PRIMARY KEY,
    discipline_name TEXT NOT NULL -- Название дисциплины
);
-- Связь учителей и дисциплин
CREATE TABLE teacher_disciplines (
    teacher_id TEXT REFERENCES teachers(teacher_id) ON DELETE CASCADE,
    discipline_id INT REFERENCES disciplines(discipline_id) ON DELETE CASCADE,
    PRIMARY KEY (teacher_id, discipline_id)
);
-- Связь студентов и дисциплин
CREATE TABLE student_disciplines (
    student_id TEXT REFERENCES students(student_id) ON DELETE CASCADE,
    discipline_id INT REFERENCES disciplines(discipline_id) ON DELETE CASCADE,
    PRIMARY KEY (student_id, discipline_id)
);
CREATE TABLE test_results (
    result_id SERIAL PRIMARY KEY,
    test_id INT REFERENCES tests(test_id) ON DELETE CASCADE,
    student_id TEXT REFERENCES students(student_id) ON DELETE CASCADE,
    score TEXT,
    contribution TEXT
);

CREATE TABLE tests (
    test_id SERIAL PRIMARY KEY,
    discipline_id INT REFERENCES disciplines(discipline_id) ON DELETE CASCADE,
    test_name TEXT NOT NULL, -- Название теста
    student_id TEXT REFERENCES students(student_id) ON DELETE CASCADE, -- Студент
    score TEXT, -- Оценка
    contribution TEXT, -- Вклад в итоговую оценку
    status TEXT DEFAULT '-1'
);
CREATE TABLE teacher_tests (
    teacher_id TEXT REFERENCES teachers(teacher_id) ON DELETE CASCADE,
    test_id INT REFERENCES tests(test_id) ON DELETE CASCADE,
    test_name TEXT NOT NULL,
    PRIMARY KEY (teacher_id, test_id)
);

CREATE TABLE teacher_tests_with_disciplines (
    teacher_id TEXT REFERENCES teachers(teacher_id) ON DELETE CASCADE,
    test_id INT REFERENCES tests(test_id) ON DELETE CASCADE,
    discipline_id INT REFERENCES disciplines(discipline_id) ON DELETE CASCADE,
    test_name TEXT NOT NULL, 
    PRIMARY KEY (teacher_id, test_id)
);

CREATE TABLE questions (
    test_id INT REFERENCES tests(test_id) ON DELETE CASCADE,
    question_id INT, -- Локальный ID вопроса (начинается с 1 для каждого теста)
    discipline_id INT, -- ID дисциплины
    text_question TEXT NOT NULL, -- Текст вопроса
    correct_answer_id INT, -- ID правильного ответа
    PRIMARY KEY (test_id, question_id) -- Составной первичный ключ
);

CREATE TABLE answers (
    answer_id SERIAL PRIMARY KEY, -- Глобальный автоинкрементируемый ID
    test_id INT REFERENCES tests(test_id) ON DELETE CASCADE,
    question_id INT, -- Локальный ID вопроса
    answer_index INT, -- Локальный порядковый номер для вопроса
    text_answer TEXT NOT NULL,
    UNIQUE (question_id, answer_index, test_id), -- Локальная уникальность индекса в рамках вопроса
    FOREIGN KEY (test_id, question_id) REFERENCES questions(test_id, question_id) ON DELETE CASCADE
);

CREATE TABLE student_attempts (
    attempt_id SERIAL PRIMARY KEY,
    student_id TEXT REFERENCES students(student_id) ON DELETE CASCADE,
    test_id INT REFERENCES tests(test_id) ON DELETE CASCADE,
    question_id INT,
    resTest TEXT, -- Наилучший результат
    data TIMESTAMP DEFAULT CURRENT_TIMESTAMP, -- Дата попытки
    score INT, -- Баллы за попытку
    contribution TYPE NUMERIC(5, 2), -- Процент от максимального количества баллов
    FOREIGN KEY (test_id, question_id) REFERENCES questions(test_id, question_id) ON DELETE CASCADE
);

CREATE TABLE question_attempts (
    attempt_id INT NOT NULL, -- ID попытки студента
    test_id INT NOT NULL, -- ID теста
    question_id INT NOT NULL, -- Локальный ID вопроса (в пределах теста)
    PRIMARY KEY (attempt_id, test_id, question_id), -- Составной первичный ключ
    FOREIGN KEY (attempt_id) REFERENCES student_attempts(attempt_id) ON DELETE CASCADE, -- Связь с попытками
    FOREIGN KEY (test_id, question_id) REFERENCES questions(test_id, question_id) ON DELETE CASCADE -- Связь с вопросами
);


CREATE TABLE student_answers (
    answer_id SERIAL PRIMARY KEY, -- Уникальный ID ответа
    attempt_id INT REFERENCES student_attempts(attempt_id) ON DELETE CASCADE, -- ID попытки
    question_id INT REFERENCES questions(question_id) ON DELETE CASCADE, -- ID вопроса
    selected_answer_id INT REFERENCES answers(answer_id) ON DELETE CASCADE, -- ID выбранного ответа
    student_id TEXT REFERENCES students(student_id) ON DELETE CASCADE, -- ID студента (тип TEXT)
    is_correct BOOLEAN, -- Верно ли отвечено на вопрос
    timestamp TIMESTAMP DEFAULT CURRENT_TIMESTAMP -- Время ответа
);

CREATE OR REPLACE FUNCTION delete_discipline_data(d_id INT) 
RETURNS VOID AS $$
BEGIN
    -- Удаляем все попытки студентов, связанные с тестами этой дисциплины
    DELETE FROM student_attempts 
    WHERE test_id IN (SELECT test_id FROM tests WHERE discipline_id = d_id);

    -- Удаляем все ответы студентов, связанные с попытками тестов этой дисциплины
    DELETE FROM student_answers 
    WHERE attempt_id IN (SELECT attempt_id FROM student_attempts WHERE test_id IN (SELECT test_id FROM tests WHERE discipline_id = d_id));

    -- Удаляем все ответы на вопросы, связанные с тестами этой дисциплины
    DELETE FROM answers 
    WHERE question_id IN (SELECT question_id FROM questions WHERE test_id IN (SELECT test_id FROM tests WHERE discipline_id = d_id));

    -- Удаляем все вопросы, связанные с тестами этой дисциплины
    DELETE FROM questions 
    WHERE test_id IN (SELECT test_id FROM tests WHERE discipline_id = d_id);

    -- Удаляем все результаты тестов, связанные с тестами этой дисциплины
    DELETE FROM test_results 
    WHERE test_id IN (SELECT test_id FROM tests WHERE discipline_id = d_id);

    -- Удаляем все тесты, связанные с этой дисциплиной
    DELETE FROM tests 
    WHERE discipline_id = d_id;

    -- Удаляем связи учителей и дисциплин
    DELETE FROM teacher_disciplines 
    WHERE discipline_id = d_id;

    -- Удаляем связи студентов и дисциплин
    DELETE FROM student_disciplines 
    WHERE discipline_id = d_id;

    -- Удаляем саму дисциплину
    DELETE FROM disciplines 
    WHERE discipline_id = d_id;
END;
$$ LANGUAGE plpgsql;

CREATE OR REPLACE FUNCTION delete_test_data(t_id INT) 
RETURNS VOID AS $$
BEGIN
    -- Удаляем все ответы студентов, связанные с попытками тестов
    DELETE FROM student_answers 
    WHERE attempt_id IN (SELECT attempt_id FROM student_attempts WHERE test_id = t_id);

    -- Удаляем все попытки студентов, связанные с тестом
    DELETE FROM student_attempts 
    WHERE test_id = t_id;

    -- Удаляем все результаты тестов, связанные с тестом
    DELETE FROM test_results 
    WHERE test_id = t_id;

    -- Удаляем все ответы на вопросы, связанные с тестом
    DELETE FROM answers 
    WHERE test_id = t_id;

    -- Удаляем все вопросы, связанные с тестом
    DELETE FROM questions 
    WHERE test_id = t_id;

    -- Удаляем сам тест
    DELETE FROM tests 
    WHERE test_id = t_id;

    -- Удаляем связи учителей с тестом
    DELETE FROM teacher_tests 
    WHERE test_id = t_id;

    -- Удаляем связи учителей с тестом и дисциплиной
    DELETE FROM teacher_tests_with_disciplines 
    WHERE test_id = t_id;
END;
$$ LANGUAGE plpgsql;

CREATE OR REPLACE FUNCTION delete_test_data_post(p_test_id INT) 
RETURNS VOID AS $$
BEGIN
    -- Удаляем все ответы, связанные с вопросами данного теста
    DELETE FROM answers 
    WHERE test_id = p_test_id;

    -- Удаляем все вопросы, связанные с данным тестом
    DELETE FROM questions 
    WHERE test_id = p_test_id;
END;
$$ LANGUAGE plpgsql;

CREATE OR REPLACE FUNCTION delete_student_data_from_discipline(p_student_id TEXT, p_discipline_id INT) 
RETURNS VOID AS $$
BEGIN
    -- Удаляем все попытки студента, связанные с тестами этой дисциплины
    DELETE FROM student_attempts 
    WHERE student_id = p_student_id AND test_id IN (SELECT test_id FROM tests WHERE discipline_id = p_discipline_id);

    -- Удаляем все ответы студента, связанные с попытками тестов этой дисциплины
    DELETE FROM student_answers 
    WHERE attempt_id IN (SELECT attempt_id FROM student_attempts WHERE student_id = p_student_id AND test_id IN (SELECT test_id FROM tests WHERE discipline_id = p_discipline_id));

    -- Удаляем все результаты тестов студента, связанные с тестами этой дисциплины
    DELETE FROM test_results 
    WHERE student_id = p_student_id AND test_id IN (SELECT test_id FROM tests WHERE discipline_id = p_discipline_id);

    -- Удаляем связи студента и дисциплины (если есть)
    DELETE FROM student_disciplines 
    WHERE student_id = p_student_id AND discipline_id = p_discipline_id;

    -- Здесь можно добавить дополнительные удаления, если есть другие таблицы, связанные с данными студента и дисциплины
END;
$$ LANGUAGE plpgsql;

-- Индексы для улучшения производительности
CREATE INDEX idx_teacher_disciplines_teacher_id ON teacher_disciplines(teacher_id);
CREATE INDEX idx_teacher_disciplines_discipline_id ON teacher_disciplines(discipline_id);
CREATE INDEX idx_student_disciplines_student_id ON student_disciplines(student_id);
CREATE INDEX idx_student_disciplines_discipline_id ON student_disciplines(discipline_id);
CREATE INDEX idx_tests_disciplines_discipline_id ON tests(discipline_id); -- Индекс для поиска тестов по дисциплин

