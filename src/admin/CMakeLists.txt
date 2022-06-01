set(EXECUTABLE_CGIs_with_PD_PI_LIBS
    admin.cgi
    )

# --- Libraries
# ---- list of independent libraries
set(INDEPENDENT_LIBRARIES_SRCs
    )

# --- MACROSes start ---

macro(set_pd_lib_properties target_lib proj_src proj_inc_list proj_lib_list)
    add_library                 (${target_lib}      ${proj_src})
    target_include_directories  (${target_lib}      PUBLIC  ${proj_inc_list})
    target_link_libraries       (${target_lib}      ${proj_lib_list})

    set(PROJECT_ADMIN_LIBS         ${PROJECT_ADMIN_LIBS}  ${target_lib})
endmacro()

# --- MACROSes finish ---

GET_DIRECTORY_PROPERTY  (XL_LIBRARY_DIR DIRECTORY       ${PROJECT_SOURCE_DIR}/src/pi/ DEFINITION XL_LIBRARY_DIR)

GET_DIRECTORY_PROPERTY  (PROJECT_PI_INCLUDE_DIRECTORIES DIRECTORY ${PROJECT_SOURCE_DIR}/src/pi/ DEFINITION PROJECT_PI_INCLUDE_DIRECTORIES)
GET_DIRECTORY_PROPERTY  (PROJECT_PI_LIBS                DIRECTORY ${PROJECT_SOURCE_DIR}/src/pi/ DEFINITION PROJECT_PI_LIBS)
GET_DIRECTORY_PROPERTY  (PROJECT_PD_LIBS                DIRECTORY ${PROJECT_SOURCE_DIR}/src/pd/ DEFINITION PROJECT_PD_LIBS)
GET_DIRECTORY_PROPERTY  (PROJECT_EXTERNAL_LIBS          DIRECTORY ${PROJECT_SOURCE_DIR}/src/pi/ DEFINITION PROJECT_EXTERNAL_LIBS)

set(PROJECT_PD_INCLUDE_DIRECTORIES
    ${PROJECT_SOURCE_DIR}/src/pd/include/
    ${PROJECT_SOURCE_DIR}/src/admin/include/
    ${PROJECT_SOURCE_DIR}/cyrillic-facets/      # --- mbstowcs 
    )

SET(PROJECT_ALL_INCLUDE_DIRECTORIES ${PROJECT_PI_INCLUDE_DIRECTORIES} ${PROJECT_PD_INCLUDE_DIRECTORIES})

# ---- list of independent libraries
foreach(INDEPENDENT_LIBRARY_SRC ${INDEPENDENT_LIBRARIES_SRCs})
    string(REGEX REPLACE "(^.*/|\\.[^.]*$)" "" file_without_ext ${INDEPENDENT_LIBRARY_SRC})

    set_pd_lib_properties(${file_without_ext} "${file_without_ext}.cpp" "${PROJECT_ALL_INCLUDE_DIRECTORIES}" "${project_link_libraries}")
endforeach()

#set_pd_lib_properties(__joint_pd "${JOINT_LIBRARY_SRCs}" "${PROJECT_ALL_INCLUDE_DIRECTORIES}" "")

# --- MSVC grouping
source_group("Headers Private"  FILES ${HDR_PRIVATE})
source_group("Headers Public"   FILES ${HDR_PUBLIC})
source_group("Sources"          FILES ${SOURCES})
source_group("Resources"        FILES ${RESOURCES})

foreach(EXECUTABLE_CGI ${EXECUTABLE_CGIs_with_PD_PI_LIBS})
    __add_executable_4libs(${EXECUTABLE_CGI} "${PROJECT_ALL_INCLUDE_DIRECTORIES}" "${PROJECT_ADMIN_LIBS}" "${PROJECT_PD_LIBS}" "${PROJECT_PI_LIBS}" "${PROJECT_EXTERNAL_LIBS}")
endforeach()

# --- Install settings

# --- Install part
install(TARGETS admin.cgi               DESTINATION     ${CGIADMINDIR}	COMPONENT admin)

#print_all_variables()
