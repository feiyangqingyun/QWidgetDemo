#pragma execution_character_set("utf-8")
#include "dbpage.h"

SqlQueryModel::SqlQueryModel(QObject *parent) : QSqlQueryModel(parent)
{
    allCenter = false;
    alignCenterColumn.clear();
    alignRightColumn.clear();
}

QVariant SqlQueryModel::data(const QModelIndex &index, int role) const
{
    QVariant value = QSqlQueryModel::data(index, role);

    if (allCenter) {
        if(role == Qt::TextAlignmentRole ) {
            value = Qt::AlignCenter;
        }
    } else {
        //逐个从列索引中查找是否当前列在其中
        int column = index.column();
        bool existCenter = alignCenterColumn.contains(column);
        bool existRight = alignRightColumn.contains(column);

        if(role == Qt::TextAlignmentRole) {
            if (existCenter) {
                value = Qt::AlignCenter;
            }

            if (existRight) {
                value = (QVariant)(Qt::AlignVCenter | Qt::AlignRight);
            }
        }
    }

    //实现鼠标经过整行换色,如果设置了hoverRow才需要处理
    if (property("hoverRow").isValid()) {
        int row = property("hoverRow").toInt();
        if (row == index.row()) {
            if(role == Qt::BackgroundRole) {
                value = QColor(property("hoverBgColor").toString());
            } else if(role == Qt::TextColorRole) {
                value = QColor(property("hoverTextColor").toString());
            }
        }
    }

    //实现隐藏部分显示,指定列和替换字符
    if (property("hideColumn").isValid()) {
        int column = property("hideColumn").toInt();
        if (column == index.column()) {
            if(role == Qt::DisplayRole) {
                QString letter = property("hideLetter").toString();
                int start = property("hideStart").toInt();
                int end = property("hideEnd").toInt();
                QString str = value.toString();

                QStringList list;
                for (int i = 0; i < str.length(); i++) {
                    if (i >= start && i <= end) {
                        list << letter;
                    } else {
                        list << str.at(i);
                    }
                }

                value = list.join("");
            }
        }
    }

    return value;
}

void SqlQueryModel::setAllCenter(bool allCenter)
{
    this->allCenter = allCenter;
}

void SqlQueryModel::setAlignCenterColumn(const QList<int> &alignCenterColumn)
{
    this->alignCenterColumn = alignCenterColumn;
}

void SqlQueryModel::setAlignRightColumn(const QList<int> &alignRightColumn)
{
    this->alignRightColumn = alignRightColumn;
}


DbCountThread::DbCountThread(QObject *parent) : QThread(parent)
{
    connName = "qt_sql_default_connection";
    sql = "select 1";

    connect(this, SIGNAL(finished()), this, SLOT(deleteLater()));
}

void DbCountThread::run()
{
    select();
}

void DbCountThread::setConnName(const QString &connName)
{
    this->connName = connName;
}

void DbCountThread::setSql(const QString &sql)
{
    this->sql = sql;
}

void DbCountThread::select()
{
    //计算用时
    QDateTime dtStart = QDateTime::currentDateTime();

    QSqlQuery query(QSqlDatabase::database(connName));
    query.exec(sql);
    query.next();
    int count = query.value(0).toUInt();

    QDateTime dtEnd = QDateTime::currentDateTime();
    double msec = dtStart.msecsTo(dtEnd);
    emit receiveCount(count, msec);
}


QScopedPointer<DbPage> DbPage::self;
DbPage *DbPage::Instance()
{
    if (self.isNull()) {
        static QMutex mutex;
        QMutexLocker locker(&mutex);
        if (self.isNull()) {
            self.reset(new DbPage);
        }
    }

    return self.data();
}

