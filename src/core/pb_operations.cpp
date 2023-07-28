#include <memory>
#include <stdexcept>
#include <utility>

#include "contact.hpp"
#include "contact_builder.hpp"
#include "pb_operations.hpp"

const char PBOperations::db_name_[] = "../db/phonebook.sqlite";

PBOperations::PBOperations() {
    int status = sqlite3_open(PBOperations::db_name_, &db_);
    if (status) {
        std::string err_msg =
            "Error opening DB: " + std::string(sqlite3_errmsg(db_));
        throw(std::runtime_error(err_msg));
    }
}

PBOperations::~PBOperations() {
    sqlite3_close(db_);
}

void PBOperations::AddContact(const Contact& contact) {
    char* err_msg = nullptr;

    unsigned int count_num = CountContacts(contact.GetNumber());
    if (count_num > 0) {
        throw(std::invalid_argument("Number " + contact.GetNumber() +
                                    " already exists in the database!"));
    }

    std::string values_str =
        "VALUES ('" + contact.GetFirstName() + "', '" + contact.GetLastName() +
        "', '" + contact.GetNumber() + "', " +
        std::to_string(contact.GetType()) + ", '" + contact.GetNickname() +
        "', '" + contact.GetAddress() + "')";

    std::string query =
        "INSERT INTO Contacts "
        "(FirstName, LastName, Number, Type, Nickname, Address) ";
    query += values_str;

    int status = sqlite3_exec(db_, query.c_str(), nullptr, 0, &err_msg);
    if (status != SQLITE_OK) {
        sqlite3_free(err_msg);
        throw std::runtime_error("Error adding contact");
    }
}

void PBOperations::UpdateContact(const Contact& contact) {
    char* err_msg = nullptr;

    /* Make sure that the new number we set doesn't already exists
     * in a different record in the db. Use the current id to exclude
     * from the search the current contact number.
     */
    unsigned int count_num =
        CountContacts(contact.GetNumber(), contact.GetUserId());
    if (count_num > 0) {
        throw(std::invalid_argument("Number " + contact.GetNumber() +
                                    " already exists in the database!"));
    }
    std::string query =
        "UPDATE Contacts SET FirstName='" + contact.GetFirstName() + "', " +
        "LastName='" + contact.GetLastName() + "', " + "Number='" +
        contact.GetNumber() + "', " +
        "Type=" + std::to_string(contact.GetType()) + ", " + "Nickname='" +
        contact.GetNickname() + "', " + "Address='" + contact.GetAddress() +
        "' WHERE Id=" + std::to_string(contact.GetUserId());

    int status = sqlite3_exec(db_, query.c_str(), nullptr, 0, &err_msg);
    if (status != SQLITE_OK) {
        sqlite3_free(err_msg);
        throw std::runtime_error("Error adding contact");
    }
}

void PBOperations::DeleteContact(int user_id) {
    char* err_msg = nullptr;

    std::string query =
        "DELETE FROM Contacts where Id=" + std::to_string(user_id);

    int status = sqlite3_exec(db_, query.c_str(), nullptr, 0, &err_msg);
    if (status != SQLITE_OK) {
        sqlite3_free(err_msg);
        throw std::runtime_error("Error deleting contact");
    }
}

int PBOperations::CountCallback(void* count, int argc, char** argv,
                                char** col_names) {
    unsigned int* cnt = reinterpret_cast<unsigned int*>(count);
    *cnt = atoi(argv[0]);
    return 0;
}

unsigned int PBOperations::CountContacts(const std::string& number) {
    char* err_msg = nullptr;
    unsigned int count = 0;
    std::string query =
        "SELECT COUNT(*) FROM Contacts where Number='" + number + "'";

    int status =
        sqlite3_exec(db_, query.c_str(), CountCallback, &count, &err_msg);
    if (status != SQLITE_OK) {
        sqlite3_free(err_msg);
        throw std::runtime_error("Error counting contacts for number " +
                                 number);
    }

    return count;
}

unsigned int PBOperations::CountContacts(const std::string& number,
                                         int exclude_id) {
    char* err_msg = nullptr;
    unsigned int count = 0;
    std::string query = "SELECT COUNT(*) FROM Contacts where Number='" +
                        number + "' AND Id!=" + std::to_string(exclude_id);

    int status =
        sqlite3_exec(db_, query.c_str(), CountCallback, &count, &err_msg);
    if (status != SQLITE_OK) {
        sqlite3_free(err_msg);
        throw std::runtime_error("Error counting contacts for number " +
                                 number);
    }

    return count;
}

