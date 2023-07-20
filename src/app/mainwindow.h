#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTableWidgetItem>
#include <memory>
#include <vector>

#include "pb_operations.hpp"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

 public:
    enum ColumnID { FirstName = 0, LastName, Number, Type, Nickname, Address };

    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

    void ListContacts(std::vector<std::shared_ptr<Contact>>& all_contacts);

    void SetPBOperations(std::shared_ptr<PBOperations> oper);
    std::shared_ptr<PBOperations> GetPBOperations();

 private slots:
    //void on_PhonebookViewTable_itemDoubleClicked(QTableWidgetItem* item);

    void on_updateButton_clicked();

 private:
    void MakeIteamReadOnly(QTableWidgetItem* item);

    Ui::MainWindow* ui;
    std::shared_ptr<PBOperations> pb_operations_;
};
#endif  // MAINWINDOW_H
