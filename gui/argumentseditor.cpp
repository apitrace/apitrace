#include "argumentseditor.h"

#include "apitracecall.h"

#include <QDebug>
#include <QDoubleSpinBox>
#include <QItemEditorFactory>
#include <QSpinBox>

#include <limits.h>
#include <float.h>


static bool
isVariantEditable(const QVariant &var)
{
    if (var.canConvert<ApiArray>()) {
        ApiArray array = var.value<ApiArray>();
        QVector<QVariant> vals = array.values();
        if (vals.isEmpty())
            return false;
        else
            return isVariantEditable(vals.first());
    }
    switch (var.userType()) {
    case QVariant::Bool:
    case QVariant::Int:
    case QVariant::UInt:
    case QVariant::LongLong:
    case QVariant::ULongLong:
    case QMetaType::Float:
    case QVariant::Double:
    case QVariant::String:
        return true;
    default:
        return false;
    }
}

static bool
isVariantStringArray(const QVariant &var)
{
    if (var.isNull() || var.userType() != QMetaType::type("ApiArray"))
        return false;

    ApiArray array = var.value<ApiArray>();
    QVector<QVariant> origValues = array.values();
    if (origValues.isEmpty() ||
        origValues.first().userType() != QVariant::String)
        return false;

    return true;
}



void setArgumentsItemEditorFactory ()
{
    QItemEditorCreatorBase *booleanComboBoxEditorCreator =
    new QStandardItemEditorCreator<BooleanComboBoxEditorCreator>();
    QItemEditorCreatorBase *uIntEditorCreator =
    new QStandardItemEditorCreator<UIntEditorCreator>();
    QItemEditorCreatorBase *intEditorCreator =
    new QStandardItemEditorCreator<IntEditorCreator>();
    QItemEditorCreatorBase *uLongLongEditorCreator =
    new QStandardItemEditorCreator<ULongLongEditorCreator>();
    QItemEditorCreatorBase *longLongEditorCreator =
    new QStandardItemEditorCreator<LongLongEditorCreator>();
    QItemEditorCreatorBase *pixmapEditorCreator =
    new QStandardItemEditorCreator<PixmapEditorCreator>();
    QItemEditorCreatorBase *floatEditorCreator =
    new QStandardItemEditorCreator<FloatEditorCreator>();
    QItemEditorCreatorBase *doubleEditorCreator =
    new QStandardItemEditorCreator<DoubleEditorCreator>();
    QItemEditorCreatorBase *stringEditorCreator =
    new QStandardItemEditorCreator<StringEditorCreator>();
    QItemEditorCreatorBase *invalidEditorCreator =
    new QStandardItemEditorCreator<InvalidEditorCreator>();

    QItemEditorFactory *factory =
        new QItemEditorFactory();

    QVariant::Type typeFloat = static_cast<QVariant::Type> (qMetaTypeId<float>());

    factory->registerEditor(QVariant::Bool, booleanComboBoxEditorCreator);
    factory->registerEditor(QVariant::UInt, uIntEditorCreator);
    factory->registerEditor(QVariant::Int, intEditorCreator);
    factory->registerEditor(QVariant::ULongLong, uLongLongEditorCreator);
    factory->registerEditor(QVariant::LongLong, longLongEditorCreator);
    factory->registerEditor(QVariant::Pixmap, pixmapEditorCreator);
    factory->registerEditor(typeFloat, floatEditorCreator);
    factory->registerEditor(QVariant::Double, doubleEditorCreator);
    factory->registerEditor(QVariant::String, stringEditorCreator);
    factory->registerEditor(QVariant::Invalid, invalidEditorCreator);

    QItemEditorFactory::setDefaultFactory(factory);
}

BooleanComboBox::BooleanComboBox(QWidget *parent)
    : QComboBox(parent)
{
    addItem(tr("False"));
    addItem(tr("True"));
}

void BooleanComboBox::setValue(bool value)
{
    setCurrentIndex(value ? 1 : 0);
}

bool BooleanComboBox::value() const
{
    return (currentIndex() == 1);
}

ArgumentsEditor::ArgumentsEditor(QWidget *parent)
    : QDialog(parent),
      m_model(new QStandardItemModel()),
      m_call(0)
{
    init();
}

