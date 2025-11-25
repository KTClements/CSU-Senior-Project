# CPU Scheduling Algorithms: Compile & Run Guide

This guide provides step-by-step instructions for compiling and running the CPU Scheduling Algorithms project on Linux (or WSL).

## Prerequisites
Ensure you have the following installed on your system:
* **Java Development Kit (JDK):** For the Java implementation.
* **GCC Compiler & Make:** For the POSIX (C) implementation.

To install these on Ubuntu:
```bash
sudo apt update
sudo apt install default-jdk build-essential
```

## Part 1: Java Implementation

### 1. Compile the Code

Navigate to the folder containing the Java files (`Driver.java`, `Task.java`, etc.) and compile all classes:

```bash
cd path/to/java/folder
javac *.java
```

### 2. Run the Schedulers

The `Driver` class takes two arguments: the algorithm code and the input file.

  * **First-Come, First-Served (FCFS)**

    ```bash
    java Driver FCFS schedule.txt
    ```

  * **Shortest-Job-First (SJF)**

    ```bash
    java Driver SJF schedule.txt
    ```

  * **Priority Scheduling**

    ```bash
    java Driver PRI schedule.txt
    ```

  * **Round-Robin (RR)**

    ```bash
    java Driver RR schedule.txt
    ```

  * **Priority with Round-Robin**

    ```bash
    java Driver PRI-RR schedule.txt
    ```

## Part 2: POSIX (C) Implementation

### 1. Compile the Code

Navigate to the folder containing the C files and `Makefile`. You must compile each scheduler individually using the `make` command.

  * **First-Come, First-Served (FCFS)**

    ```bash
    make fcfs
    ```

  * **Shortest-Job-First (SJF)**

    ```bash
    make sjf
    ```

  * **Priority Scheduling**

    ```bash
    make priority
    ```

  * **Round-Robin (RR)**

    ```bash
    make rr
    ```

  * **Priority with Round-Robin**

    ```bash
    make priority_rr
    ```

### 2. Run the Schedulers

Once compiled, executable files will be created in the directory. Run them by passing the input file as an argument.

```bash
./fcfs schedule.txt
./sjf schedule.txt
./priority schedule.txt
./rr schedule.txt
./priority_rr schedule.txt
```

### 3. Clean Up

To remove all compiled object files (`.o`) and executables:

```bash
make clean
```