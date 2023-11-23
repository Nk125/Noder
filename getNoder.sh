case $1 in
clone)
git clone https://github.com/Nk125/Noder Noder
;;
update)
cd Noder
git stash & git pull
;;
*)
echo -e "Usage:\n  1st arg: clone/update"
;;
esac
