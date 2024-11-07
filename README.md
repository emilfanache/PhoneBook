# Phonebook

Phonebook is an application done strinctly for learning purposes. It is the first application where I used QT to build a graphical interface. I also insisted on using a builder design pattern with chained calls to build the contact list. It is an application that puts together C++, Python3, Sqlite3, Qt and CMake.


## Requirements

These are the recommended versions:<br/>
C++17 (g++ compiler)<br/>
Cmake 3.20 (or later)<br/>
Sqlite3 (libsqlite3-dev) <br/>

For Qt: <br/>
qt6-base-dev<br/>
qt6-tools-dev<br/>

For the database generator script:<br/>
Python 3.10.12<br/>
pip 22.0.2<br/>
names module installed via pip<br/>

## Quick start

For database generation:<br/>
```
cd Phonebook/db/
python3 build_db.py  -p -n 50 # this generates 50 random entries in the database

```

There are several other options to build the db, including using a records file where the data needs to be separated by a "|" character<br/>

```
$ cat records.txt
John|Walker|514514514|1|Jwalk|1234 Avenue Boot, H1S 118, Montreal, Quebec
Samantha|Jhonson|5235346535|2|Sammy|42432 Avenue Venue, H12 222, Montreal, Quebec
Dan|Thomas|8675309|0|Tommy|3050 Avenue Van Horne, H3S 1R2, Montreal, Quebec
Michael|Steinberg|989898989898|2|Mike|2070 Abel Street, J21 321, Montreal, Quebec

python3 build_db.py -r records.txt -p  #-p prints the content of the db after inserting data

```

For building & running:

```bash
mkdir build && cd build
cmake ..
make
./phonebook
```

## Limitations
At the moment the tool is not supported on Windows hosts.<br/>

## Special thanks

https://www.youtube.com/@ProgrammingKnowledge<br/>
https://stackoverflow.com/<br/>
https://doc.qt.io/<br/>
