project(en-US)

add_library(${PROJECT_NAME} SHARED
	dllmain.cpp
    en_us.rc)

cmake_path(GET CMAKE_CURRENT_SOURCE_DIR PARENT_PATH PARENT_DIR)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E make_directory ${ARTIFACT_DIR}/${PROJECT_NAME}
  COMMAND muirct -q ${PARENT_DIR}/localization.rcconfig -v 2 -x 0x0409 -g 0x0409 $<TARGET_FILE:${PROJECT_NAME}> ${ARTIFACT_DIR}/localization.dll ${ARTIFACT_DIR}/${PROJECT_NAME}/localization.dll.mui
  COMMAND muirct -c ${ARTIFACT_DIR}/localization.dll -e ${ARTIFACT_DIR}/${PROJECT_NAME}/localization.dll.mui
)