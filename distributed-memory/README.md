# needleman-wunsch
Implementation of Needleman-Wunsch Algorithm for Informatics college course at Universidade Estadual de Maringá (UEM).

# Prerequisites
- Linux based system
- g++ 9.3.0>=
- QT 5>= (qmake)
- make

# Instructions
The compile phase generated files will be placed in "{{projectRootDir}}/build/" directory.

To compile run:  
- Generate the {{projectRootDir}}/Makefile:
    ```shell
    $ qmake needleman-wunsch.pro
    ```
- Compile project to generate the {{projectRootDir}}/build/nw executable:
    ```shell
    $ make
    ```

To execute run:  
```shell
$ ./build/nw {{path to the file with dnaA text}} {{path to the file with dnaB text}} {{: sequential mode | -p: parallel mode}}
```

# Examples
You can use the files in [tests](./tests/) for test.

In sequential mode:  
```shell
$ ./build/nw tests/10000_seq1.txt tests/10000_seq2.txt
```

In parallel mode:  
```shell
$ ./build/nw tests/10000_seq1.txt tests/10000_seq2.txt -p
```