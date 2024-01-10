cd ..
cmake -S Node/ -B build/ -G Ninja -DCMAKE_CXX_COMPILER_LAUNCHER=ccache -DCMAKE_CXX_FLAGS="-static" -DCMAKE_EXE_LINKER_FLAGS="-fuse-ld=mold" -DCMAKE_BUILD_TYPE=Release -DBUILD_SHARED_LIBS=OFF -DOPENSSL_USE_STATIC_LIBS=TRUE -DCMAKE_FIND_LIBRARY_SUFFIXES=".a"
cmake --build build --target node -j 6
