import sqlite3
import sys
import names
import random
import collections
import os
from optparse import OptionParser


DB_FILE="phonebook.sqlite"

def delete_db(db_file_path):
    if os.path.isfile(db_file_path):
        try:
            os.unlink(db_file_path)
            return True
        except:
            print(f"Unable to delete db {db_file_path}")
            return False
    return True


def _generate_entry():
    fname = names.get_first_name()
    lname = names.get_last_name()
    # generate 10 digits numbers to be used as phone numbers
    number = random.randrange(10**9, 10**10 - 1)
    phone_type = random.randrange(0, 3)
    entry = {
        "first_name" : fname,
        "last_name" : lname,
        "number" : number,
        "phone_type" : phone_type
    }
    return entry


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


def db_insert_records_from_file(connection, records_file):
    file = open(records_file, 'r')
    lines = file.readlines()
    for l in lines:
        if not l.strip():
            continue
        data = l.rstrip().split("|")
        if len(data) != 6:
            continue
        data_str = "('{0}', '{1}', '{2}', '{3}', '{4}', '{5}')".format(data[0],
            data[1], data[2], data[3], data[4], data[5])
        query_str = '''
            INSERT INTO Contacts
            (FirstName, LastName, Number, Type, Nickname, Address)
            VALUES
        '''
        query_str += data_str

        connection.cursor().execute(query_str)
        connection.commit()
    file.close()

def db_insert_generated_record(connection, num_records):
    for _ in range(num_records):
        entry = _generate_entry()
        # use first name as nickname
        data_str = "('{0}', '{1}', '{2}', '{3}', '{4}', '')".format(entry["first_name"],
            entry["last_name"], entry["number"], entry["phone_type"], entry["first_name"])
        query_str = '''
            INSERT INTO Contacts
            (FirstName, LastName, Number, Type, Nickname, Address)
            VALUES
        '''
        query_str += data_str
        # print(query_str)
        connection.cursor().execute(query_str)
        connection.commit()


def db_print_values(connection):
    query_str = "SELECT * FROM Contacts"
    result = connection.cursor().execute(query_str)
    for row in result:
        print(row)

if __name__ == '__main__':
    parser = OptionParser()
    parser.add_option("-a", "--append", dest="append_to_db",
                    action="store_true", default=False,
                    help="Append records to the existing file. Without this option the db is overwritten")
    parser.add_option("-r", "--records",
                    dest = "records_fname",
                    default = None,
                    help = "Add records in the db read from external file",
                    metavar = "FILE")
    parser.add_option('-n', dest = 'num',
                        type = 'int',
                        default = 10,
                        help = 'Number of records to genrate')
    parser.add_option("-p", "--print",
                        action = "store_true",
                        dest = "print",
                        default = False,
                        help = "Print the db records after insertion")

    (options, args) = parser.parse_args()

    # first decide whether we append or overwrite the db
    if not options.append_to_db:
        if not delete_db(DB_FILE):
            sys.exit(0)

        connection = db_connect(DB_FILE)
        if connection is None:
            sys.exit(0)

        db_create_table(connection)
    else:
        connection = db_connect(DB_FILE)
        if connection is None:
            sys.exit(0)


    # scenario 1 - read records from external file, every term separated by "|"
    if options.records_fname:
        if not os.path.isfile(options.records_fname):
            print(f"Unable to find record file {options.records_fname}")
            sys.exit(0)
        db_insert_records_from_file(connection, options.records_fname)
    else:
        # scenario 2 - generate records, just specify the number, default option
        if int(options.num) <= 0:
            print(f"Invalid number of records {options.num}")
            sys.exit(0)

        if int(options.num) > 50:
            print("You can't insert more than 50 records at a time!")
            sys.exit(0)
        db_insert_generated_record(connection, options.num)


    if options.print:
        db_print_values(connection)

    print("Done!")
