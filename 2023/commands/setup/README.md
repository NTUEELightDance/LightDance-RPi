## why i include boost/serialization

to store all STL structure into BIN

## Officail setup

please follow exactly the steps in the [website](https://www.boost.org/doc/libs/1_81_0/more/getting_started/unix-variants.html)

and you will find there has errors occured ; )

## Quick Setup

### Step.1 get Boost

1. in Unix, Download [boost_1_81_0.tar.bz2](https://www.boost.org/users/history/version_1_81_0.html) 2.
2. In the directory where you want to put the Boost installation, execute

```
tar --bzip2 -xf /path/to/boost_1_81_0.tar.bz2
```

( "`/path/to/`" has highly prob. to be "`~/Downloads`")

(e.g.
I installed it under "`~/`"
, i.e. after extracting there will be "`~/boost_1_81_0`"
)

### Step.2 Add into Your IDE Compiler Path

1. open **C/C++ Configuration**
2. add "`~/boost_1_81_0`" or your path into **Include path**

### Step.3 Prepare to Use a Boost Library Binary

#### 3.1

do the 5.2.1 in the website

```

$ cd ~/boost_1_81_0/tools/build
$ ./bootstrap.sh
$ ./b2 install --prefix=/tmp/build-boost/

```

#### 3.2

```

$ cd ~/boost_1_81_0/
$ b2 --build-dir=/tmp/build-boost toolset=gcc stage

```

### Step.4 Compile your files

```

$ cd .../LightDance-RPi/2023/commands/setup/
$ g++ -I ~/boost_1_81_0 load.cpp player.cpp -o load ~/boost_1_81_0/stage/lib/libboost_serialization.a

```

### Step.5 Run

```

$ ./load dancer <dancerName>.json

```
