# CleanCaches

For cleaning a content in directory or deleting files that have been listed, but this application is applied only into ```Linux``` or ```POSIX``` terminals.

##How to build
* Open terminal, and clone this repository, then go into this folder repository
* Ensure you have a GNU C Compiler, and then Type a command sh build.sh
* Or you can compile it manually by :
  ```
  gcc clean.c -o clean
  ```
##How to use
* Please sets ```Permission``` into ```rwxrwxr-x``` (0775)
* Please sets ```UID and GID``` into ```root``` (0)
* After that, type a :
  ```
  clean -help
  ```
  for more information

##Notice
* Please reconfigure file ```DataListPath``` before building, because for file location that used to listing path to clean, default configuration is set to ```"/etc/.cacheflist.cdt"```

