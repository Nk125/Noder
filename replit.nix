{ pkgs }: {
	deps = [
		pkgs.clang_12
		pkgs.ccls
		pkgs.gdb
		pkgs.gnumake
		pkgs.openssl
		pkgs.cmake
	];
}