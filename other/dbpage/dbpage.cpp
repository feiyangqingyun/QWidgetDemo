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
        if (role == Qt::TextAlignmentRole) {
            value = Qt::AlignCenter;
        }
    } else {
        //逐个从列索引中查找是否当前列在其中
        int column = index.column();
        bool existCenter = alignCenterColumn.contains(column);
        bool existRight = alignRightColumn.contains(column);

        if (role == Qt::TextAlignmentRole) {
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
            if (role == Qt::BackgroundRole) {
                value = QColor(property("hoverBgColor").toString());
            } else if (role == Qt::ForegroundRole) {
                value = QColor(property("hoverTextColor").toString());
            }
        }
    }

    //实现隐藏部分显示,指定列和替换字符
    if (property("hideColumn").isValid()) {
        int column = property("hideColumn").toInt();
        if (column == index.column()) {
            if (role == Qt::DisplayRole) {
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
    int count = 0;
    QSqlQuery query(QSqlDatabase::database(connName));
    if (query.exec(sql)) {
        if (query.next()) {
            count = query.value(0).toUInt();
        }
    }

    QDateTime dtEnd = QDateTime::currentDateTime();
    double msec = dtStart.msecsTo(dtEnd);
    emit receiveCount(count, msec);
}


DbPage::DbPage(QObject *parent) : QObject(parent)
{
    startIndex = 0;
    queryModel = new SqlQueryModel;

    pageCurrent = 1;
    pageTotal = 0;
    recordsTotal = 0;
    recordsPerpage = 0;

    labPageTotal = 0;
    labPageCurrent = 0;
    labRecordsTotal = 0;
    labRecordsPerpage = 0;
    labSelectTime = 0;
    labSelectInfo = 0;

    tableView = 0;
    btnFirst = 0;
    btnPrevious = 0;
    btnNext = 0;
    btnLast = 0;

    countName = "*";
    connName = "qt_sql_default_connection";
    dbType = DbType_Sqlite;

    pageCurrent = 0;
    pageTotal = 0;
    recordsTotal = 0;
    recordsPerpage = 30;

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

    if (labPageTotal != 0) {
        labPageTotal->setText(QString("共 %1 页").arg(pageTotal));
    }

    if (labRecordsTotal != 0) {
        labRecordsTotal->setText(QString("共 %1 条").arg(recordsTotal));
    }

    if (labRecordsPerpage != 0) {
        labRecordsPerpage->setText(QString("每页 %1 条").arg(recordsPerpage));
    }

    if (labSelectInfo != 0) {
        //labSelectInfo->setText(QString("共 %1 条  每页 %2 条  共 %3 页  第 %4 页").arg(recordsTotal).arg(recordsPerpage).arg(pageTotal).arg(pageCurrent));
        labSelectInfo->setText(QString("第 %1 页  每页 %2 条  共 %3 页  共 %4 条").arg(pageCurrent).arg(recordsPerpage).arg(pageTotal).arg(recordsTotal));
    }

    //发送结果信号
    if (recordsTotal != recordsPerpage) {
        emit receivePage(pageCurrent, pageTotal, recordsTotal, recordsPerpage);
        //qDebug() << TIMEMS << startIndex << pageCurrent << pageTotal << recordsTotal << recordsPerpage;
    }

    changeBtnEnable();
}

QString DbPage::getPageSql()
{
    //组织分页SQL语句,不同的数据库分页语句不一样
    QString sql = QString("select %1 from %2 %3 order by %4").arg(selectColumn).arg(tableName).arg(whereSql).arg(orderSql);
    if (dbType == DbType_PostgreSQL || dbType == DbType_KingBase) {
        sql = QString("%1 limit %3 offset %2;").arg(sql).arg(startIndex).arg(recordsPerpage);
    } else if (dbType == DbType_SqlServer) {
        //取第m条到第n条记录：select top (n-m+1) id from tablename where id not in (select top m-1 id from tablename)
        //sql = QString("select %1 from %2 %3 order by %4").arg(selectColumn).arg(tableName).arg(whereSql).arg(orderSql);
    } else if (dbType == DbType_Oracle) {
        //暂时没有找到好办法
    } else {
        sql = QString("%1 limit %2,%3;").arg(sql).arg(startIndex).arg(recordsPerpage);
    }

    return sql;
}

void DbPage::slot_receiveCount(quint32 count, double msec)
{
    if (labSelectTime != 0) {
        labSelectTime->setText(QString("查询用时 %1 秒").arg(QString::number(msec / 1000, 'f', 3)));
    }

    recordsTotal = count;
    int yushu = recordsTotal % recordsPerpage;

    //不存在余数,说明是整行,例如300%5==0
    if (yushu == 0) {
        if (recordsTotal > 0 && recordsTotal < recordsPerpage) {
            pageTotal = 1;
        } else {
            pageTotal = recordsTotal / recordsPerpage;
        }
    } else {
        pageTotal = (recordsTotal / recordsPerpage) + 1;
    }

    bindData(getPageSql());
}

//设置显示数据的表格控件,当前翻页信息的标签控件等
void DbPage::setControl(QTableView *tableView,
                        QLabel *labPageTotal, QLabel *labPageCurrent,
                        QLabel *labRecordsTotal, QLabel *labRecordsPerpage,
                        QLabel *labSelectTime, QLabel *labSelectInfo,
                        QAbstractButton *btnFirst, QAbstractButton *btnPrevious,
                        QAbstractButton *btnNext, QAbstractButton *btnLast,
                        const QString &countName, const QString &connName)
{
    this->tableView = tableView;
    this->labPageTotal = labPageTotal;
    this->labPageCurrent = labPageCurrent;
    this->labRecordsTotal = labRecordsTotal;
    this->labRecordsPerpage = labRecordsPerpage;
    this->labSelectTime = labSelectTime;
    this->labSelectInfo = labSelectInfo;

    this->btnFirst = btnFirst;
    this->btnPrevious = btnPrevious;
    this->btnNext = btnNext;
    this->btnLast = btnLast;

    this->countName = countName;
    this->connName = connName;
    this->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);

    if (btnFirst == 0 || btnPrevious == 0 || btnNext == 0 || btnLast == 0) {
        return;
    }

    //挂载翻页按钮事件
    connect(btnFirst, SIGNAL(clicked()), this, SLOT(first()));
    connect(btnPrevious, SIGNAL(clicked()), this, SLOT(previous()));
    connect(btnNext, SIGNAL(clicked()), this, SLOT(next()));
    connect(btnLast, SIGNAL(clicked()), this, SLOT(last()));
}

void DbPage::setControl(QTableView *tableView,
                        QLabel *labPageTotal, QLabel *labPageCurrent,
                        QLabel *labRecordsTotal, QLabel *labRecordsPerpage,
                        QLabel *labSelectTime, QLabel *labSelectInfo,
                        const QString &countName, const QString &connName)
{
    setControl(tableView, labPageTotal, labPageCurrent, labRecordsTotal, labRecordsPerpage, labSelectTime, labSelectInfo, 0, 0, 0, 0, countName, connName);
}

void DbPage::setControl(QTableView *tableView,
                        QAbstractButton *btnFirst, QAbstractButton *btnPrevious,
                        QAbstractButton *btnNext, QAbstractButton *btnLast,
                        const QString &countName, const QString &connName)
{
    setControl(tableView, 0, 0, 0, 0, 0, 0, btnFirst, btnPrevious, btnNext, btnLast, countName, connName);
}

void DbPage::setControl(QTableView *tableView, const QString &countName, const QString &connName)
{
    setControl(tableView, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, countName, connName);
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

void DbPage::setRecordsPerpage(int recordsPerpage)
{
    this->recordsPerpage = recordsPerpage;
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

void DbPage::changeBtnEnable()
{
    if (btnFirst == 0 || btnPrevious == 0 || btnNext == 0 || btnLast == 0) {
        return;
    }

    //下面默认对上一页下一页按钮禁用
    //也可以取消注释对第一页末一页同样处理
    //因为到了第一页就可以不必再单击第一页和上一页
    if (pageTotal <= 1) {
        //如果只有一页数据则翻页按钮不可用
        btnFirst->setEnabled(false);
        btnLast->setEnabled(false);
        btnPrevious->setEnabled(false);
        btnNext->setEnabled(false);
    } else {
        //判断是否在首页末页禁用按钮
        bool first = (pageCurrent == 1);
        bool last = (pageCurrent == pageTotal);
        btnFirst->setEnabled(!first);
        btnLast->setEnabled(!last);
        btnPrevious->setEnabled(!first);
        btnNext->setEnabled(!last);
    }
}

void DbPage::select()
{
    //重置开始索引
    startIndex = 0;
    pageCurrent = 1;
    pageTotal = 1;
    changeBtnEnable();

    //假设只有一页
    slot_receiveCount(recordsPerpage, 0);

    //文本显示正在查询...
    QString info = "正在查询...";
    if (labSelectInfo != 0) {
        labSelectInfo->setText(info);
    }
    if (labPageCurrent != 0) {
        labPageCurrent->setText(info);
    }
    if (labPageTotal != 0) {
        labPageTotal->setText(info);
    }
    if (labRecordsTotal != 0) {
        labRecordsTotal->setText(info);
    }
    if (labRecordsPerpage != 0) {
        labRecordsPerpage->setText(info);
    }
    if (labSelectTime != 0) {
        labSelectTime->setText(info);
    }

    //开始分页绑定数据前,计算好总数据量以及行数
    QString sql = QString("select count(%1) from %2 %3").arg(countName).arg(tableName).arg(whereSql);

    //采用线程执行查询复合条件的记录行数
    DbCountThread *dbCountThread = new DbCountThread(this);
    //绑定查询结果信号槽,一旦收到结果则立即执行
    connect(dbCountThread, SIGNAL(receiveCount(quint32, double)), this, SIGNAL(receiveCount(quint32, double)));
    connect(dbCountThread, SIGNAL(receiveCount(quint32, double)), this, SLOT(slot_receiveCount(quint32, double)));

    //设置数据库连接名称和查询语句,并启动线程
    dbCountThread->setConnName(connName);
    dbCountThread->setSql(sql);
    //从5.10开始不支持数据库在线程中执行
#if (QT_VERSION <= QT_VERSION_CHECK(5,10,0))
    dbCountThread->start();
#else
    dbCountThread->select();
#endif
}

void DbPage::selectPage(int page)
{
    //必须小于总页数+不是当前页
    if (page >= 1 && page <= pageTotal && page != pageCurrent) {
        //计算指定页对应开始的索引
        startIndex = (page - 1) * recordsPerpage;
        pageCurrent = page;
        bindData(getPageSql());
    }
}

void DbPage::first()
{
    //当前页不是第一页才能切换到第一页
    if (pageTotal > 1 && pageCurrent != 1) {
        startIndex = 0;
        pageCurrent = 1;
        bindData(getPageSql());
    }
}

void DbPage::previous()
{
    //当前页不是第一页才能上一页
    if (pageCurrent > 1) {
        pageCurrent--;
        startIndex -= recordsPerpage;
        bindData(getPageSql());
    }
}

void DbPage::next()
{
    //当前页小于总页数才能下一页
    if (pageCurrent < pageTotal) {
        pageCurrent++;
        startIndex += recordsPerpage;
        bindData(getPageSql());
    }
}

void DbPage::last()
{
    //当前页不是末尾页才能切换到末尾页
    if (pageTotal > 1 && pageCurrent != pageTotal) {
        startIndex = (pageTotal - 1) * recordsPerpage;
        pageCurrent = pageTotal;
        bindData(getPageSql());
    }
}
