case $1 in
compile)
./getNoder.sh update
cd Noder
cmake Node
make node
echo "Built"
;;
run)
echo "Running"
cd Noder
./node
;;
clean)
rm -rf Noder/CMakeFiles
rm -rf Noder/CMakeCache.txt
rm -rf Noder/cmake_install.cmake
rm -rf Noder/Makefile
rm -rf Noder/node
echo "Cleaned"
;;
*)
echo -e "Usage:\n  1st arg: compile/run/clean"
;;
esac