ArgumentsEditor::~ArgumentsEditor()
{
    delete m_model;
}

void ArgumentsEditor::setCall(ApiTraceCall *call)
{
    m_call = call;
    setupCall();
}

ApiTraceCall * ArgumentsEditor::call() const
{
    return m_call;
}

void ArgumentsEditor::init()
{
    m_ui.setupUi(this);

    connect(m_ui.selectStringCB, SIGNAL(currentIndexChanged(int)),
            SLOT(currentSourceChanged(int)));
    connect(m_ui.glslEdit, SIGNAL(textChanged()),
            SLOT(sourceChanged()));
    connect(m_ui.revertButton, SIGNAL(clicked()),
            SLOT(revert()));

    setArgumentsItemEditorFactory ();

    m_ui.argsTree->setModel(m_model);

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

    m_ui.callLabel->setText(m_call->name());
    QStandardItem *rootItem = m_model->invisibleRootItem();
    for (int i = 0; i < m_call->argNames().count(); ++i) {
        QString argName = m_call->argNames()[i];
        QVariant val = m_call->arguments()[i];
        QStandardItem *nameItem = new QStandardItem(argName);
        nameItem->setFlags(nameItem->flags() ^ Qt::ItemIsEditable);
        QList<QStandardItem*> topRow;
        topRow.append(nameItem);

        if (val.canConvert<ApiArray>()) {
            ApiArray array = val.value<ApiArray>();
            QVector<QVariant> vals = array.values();

            QVariant firstVal = vals.value(0);
            if (firstVal.userType() == QVariant::String) {
                m_ui.argsTabWidget->addTab(
                    m_ui.shaderTab, argName);
                setupShaderEditor(vals);
                delete nameItem;
                continue;
            } else if (isVariantEditable(firstVal)) {
                for (int i = 0; i < vals.count(); ++i) {
                    QList<QStandardItem*> row;

                    QStandardItem *idx = new QStandardItem();
                    idx->setFlags(idx->flags() ^ Qt::ItemIsEditable);
                    idx->setText(tr("%1)").arg(i));

                    QStandardItem *col = new QStandardItem();
                    col->setFlags(col->flags() | Qt::ItemIsEditable);
                    col->setData(vals[i], Qt::EditRole);
                    row.append(idx);
                    row.append(col);
                    nameItem->appendRow(row);
                }
            } else {
                qDebug()<<"\tUnsupported array = "<<firstVal;
                delete nameItem;
                continue;
            }
        } else  if (val.canConvert<ApiPointer>()) {
            ApiPointer ptr = val.value<ApiPointer>();
            QStandardItem *item = new QStandardItem();
            item->setFlags(item->flags() ^ Qt::ItemIsEditable);
            item->setText(ptr.toString());
            QIcon icon(":/resources/emblem-locked.png");
            item->setIcon(icon);
            item->setToolTip(tr("Argument is read-only"));
            topRow.append(item);
        } else if (val.canConvert<ApiEnum>()) {
            ApiEnum en = val.value<ApiEnum>();
            QStandardItem *item = new QStandardItem();
            item->setFlags(item->flags() ^ Qt::ItemIsEditable);
            item->setText(en.toString());
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

            if (isVariantEditable(val)) {
                item->setFlags(item->flags() | Qt::ItemIsEditable);
            } else {
                QIcon icon(":/resources/emblem-locked.png");
                item->setIcon(icon);
                item->setFlags(item->flags() ^ Qt::ItemIsEditable);
                item->setToolTip(tr("Argument is read-only"));
            }
            item->setData(val, Qt::EditRole);
            topRow.append(item);
        }
        rootItem->appendRow(topRow);
    }
}

void ArgumentsEditor::setupShaderEditor(const QVector<QVariant> &sources)
{
    m_ui.selectStringCB->clear();
    m_ui.glslEdit->clear();
    for (int i = 0; i < sources.count(); ++i) {
        m_ui.selectStringCB->addItem(
            tr("Shader string: %1").arg(i),
            sources[i]);
    }
    m_ui.selectStringCB->setCurrentIndex(0);
}

