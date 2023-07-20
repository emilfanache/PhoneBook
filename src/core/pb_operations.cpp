#include <memory>
#include <stdexcept>
#include <utility>

#include "contact.hpp"
#include "contact_builder.hpp"
#include "pb_operations.hpp"

const char PBOperations::db_name_[] =
    "/home/emil/PhoneBook/db/phonebook.sqlite";

PBOperations::PBOperations() {
    int status = sqlite3_open(PBOperations::db_name_, &db_);
    if (status) {
        std::cout << "Error opening DB: " << sqlite3_errmsg(db_) << std::endl;
    }
}

PBOperations::~PBOperations() {
    // std::cout << "Destructor called!" << std::endl;
    sqlite3_close(db_);
}

void PBOperations::AddContact(const Contact& contact) {
    char* err_msg = nullptr;

    std::string values_str =
        "VALUES ('" + contact.GetFirstName() + "', '" + contact.GetLastName() +
        "', '" + contact.GetNumber() + "', " +
        std::to_string(contact.GetType()) + ", '" + contact.GetNickname() +
        "', '" + contact.GetAddress() + "')";

    std::string query =
        "INSERT INTO Contacts "
        "(FirstName, LastName, Number, Type, Nickname, Address) ";

    query += values_str;
    // std::cout << "Query: " << query.c_str() << std::endl;

    int status = sqlite3_exec(db_, query.c_str(), nullptr, 0, &err_msg);
    if (status != SQLITE_OK) {
        // std::cout << "status=" << status << " " << err_msg << std::endl;
        sqlite3_free(err_msg);
        throw std::runtime_error("Error adding contact");
    }
}

void PBOperations::UpdateContact(const Contact& contact) {
    char* err_msg = nullptr;
    std::string query =
        "UPDATE Contacts SET FirstName='" + contact.GetFirstName() + "', " +
        "LastName='" + contact.GetLastName() + "', " +
        "Type=" + std::to_string(contact.GetType()) + ", " + "Nickname='" +
        contact.GetNickname() + "', " + "Address='" + contact.GetAddress() +
        "' " + "WHERE number='" + contact.GetNumber() + "'";

    int status = sqlite3_exec(db_, query.c_str(), nullptr, 0, &err_msg);
    if (status != SQLITE_OK) {
        // std::cout << "status=" << status << " " << err_msg << std::endl;
        sqlite3_free(err_msg);
        throw std::runtime_error("Error adding contact");
    }
}

void PBOperations::DeleteContact(const Contact& contact) {
    char* err_msg = nullptr;

    std::string query =
        "DELETE FROM Contacts where Number='" + contact.GetNumber() + "'";
    // std::cout << "Query: " << query.c_str() << std::endl;

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
    unsigned int count;
    std::string query = "SELECT * FROM Contacts where Number='" + number + "'";

    int status =
        sqlite3_exec(db_, query.c_str(), CountCallback, &count, &err_msg);
    if (status != SQLITE_OK) {
        // std::cout << "status=" << status << " " << err_msg << std::endl;
        sqlite3_free(err_msg);
        throw std::runtime_error("Error adding contact");
    }

    return count;
}

unsigned int PBOperations::CountAllContacts() {
    char* err_msg = nullptr;
    unsigned int count;
    std::string query = "SELECT * FROM Contacts";

    int status =
        sqlite3_exec(db_, query.c_str(), CountCallback, &count, &err_msg);
    if (status != SQLITE_OK) {
        // std::cout << "status=" << status << " " << err_msg << std::endl;
        sqlite3_free(err_msg);
        throw std::runtime_error("Error adding contact");
    }

    return count;
}

std::shared_ptr<Contact> PBOperations::GetContact(const std::string& number) {
    sqlite3_stmt* stmt;
    std::string query = "SELECT * FROM Contacts where Number='" + number + "'";
    unsigned int count;

    try {
        count = CountContacts(number);
    } catch (std::runtime_error& e) {
        count = 0;
    }

    if (!count) {
        return nullptr;
    }

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
            .HasAddress(addr);

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
                .HasAddress(addr);
        all_contacts->push_back(
            std::make_shared<Contact>(std::move(local_contact)));
    }
}
