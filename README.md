# General_Codebase
Codebase for Various Projects

# Dialogue Analysis
A project for analysing media narratives in traditional literature, traditional media (movies & TV) and interactive media.

Contains
- dialogueParser.py: python code for categorising and capturing textual data. Places this captured textual data within a csv file which is read by 
- dialogueAnalyzer.py: analyzer that graphs the distributions of dialogue types. The analyzer creates a timeline and pie chart.
- Report.html: a HTML report with image examples comparing one of the most popular casual games (Genshin Impact) and a game coined 
as having the greatest story of all time (Disco Elysium) to determine what kinds of dialogue engages consumers on a deeper level.
- Image1.png through Image5.png

# University Projects
## MultiLinked List
Contains an mtll.c, main.c, mtll.h, tests.sh, Makefile, and tests folder
Tests folder contains 
- "normal_test.in" & "normal_test.out"
- "large_test.in" & "large_test.out"
- "corner_test.in" & "corner_test.out"
- "corner_test2.in" & "corner_test2.out"
- "corner_test3.in" & "corner_test3.out"

- main.c runs a stdin scanner to add, remove, insert, etc. elements on a multilinked list
- mtll.c contains the methods and structure of the multilinked list
- mtll.h contains the structure of the mtll.c class
- Makefile contains the compiler flags and protocol for running the file
- tests.sh runs the tests agains the code.

## Pool Game
Contains various classes in builder, strategy, and factory design patterns required for creating a pool game. 
Classes include:
- BallReader.java
- BallReaderFactory.java
- Reader.java
- ReaderFactory.java
- TableReader.java
- TableReaderFactory.java
^ Creation and instantiation classes for the pool table

- Ball.java
- BallBuilder.java
- Pocket.java
- PoolBallBuilder.java
- Table.java
^ Creation, storage, and instantiation classes for the pool balls and table itself.

- BallStrategy.java
- BlackStrategy.java
- BlueStrategy.java
- PocketStrategy.java
^ Strategy classes for handling various types of balls.

- App.java (Partly provided by university tutor for assignment purposes)
- Config.java (Partly provided by university tutor for assignment purposes)
- GameManager.java (Partly provided by university tutor for assignment purposes)
- Snapshot.java
^ Classes for the operation and handling of the game itself.

# Bitburner
Contains parts of my codebase for Bitburner, an open-source coding game with dozens of complex javascript coding problems. 
Attached is the following:
- Batcher.js: Contains an algorithm that assigns various workers across networks of workable nodes, before compiling and resetting
tasks following a scheduling design pattern similar to JIT scheduling. 

- ContractAlgos.js: Contains various functions that solve a wide array of coding problems, e.g. Shortest path in grid, largest prime factor,
stock trade optimisation, total ways to achieve sum, etc. 
