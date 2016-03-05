# n32_ccache
Compilation caching system for Windows system (ccache equivalence)

## Introduction
It was many years ago when I came accross ccache. It did an excelent job on my Linux-based projects. However, many times I had to move with development to Ms Windows stations. This is how n32_ccache was born..

[Original ccache](https://ccache.samba.org/ "Original ccache")

## How it works?
The basic ide behind ccache-like systems can be summarised by the following points:

1. launch compilation to the intermediate code
2. count a hash on a merge of intermediate source code along with the compilation command line
3. if such a hash does exist, load previously compiled object and store it where it is expected (*-o* option)
4. if not, launch final compilation of this object and store it into both places:
  * caching storage (can be RAM, disc, other?)
  * the expected compiler path (again, based on -o)

For the purpose of hashing usually MD4 is enough. The MD5 is much better for the range of possible hashing values, however, it is also more expensive when CPU load is taken into account.

This particular implementation of ccache, forks and leaves a server in memory. Whenever after a new command is to be processes this command (new instance of n32_ccache) connects to this server via Windows IPC. I did for optimisation and eagerness to undertsnad Windows pecularities on IPC.

## How to use
Under no condition this implementation should be regarded as finished or optimal. However, it works for me since early 2011.

### Variables used:
N32_CCACHE_MODE [MODE_RAM, MODE_DISC]


## Status
Under no condition this implementation should be regarded as finished or optimal. Many constructions used, after that time, looks sometimes naive (there are far more optmial coutnerparts). Notwithstanding, it works for me since early 2011.

## Conclusions
After those years I find this tool still useful and helping me whenever Windows C/C++ is needed. The performance iprovement for C files is not significant (if any), however, for complex C++ files with templates it does its trick.
