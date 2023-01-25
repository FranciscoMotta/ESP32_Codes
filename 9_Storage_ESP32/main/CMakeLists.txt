set(COMPONENT_SRCS "hello_world_main.c")
set(COMPONENT_ADD_INCLUDEDIRS ".")

set(COMPONENT_EMBED_TXTFILES
    "files/index.html"
    "files/texto.txt"
)

set(COMPONENT_EMBED_FILES
    "files/pinout.png"
)

register_component()

target_compile_options(${COMPONENT_LIB} PRIVATE "-Wno-format")