DbPage::DbPage(QObject *parent) : QObject(parent)
{
    startIndex = 0;
    tempSql = "";
    sql = "";
    queryModel = new SqlQueryModel;

    pageCurrent = 1;
    pageCount = 0;
    resultCount = 0;
    resultCurrent = 0;

    labPageCount = 0;
    labPageCurrent = 0;
    labResultCount = 0;
    labResultCurrent = 0;
    labResult = 0;
    labInfo = 0;

    tableView = 0;
    btnFirst = 0;
    btnPre = 0;
    btnNext = 0;
    btnLast = 0;

    countName = "*";
    connName = "qt_sql_default_connection";
    dbType = DbType_Sqlite;

    pageCurrent = 0;
    pageCount = 0;
    resultCount = 0;
    resultCurrent = 30;

    tableName = "";
    selectColumn = "*";
    orderSql = "";
    whereSql = "";
    columnNames.clear();
    columnWidths.clear();

    insertColumnIndex = -1;
    insertColumnName = "";
    insertColumnWidth = 50;
}

void DbPage::bindData(const QString &columnName, const QString &orderColumn, const QString &tableName,
                      QComboBox *cbox, const QString &connName)
{
    QSqlQuery query(QSqlDatabase::database(connName));
    query.exec("select " + columnName + " from " + tableName + " order by " + orderColumn + " asc");
    while (query.next()) {
        cbox->addItem(query.value(0).toString());
    }
}

void DbPage::bindData(const QString &columnName, const QString &orderColumn, const QString &tableName,
                      QList<QComboBox *> cboxs, const QString &connName)
{
    QSqlQuery query(QSqlDatabase::database(connName));
    query.exec("select " + columnName + " from " + tableName + " order by " + orderColumn + " asc");
    while (query.next()) {
        foreach (QComboBox *cbox, cboxs) {
            cbox->addItem(query.value(0).toString());
        }
    }
}

void DbPage::bindData(const QString &sql)
{
    queryModel->setQuery(sql, QSqlDatabase::database(connName));
    tableView->setModel(queryModel);

    //依次设置列标题列宽
    int columnCount = tableView->model()->columnCount();
    int nameCount = columnNames.count();
    columnCount = columnCount > nameCount ? nameCount : columnCount;

    QList<QString> columnNames = this->columnNames;
    QList<int> columnWidths = this->columnWidths;

    //根据设置添加新列,将对应新列的标题名称和宽度按照索引位置插
    if (insertColumnIndex >= 0) {
        columnCount++;
        columnNames.insert(insertColumnIndex, insertColumnName);
        columnWidths.insert(insertColumnIndex, insertColumnWidth);
        queryModel->insertColumn(insertColumnIndex);
    }

    //设置列标题和列宽度
    for (int i = 0; i < columnCount; i++) {
        queryModel->setHeaderData(i, Qt::Horizontal, columnNames.at(i));
        tableView->setColumnWidth(i, columnWidths.at(i));
    }

    if (labPageCurrent != 0) {
        labPageCurrent->setText(QString("第 %1 页").arg(pageCurrent));
    }

    if (labPageCount != 0) {
        labPageCount->setText(QString("共 %1 页").arg(pageCount));
    }

    if (labResultCount != 0) {
        labResultCount->setText(QString("共 %1 条").arg(resultCount));
    }

    if (labResultCurrent != 0) {
        labResultCurrent->setText(QString("每页 %1 条").arg(resultCurrent));
    }

    if (labInfo != 0) {
        labInfo->setText(QString("共 %1 条  每页 %2 条  共 %3 页  第 %4 页").arg(resultCount).arg(resultCurrent).arg(pageCount).arg(pageCurrent));
    }

    //发送结果信号
    emit receivePage(pageCurrent, pageCount, resultCount, resultCurrent);
}

