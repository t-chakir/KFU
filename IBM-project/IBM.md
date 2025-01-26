# Проект сайт для создание и написание тестов

**Моя часть проекта: [главный модуль](https://github.com/t-chakir/KFU/blob/main/IBM-project/main-module/main-module.md)**

- [Работа с токино и Get, Post запросом](https://github.com/t-chakir/KFU/tree/main/IBM-project/main-module/reg)
- [Логика тестов и попыток на тест](https://github.com/t-chakir/KFU/tree/main/IBM-project/main-module/test)
- [Дисциплины](https://github.com/t-chakir/KFU/tree/main/IBM-project/main-module/discipline)
- [База данных на PostgreSql](https://github.com/t-chakir/KFU/tree/main/IBM-project/main-module/database)
- [CMake](https://github.com/t-chakir/KFU/blob/main/IBM-project/main-module/CMakeLists.txt)

В моей части проекта я использовал порт **8000** для запуска HTTP-сервера. Для работы с данными используется **СУБД PostgreSQL**. Если вы хотите подключить свою базу данных, следуйте этим рекомендациям:

1. **Создание базы данных:**
   - Если вы работаете с **Docker**, используйте готовый образ PostgreSQL для быстрой настройки.
   - Для удобства работы с базой данных в **Visual Studio Code** установите расширение **[SQLTools](https://marketplace.visualstudio.com/items?itemName=mtxr.sqltools)**. Оно поддерживает PostgreSQL и позволяет выполнять SQL-запросы прямо из редактора.

2. **Настройка базы данных:**
   - Создайте базу данных и пользователя с уникальными именами.

3. **Настройка CMake:**
   - В проекте используется **CMake** для сборки. Чтобы сгенерировать файлы для компилятора, выполните следующие команды:
     ```bash
     mkdir build
     cd build
     cmake ..
     make
     ```
