case $1 in
clone)
echo "Cloning in directory Noder..."
git clone https://github.com/Nk125/Noder Noder

if [[ $? != 0 ]]; then
	echo "Failed to complete clone process"
	exit $?
fi

echo "Finished clone"
;;
update)
echo "Updating..."

if ! [ -d ".git" ] && [ -d "../Noder/.git" ]; then
	echo "Moving to Noder..."
	cd ../Noder
else
	echo "Couldn't detect a valid git repository"
	exit 1
fi

git stash & git pull

if [[ $? != 0 ]]; then
	echo "Failed to complete Git tasks"
	exit $?
fi

echo "Finished Git tasks"
;;
*)
echo -e "Usage:\n  1st arg: clone/update"
;;
esac
