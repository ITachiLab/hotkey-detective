set(ARTIFACT_DIR ${PROJECT_BINARY_DIR})

# en_us must be always first, it is the fallback language
add_subdirectory(en_us)
add_subdirectory(pl_pl)