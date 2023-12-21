buildPath="$(! [ "$2" = "" ] && echo $2 || echo "build")"
sourceDir="$(! [ "$3" = "" ] && echo $3 || echo "/Noder/Node")"

case $1 in
compile)
scriptdir="$(dirname $0)"
cd "${scriptdir}"
"${scriptdir}/getNoder.sh" update
if [[ $? != 0 ]]; then
    echo "Failed to update repo, compiling anyway"
else
    echo "Updated repo, running updated script"

    scriptrepodir="."

    if [ -d Noder/.git ]; then
        scriptrepodir="Noder/Scripts"
    elif [ -d ../Noder/.git ]; then
        scriptrepodir="../Noder/Scripts"
    elif [ -d .git ]; then
        scriptrepodir="./Scripts"
    fi

    for f in "${scriptrepodir}"; do
        for g in "${scriptdir}"; do
            echo "$f $g"
            if ! [ cmp -s "$f" "$g" ] && "$(basename $f)" = "$(basename $g)"; then
                cp -f "$f" "$g"
            fi
        done
    done
    
    exit $?
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
