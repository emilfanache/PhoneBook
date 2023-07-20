#include <string>
#include <utility>

#include <QDebug>
#include <QObject>
#include <QPushButton>

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "contact.hpp"
#include "contact_builder.hpp"
#include "pb_operations.hpp"

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
    std::vector<std::shared_ptr<Contact>> all_contacts;

    ui->setupUi(this);
    SetPBOperations(std::make_shared<PBOperations>());
    // ui->PhonebookViewTable->horizontalHeader()->setSectionResizeMode(
    //     QHeaderView::Stretch);

    ListContacts(all_contacts);
    QObject::connect(ui->updateButton, SIGNAL(clicked()), this,
                     SLOT(on_updateButton_clicked()));
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::MakeIteamReadOnly(QTableWidgetItem* item) {
    item->setFlags(item->flags() ^ Qt::ItemIsEditable);
}

void MainWindow::SetPBOperations(std::shared_ptr<PBOperations> oper) {
    pb_operations_ = std::move(oper);
}

std::shared_ptr<PBOperations> MainWindow::GetPBOperations() {
    return pb_operations_;
}

void MainWindow::ListContacts(
    std::vector<std::shared_ptr<Contact>>& all_contacts) {
    try {
        GetPBOperations()->GetAllContacts(&all_contacts);
        ui->PhonebookViewTable->setRowCount(all_contacts.size());
    } catch (...) {
        return;
    }

    int row_id = 0;
    QTableWidget* tbl = ui->PhonebookViewTable;
    for (const auto& contact : all_contacts) {
        tbl->setItem(row_id, MainWindow::ColumnID::FirstName,
                     new QTableWidgetItem(
                         QString::fromStdString(contact->GetFirstName())));
        tbl->setItem(row_id, MainWindow::ColumnID::LastName,
                     new QTableWidgetItem(
                         QString::fromStdString(contact->GetLastName())));
        tbl->setItem(
            row_id, MainWindow::ColumnID::Number,
            new QTableWidgetItem(QString::fromStdString(contact->GetNumber())));
        // make number column read only
        QTableWidgetItem* item =
            tbl->item(row_id, MainWindow::ColumnID::Number);
        MakeIteamReadOnly(item);

        tbl->setItem(row_id, MainWindow::ColumnID::Type,
                     new QTableWidgetItem(QString::fromStdString(
                         Contact::GetType(contact->GetType()))));
        tbl->setItem(row_id, MainWindow::ColumnID::Nickname,
                     new QTableWidgetItem(
                         QString::fromStdString(contact->GetNickname())));
        tbl->setItem(row_id, MainWindow::ColumnID::Address,
                     new QTableWidgetItem(
                         QString::fromStdString(contact->GetAddress())));
        row_id++;
    }
}

void MainWindow::on_updateButton_clicked() {
    QTableWidget* tbl = ui->PhonebookViewTable;
    int row_count = ui->PhonebookViewTable->rowCount();
    for (int row = 0; row < row_count; row++) {
        std::string f_name = tbl->item(row, MainWindow::ColumnID::FirstName)
                                 ->text()
                                 .toStdString();
        std::string l_name = tbl->item(row, MainWindow::ColumnID::LastName)
                                 ->text()
                                 .toStdString();
        std::string num =
            tbl->item(row, MainWindow::ColumnID::Number)->text().toStdString();
        std::string type_str =
            tbl->item(row, MainWindow::ColumnID::Type)->text().toStdString();
        std::string nick = tbl->item(row, MainWindow::ColumnID::Nickname)
                               ->text()
                               .toStdString();
        std::string addr =
            tbl->item(row, MainWindow::ColumnID::Address)->text().toStdString();

        Contact::PhoneType type = Contact::GetType(type_str);

        Contact contact = Contact::Build(f_name, num)
                              .HasLastName(l_name)
                              .HasPhoneType(type)
                              .HasNickname(nick)
                              .HasAddress(addr);

        // std::cout << contact << std::endl;

        GetPBOperations()->UpdateContact(contact);
    }
}
