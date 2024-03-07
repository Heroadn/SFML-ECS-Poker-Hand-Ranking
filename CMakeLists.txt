# CMakeList.txt: projeto de CMake para Database_hello, incluir origem e definir
# lógica específica a um projeto aqui.
#
cmake_minimum_required (VERSION 3.8)
project (
	"SFML-Cards"
	LANGUAGES CXX C
    VERSION 0.0.0.0)

#
set(CMAKE_CXX_STANDARD 20)

#parkages
 find_package(SFML COMPONENTS system window graphics CONFIG REQUIRED)

# Adicione a origem ao executável deste projeto.
add_executable (Cards "src/main.cpp" "src/Card.hpp" "src/Deck.hpp")

target_link_libraries(Cards PRIVATE
	sfml-system sfml-network sfml-graphics sfml-window
)

# Setting ASSETS_PATH
target_compile_definitions(Cards PUBLIC ASSETS_PATH="${CMAKE_CURRENT_SOURCE_DIR}/assets/") # Set the asset path macro to the absolute path on the dev machine

# TODO: Adicione testes e instale destinos, se necessário.
