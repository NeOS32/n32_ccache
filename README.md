<!--
SPDX-FileCopyrightText: 2011 Sebastian Serewa <neos32.project@gmail.com>

SPDX-License-Identifier: Apache-2.0
-->

# n32_ccache
Compilation caching system for Windows system (ccache equivalence) based on mingw32.

## Introduction
It was many years ago when I came accross ccache. It did an excelent job on my Linux-based projects. However, many times I had to move with development to Ms Windows stations. This is how n32_ccache was born..

[Original ccache](https://ccache.samba.org/ "Original ccache")

## How does it work?
The basic ide behind ccache-like systems can be summarised by the following points:

1. launch compilation to the intermediate code
2. count a hash on a merge of intermediate source code along with the compilation command line
3. if such a hash does exist, load previously compiled object and store it where it is expected (*-o* option)
4. if not, launch final compilation of this object and store it into both places:
  * caching storage (can be RAM, disc, other?)
  * the expected compiler path (again, based on -o)

For the purpose of hashing usually MD4 is enough. The MD5 is much better for the range of possible hashing values, however, it is also more expensive when CPU load is taken into account.

This particular implementation of ccache, forks and leaves a server in memory. Whenever after a new command is to be processed, this command (meaning new instance of n32_ccache) connects to this server via Windows IPC. I did for optimisation and eagerness to understand Windows pecularities on IPC.

## How to use
Here goes example command launch:
```
  c:\n32_ccache-IA32-i386-RELEASE.exe mingw32-gcc.exe [...] -c  time/time.c -o time/time.o
```

Server mode can be used with:
```
n32_ccache-IA32-i386-RELEASE.exe --server
```

### Environmental variables used
Since beggining it was my intention to have two possibilities of objects storage. One of them was permament storage (MODE_DISC) and the other temporary in RAM (MODE_RAM below).

| Variable | Value | Description |
| --- | --- | --- |
| N32_CCACHE_MODE | MODE_RAM | Cached objects to be stored in RAM |
| N32_CCACHE_MODE | MODE_DISC | Cached objects to be stored on local disc |
| N32_CCACHE_DIR | n/a | Path to the local storage |

## Status
Under no condition this implementation should be regarded as finished or optimal. Many structures and algorithms used are not optimal (there are far more optmial counterparts). Notwithstanding, it works for me since early 2011.

The tool was compiled with mingw32 toolset.

To do list:
- [x] N32_CCACHE_MODE with MODE_DISC
- [ ] N32_CCACHE_MODE with MODE_RAM
- [ ] statistics printing
- [ ] support for excepions handling 
- [ ] support for C++11

## Conclusions
After those years I find this tool still useful and helping me whenever Windows C/C++ is needed. The performance iprovement for C files is not significant (if any), however, for complex C++ files with templates it does its trick.