unsigned int PBOperations::CountAllContacts() {
    char* err_msg = nullptr;
    unsigned int count = 0;
    std::string query = "SELECT COUNT(*) FROM Contacts";

    int status =
        sqlite3_exec(db_, query.c_str(), CountCallback, &count, &err_msg);
    if (status != SQLITE_OK) {
        sqlite3_free(err_msg);
        throw std::runtime_error("Error adding contact");
    }

    return count;
}

std::shared_ptr<Contact> PBOperations::GetContact(int user_id) {
    sqlite3_stmt* stmt;
    std::string query =
        "SELECT * FROM Contacts where Id=" + std::to_string(user_id);
    unsigned int count;

    int status =
        sqlite3_prepare_v2(db_, query.c_str(), query.size(), &stmt, nullptr);

    if (status != SQLITE_OK) {
        throw std::runtime_error("Error preparing contact data!");
    }

    status = sqlite3_step(stmt);
    if (status != SQLITE_ROW) {
        throw std::runtime_error("Error getting contact data!");
    }

    std::string f_name = std::string(reinterpret_cast<const char*>(
        sqlite3_column_text(stmt, PBOperations::FieldColumn::FirstName)));

    std::string l_name = std::string(reinterpret_cast<const char*>(
        sqlite3_column_text(stmt, PBOperations::FieldColumn::LastName)));

    std::string num = std::string(reinterpret_cast<const char*>(
        sqlite3_column_text(stmt, PBOperations::FieldColumn::PhoneNumber)));

    unsigned int type =
        sqlite3_column_int(stmt, PBOperations::FieldColumn::PhoneType);

    std::string nick = std::string(reinterpret_cast<const char*>(
        sqlite3_column_text(stmt, PBOperations::FieldColumn::Nickname)));

    std::string addr = std::string(reinterpret_cast<const char*>(
        sqlite3_column_text(stmt, PBOperations::FieldColumn::Address)));

    Contact local_contact =
        Contact::Build(f_name, num)
            .HasLastName(l_name)
            .HasPhoneType(static_cast<Contact::PhoneType>(type))
            .HasNickname(nick)
            .HasAddress(addr)
            .HasUserId(user_id);

    return std::make_shared<Contact>(std::move(local_contact));
}

void PBOperations::GetAllContacts(
    std::vector<std::shared_ptr<Contact>>* all_contacts) {
    sqlite3_stmt* stmt;
    std::string query = "SELECT * FROM Contacts";

    int status =
        sqlite3_prepare_v2(db_, query.c_str(), query.size(), &stmt, nullptr);

    if (status != SQLITE_OK) {
        throw std::runtime_error("Error preparing contact data!");
    }

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int unsigned user_id =
            sqlite3_column_int(stmt, PBOperations::FieldColumn::ID);

        std::string f_name = std::string(reinterpret_cast<const char*>(
            sqlite3_column_text(stmt, PBOperations::FieldColumn::FirstName)));

        std::string l_name = std::string(reinterpret_cast<const char*>(
            sqlite3_column_text(stmt, PBOperations::FieldColumn::LastName)));

        std::string num = std::string(reinterpret_cast<const char*>(
            sqlite3_column_text(stmt, PBOperations::FieldColumn::PhoneNumber)));

        unsigned int type =
            sqlite3_column_int(stmt, PBOperations::FieldColumn::PhoneType);

        std::string nick = std::string(reinterpret_cast<const char*>(
            sqlite3_column_text(stmt, PBOperations::FieldColumn::Nickname)));

        std::string addr = std::string(reinterpret_cast<const char*>(
            sqlite3_column_text(stmt, PBOperations::FieldColumn::Address)));

        Contact local_contact =
            Contact::Build(f_name, num)
                .HasLastName(l_name)
                .HasPhoneType(static_cast<Contact::PhoneType>(type))
                .HasNickname(nick)
                .HasAddress(addr)
                .HasUserId(user_id);
        all_contacts->push_back(
            std::make_shared<Contact>(std::move(local_contact)));
    }
}
