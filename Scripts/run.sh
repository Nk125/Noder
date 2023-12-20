buildPath="$(! [ "$2" = "" ] && echo $2 || echo "build")"
sourceDir="$(! [ "$3" = "" ] && echo $3 || echo "/Noder/Node")"

case $1 in
compile)
scriptdir="$(dirname $0)"
"${scriptdir}/getNoder.sh" update
if [[ $? != 0 ]]; then
    echo "Failed to update repo, compiling anyway"
fi
mkdir "${buildPath}" 1>/dev/null 2>&1
echo "Configuring"
cmake -B "${buildPath}" -S "${sourceDir}"
echo "Building"
cmake --build "${buildPath}" --target node
echo "Built"
;;
run)
echo "Running node"
"${buildPath}/node"
;;
clean)
rm -rf "${buildPath}"
echo "Cleaned"
;;
*)
echo -e "Usage:\n  1st arg: compile/run/clean"
;;
esac
