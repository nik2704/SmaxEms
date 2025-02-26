cmake_minimum_required(VERSION 3.12)

# Устанавливаем версию проекта
set(PATCH_VERSION "1" CACHE INTERNAL "Patch version")
set(PROJECT_VERSION 0.0.${PATCH_VERSION})
project(smax_ems VERSION ${PROJECT_VERSION})

# Опция для сборки с Boost Test
option(WITH_BOOST_TEST "Whether to build Boost test" ON)

# Включаем языки C и C++
enable_language(C)
enable_language(CXX)

# Настройки Boost
set(Boost_USE_RELEASE_LIBS ON)
set(Boost_USE_MULTITHREAD ON)
set(BOOST_ROOT "/usr")
set(BOOST_INCLUDEDIR "${BOOST_ROOT}/include")
set(BOOST_LIBRARYDIR "${BOOST_ROOT}/lib")

# Поиск и подключение зависимостей
# find_package(nlohmann_json CONFIG REQUIRED)
find_package(Boost 1.58 REQUIRED COMPONENTS program_options system filesystem)
find_package(OpenSSL REQUIRED)

# Добавление исполняемого файла
add_executable(smax_ems
    main.cpp
    client/RestClient.cpp
    client/ConnectionProperties.cpp
    client/SMAXClient.cpp
    utils/utils.cpp
)

# Подключение библиотек
target_link_libraries(smax_ems
    # nlohmann_json::nlohmann_json
    ${Boost_LIBRARIES}
    OpenSSL::SSL OpenSSL::Crypto
)

# Настройки стандарта C++
set_target_properties(smax_ems PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)

# Включаем директории заголовков
target_include_directories(smax_ems
    PRIVATE ${CMAKE_BINARY_DIR}
    # PRIVATE ${nlohmann_json_INCLUDE_DIRS}
    PRIVATE ${Boost_INCLUDE_DIRS}
)

# Настройки компиляции
if (MSVC)
    target_compile_options(smax_ems PRIVATE /W4)
else()
    target_compile_options(smax_ems PRIVATE -Wall -Wextra -pedantic -Werror)
endif()

# Установка бинарника
install(TARGETS smax_ems RUNTIME DESTINATION bin)

# Настройки для CPack (создание .deb пакета)
set(CPACK_GENERATOR DEB)
set(CPACK_PACKAGE_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${PROJECT_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${PROJECT_VERSION_PATCH})
set(CPACK_PACKAGE_CONTACT nik2704@yandex.ru)

include(CPack)