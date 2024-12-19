set(ARTIFACT_DIR ${PROJECT_BINARY_DIR})
# by default English - but you also have to generate localization.dll with fallback language first
set(FALLBACK_LANGUAGE_CODE 0x409)

# fallback language must be first - it is generating localization.dll
add_subdirectory(en_us)

add_subdirectory(pl_pl)
add_subdirectory(it_it)
add_subdirectory(de_de)
add_subdirectory(es_es)