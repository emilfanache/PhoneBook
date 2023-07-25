#include <string>
#include <utility>

#include <QDebug>
#include <QObject>
#include <QPushButton>

#include "editform.h"
#include "insertform.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "contact.hpp"
#include "contact_builder.hpp"
#include "pb_operations.hpp"

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
    SetPBOperations(std::make_shared<PBOperations>());
    ListContacts();
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

void MainWindow::ListContacts() {
    std::vector<std::shared_ptr<Contact>> all_contacts;
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

        item = tbl->item(row_id, MainWindow::ColumnID::Type);
        tbl->setItem(row_id, MainWindow::ColumnID::Type,
                     new QTableWidgetItem(QString::fromStdString(
                         Contact::GetType(contact->GetType()))));
        item = tbl->item(row_id, MainWindow::ColumnID::Type);
        MakeIteamReadOnly(item);

        tbl->setItem(row_id, MainWindow::ColumnID::Nickname,
                     new QTableWidgetItem(
                         QString::fromStdString(contact->GetNickname())));
        tbl->setItem(row_id, MainWindow::ColumnID::Address,
                     new QTableWidgetItem(
                         QString::fromStdString(contact->GetAddress())));

        QWidget* pWidget = new QWidget();
        QPushButton* delete_row = new QPushButton("Delete", this);
        QHBoxLayout* pLayout = new QHBoxLayout(pWidget);
        pLayout->addWidget(delete_row);
        tbl->setCellWidget(row_id, MainWindow::ColumnID::Delete, pWidget);
        connect(delete_row, SIGNAL(clicked(bool)), this, SLOT(deleteThisRow()));

        QPushButton* edit_row = new QPushButton("Edit", this);
        pLayout->addWidget(edit_row);
        pLayout->setAlignment(Qt::AlignCenter);
        pLayout->setContentsMargins(0, 0, 0, 0);
        pWidget->setLayout(pLayout);
        tbl->setCellWidget(row_id, MainWindow::ColumnID::Edit, pWidget);
        connect(edit_row, SIGNAL(clicked(bool)), this, SLOT(editThisRow()));
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
        GetPBOperations()->UpdateContact(contact);
    }
}

void MainWindow::deleteThisRow() {
    QTableWidget* tbl = ui->PhonebookViewTable;
    QWidget* w = qobject_cast<QWidget*>(sender()->parent());
    if (w) {
        int row = tbl->indexAt(w->pos()).row();
        std::string num =
            tbl->item(row, MainWindow::ColumnID::Number)->text().toStdString();
        GetPBOperations()->DeleteContact(num);
        tbl->removeRow(row);
        tbl->setCurrentCell(0, 0);
    }
}

void MainWindow::editThisRow() {
    QTableWidget* tbl = ui->PhonebookViewTable;
    QWidget* w = qobject_cast<QWidget*>(sender()->parent());
    if (w) {
        int row = tbl->indexAt(w->pos()).row();
        std::string num =
            tbl->item(row, MainWindow::ColumnID::Number)->text().toStdString();
        EditForm* editForm = new EditForm(num, GetPBOperations(), this);
        connect(editForm, SIGNAL(TriggerTableUpdate()), this,
                SLOT(ReceiveTableUpdateTrigger()));
        editForm->show();
    }
}

void MainWindow::ReceiveTableUpdateTrigger() {
    ui->PhonebookViewTable->clear();
    ui->PhonebookViewTable->setRowCount(0);
    ListContacts();
}

void MainWindow::on_addButton_clicked() {
    InsertForm* insForm = new InsertForm(GetPBOperations(), this);
    connect(insForm, SIGNAL(TriggerTableUpdate()), this,
            SLOT(ReceiveTableUpdateTrigger()));
    insForm->show();
}

void MainWindow::on_closeMainButton_clicked() {
    this->close();
}
