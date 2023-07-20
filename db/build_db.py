import sqlite3
import sys


DB_FILE="phonebook.sqlite"
RECORDS_FILE="records.txt"

def db_connect(db_file):
    conn = None
    try:
        conn = sqlite3.connect(db_file)
    except:
        print("Error connecting to the database.")
        return None
    return conn

def db_create_table(connection):
    query_str = '''
        CREATE TABLE "Contacts" (
            "Id"    INTEGER UNIQUE,
            "FirstName" TEXT,
            "LastName"  TEXT,
            "Number"    TEXT,
            "Type"  INTEGER,
            "Nickname"  TEXT,
            "Address"   TEXT,
            PRIMARY KEY("Id" AUTOINCREMENT)
        )
        '''
    connection.cursor().execute(query_str)
    connection.commit()


def db_insert_records(connection):
    file = open(RECORDS_FILE, 'r')
    lines = file.readlines()
    for l in lines:
        if not l.strip():
            continue
        data = l.rstrip().split("|")
        data_str = " ('" + data[0] + "', '" + data[1] + "', '" + data[2] + "', "
        data_str += data[3] + ", '" + data[4] + "', '" + data[5] + "')"
        query_str = '''
            INSERT INTO Contacts
            (FirstName, LastName, Number, Type, Nickname, Address)
            VALUES
        '''
        query_str += data_str

        connection.cursor().execute(query_str)
        connection.commit()
    file.close()

def db_print_values(connection):
    query_str = "SELECT * FROM Contacts"
    result = connection.cursor().execute(query_str)
    for row in result:
        print(row)

if __name__ == '__main__':
    connection = db_connect(DB_FILE)
    if connection is None:
        sys.exit(0)

    db_create_table(connection)
    db_insert_records(connection)
    db_print_values(connection)

