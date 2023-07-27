#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QLabel>
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
    enum ColumnID {
        FirstName = 0,
        LastName,
        Number,
        Type,
        Nickname,
        Address,
        Actions,
        ID
    };

    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

    void ListContacts();
    void SetPBOperations(std::shared_ptr<PBOperations> oper);
    static void PrintErrorLabel(QLabel* label, const char* err_str);

 private slots:
    void deleteThisRow();
    void editThisRow();
    void on_updateButton_clicked();
    void on_addButton_clicked();
    void ReceiveTableUpdateTrigger();
    void on_closeMainButton_clicked();
    void on_refreshButton_clicked();

 private:
    Ui::MainWindow* ui;
    std::shared_ptr<PBOperations> pb_operations_;
};
#endif  // MAINWINDOW_H
