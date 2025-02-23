# Валидатор и форматировщик данных

## 📋 Описание

Этот проект предоставляет инструмент для проверки и форматирования файлов в форматах **JSON**, **XML** и **YAML**. Программа позволяет проверять корректность структуры файлов, а также валидировать XML с использованием DTD.

## 🗂️ Структура проекта

```
├── main.cpp
├── utils
│   ├── utils.h
│   └── utils.cpp
├── dataformatter
│   ├── DataFormatter.h
│   ├── JSONformatter.h
│   ├── JSONformatter.cpp
│   ├── XMLformatter.h
│   ├── XMLformatter.cpp
│   ├── YAMLformatter.h
│   └── YAMLformatter.cpp
└── sample_data
    ├── json
    │   ├── json_err_1.txt
    │   ├── json_err_2.txt
    │   ├── json_err_3.txt
    │   ├── json_err_4.txt
    │   └── json_ok.txt
    ├── xml
    │   ├── schema.dtd
    │   ├── xml_dtd_err.txt
    │   ├── xml_embedded_dtd.txt
    │   ├── xml_external_dtd.txt
    │   ├── xml_ok.txt
    │   └── xml_syntax_err.txt
    └── yml
        ├── yaml_err.txt
        └── yaml_ok.txt
```

## ⚙️ Установка

1. Убедитесь, что у вас установлен компилятор с поддержкой C++98 и следующие библиотеки:
   - **Boost**
   - **nlohmann_json**
   - **LibXml2**
   - **yaml-cpp**
2. Склонируйте репозиторий:
   ```bash
   git clone https://github.com/nik2704/FileValidator
   cd FileValidator
   ```
3. Соберите проект с помощью **CMake**:
   ```bash
   mkdir build
   cd build
   cmake ..
   make
   ```

## 🚀 Использование

Программа поддерживает проверку и форматирование JSON, XML и YAML файлов.

### 📖 Примеры запуска:

1. **Проверка JSON файла на корректность:**
   ```bash
   ./validator -s sample_data/json/json_ok.txt -f json -c check-format
   ```

2. **Проверка XML с DTD:**
   ```bash
   ./validator -s sample_data/xml/xml_ok.txt -f xml -d sample_data/xml/schema.dtd
   ```

3. **Форматирование YAML файла и вывод в файл:**
   ```bash
   ./validator -s sample_data/yml/yaml_ok.txt -f yaml -c format -o file -t formatted_output.yaml
   ```

### 🛠️ Доступные опции:

| Параметр            | Описание                                                          |
|---------------------|-------------------------------------------------------------------|
| `-s, --input`       | **(Обязательный)** Имя входного файла                             |
| `-f, --format`      | Формат файла: `json`, `xml`, `yaml`, `yml` (по умолчанию: json)   |
| `-c, --check`       | Тип проверки: `check-format` или `format` (по умолчанию: format)  |
| `-d, --dtd`         | Имя файла DTD для проверки XML                                   |
| `-o, --output`      | Вывод результата: `console` или `file` (по умолчанию: console)    |
| `-t, --target`      | Имя целевого файла для вывода (если выбран `file`)               |
| `-e, --xml_element` | Имя элемента для проверки (только для XML)                       |
| `-a, --xml_attr`    | Имя атрибута для проверки (только для XML)                        |
| `-h, --help`        | Показать справку по командам                                     |

## 🧩 Архитектура

- **DataFormatter** — абстрактный базовый класс для всех форматтеров.
- **JsonFormatter**, **XmlFormatter**, **YamlFormatter** — реализации для проверки и форматирования соответствующих форматов.
- **utils** — вспомогательные функции для валидации и обработки данных.
- **sample_data** — примеры входных файлов для тестирования.

## 🧪 Тестовые файлы

- **JSON:** правильные и ошибочные примеры.
- **XML:** файлы с различными DTD, валидные и с ошибками.
- **YAML:** корректные и ошибочные примеры.

## 💡 Планы по развитию

- Добавить поддержку **XSD** для XML.
- Улучшить обработку ошибок.
- Реализовать модульные тесты с использованием **Boost.Test**.

## 🖋️ Автор

- **Николай**

## 📄 Лицензия

Этот проект лицензируется под MIT License.