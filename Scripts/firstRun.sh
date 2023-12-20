# NK125 First Run script for Noder

if [[ "$1" = "" ]]; then
    selfname="$(basename $0)"
    current="$(pwd)"
    basedir=$([ -d ../.git ] && echo ".." || echo ".")

    mkdir "${basedir}/Scripts" 1>/dev/null 2>&1

    echo "Moving all scripts..."
    mv *.sh "${basedir}/Scripts" 1>/dev/null 2>&1

    if ! [[ $(echo "${selfname}" | grep "\.sh") ]]; then
        echo "Moving self..."
        mv "${selfname}" "${basedir}/Scripts" 1>/dev/null 2>&1
    fi

    echo "Running copy of self"
    "${basedir}/Scripts/${selfname}" "${current}" $([ "${basedir}" = "." ] && echo "nogit")
    exit $?
fi

basedir=$1
insideRepo="$([ "$2" != "nogit" ])"
alreadySubDownloadedNoder=$([ -d Noder/.git ])
scriptdir="$(pwd)/Scripts"
chmod +x *.sh 1>/dev/null 2>&1
cd "${basedir}"

getScript() {
    "${scriptdir}/$1" 1>/dev/null 2>&1

    if [[ $? != 0 ]]; then
        echo "No $2 script detected, getting script..."

        curl --version 1>/dev/null 2>&1

        if [[ $? = 0 ]]; then
            echo "Downloading $2 script..."
            curl "https://raw.githubusercontent.com/Nk125/Noder/main/Scripts/$1" 1>"${scriptdir}/$1" 2>/dev/null
            chmod +x "${scriptdir}/$1"
        else
            echo "Please install curl and run this script again"
            exit 1
        fi
    fi
}

getScript "getNoder.sh" "cloner"

if [[ ${insideRepo} ]]; then
    cd "${basedir}/.."
elif [[ ${alreadySubDownloadedNoder} ]]; then
    echo "Detected a git repository in Noder subdirectory, moving there..."	

    cd Noder
fi

if [[ ${insideRepo} || ${alreadySubDownloadedNoder} ]]; then
    echo "Updating repository... (This will discard local code changes)"
    "${scriptdir}/getNoder.sh" update

    if [[ $? != 0 ]]; then
        echo "Failed to update repo, leaving it as is"
    fi
else
    echo "Cloning git repo..."

    "${scriptdir}/getNoder.sh" clone

    if [[ $? != 0 ]]; then
        echo "Failed to clone Noder"
        exit $?
    fi

    echo "Moving to Noder"

    cd Noder
fi

repodir="$(pwd)"
buildpath="${basedir}/build"
sourcedir="${repodir}/Node"

sed -i -e "s|PATH_TO_SCRIPTS|${scriptdir}|g" ".replit"
sed -i -e "s|BUILD_PATH|${buildpath}|g" ".replit"
sed -i -e "s|SOURCE_DIR|${sourcedir}|g" ".replit"

copyReplFiles() {
    if [[ -w "$1" ]]; then
        prevDir="$(pwd)"
        cd "$1"
        curDir="$(pwd)"

        echo "Copying replit files to: ${curDir}"

        cp "${prevDir}/replit.nix" "${curDir}"
        cp "${prevDir}/.replit" "${curDir}"
        cd "${prevDir}"
    fi
}

copyReplFiles "../.."

copyReplFiles "${basedir}"

echo "Running compiler script"

getScript "run.sh" "runner"

"${scriptdir}/run.sh" compile "${buildpath}" "${sourcedir}"

echo "Moving nodeconfig.json to executable local path..."

mv nodeconfig.json "${buildpath}"

echo "Finished setup, running..."

"${scriptdir}/run.sh" run "${buildpath}"