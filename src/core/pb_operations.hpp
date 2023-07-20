#ifndef PB_OPERATIONS_HPP_
#define PB_OPERATIONS_HPP_

#include <sqlite3.h>
#include <memory>
#include <string>
#include <vector>

class Contact;

class PBOperations {
 public:
    enum FieldColumn {
        ID = 0,
        FirstName,
        LastName,
        PhoneNumber,
        PhoneType,
        Nickname,
        Address
    };

    PBOperations();
    ~PBOperations();

    void AddContact(const Contact& contact);
    void DeleteContact(const Contact& contact);
    std::shared_ptr<Contact> GetContact(const std::string& number);
    void GetAllContacts(std::vector<std::shared_ptr<Contact>>* all_contacts);
    unsigned int CountAllContacts();
    void UpdateContact(const Contact& contact);

    static const char db_name_[];

 private:
    static int CountCallback(void* count, int argc, char** argv,
                             char** col_name);
    unsigned int CountContacts(const std::string& number);

    sqlite3* db_;
};

#endif  // PB_OPERATIONS_HPP_
