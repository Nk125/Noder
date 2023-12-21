{ pkgs }: {
	deps = [
		pkgs.clang_12
		pkgs.ccls
		pkgs.ccache
		pkgs.gdb
		pkgs.gnumake
		pkgs.openssl
		pkgs.cmake
	];
}