void DbPage::slot_receiveCount(quint32 count, double msec)
{
    if (labResult != 0) {
        labResult->setText(QString("查询用时 %1 秒").arg(QString::number(msec / 1000, 'f', 3)));
    }

    resultCount = count;

    int yushu = resultCount % resultCurrent;

    //不存在余数,说明是整行,例如300%5==0
    if (yushu == 0) {
        if (resultCount > 0 && resultCount < resultCurrent) {
            pageCount = 1;
        } else {
            pageCount = resultCount / resultCurrent;
        }
    } else {
        pageCount = (resultCount / resultCurrent) + 1;
    }

    //2014-10-9增加翻页按钮可用不可用处理,如果只有一页数据,则翻页按钮不可用
    if (pageCount <= 1) {
        btnFirst->setEnabled(false);
        btnLast->setEnabled(false);
        btnNext->setEnabled(false);
        btnPre->setEnabled(false);
    } else {
        btnFirst->setEnabled(true);
        btnLast->setEnabled(true);
        btnNext->setEnabled(true);
        btnPre->setEnabled(true);
    }

    tempSql = QString("select %1 from %2 %3 order by %4").arg(selectColumn).arg(tableName).arg(whereSql).arg(orderSql);
    sql = QString("%1 limit %2,%3;").arg(tempSql).arg(startIndex).arg(resultCurrent); //组织分页SQL语句

    bindData(sql);
}

void DbPage::first()
{
    if (pageCount > 1) {
        startIndex = 0;
        pageCurrent = 1;
        sql = QString("%1 limit %2,%3;").arg(tempSql).arg(startIndex).arg(resultCurrent);
        bindData(sql);
        btnLast->setEnabled(true);
        btnNext->setEnabled(true);
    }

    btnFirst->setEnabled(false);
    btnPre->setEnabled(false);
}

void DbPage::previous()
{
    if (pageCurrent > 1) {
        pageCurrent--;
        startIndex -= resultCurrent;
        sql = QString("%1 limit %2,%3;").arg(tempSql).arg(startIndex).arg(resultCurrent);
        bindData(sql);
        btnLast->setEnabled(true);
        btnNext->setEnabled(true);
    }

    if (pageCurrent == 1) {
        btnFirst->setEnabled(false);
        btnPre->setEnabled(false);
    }
}

void DbPage::next()
{
    if (pageCurrent < pageCount) {
        pageCurrent++;
        startIndex += resultCurrent;
        sql = QString("%1 limit %2,%3;").arg(tempSql).arg(startIndex).arg(resultCurrent);
        bindData(sql);
        btnFirst->setEnabled(true);
        btnPre->setEnabled(true);
    }

    if (pageCurrent == pageCount) {
        btnLast->setEnabled(false);
        btnNext->setEnabled(false);
    }
}

void DbPage::last()
{
    if (pageCount > 0) {
        startIndex = (pageCount - 1) * resultCurrent;
        pageCurrent = pageCount;
        sql = QString("%1 limit %2,%3;").arg(tempSql).arg(startIndex).arg(resultCurrent);
        bindData(sql);
        btnFirst->setEnabled(true);
        btnPre->setEnabled(true);
    }

    btnLast->setEnabled(false);
    btnNext->setEnabled(false);
}

//设置显示数据的表格控件,当前翻页信息的标签控件等
void DbPage::setControl(QTableView *tableView,
                        QLabel *labPageCount, QLabel *labPageCurrent,
                        QLabel *labResultCount, QLabel *labResultCurrent,
                        QLabel *labResult, QLabel *labInfo,
                        QAbstractButton *btnFirst, QAbstractButton *btnPre,
                        QAbstractButton *btnNext, QAbstractButton *btnLast,
                        const QString &countName, const QString &connName)
{
    this->tableView = tableView;
    this->labPageCount = labPageCount;
    this->labPageCurrent = labPageCurrent;
    this->labResultCount = labResultCount;
    this->labResultCurrent = labResultCurrent;
    this->labResult = labResult;
    this->labInfo = labInfo;

    this->btnFirst = btnFirst;
    this->btnPre = btnPre;
    this->btnNext = btnNext;
    this->btnLast = btnLast;

    this->countName = countName;
    this->connName = connName;
    this->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);

    //挂载翻页按钮事件
    connect(btnFirst, SIGNAL(clicked()), this, SLOT(first()));
    connect(btnPre, SIGNAL(clicked()), this, SLOT(previous()));
    connect(btnNext, SIGNAL(clicked()), this, SLOT(next()));
    connect(btnLast, SIGNAL(clicked()), this, SLOT(last()));
}