void ArgumentsEditor::currentSourceChanged(int idx)
{
    QVariant source = m_ui.selectStringCB->itemData(idx);
    QString str = source.toString();
    m_ui.glslEdit->setPlainText(source.toString());
    m_ui.lengthLabel->setText(
        tr("%1").arg(str.length()));
}

void ArgumentsEditor::sourceChanged()
{
    QString str = m_ui.glslEdit->toPlainText();
    m_ui.lengthLabel->setText(
        tr("%1").arg(str.length()));

    m_ui.selectStringCB->setItemData(
        m_ui.selectStringCB->currentIndex(),
        str);
}

void ArgumentsEditor::accept()
{
    QStringList argNames = m_call->argNames();
    QVector<QVariant> originalValues = m_call->arguments();
    QVector<QVariant> newValues;
    bool changed = false;
    for (int i = 0; i < argNames.count(); ++i) {
        bool valChanged = false;
        QString argName = argNames[i];
        QVariant argValue = originalValues[i];
        QVariant editorValue = valueForName(argName, argValue, &valChanged);

        newValues.append(editorValue);
#if 0
        qDebug()<<"Arg = "<<argName;
        qDebug()<<"\toriginal = "<<argValue;
        qDebug()<<"\teditor   = "<<editorValue;
        qDebug()<<"\tchanged  = "<<valChanged;
#endif
        if (valChanged)
            changed = true;
    }
    if (changed)
        m_call->setEditedValues(newValues);
    QDialog::accept();
}

QVariant ArgumentsEditor::valueForName(const QString &name,
                                       const QVariant &originalValue,
                                       bool *changed) const
{
    QVariant val;

    *changed = false;

    //Handle string arrays specially
    if (isVariantStringArray(originalValue)) {
        ApiArray array = originalValue.value<ApiArray>();
        return arrayFromEditor(array, changed);
    }

    if (!isVariantEditable(originalValue)) {
        return originalValue;
    }

    for (int topRow = 0; topRow < m_model->rowCount(); ++topRow) {
        QModelIndex nameIdx = m_model->index(topRow, 0, QModelIndex());
        QString argName = nameIdx.data().toString();
        /* we display shaders in a separate widget so
         * the ordering might be different */
        if (argName == name) {
            if (originalValue.userType() == QMetaType::type("ApiArray")) {
                ApiArray array = originalValue.value<ApiArray>();
                val = arrayFromIndex(nameIdx, array, changed);
            } else {
                QModelIndex valIdx = m_model->index(topRow, 1, QModelIndex());
                val = valIdx.data();
                if (val != originalValue)
                    *changed = true;
            }
        }
    }
    return val;
}

QVariant ArgumentsEditor::arrayFromIndex(const QModelIndex &parentIndex,
                                         const ApiArray &origArray,
                                         bool *changed) const
{
    QVector<QVariant> origValues = origArray.values();

    *changed = false;

    if (origValues.isEmpty())
        return QVariant::fromValue(ApiArray());

    QVector<QVariant> lst;
    for (int i = 0; i < origValues.count(); ++i) {
        QModelIndex valIdx = m_model->index(i, 1, parentIndex);
        QVariant var = valIdx.data();
        QVariant origValue = origValues[i];
        if (var != origValue)
            *changed = true;
        //qDebug()<<"\t\tarray "<<i<<") "<<var;
        lst.append(var);
    }
    return QVariant::fromValue(ApiArray(lst));
}

QVariant ArgumentsEditor::arrayFromEditor(const ApiArray &origArray,
                                          bool *changed) const
{
    QVector<QVariant> vals;
    QVector<QVariant> origValues = origArray.values();

    Q_ASSERT(isVariantStringArray(QVariant::fromValue(origArray)));
    *changed = false;
    //shaders
    for (int i = 0; i < m_ui.selectStringCB->count(); ++i) {
        QVariant val = m_ui.selectStringCB->itemData(i);
        QVariant origValue = origValues[i];
        if (origValue != val)
            *changed = true;
        vals.append(val);
    }
    return QVariant::fromValue(ApiArray(vals));
}

void ArgumentsEditor::revert()
{
    m_call->revert();
    setupCall();
}

#include "argumentseditor.moc"
