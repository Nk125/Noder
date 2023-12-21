{ pkgs }: {
    deps = [
        pkgs.ccache
        pkgs.ccls
        pkgs.cmake
        pkgs.gdb
        pkgs.lld
        pkgs.openssl
    ];
}