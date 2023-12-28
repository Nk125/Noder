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
	selfname="$(basename "$0")"

	if [ -d Noder/.git ]; then
		scriptrepodir="Noder/Scripts"
	elif [ -d ../Noder/.git ]; then
		scriptrepodir="../Noder/Scripts"
	elif [ -d .git ]; then
		scriptrepodir="./Scripts"
	fi

 	schedRerun=$(cmp -s "${scriptrepodir}/${selfname}" "${scriptdir}/${selfname}"; echo $?)

	find "${scriptrepodir}" -maxdepth 1 -type f -exec cp -f "{}" "${scriptdir}" \;
	find "${scriptdir}" -maxdepth 1 -type f -exec chmod +x "{}" \;

	if [ ${schedRerun} = 1 ]; then
		echo "Re-running self..."
		"${scriptdir}/${selfname}" $*
		exit $?
	else
		echo "Run script is already up-to-date"
	fi
    
	# The updated script will run in the next run
fi
mkdir "${buildPath}" 1>/dev/null 2>&1
echo "Configuring"
cmake -B "${buildPath}" -S "${sourceDir}" -DCMAKE_CXX_COMPILER_LAUNCHER=ccache -DCMAKE_CXX_FLAGS="-fuse-ld=gold" -G "Ninja" -DCMAKE_MAKE_PROGRAM=ninja
echo "Building"
cmake --build "${buildPath}" --target node
echo "Built"
exit $?
;;
run)
echo "Running node"
cd "${buildPath}"
"./node"
;;
clean)
cmake --build "${buildPath}" --target clean
echo "Cleaned"
exit $?
;;
*)
echo -e "Usage:\n  1st arg: compile/run/clean"
;;
esac
