#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QMessageBox>
#include <QPushButton>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(":memory:");
   // db.setDatabaseName("db.sqlite");
    if (!db.open()) {
        QMessageBox::critical(nullptr, "Cannot open database","can't open/create DB");
        return;
    }

    ui->tableView->verticalHeader()->hide();
    ui->tableView->setObjectName("groups");
    ui->tableView_2->verticalHeader()->hide();
    ui->tableView_2->setObjectName("albums");
    ui->tableView_3->verticalHeader()->hide();

    ui->tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    ui->tableView_2->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableView_2->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableView_2->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->tableView_2->horizontalHeader()->setStretchLastSection(true);


    ui->tableView_3->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableView_3->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableView_3->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableView_3->horizontalHeader()->setStretchLastSection(true);


    create_tables();
    insert_tables();
    init_models();

}

MainWindow::~MainWindow()
{
    db.close();
   // QFile::remove("db.sqlite");
    delete ui;
}

void MainWindow::create_tables()
{
    QSqlQuery query(db);

    if(!query.exec("PRAGMA foreign_keys = ON"))//включаю внешние ключи
       qDebug() << db.lastError();

    query.exec(
          "CREATE TABLE groups"
          "("
          " id INTEGER PRIMARY KEY AUTOINCREMENT,"
          " name VARCHAR(250)   )"
          );

    query.exec(
          "CREATE TABLE albums"
          "("
          " id INTEGER PRIMARY KEY AUTOINCREMENT,"
          " group_id INTEGER                    ,"
          " name VARCHAR(250)                   ,"
          " FOREIGN KEY (group_id) REFERENCES groups(id) ON DELETE CASCADE)"
          );


    query.exec(
          "CREATE TABLE songs"
          "("
          " id INTEGER PRIMARY KEY AUTOINCREMENT,"
          " group_id INTEGER                    ,"
          " album_id INTEGER                    ,"
          " name  VARCHAR(250)                  ,"
          " FOREIGN KEY (group_id) REFERENCES groups(id) ON DELETE CASCADE,"
          " FOREIGN KEY (album_id) REFERENCES albums(id) ON DELETE CASCADE )"
          );
}

void MainWindow::insert_tables()
{
    QSqlQuery query(db);

    query.exec("INSERT INTO groups VALUES(1, 'Metallica')");
    query.exec("INSERT INTO groups VALUES(2, 'Rammstein')");

    query.exec("INSERT INTO albums VALUES(1, 1, 'Master of Puppets 1986')");
    query.exec("INSERT INTO albums VALUES(2, 2, 'Mutter 2001')");

    query.exec("INSERT INTO songs VALUES(1, 1, 1, 'Battery')");
    query.exec("INSERT INTO songs VALUES(2, 1, 1, 'Welcome Home (Sanitarium)')");
    query.exec("INSERT INTO songs VALUES(3, 2, 2, 'Sonne')");
    query.exec("INSERT INTO songs VALUES(4, 2, 2, 'Links 2-3-4')");

}

void MainWindow::init_models()
{
QSqlTableModel *model_groups = new QSqlTableModel(this, db);
model_groups->setTable("groups");
model_groups->setEditStrategy(QSqlTableModel::OnManualSubmit);
model_groups->select();

ui->tableView->setModel(model_groups);

model_groups->insertColumn(2);
model_groups->setHeaderData(2, Qt::Horizontal, "Delete");

create_delbuttons(2, model_groups, ui->tableView);


QSqlTableModel *model_albums = new QSqlTableModel(this, db);
model_albums->setTable("albums");
model_albums->setEditStrategy(QSqlTableModel::OnManualSubmit);
model_albums->select();

ui->tableView_2->setModel(model_albums);


model_albums->insertColumn(3);
model_albums->setHeaderData(3, Qt::Horizontal, "Delete");

create_delbuttons(3, model_albums, ui->tableView_2);

QSqlTableModel *model_songs = new QSqlTableModel(this, db);
model_songs->setTable("songs");
model_songs->setEditStrategy(QSqlTableModel::OnManualSubmit);
model_songs->select();

ui->tableView_3->setModel(model_songs);

model_songs->insertColumn(4);
model_songs->setHeaderData(4, Qt::Horizontal, "Delete");
create_delbuttons(4, model_songs, ui->tableView_3);


}

void MainWindow::create_delbuttons(int column, QSqlTableModel *model, QTableView *table)
{
    for(int i = 0; i < table->horizontalHeader()->count(); i++)
    {
        QPushButton *delButton = new QPushButton(this);
        delButton->setFixedSize(24,24);
        delButton->setText("X");

        QWidget* wdg = new QWidget;
        QHBoxLayout* layout = new QHBoxLayout(wdg);
        layout->addWidget(delButton);
        layout->setAlignment(Qt::AlignCenter);
        layout->setMargin(0);
        wdg->setLayout(layout);

        table->setIndexWidget(model->index(i, column), wdg);
        connect(delButton , &QPushButton::clicked , this , &MainWindow::delete_row);
    }
}

void MainWindow::delete_row()
{

QList <QTableView *>  tables = this->findChildren<QTableView *>();

for(QTableView *table : tables)
{
    QItemSelectionModel * selection = table->selectionModel();
    QSqlTableModel *model = qobject_cast <QSqlTableModel *>(table->model());
    QModelIndexList indexes = selection->selectedIndexes();
    if(!indexes.isEmpty())
    {
        int row = indexes.at(0).row();
        table->model()->removeRow(row);
        model->submitAll();
    }

}

init_models();
}

