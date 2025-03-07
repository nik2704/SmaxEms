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
set(Boost_USE_STATIC_LIBS ON)  # Статическая линковка Boost
set(Boost_USE_MULTITHREAD ON)  # Многопоточность
set(BOOST_ROOT "/usr")
# Find Boost with the required components
find_package(Boost 1.58 REQUIRED COMPONENTS program_options system filesystem)

# Find OpenSSL
set(OPENSSL_USE_STATIC_LIBS TRUE)  # Ensure static linking for OpenSSL
find_package(OpenSSL REQUIRED)

# Find nlohmann_json (header-only, no need for linking)
find_package(nlohmann_json REQUIRED)

# Добавление исполняемого файла
add_executable(smax_ems
    main.cpp
    Parser/Parser.cpp
    RestClient/RestClient.cpp
    SmaxClient/ConnectionProperties.cpp
    SmaxClient/SMAXClient.cpp
    SmaxClient/ConsoleSpinner.cpp
    SmaxClient/ResponseHelper.cpp
    utils/utils.cpp
)

# Подключение библиотек
target_link_libraries(smax_ems
    ${Boost_LIBRARIES}  # Automatically includes necessary Boost libraries
    OpenSSL::SSL OpenSSL::Crypto  # Statically linked OpenSSL libraries
    nlohmann_json::nlohmann_json  # Header-only, no linking necessary
)

# Настройки стандарта C++
set_target_properties(smax_ems PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)

# Включаем директории заголовков
target_include_directories(smax_ems
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
