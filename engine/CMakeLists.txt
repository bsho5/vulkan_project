cmake_minimum_required(VERSION 3.19)

project(
  bisho
  VERSION 1.0
  LANGUAGES CXX C)

find_package(glfw3 REQUIRED)
link_directories(
        #brew lib
        /usr/local/lib
)

set(SHADER_DIR shaders)
file(GLOB SHADERS ${SHADER_DIR}/*.vert ${SHADER_DIR}/*.frag ${SHADER_DIR}/*.comp ${SHADER_DIR}/*.geom ${SHADER_DIR}/*.tesc ${SHADER_DIR}/*.tese ${SHADER_DIR}/*.mesh ${SHADER_DIR}/*.task ${SHADER_DIR}/*.rgen ${SHADER_DIR}/*.rchit ${SHADER_DIR}/*.rmiss)

find_package(Vulkan)

foreach(SHADER IN LISTS SHADERS)
    get_filename_component(FILENAME ${SHADER} NAME)
    add_custom_command(OUTPUT ../engine/${SHADER_DIR}/${FILENAME}.spv
        COMMAND ${Vulkan_GLSLC_EXECUTABLE} ${SHADER} -o ../engine/${SHADER_DIR}/${FILENAME}.spv
        DEPENDS ${SHADER}
        COMMENT "Compiling ${FILENAME}")
        list(APPEND SPV_SHADERS ../engine/${SHADER_DIR}/${FILENAME}.spv)
endForeach()

add_custom_target(shaders ALL DEPENDS ${SPV_SHADERS})


file(GLOB engine CONFIGURE_DEPENDS "*.h" "*.cpp")
link_directories(cpp-files)
link_directories(hpp-files)
add_subdirectory(lib/glm-master)

# == exe ==
add_executable(vk ${engine} ${SHADERS})
target_link_libraries(vk PRIVATE glfw)
target_link_libraries(vk PRIVATE vulkan)
target_link_libraries(vk PRIVATE glm)
add_dependencies(vk shaders)


