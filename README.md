# LoL Injector

A pre-launch executable to assist on playing League of Legends on Linux. 

# Motivation

Since the end of 2020, League of Legends on Linux is being affected by a really
annoying bug: the game simply does not launch. When the League client process 
(actually the LeagueClientUx process) starts, it waits for a SSL response on a 
port opened by the parent process. However, because of the issue, this parent 
process takes too long to open the port, resulting in the League client hitting 
a timeout and not launching. 

The community on the LeagueOfLinux subreddit has worked hard to workaround this, 
providing some ways to bypass the bug. The most used workaround is a [simple bash script](https://www.reddit.com/r/leagueoflinux/comments/j07yrg/starting_the_client_script/) that suspends the League client process until a SSL response is received 
on the port. This has the disavantage of a long wait until the game is launched, 
from 2 to 5 minutes.

In the beginning of 2022, Reddit user [u/FakedCake](https://www.reddit.com/user/FakedCake) released a Python script that 
injects into the Riot Client process using Frida and, with the help of some DLL 
tinkering, effectively removes the timeout and reduces significantly the launch 
time to 20 to 50 seconds. The code for the script can be found on his [GitHub repo](https://github.com/CakeTheLiar/launchhelper).

This repo contains the source code for an executable that does the same job as 
the Python script, but without the need for the Python runtime, which makes it 
more portable and easier for the players.

# Usage

You may download the latest version of the executable by [clicking here](https://github.com/Matheuschn/lol-injector/releases/latest/download/launchhelper).

Before launching League of Legends, run the executable and let it run on the 
background while the game starts. After it injects and makes sure the game started 
correctly, it will close itself.

If you don't want to input your password every time the program runs, run the 
command `sudo setcap cap_sys_ptrace=eip PATH`, where PATH is the path to the 
executable.

If you are using Lutris, you may register the executable as a pre-launch script. 
Remember that you may need to set abi.vsyscall32=0, depending on how you installed 
the game. If you do, create a .sh file (for example, launch.sh) that sets vsyscall32 and then calls the executable.

# Building

To build on Linux, you will need the frida-core library available [here](https://github.com/frida/frida/releases/download/15.1.17/frida-core-devkit-15.1.17-linux-x86.tar.xz). After downloading it, save it in the lib/frida-core-x86 folder and extract it.

With the library set up, you may run `make` command on the cloned repo to build the 
executable, which will be available on the bin/ folder.

You may note the executable size is quite large. If you want to redistribute 
the executable, run the `make dist` command on the cloned repo. This will build 
the executable and run an executable packer to compress the binary, reducing 
massively its size.

# Known Issues

The executable can't run with M-Reimer's [wine-lol](https://github.com/M-Reimer/wine-lol), since it uses a patched libc. I'm searching for a way to fix this, but 
any help is appreciated.