void DbPage::setConnName(const QString &connName)
{
    this->connName = connName;
}

void DbPage::setDbType(const DbPage::DbType &dbType)
{
    this->dbType = dbType;
}

void DbPage::setTableName(const QString &tableName)
{
    this->tableName = tableName;
}

void DbPage::setSelectColumn(const QString &selectColumn)
{
    this->selectColumn = selectColumn;
}

void DbPage::setOrderSql(const QString &orderSql)
{
    this->orderSql = orderSql;
}

void DbPage::setWhereSql(const QString &whereSql)
{
    this->whereSql = whereSql;
}

void DbPage::setResultCurrent(int resultCurrent)
{
    this->resultCurrent = resultCurrent;
}

void DbPage::setColumnNames(const QList<QString> &columnNames)
{
    this->columnNames = columnNames;
}

void DbPage::setColumnWidths(const QList<int> &columnWidths)
{
    this->columnWidths = columnWidths;
}

void DbPage::setAllCenter(bool allCenter)
{
    queryModel->setAllCenter(allCenter);
}

void DbPage::setAlignCenterColumn(const QList<int> &alignCenterColumn)
{
    queryModel->setAlignCenterColumn(alignCenterColumn);
}

void DbPage::setAlignRightColumn(const QList<int> &alignRightColumn)
{
    queryModel->setAlignRightColumn(alignRightColumn);
}

void DbPage::setInsertColumnIndex(int insertColumnIndex)
{
    this->insertColumnIndex = insertColumnIndex;
}

void DbPage::setInsertColumnName(const QString &insertColumnName)
{
    this->insertColumnName = insertColumnName;
}

void DbPage::setInsertColumnWidth(int insertColumnWidth)
{
    this->insertColumnWidth = insertColumnWidth;
}

void DbPage::select()
{
    //重置开始索引
    startIndex = 0;
    pageCurrent = 1;

    //假设只有一页
    slot_receiveCount(resultCurrent, 0);

    //全部禁用按钮,文本显示正在查询...
    btnFirst->setEnabled(false);
    btnLast->setEnabled(false);
    btnNext->setEnabled(false);
    btnPre->setEnabled(false);

    QString info = "正在查询...";

    if (labInfo != 0) {
        labInfo->setText(info);
    }

    if (labPageCurrent != 0) {
        labPageCurrent->setText(info);
    }

    if (labPageCount != 0) {
        labPageCount->setText(info);
    }

    if (labResultCount != 0) {
        labResultCount->setText(info);
    }

    if (labResultCurrent != 0) {
        labResultCurrent->setText(info);
    }

    if (labResult != 0) {
        labResult->setText(info);
    }

    //开始分页绑定数据前,计算好总数据量以及行数
    tempSql = QString("select count(%1) from %2 %3").arg(countName).arg(tableName).arg(whereSql);

    //采用线程执行查询复合条件的记录行数
    DbCountThread *dbCountThread = new DbCountThread(this);
    //绑定查询结果信号槽,一旦收到结果则立即执行
    connect(dbCountThread, SIGNAL(receiveCount(quint32, double)), this, SIGNAL(receiveCount(quint32, double)));
    connect(dbCountThread, SIGNAL(receiveCount(quint32, double)), this, SLOT(slot_receiveCount(quint32, double)));

    //设置数据库连接名称和查询语句,并启动线程
    dbCountThread->setConnName(connName);
    dbCountThread->setSql(tempSql);
    //从5.10开始不支持数据库在线程中执行
#if (QT_VERSION <= QT_VERSION_CHECK(5,10,0))
    dbCountThread->start();
#else
    dbCountThread->select();
#endif
}
