A copy of libicedb is automatically checked out. This system image
automatically has installed all dependencies.

First build instructions:

1. You should set up your GitHub credentials / create a
new SSH key on this VM and associate it with your GitHub account.

2. Update your copy of the repository:
	
```
cd libicedb
git fetch
git branch --set-upstream-to=origin/master master
git pull
```

3. Create the build directory and generate the makefiles. Defaults to
using g++ 7. 
```
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=RelWithDebInfo -DCMAKE_INSTALL_PREFIX=~/icedb-install-dir ..
```

4. Build the library
```
make
```

5. Install the library
```
make install
```

5. All of the binaries are in the 'RelWithDebInfo' directory.

