#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtSql/QtSql>

class QTableView;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:

    void create_tables();
    void insert_tables();
    void init_models();
    void create_delbuttons(int column, QSqlTableModel *model, QTableView *table);

    void delete_row();

private:
    Ui::MainWindow *ui;
    QSqlDatabase db;

};
#endif // MAINWINDOW_H
