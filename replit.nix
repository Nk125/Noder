{ pkgs }: {
    deps = [
        pkgs.ccache
        pkgs.ccls
        pkgs.clang_12
        pkgs.cmake
        pkgs.gdb
        pkgs.ninja
        pkgs.openssl
    ];
}