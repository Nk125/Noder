{ pkgs }: {
    deps = [
        pkgs.ccache
        pkgs.ccls
        pkgs.cmake
        pkgs.gdb
        pkgs.ninja
        pkgs.openssl
    ];
}