#include "argumentseditor.h"

#include <QDebug>
#include <QItemEditorFactory>

#include "apitracecall.h"

ArgumentsEditor::ArgumentsEditor(QWidget *parent)
    : QWidget(parent),
      m_model(new QStandardItemModel()),
      m_call(0)
{
    init();
}

ArgumentsEditor::~ArgumentsEditor()
{
}

void ArgumentsEditor::setCall(ApiTraceCall *call)
{
    if (m_call != call) {
        m_call = call;
        setupCall();
    }
}

ApiTraceCall * ArgumentsEditor::call() const
{
    return m_call;
}

void ArgumentsEditor::init()
{
    m_ui.setupUi(this);

    m_ui.argsTree->setModel(m_model);
#if 0
    const QItemEditorFactory *factory = QItemEditorFactory::defaultFactory();

    QItemEditorCreatorBase *apiBitmaskCreator =
        new QStandardItemEditorCreator<ApiBitmaskEditor>();

    factory->registerEditor(QMetaType::type("ApiBitmask") ,
                            apiBitmaskCreator);

    QItemEditorFactory::setDefaultFactory(factory);
#endif
}

void ArgumentsEditor::setupCall()
{
    m_model->clear();

    QStringList headers;
    headers.append(tr("Argument"));
    headers.append(tr("Value"));
    m_model->setColumnCount(2);
    m_model->setHorizontalHeaderLabels(headers);
    m_ui.argsTabWidget->removeTab(
        m_ui.argsTabWidget->indexOf(m_ui.shaderTab));

    if (!m_call)
        return;

    m_ui.callLabel->setText(m_call->name);
    QStandardItem *rootItem = m_model->invisibleRootItem();
    for (int i = 0; i < m_call->argNames.count(); ++i) {
        QString argName = m_call->argNames[i];
        QVariant val = m_call->argValues[i];
        QStandardItem *nameItem = new QStandardItem(argName);
        nameItem->setFlags(nameItem->flags() ^ Qt::ItemIsEditable);
        QList<QStandardItem*> topRow;
        topRow.append(nameItem);
        qDebug()<<"arg "<<argName<<", val = "<<val;

        if (val.canConvert<ApiArray>()) {
            ApiArray array = val.value<ApiArray>();
            QList<QVariant> vals = array.values();
            QVariant firstVal = vals.value(0);

            if (firstVal.userType() == QVariant::String) {
                m_ui.argsTabWidget->addTab(
                    m_ui.shaderTab, argName);
                setupShaderEditor(vals);
                delete nameItem;
                continue;
            } else {
                for (int i = 0; i < vals.count(); ++i) {
                    QList<QStandardItem*> row;

                    QStandardItem *idx = new QStandardItem();
                    idx->setFlags(idx->flags() ^ Qt::ItemIsEditable);
                    idx->setText(tr("%1)").arg(i));

                    QStandardItem *col = new QStandardItem();
                    col->setFlags(col->flags() | Qt::ItemIsEditable);
                    col->setData(vals[i], Qt::DisplayRole);
                    row.append(idx);
                    row.append(col);
                    nameItem->appendRow(row);
                };
            }
            qDebug()<<"\tarray first = "<<vals[0];
        } else  if (val.canConvert<ApiPointer>()) {
            ApiPointer ptr = val.value<ApiPointer>();
            QStandardItem *item = new QStandardItem();
            item->setFlags(item->flags() ^ Qt::ItemIsEditable);
            item->setText(ptr.toString());
            QIcon icon(":/resources/emblem-locked.png");
            item->setIcon(icon);
            item->setToolTip(tr("Argument is read-only"));
            topRow.append(item);
        } else if (val.canConvert<ApiBitmask>()) {
            ApiBitmask mask = val.value<ApiBitmask>();
            QStandardItem *item = new QStandardItem();
            item->setFlags(item->flags() ^ Qt::ItemIsEditable);
            item->setText(mask.toString());
            QIcon icon(":/resources/emblem-locked.png");
            item->setIcon(icon);
            item->setToolTip(tr("Argument is read-only"));
            topRow.append(item);
        } else if (val.canConvert<ApiStruct>()) {
            ApiStruct str = val.value<ApiStruct>();
            QStandardItem *item = new QStandardItem();
            item->setFlags(item->flags() ^ Qt::ItemIsEditable);
            item->setText(str.toString());
            QIcon icon(":/resources/emblem-locked.png");
            item->setIcon(icon);
            item->setToolTip(tr("Argument is read-only"));
            topRow.append(item);
        } else if (val.userType() == QVariant::ByteArray) {
            QByteArray ba = val.value<QByteArray>();
            QStandardItem *item = new QStandardItem();
            item->setFlags(item->flags() ^ Qt::ItemIsEditable);
            item->setText(
                tr("<binary data, size = %1 bytes>").arg(ba.size()));
            QIcon icon(":/resources/emblem-locked.png");
            item->setIcon(icon);
            item->setToolTip(tr("Argument is read-only"));
            topRow.append(item);
        } else {
            QStandardItem *item
                = new QStandardItem();
            item->setFlags(item->flags() | Qt::ItemIsEditable);
            item->setData(val, Qt::DisplayRole);
            topRow.append(item);
        }
        rootItem->appendRow(topRow);
    }
}

void ArgumentsEditor::setupShaderEditor(const QList<QVariant> &sources)
{
    
}

#include "argumentseditor.moc"
