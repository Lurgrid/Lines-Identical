
<h1 align="center">
  <br>
  <a href="https://github.com/Lurgrid/Lines-Identical"><img src="https://github.com/Lurgrid/Lines-Identical/assets/61415485/c39fef7d-1cb7-4761-b3f2-e36b3bc4284e" alt="Lnid" width="200"></a>
  <br>
  Lines-Identical (lnid)
  <br>
</h1>

<h4 align="center">A program that find identical lines in file(s)</h4>

<p align="center">
  <a href="#key-features">Key Features</a> •
  <a href="#how-to-use">How To Use</a> •
  <a href="#credits">Credits</a> •
  <a href="#related">Related</a> •
  <a href="#license">License</a>
</p>

<div style="display:flex;justify-content:center;">
  <img src="https://github.com/Lurgrid/Lines-Identical/assets/61415485/0b4597a6-bdbf-4e66-b910-0ff2fc3f7589"/>
</div>

## Key Features

* Give the index of the lines that are Identical, with the content of the line 
* Quite a few options available (sorting, transforming, informing, filtering options)
* Same syntax as of a Linux program regarding options and stuff

## How To Use

To clone and run this application, you'll need [Git](https://git-scm.com) and a C compiler just like [GCC](https://gcc.gnu.org/install/download.html) or [CLANG](https://releases.llvm.org/download.html) installed on your computer. Based on the compiler that you have, change (or not) the line below : 

> **Note**
> Everything is made to be compilable by a Linux system. Nothing is promised 
> when it come to Windows User. 

makefile

```bash
... 
CC = gcc #Compiler name 
...
```

Them from your command line :

```bash
# Clone this repository
$ git clone https://github.com/Lurgrid/Lines-Identical.git

# Go into the repository
$ cd Lines-Identical

# Compile the project
$ make && cd ./lnid

# To use it
$ ./lnid ...
```

> **Note**
> Some command are built-in on the makefile. Like the ```make clean``` one that remove all 
> compiled file from the project directory. Or the ```make dist <name>``` that 
> creat a compresed file (tar.gz) of the project with the name, ```<name>``` (after an iteration of the ```make clean``` command).


## Emailware

Lnid is an [emailware](https://en.wiktionary.org/wiki/emailware). Meaning, if you liked using this app or it has helped you in any way, we'd like you send us an email at <theo.renauxv@gmail.com> or <Lurgrid@gmail.com> about anything you'd want to say about this software.

## Credits

This software uses the following open source packages:

- [Opt-L](https://github.com/titusse3/Opt-L.git)

The readme of this project was made using a template from [Markdownify](https://github.com/amitmerchant1990/electron-markdownify).

## Related

[Opt-L](https://github.com/titusse3/Opt-L.git) - The option module use for this program.

## You may also like...

In comming...

## License

GPLv3

---

> GitHub [@titusse3](https://github.com/titusse3) &nbsp;&middot;&nbsp;
> GitHub [@Lurgrid](https://github.com/Lurgrid) &nbsp;&middot;&nbsp;
