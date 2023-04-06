# Project 1: System Calls and Error Checking

This is Project 1 of [CSE-34341-SVS-Spring-2023](https://github.com/patrick-flynn/CSE34341-SVS-Sp2023/blob/main/index.md)

## Student

* Tram Trinh (htrinh@nd.edu)

# Project Goals

This project is a warm up assignment for the course. The basic concept is very simple: to write a program that copies files and directories from one place to another. However, the main challenge of engineering operating systems is dealing with errors and unexpected conditions. Thus, the main focus of this assignment will be on the correct handling of errors. The goals of this project are:

- To review knowledge of basic C programming.
- To learn the most essential Unix system calls.
- To gain experience in rigorous error handling.

## Part 1: File Copy

Write a C program called `filecopy` which duplicates a file from one place to another. The program will be invoked as follows:

```
filecopy SourceFile TargetFile
```

If successful, the program should output the total number of bytes copied:

```
filecopy: copied 23847 bytes from ABC to XYZ
```

If not successful, it should print a suitable error message, like this:

```
filecopy: couldn't open ABC: No such file or directory.
```

## Part 2: Recursive Copy

Write a C program called `treecopy` which duplicates a directory tree from one place to another. The program will be invoked as follows:

```
treecopy SourcePath TargetPath
```

`treecopy` must create an exact copy of SourcePath under the new name TargetPath. If the source path is just a file, then a simple file copy takes place in the same way as `filecopy` operates. But if it is a directory, then the entire directory and its contents should be copied recursively. As the program runs, it should output what is being copied:

```
source -> target
source/x -> target/x
source/y -> target/y
source/y/z -> target/y/z
...
```

Upon successful completion, `treecopy` should report the total number of directories, files, and bytes copied, and exit with result 0. For example:

```
treecopy: copied 5 directories, 34 files, and 238475 bytes from ABC to XYZ
```

If `treecopy` encounters eny kind of error or user mistake, it must immediately stop and emit a message that states the program name, the failing operation, and the reason for the failure, and then exit with result 1. For example:

```
treecopy: couldn't create file mishmash: Permission Denied.
treecopy: couldn't open directory foobar: File Exists.
treecopy: couldn't write to file bizbaz: Disk Full.
```

If the program is invoked incorrectly, then it should immediately exit with a helpful message:

```
treecopy: Too many arguments!
usage: treecopy <sourcefile> <targetfile>
```

**Special Cases:** If the destination path already exists, or the source contains something other than a directory or file, then `treecopy` should halt without copying anything more and display a suitable error message.

In short, there should be no possible way to invoke the program that results in a segmentation fault, a cryptic message, or a silent failure. Either the program runs successfully to completion, or it emits a helpful and detailed error message to the user.

## System Calls

To carry out this assignment, you will need to learn about the following system calls:

```
open(), creat(), read(), write(), close(), stat(), mkdir(), opendir(), closedir(), readdir(), strerror(), errno(), exit()
```

Manual pages ("man pages") provide the complete reference documentation for system calls. They are available on any Linux machine by typing `man` with the section number and topic name. Section 1 is for programs, section 2 for system calls, section 3 for C library functions. For example `man 2 open` gives you the man page for the `open()` syscall. You can also find manual pages online at [die.net](https://linux.die.net/man/) and other places.

As you probably already know, man pages are a little difficult to digest, because they give complete information about one call, but not how they all fit together. However, with a little practice, you can become an expert with man pages. Consider the man page for `open`. At the top, it tells you what include files are needed in order to use open:

```
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
```

It also gives the the prototype for several variations of the system call:

```
int open(const char *pathname, int flags);
int open(const char *pathname, int flags, mode_t mode);
int creat(const char *pathname, mode_t mode);
```

The documentation contains many more details than you need, but it is usually a good bet to read the first paragraph, and then skim ahead until you find what you are looking for. It is also always fruitful to read the RETURN VALUE section, and to use the SEE ALSO section to look up related system calls.

## Handling Errors

An operating system (and any program that deals with it) must be vigilant in dealing with errors and unexpected conditions. End users constantly attempt operations that are illegal or make no sense. Digital devices behave in unpredictable ways. So, when you write operating system code, you must always check return codes and take an appropriate action on failure.

How do you know if a system call succeeded or failed? In general read the RETURN VALUE section of the manual page, and it will tell you exactly how success or failure is indicated. However, nearly all Unix system calls that return an integer (`open()`, `read()`, `write()`, `close()`, _etc._) have the following convention:
- If the call succeeded, it returns an integer greater than or equal to zero.
- If the call failed, it returns an integer less than zero, and sets the global variable errno to reflect the reason for the error.

And, nearly all C library calls that return a pointer (`malloc()`, `fopen()`, _etc._) have a slightly different convention:
- If the call succeeded, it returns a non-null pointer.
- If the call failed, it returns null, and sets the global variable `errno` (`man 3 errno` is its man page) to reflect the reason for the error.

The `errno` variable is simply an integer that explains the reason behind an error. Each integer value is given a macro name that makes it easy to remember, like EPERM for permission denied. All of the error types are defined in the file `/usr/include/asm/errno.h`. Here are a few of them:

```
#define EPERM            1      /* Operation not permitted */
#define ENOENT           2      /* No such file or directory */
#define ESRCH            3      /* No such process */
#define EINTR            4      /* Interrupted system call */
...
```

You can check for specific kinds of errors like this:

```
fd = open(filename,O_RDONLY,0);
if(fd<0) {
	 if(errno==EPERM) {
		printf("Error: Permission Denied\n");
	} else {
		printf("Some other error.\n");
		...
	}
	exit(1);
}
```

This would get rather tedious with 129 different error messages. Instead, use the `strerror()` (`man 3 strerror`) function to convert `errno` into a string, and print out a descriptive message like this:

```
fd = open(filename,O_RDONLY,0);
if(fd<0) {
	 printf("Unable to open %s: %s\n",filename,strerror(errno));
	 exit(1);
}
```

## Testing

Make sure to test your program on a wide variety of conditions. Start by testing single files, both small and large. How do you know if the file copy worked correctly? Use the program `md5sum` to take the checksum of both files, and double check that it matches:

```
% md5sum /tmp/SourceFile
b92891465b9617ae76dfff2f1096fc97  /tmp/SourceFile
% md5sum /tmp/TargetFile
b92891465b9617ae76dfff2f1096fc97  /tmp/TargetFile
```
