ccola_component(protoc_echo_plugin_csharp x86: executable other: skip)

    ccola_compile_options(
    win:
        /wd4018 /wd4251 /wd4273 /wd4800
    )

    ccola_inheritable_definitions(LIBPROTOC_EXPORTS)

    ccola_sources(
        Main.cpp
        ProtoCEchoPluginCSharp.cpp
        ProtoCEchoPluginCSharp.hpp
    )

    ccola_dependencies(
        libprotobuf
        libprotoc
        echo_attributes
        infra.stream
    )

ccola_end_component()
