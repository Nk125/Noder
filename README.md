## Noder

An http flooder with multi-device support

### How can i start using it?

You can simply run this command on a replit server to configure and start using!

```bash
curl "https://raw.githubusercontent.com/Nk125/Noder/main/Scripts/firstRun.sh" > firstRun.sh && chmod +x firstRun.sh && ./firstRun.sh
```

Remember to do this in the base directory, and it will replace .replit and replit.nix files so take caution!

Not only replit servers are supported! Also Mac OS, Windows and Linux machines can run it!

The only requirements are a valid C/C++ compiler, CMake and OpenSSL.

### How to control the devices?

In your host device (can be replit or whatever you want), you need to manually build controller

Building it is super simple, you need the same requirements as the server (OpenSSL, C/C++ compiler and CMake)

Optionally to simplify the build process you can install Git.

```bash
git clone https://github.com/Nk125/Noder Noder
cd Noder
# Else if you don't have git you can download it manually from github, as the build process is the same!
mkdir build
cmake -B build -S Controller
cmake --build build --target node
# Run:
./build/controller
```

Then you can run the resulting executable

There are a bunch of options you can use in controller!

### Configuration

You must be aware of nodeconfig.json, this has a token value that must be changed.

This token is like a password that you enter in the controller to authorize your actions.

If this token is leaked, external people could access to your node and potentially do unwanted actions!

### Contribution

Contribution is always well received!

You can use the pull request function to add anything you think could be useful!

