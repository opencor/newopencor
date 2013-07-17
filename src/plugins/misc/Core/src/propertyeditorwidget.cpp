//==============================================================================
// Property editor widget
//==============================================================================

#include "propertyeditorwidget.h"

//==============================================================================

#include <float.h>

//==============================================================================

#include <QHeaderView>
#include <QKeyEvent>
#include <QScrollBar>
#include <QSettings>
#include <QStandardItem>
#include <QVariant>

//==============================================================================

namespace OpenCOR {
namespace Core {

//==============================================================================

TextEditorWidget::TextEditorWidget(QWidget *pParent) :
    QLineEdit(pParent)
{
#ifdef Q_OS_MAC
    setAttribute(Qt::WA_MacShowFocusRect, 0);
    // Note: the above removes the focus border since it messes up the look of
    //       our editor
#endif
    setFrame(QFrame::NoFrame);
}

//==============================================================================

void TextEditorWidget::keyPressEvent(QKeyEvent *pEvent)
{
    // Check some key combinations

    if (   !(pEvent->modifiers() & Qt::ShiftModifier)
        && !(pEvent->modifiers() & Qt::ControlModifier)
        && !(pEvent->modifiers() & Qt::AltModifier)
        && !(pEvent->modifiers() & Qt::MetaModifier)) {
        // None of the modifiers is selected

        if (pEvent->key() == Qt::Key_Up) {
            // The user wants to go to the previous property

            emit goToPreviousPropertyRequested();

            // Accept the event

            pEvent->accept();
        } else if (pEvent->key() == Qt::Key_Down) {
            // The user wants to go to the next property

            emit goToNextPropertyRequested();

            // Accept the event

            pEvent->accept();
        } else {
            // Default handling of the event

            QLineEdit::keyPressEvent(pEvent);
        }
    } else {
        // Default handling of the event

        QLineEdit::keyPressEvent(pEvent);
    }
}

//==============================================================================

IntegerEditorWidget::IntegerEditorWidget(QWidget *pParent) :
    TextEditorWidget(pParent)
{
    // Set a validator which accepts any integer

    setValidator(new QRegExpValidator(QRegExp("^[+-]?[0-9]*([eE][+-]?[0-9]+)?$"), this));
}

//==============================================================================

DoubleEditorWidget::DoubleEditorWidget(QWidget *pParent) :
    TextEditorWidget(pParent)
{
    // Set a validator which accepts any double

    setValidator(new QRegExpValidator(QRegExp("^[+-]?[0-9]*\\.?[0-9]+([eE][+-]?[0-9]+)?$"), this));
}

//==============================================================================

ListEditorWidget::ListEditorWidget(QWidget *pParent) :
    QComboBox(pParent)
{
    // Customise the focus policy
    // Note #1: this is not strictly necessary on Windows and Linux, but
    //          definitely is on OS X since otherwise we wouldn't be able to
    //          change the value of a property by simply double-clicking on it.
    //          Indeed, with Qt 5, the focus policy of a QComboBox has changed
    //          to be more native, but that breaks things for us, so...
    // Note #2: see https://bugreports.qt-project.org/browse/QTBUG-29398#comment-208467
    //          for more information...

    setFocusPolicy(Qt::WheelFocus);
}

//==============================================================================

void ListEditorWidget::keyPressEvent(QKeyEvent *pEvent)
{
    // Check some key combinations

    if (   !(pEvent->modifiers() & Qt::ShiftModifier)
        && !(pEvent->modifiers() & Qt::ControlModifier)
        && !(pEvent->modifiers() & Qt::AltModifier)
        && !(pEvent->modifiers() & Qt::MetaModifier)) {
        // None of the modifiers is selected

        if (pEvent->key() == Qt::Key_Up) {
            // The user wants to go to the previous property

            emit goToPreviousPropertyRequested();

            // Accept the event

            pEvent->accept();
        } else if (pEvent->key() == Qt::Key_Down) {
            // The user wants to go to the next property

            emit goToNextPropertyRequested();

            // Accept the event

            pEvent->accept();
        } else {
            // Default handling of the event

            QComboBox::keyPressEvent(pEvent);
        }
    } else {
        // Default handling of the event

        QComboBox::keyPressEvent(pEvent);
    }
}

//==============================================================================

void ListEditorWidget::mouseDoubleClickEvent(QMouseEvent *pEvent)
{
    // Make sure that we have at least one item

    if (!count())
        return;

    // We want to go to the next item in the list (and go back to the first one
    // if we are at the end of the list), so determine the new current index

    int newCurrentIndex = currentIndex();

    do {
        ++newCurrentIndex;

        if (newCurrentIndex == count())
            newCurrentIndex = 0;
    } while (itemText(newCurrentIndex).isEmpty());

    // Set the new current index

    setCurrentIndex(newCurrentIndex);

    // Accept the event

    pEvent->accept();
}

//==============================================================================

void ListEditorWidget::mousePressEvent(QMouseEvent *pEvent)
{
    // Check whether the user clicked on the arrow and, if so, allow the default
    // handling of the event (so that the list of items gets displayed) while do
    // nothing if the user clicked somewhere else (this to so that if the user
    // double clicks on the widget, then we can select the next item)
    // Note: we would normally call style()->hitTestComplexControl() and, if it
    //       returns QStyle::SC_ComboBoxArrow, then allow the default handling
    //       of the event, but if this works fine on Windows and Linux, it just
    //       doesn't work on OS X. Indeed, no matter where we are over the
    //       widget, style()->hitTestComplexControl() will always (and as
    //       expected; [QtSources]/qtbase/src/widgets/styles/qmacstyle_mac.mm)
    //       return QStyle::SC_ComboBoxArrow. So, to get the behaviour we are
    //       after, we do what is done in
    //       [QtSources]/qtbase/src/widgets/styles/qcommonstyle.cpp...

    // Retrieve our style option

    QStyleOptionComboBox styleOption;

    initStyleOption(&styleOption);

    // Go through the different sub controls (starting with the arrow) and
    // determine over which one we are

    uint subControl = QStyle::SC_ComboBoxArrow;
    QRect subControlRect;

    while (subControl) {
        // Retrieve the sub control's region

        subControlRect = style()->subControlRect(QStyle::CC_ComboBox,
                                                 &styleOption,
                                                 QStyle::SubControl(subControl),
                                                 this);

        // Check whether the sub control exists (i.e. its region is valid) and,
        // if so, whether we ore over it

        if (subControlRect.isValid() && subControlRect.contains(pEvent->pos()))
            // The sub control exists and we are over it, so...

            break;

        // Either the sub control doesn't exist or we are not over it, so try
        // the next sub control

        subControl >>= 1;
    }

    // Check whether the 'current' sub control is the arrow we are after

    if (QStyle::SubControl(subControl) == QStyle::SC_ComboBoxArrow)
        // It is the arrow we are after, so...

        QComboBox::mousePressEvent(pEvent);
    else
        // Accept the event

        pEvent->accept();
}

//==============================================================================

QWidget * PropertyItemDelegate::createEditor(QWidget *pParent,
                                             const QStyleOptionViewItem &pOption,
                                             const QModelIndex &pIndex) const
{
    Q_UNUSED(pOption);

    // Create and return an editor for our item, based on its type

    QWidget *editor;
    Property *property = static_cast<PropertyItem *>(qobject_cast<const QStandardItemModel *>(pIndex.model())->itemFromIndex(pIndex))->owner();

    switch (property->type()) {
    case Property::String:
        editor = new TextEditorWidget(pParent);

        break;
    case Property::Integer:
        editor = new IntegerEditorWidget(pParent);

        break;
    case Property::Double:
        editor = new DoubleEditorWidget(pParent);

        break;
    case Property::List: {
        ListEditorWidget *listEditor = new ListEditorWidget(pParent);

        // Add the value items to the list, keeping in mind separators

        foreach (const QString &valueItem, property->listValue())
            if (valueItem.isEmpty())
                listEditor->insertSeparator(listEditor->count());
            else
                listEditor->addItem(valueItem);

        editor = listEditor;

        // Propagate the signal telling us about the list property value having
        // changed

        connect(listEditor, SIGNAL(currentIndexChanged(const QString &)),
                this, SIGNAL(listPropertyChanged(const QString &)));

        break;
    }
    default:
        // Property::Section

        return 0;
    }

    // Propagate a few signals

    connect(editor, SIGNAL(goToPreviousPropertyRequested()),
            this, SIGNAL(goToPreviousPropertyRequested()));
    connect(editor, SIGNAL(goToNextPropertyRequested()),
            this, SIGNAL(goToNextPropertyRequested()));

    // Let people know that there is a new editor

    emit openEditor(editor);

    // Return the editor

    return editor;
}

//==============================================================================

bool PropertyItemDelegate::eventFilter(QObject *pObject, QEvent *pEvent)
{
    // We want to handle key events ourselves, so...

    if (pEvent->type() == QEvent::KeyPress)
        return false;
    else
        return QStyledItemDelegate::eventFilter(pObject, pEvent);
}

//==============================================================================

void PropertyItemDelegate::paint(QPainter *pPainter,
                                 const QStyleOptionViewItem &pOption,
                                 const QModelIndex &pIndex) const
{
    // Paint the item as normal, except if it is a section item

    PropertyItem *propertyItem = static_cast<PropertyItem *>(qobject_cast<const QStandardItemModel *>(pIndex.model())->itemFromIndex(pIndex));

    QStyleOptionViewItemV4 option(pOption);

    initStyleOption(&option, pIndex);

    if (propertyItem->owner()->type() == Property::Section) {
        // Make our section item bold

        option.font.setBold(true);
    }

    QStyledItemDelegate::paint(pPainter, option, pIndex);
}

//==============================================================================

PropertyItem::PropertyItem(Property *pOwner) :
    QStandardItem(),
    mOwner(pOwner)
{
    // By default, the property item is not editable

    setEditable(false);
}

//==============================================================================

Property * PropertyItem::owner() const
{
    // Return our owner

    return mOwner;
}

//==============================================================================

Property::Property(const Type &pType, PropertyEditorWidget *pParent) :
    QObject(pParent),
    mOwner(pParent),
    mType(pType),
    mId(QString()),
    mName(new PropertyItem(this)),
    mValue(new PropertyItem(this)),
    mUnit(new PropertyItem(this)),
    mListValue(QStringList()),
    mEmptyListValue(QString("???")),
    mExtraInfo(QString()),
    mParentProperty(0),
    mProperties(QList<Property *>())
{
    // Note: mName, mValue and mUnit get owned by our property editor widget, so
    //       no need to delete them afterwards...
}

//==============================================================================

Property::Type Property::type() const
{
    // Return our type

    return mType;
}

//==============================================================================

QStandardItem * Property::parent() const
{
    // Return our parent

    return mName->parent();
}

//==============================================================================

int Property::row() const
{
    // Return our row

    return mName->row();
}

//==============================================================================

Property * Property::parentProperty() const
{
    // Return our parent property

    return mParentProperty;
}

//==============================================================================

void Property::setParentProperty(Property *pProperty)
{
    // Set our parent property

    mParentProperty = pProperty;
}

//==============================================================================

QList<Property *> Property::properties() const
{
    // Return our properties

    return mProperties;
}

//==============================================================================

void Property::add(Property *pProperty)
{
    // Add the property to ourselves

    mProperties << pProperty;

    mName->appendRow(pProperty->items());

    // Let the property that we are its parent

    pProperty->setParentProperty(this);
}

//==============================================================================

void Property::addTo(QStandardItem *pParent)
{
    // Add ourselves to the given parent

    pParent->appendRow(items());
}

//==============================================================================

QList<QStandardItem *> Property::items() const
{
    // Return our items as a list

    return QList<QStandardItem *>() << mName << mValue << mUnit;
}

//==============================================================================

QModelIndex Property::index() const
{
    // Return our index

    return mName->index();
}

//==============================================================================

bool Property::hasIndex(const QModelIndex &pIndex) const
{
    // Return whether the given is that our name, value or unit item

    return (mName->index()  == pIndex) || (mValue->index() == pIndex) || (mUnit->index()  == pIndex);
}

//==============================================================================

QString Property::id() const
{
    // Return our id

    return mId;
}

//==============================================================================

void Property::setId(const QString &pId)
{
    // Set our id

    mId = pId;
}

//==============================================================================

bool Property::isCheckable() const
{
    // Return whether our name item is checkable

    return mName->isCheckable();
}

//==============================================================================

void Property::setCheckable(const bool &pCheckable)
{
    // Make our name item (un)checkable

    mName->setCheckable(pCheckable);
}

//==============================================================================

bool Property::isChecked() const
{
    // Return whether our name item is checked

    return mName->checkState() == Qt::Checked;
}

//==============================================================================

void Property::setChecked(const bool &pChecked)
{
    // Make our name item (un)checked

    mName->setCheckState(pChecked?Qt::Checked:Qt::Unchecked);
}

//==============================================================================

bool Property::isEditable() const
{
    // Return whether our value item is editable

    return mValue->isEditable();
}

//==============================================================================

void Property::setEditable(const bool &pEditable)
{
    // Make our value item (non-)editable

    mValue->setEditable(pEditable);
}

//==============================================================================

QString Property::name() const
{
    // Return our name

    return mName->text();
}

//==============================================================================

void Property::setName(const QString &pName)
{
    // Set our name

    if (pName.compare(mName->text())) {
        mName->setText(pName);

        updateToolTip();
    }
}

//==============================================================================

QIcon Property::icon() const
{
    // Return our icon

    return mName->icon();
}

//==============================================================================

void Property::setIcon(const QIcon &pIcon)
{
    // Set our icon

    mName->setIcon(pIcon);
}

//==============================================================================

int Property::integerValue() const
{
    // Return our value as an integer, if it is of that type

    if (mType == Integer)
        return mValue->text().toInt();
    else
        // Our value is not of integer type, so...

        return 0;
}

//==============================================================================

void Property::setIntegerValue(const int &pIntegerValue)
{
    // Set our value, should it be of integer type

    if (mType == Integer)
        setValue(QString::number(pIntegerValue));
}

//==============================================================================

double Property::doubleValue() const
{
    // Return our value as a double, if it is of that type

    if (mType == Double)
        return mValue->text().toDouble();
    else
        // Our value is not of double type, so...

        return 0.0;
}

//==============================================================================

void Property::setDoubleValue(const double &pDoubleValue,
                              const bool &pEmitSignal)
{
    // Set our value, should it be of double type

    if (mType == Double)
        setValue(QString::number(pDoubleValue, 'g', 15), false, pEmitSignal);
        // Note: we want as much precision as possible, hence we use 15 (see
        //       http://en.wikipedia.org/wiki/Double_precision)...
}

//==============================================================================

QString Property::value() const
{
    // Return our value

    return mValue->text();
}

//==============================================================================

void Property::setValue(const QString &pValue, const bool &pForce,
                        const bool &pEmitSignal)
{
    // Set our value

    if (pValue.compare(mValue->text()) || pForce) {
        QString oldValue = mValue->text();

        mValue->setText(pValue);

        updateToolTip();

        // Let people know if we have a new value

        if (pEmitSignal && pValue.compare(oldValue))
            emit valueChanged(oldValue, pValue);
    }
}

//==============================================================================

QStringList Property::listValue() const
{
    // Return our list value, if any

    return (mType == List)?mListValue:QStringList();
}

//==============================================================================

void Property::setListValue(const QStringList &pListValue)
{
    // Make sure that there would be a point in setting the list value

    if (mType != List)
        return;

    // Clean up the list value we were given:
    //  - Remove leading empty items
    //  - Add items, making sure that only one empty item (i.e. separator) can
    //    be used at once
    //  - Remove the trailing empty item, if any

    QStringList listValue = QStringList();
    int i = 0;
    int iMax = pListValue.count();

    for (; i < iMax; ++i)
        if (!pListValue.at(i).isEmpty())
            break;

    bool previousItemIsSeparator = false;

    for (; i < iMax; ++i) {
        QString listValueItem = pListValue.at(i);

        if (!listValueItem.isEmpty()) {
            listValue << listValueItem;

            previousItemIsSeparator = false;
        } else if (!previousItemIsSeparator) {
            listValue << listValueItem;

            previousItemIsSeparator = true;
        }
    }

    if (!listValue.isEmpty() && listValue.last().isEmpty())
        listValue.removeLast();

    // Set our list value, if appropriate

    if (listValue != mListValue) {
        mListValue = listValue;

        // Update our value using the first item of our new list, if it isn't
        // empty, otherwise use our empty list value

        setValue(mListValue.isEmpty()?mEmptyListValue:mListValue.first());
    }
}

//==============================================================================

QString Property::emptyListValue() const
{
    // Return our empty list value

    return mEmptyListValue;
}

//==============================================================================

void Property::setEmptyListValue(const QString &pEmptyListValue)
{
    // Set our empty list value, if appropriate

    if ((mType == List) && pEmptyListValue.compare(mEmptyListValue)) {
        mEmptyListValue = pEmptyListValue;

        // Keep our current value, if the list is not empty, otherwise update it
        // with our new empty list value

        setValue(mListValue.isEmpty()?mEmptyListValue:mValue->text());
    }
}

//==============================================================================

QString Property::unit() const
{
    // Return our unit

    return mUnit->text();
}

//==============================================================================

void Property::setUnit(const QString &pUnit)
{
    // Set our unit, if it's not of section type

    if ((mType != Section) && pUnit.compare(mUnit->text())) {
        mUnit->setText(pUnit);

        updateToolTip();
    }
}

//==============================================================================

QString Property::extraInfo() const
{
    // Return our extra info

    return mExtraInfo;
}

//==============================================================================

void Property::setExtraInfo(const QString &pExtraInfo)
{
    // Set our extra info

    if (pExtraInfo.compare(mExtraInfo)) {
        mExtraInfo = pExtraInfo;

        updateToolTip();
    }
}

//==============================================================================

bool Property::isVisible() const
{
    // Return our visibility

    return mOwner->isRowHidden(mName->row(),
                               mName->parent()?
                                   mName->parent()->index():
                                   qobject_cast<QStandardItemModel *>(mOwner->model())->invisibleRootItem()->index());
}

//==============================================================================

void Property::setVisible(const bool &pVisible)
{
    // Set our visibility

    mOwner->setRowHidden(mName->row(),
                         mName->parent()?
                             mName->parent()->index():
                             qobject_cast<QStandardItemModel *>(mOwner->model())->invisibleRootItem()->index(),
                         !pVisible);

    // Let people know that our visibility has changed

    emit visibilityChanged(pVisible);
}

//==============================================================================

void Property::select() const
{
    // Have our owner select us (i.e. our value)

    mOwner->setCurrentIndex(mValue->index());
}

//==============================================================================

void Property::edit() const
{
    // Have our owner edit our value

    if (mValue->isEditable())
        mOwner->edit(mValue->index());
}

//==============================================================================

void Property::updateToolTip()
{
    // Update our tool tip based on the values of our name, value and unit

    QString toolTip = mName->text();

    if (mType != Section) {
        toolTip += QObject::tr(": ");

        if (mValue->text().isEmpty())
            toolTip += "???";
        else
            toolTip += mValue->text();

        if (!mUnit->text().isEmpty())
            toolTip += " "+mUnit->text();
    }

    if (!mExtraInfo.isEmpty())
        toolTip += " ("+mExtraInfo+")";

    mName->setToolTip(toolTip);

    if (mType != Section) {
        mValue->setToolTip(toolTip);
        mUnit->setToolTip(toolTip);
    }
}

//==============================================================================

PropertyEditorWidgetGuiStateProperty::PropertyEditorWidgetGuiStateProperty(Property *pProperty,
                                                                           const bool &pHidden,
                                                                           const bool &pExpanded,
                                                                           const QString &pValue) :
    mProperty(pProperty),
    mHidden(pHidden),
    mExpanded(pExpanded),
    mValue(pValue)
{
}

//==============================================================================

Property * PropertyEditorWidgetGuiStateProperty::property() const
{
    // Return our property

    return mProperty;
}

//==============================================================================

bool PropertyEditorWidgetGuiStateProperty::isHidden() const
{
    // Return whether our property is hidden

    return mHidden;
}

//==============================================================================

bool PropertyEditorWidgetGuiStateProperty::isExpanded() const
{
    // Return whether our property is expanded

    return mExpanded;
}

//==============================================================================

QString PropertyEditorWidgetGuiStateProperty::value() const
{
    // Return our property's value

    return mValue;
}

//==============================================================================

PropertyEditorWidgetGuiState::PropertyEditorWidgetGuiState(const QModelIndex &pCurrentProperty) :
    mProperties(PropertyEditorWidgetGuiStateProperties()),
    mCurrentProperty(pCurrentProperty)
{
}

//==============================================================================

PropertyEditorWidgetGuiState::~PropertyEditorWidgetGuiState()
{
    // Delete some internal objects

    foreach (PropertyEditorWidgetGuiStateProperty *property, mProperties)
        delete property;
}

//==============================================================================

PropertyEditorWidgetGuiStateProperties PropertyEditorWidgetGuiState::properties() const
{
    // Return our properties

    return mProperties;
}

//==============================================================================

void PropertyEditorWidgetGuiState::addProperty(PropertyEditorWidgetGuiStateProperty *pProperty)
{
    // Add the property to our list

    mProperties << pProperty;
}

//==============================================================================

QModelIndex PropertyEditorWidgetGuiState::currentProperty() const
{
    // Return our current property

    return mCurrentProperty;
}

//==============================================================================

void PropertyEditorWidget::constructor(const bool &pAutoUpdateHeight)
{
    // Some initialisations

    mAutoUpdateHeight = pAutoUpdateHeight;

    mProperties = Properties();

    mProperty = 0;
    mPropertyEditor = 0;

    mRightClicking = false;

    // Customise ourselves

    setRootIsDecorated(false);

    // Create and set our data model

    mModel = new QStandardItemModel(this);

    setModel(mModel);

    // Create our item delegate and set it, after making sure that we handle a
    // few of its signals

    PropertyItemDelegate *propertyItemDelegate = new PropertyItemDelegate();

    connect(selectionModel(), SIGNAL(currentChanged(const QModelIndex &, const QModelIndex &)),
            this, SLOT(editorClosed()));

    connect(propertyItemDelegate, SIGNAL(openEditor(QWidget *)),
            this, SLOT(editorOpened(QWidget *)));
    connect(propertyItemDelegate, SIGNAL(closeEditor(QWidget *, QAbstractItemDelegate::EndEditHint)),
            this, SLOT(editorClosed()));

    connect(propertyItemDelegate, SIGNAL(goToPreviousPropertyRequested()),
            this, SLOT(goToPreviousProperty()));
    connect(propertyItemDelegate, SIGNAL(goToNextPropertyRequested()),
            this, SLOT(goToNextProperty()));

    connect(propertyItemDelegate, SIGNAL(listPropertyChanged(const QString &)),
            this, SIGNAL(listPropertyChanged(const QString &)));

    setItemDelegate(propertyItemDelegate);

    // Resize our height in case some data has changed or one of the properties
    // gets expanded/collapsed

    connect(mModel, SIGNAL(itemChanged(QStandardItem *)),
            this, SLOT(updateHeight()));

    connect(this, SIGNAL(collapsed(const QModelIndex &)),
            this, SLOT(updateHeight()));
    connect(this, SIGNAL(expanded(const QModelIndex &)),
            this, SLOT(updateHeight()));

    // Further customise ourselves

    setSelectionMode(QAbstractItemView::SingleSelection);

    header()->setSectionsMovable(false);

    // Retranslate ourselves

    retranslateUi();

    // Initialise our height

    updateHeight();
}

//==============================================================================

PropertyEditorWidget::PropertyEditorWidget(const bool &pAutoUpdateHeight,
                                           QWidget *pParent) :
    TreeViewWidget(pParent)
{
    // Construct our object

    constructor(pAutoUpdateHeight);
}

//==============================================================================

PropertyEditorWidget::PropertyEditorWidget(QWidget *pParent) :
    TreeViewWidget(pParent)
{
    // Construct our object

    constructor();
}

//==============================================================================

PropertyEditorWidget::~PropertyEditorWidget()
{
    // Delete some internal objects

    foreach (Property *property, mProperties)
        delete property;
}

//==============================================================================

void PropertyEditorWidget::retranslateEmptyListProperties(QStandardItem *pItem)
{
    // Retranslate the current item, should it be an empty list

    Property *prop = property(pItem->index());

    if (prop && (prop->type() == Property::List) && prop->listValue().isEmpty())
        prop->setValue(prop->emptyListValue());

    // Retranslate the current item's children, if any

    for (int i = 0, iMax = pItem->rowCount(); i < iMax; ++i)
        retranslateEmptyListProperties(pItem->child(i));
}

//==============================================================================

void PropertyEditorWidget::retranslateUi()
{
    // Retranslate our header labels

    mModel->setHorizontalHeaderLabels(QStringList() << tr("Property")
                                                    << tr("Value")
                                                    << tr("Unit"));

    // 'Retranslate' the value of all empty list properties

    retranslateEmptyListProperties(mModel->invisibleRootItem());
}

//==============================================================================

static const QString SettingsColumnWidth = "ColumnWidth%1";

//==============================================================================

void PropertyEditorWidget::loadSettings(QSettings *pSettings)
{
    // Retrieve the width of each column

    for (int i = 0, iMax = header()->count(); i < iMax; ++i)
        setColumnWidth(i, pSettings->value(SettingsColumnWidth.arg(i),
                                           columnWidth(i)).toInt());
}

//==============================================================================

void PropertyEditorWidget::saveSettings(QSettings *pSettings) const
{
    // Keep track of the width of each column

    for (int i = 0, iMax = header()->count(); i < iMax; ++i)
        pSettings->setValue(SettingsColumnWidth.arg(i), columnWidth(i));
}

//==============================================================================

int PropertyEditorWidget::childrenRowHeight(const QStandardItem *pItem) const
{
    // Return the total height of the given index's children

    int res = 0;

    if (pItem->hasChildren())
        for (int i = 0, iMax = pItem->rowCount(); i < iMax; ++i) {
            QStandardItem *childItem = pItem->child(i, 0);
            int childIndexHeight = rowHeight(childItem->index());

            if (childIndexHeight)
                res += childIndexHeight+childrenRowHeight(childItem);
        }

    return res;
}

//==============================================================================

QSize PropertyEditorWidget::sizeHint() const
{
    // Return either our default/ideal size, depending on the case

    if (mAutoUpdateHeight) {
        // We automatically resize our height, so determine our ideal size which
        // is based on the width of our different columns, and the height of our
        // header and our different rows

        int hintWidth  = 0;
        int hintHeight = header()->height();

        for (int i = 0, iMax = header()->count(); i < iMax; ++i)
            hintWidth += columnWidth(i);

        for (int i = 0, iMax = mModel->rowCount(); i < iMax; ++i) {
            QStandardItem *rowItem = mModel->item(i, 0);
            int rowItemHeight = rowHeight(rowItem->index());

            if (rowItemHeight)
                // Our current row has some height, meaning that it is visible,
                // so we can its height to ou hintHeight, as well as retrieve
                // the total height of our row's children

                hintHeight += rowItemHeight+childrenRowHeight(rowItem);
        }

        return QSize(hintWidth, hintHeight);
    } else {
        // We don't automatically resize our height, so our ideal size is our
        // maximum size...

        return maximumSize();
    }
}

//==============================================================================

void PropertyEditorWidget::selectFirstProperty()
{
    // Convenience function to select the first property, i.e. the first item

    selectFirstItem();
}

//==============================================================================

PropertyEditorWidgetGuiState * PropertyEditorWidget::guiState()
{
    // Finish the property editing, if any

    finishPropertyEditing();

    // Retrieve our GUI state

    PropertyEditorWidgetGuiState *res = new PropertyEditorWidgetGuiState(currentIndex());

    // Retrieve the hidden state, expanded state and value of our different
    // properties

    foreach (Property *property, mProperties)
        res->addProperty(new PropertyEditorWidgetGuiStateProperty(property,
                                                                  isRowHidden(property->row(),
                                                                              property->parent()?
                                                                                  property->parent()->index():
                                                                                  mModel->invisibleRootItem()->index()),
                                                                  isExpanded(property->index()),
                                                                  property->value()));

    // Return our GUI state

    return res;
}

//==============================================================================

void PropertyEditorWidget::setGuiState(PropertyEditorWidgetGuiState *pGuiState)
{
    // Set our GUI state

    // Set the hidden state, expanded state and value of our different
    // properties

    foreach (PropertyEditorWidgetGuiStateProperty *guiStateProperty, pGuiState->properties()) {
        setRowHidden(guiStateProperty->property()->row(),
                     guiStateProperty->property()->parent()?
                         guiStateProperty->property()->parent()->index():
                         mModel->invisibleRootItem()->index(),
                     guiStateProperty->isHidden());

        setExpanded(guiStateProperty->property()->index(),
                    guiStateProperty->isExpanded());

        guiStateProperty->property()->setValue(guiStateProperty->value());
    }

    // Set our current index, if it is valid

    if (pGuiState->currentProperty().isValid())
        setCurrentIndex(pGuiState->currentProperty());
}

//==============================================================================

Property * PropertyEditorWidget::addProperty(const Property::Type &pType,
                                             Property *pParent)
{
    // Create our property

    Property *res = new Property(pType, this);

    // Populate our data model with our property

    if (pParent) {
        // We want to add a child property

        pParent->add(res);

        // If we want to see the child property, we need root decoration

        setRootIsDecorated(true);
    } else {
        // We want to add a root property

        res->addTo(mModel->invisibleRootItem());
    }

    // Span ourselves if we are of section type

    if (pType == Property::Section)
        setFirstColumnSpanned(res->row(),
                              pParent?pParent->index():mModel->invisibleRootItem()->index(),
                              true);

    // Keep track of our property's change of visibility

    connect(res, SIGNAL(visibilityChanged(const bool &)),
            this, SLOT(updateHeight()));

    // Keep track of our property's change of value

    connect(res, SIGNAL(valueChanged(const QString &, const QString &)),
            this, SLOT(emitPropertyChanged()));

    // Keep track of our property and return it

    mProperties << res;

    return res;
}

//==============================================================================

Property * PropertyEditorWidget::addSectionProperty(const QString &pName,
                                                    Property *pParent)
{
    // Add a section property and return its information

    Property *res = addProperty(Property::Section, pParent);

    res->setName(pName);

    return res;
}

//==============================================================================

Property * PropertyEditorWidget::addSectionProperty(Property *pParent)
{
    // Add a section property and return its information

    return addSectionProperty(QString(), pParent);
}

//==============================================================================

Property * PropertyEditorWidget::addIntegerProperty(const int &pValue,
                                                    Property *pParent)
{
    // Add an integer property and return its information

    Property *res = addProperty(Property::Integer, pParent);

    res->setIntegerValue(pValue);

    return res;
}

//==============================================================================

Property * PropertyEditorWidget::addIntegerProperty(Property *pParent)
{
    // Add an integer property and return its information

    return addIntegerProperty(0, pParent);
}

//==============================================================================

Property * PropertyEditorWidget::addDoubleProperty(const double &pValue,
                                                   Property *pParent)
{
    // Add a double property and return its information

    Property *res = addProperty(Property::Double, pParent);

    res->setDoubleValue(pValue);

    return res;
}

//==============================================================================

Property * PropertyEditorWidget::addDoubleProperty(Property *pParent)
{
    // Add a double property and return its information

    return addDoubleProperty(0.0, pParent);
}

//==============================================================================

Property * PropertyEditorWidget::addListProperty(const QStringList &pStringList,
                                                 Property *pParent)
{
    // Add a list property and return its information
    // Note: a list property must necessarily be editable...

    Property *res = addProperty(Property::List, pParent);

    res->setEditable(true);
    res->setListValue(pStringList);

    return res;
}

//==============================================================================

Property * PropertyEditorWidget::addListProperty(Property *pParent)
{
    // Add a list property and return its information

    return addListProperty(QStringList(), pParent);
}

//==============================================================================

Property * PropertyEditorWidget::addStringProperty(const QString &pString,
                                                   Property *pParent)
{
    // Add a string property and return its information

    Property *res = addProperty(Property::String, pParent);

    res->setValue(pString);

    return res;
}

//==============================================================================

Property * PropertyEditorWidget::addStringProperty(Property *pParent)
{
    // Add a string property and return its information

    return addStringProperty(QString(), pParent);
}

//==============================================================================

void PropertyEditorWidget::keyPressEvent(QKeyEvent *pEvent)
{
    // Check some key combinations

    if (   (pEvent->modifiers() & Qt::ControlModifier)
        && (pEvent->key() == Qt::Key_A)) {
        // The user wants to select everything which we don't want to allow,
        // so just accept the event...

        pEvent->accept();
    } else if (   (pEvent->key() == Qt::Key_Return)
               || (pEvent->key() == Qt::Key_Enter)) {
        // The user wants to start/stop editing the property

        if (mPropertyEditor)
            // We are currently editing a property, so stop editing it

            editProperty(0);
        else
            // We are not currently editing a property, so start editing the
            // current one
            // Note: we could use mProperty, but if it was to be empty then we
            //       would have to use currentIndex().row(), so we might as well
            //       use the latter all the time...

            editProperty(property(currentIndex()));

        // Accept the event

        pEvent->accept();
    } else if (pEvent->key() == Qt::Key_Escape) {
        // The user wants to cancel the editing of the property

        finishPropertyEditing(false);

        // Accept the event

        pEvent->accept();
    } else if (   !(pEvent->modifiers() & Qt::ShiftModifier)
               && !(pEvent->modifiers() & Qt::ControlModifier)
               && !(pEvent->modifiers() & Qt::AltModifier)
               && !(pEvent->modifiers() & Qt::MetaModifier)) {
        // None of the modifiers is selected

        if (pEvent->key() == Qt::Key_Up) {
            // The user wants to go the previous property

            goToPreviousProperty();

            // Accept the event

            pEvent->accept();
        } else if (pEvent->key() == Qt::Key_Down) {
            // The user wants to go to the next property

            goToNextProperty();

            // Accept the event

            pEvent->accept();
        } else {
            // Default handling of the event

            TreeViewWidget::keyPressEvent(pEvent);
        }
    } else {
        // Default handling of the event

        TreeViewWidget::keyPressEvent(pEvent);
    }
}

//==============================================================================

void PropertyEditorWidget::mouseMoveEvent(QMouseEvent *pEvent)
{
    // Default handling of the event
    // Note: this will finish the editing of our 'old' property, if any

    TreeViewWidget::mouseMoveEvent(pEvent);

    // Edit our 'new' property, but only if we are not right-clicking
    // Note: pEvent->button() is always equal to Qt::NoButton for mouse move
    //       events, hence we rely on mRightClicking instead...

    if (mRightClicking) {
        // We are right clicking, so we just make sure that the 'new' property
        // is selected

        selectProperty(property(indexAt(pEvent->pos())));
    } else {
        // We are not right clicking, so we can edit the 'new' property (which
        // will, as a result, also select it)

        editProperty(property(indexAt(pEvent->pos())));
    }
}

//==============================================================================

void PropertyEditorWidget::mousePressEvent(QMouseEvent *pEvent)
{
    // Keep track of our 'old' property, if any

    Property *oldProperty = mProperty;

    // Default handling of the event
    // Note: this will finish the editing of our 'old' property, if any

    TreeViewWidget::mousePressEvent(pEvent);

    // Edit our 'new' property, but only if we are not right-clicking and if
    // there is a 'new' property and it is different from our 'old' property

    mRightClicking = pEvent->button() == Qt::RightButton;

    Property *newProperty = property(indexAt(pEvent->pos()));

    if (!mRightClicking && newProperty && (newProperty != oldProperty))
        editProperty(newProperty);
}

//==============================================================================

void PropertyEditorWidget::mouseReleaseEvent(QMouseEvent *pEvent)
{
    // Reset our right clicking state

    mRightClicking = false;

    // Default handling of the event
    // Note: this will finish the editing of our 'old' property, if any

    TreeViewWidget::mouseReleaseEvent(pEvent);
}

//==============================================================================

void PropertyEditorWidget::resizeEvent(QResizeEvent *pEvent)
{
    // Default handling of the event

    TreeViewWidget::resizeEvent(pEvent);

    // Update our height

    updateHeight();
}

//==============================================================================

void PropertyEditorWidget::updateHeight()
{
    // Update our height, so that we don't have any blank space at the bottom,
    // but only if required

    if (mAutoUpdateHeight) {
        QSize idealSize = sizeHint();

        setFixedHeight( idealSize.height()
                       +((width() < idealSize.width())?
#if defined(Q_OS_WIN) || defined(Q_OS_MAC)
                            horizontalScrollBar()->height():
#elif defined(Q_OS_LINUX)
                            horizontalScrollBar()->height()+3:
                            // Note: why on earth does Linux require three more
                            //       pixels?!... Indeed, if we don't add them,
                            //       then there won't be enough space to show
                            //       the last property (upon selecting it) and
                            //       the widget will increase its height,
                            //       resulting in some blank space at the bottom
                            //       and a vertical bar being shown! We could
                            //       prevent the vertical bar from ever being
                            //       shown, but not sure what could be done
                            //       about the blank space, hence we 'manually'
                            //       add those three pixels...
#else
    #error Unsupported platform
#endif
                            0));
        // Note: the new height consists of our ideal height to which we add the
        //       height of our horizontal scroll bar, should it be shown (i.e.
        //       if our width is smaller than that of our ideal size)...
    }
}

//==============================================================================

void PropertyEditorWidget::editorOpened(QWidget *pEditor)
{
    // Keep track of some information about the property

    mProperty = currentProperty();
    mPropertyEditor = pEditor;

    // We are starting the editing of a property, so make sure that if we are to
    // edit a list item, then its original value gets properly set
    // Note: indeed, by default the first list item will be selected...

    if (mProperty->type() == Property::List) {
        QString propertyValue = mProperty->value();
        QStringList propertyListValue = mProperty->listValue();
        ListEditorWidget *propertyEditor = static_cast<ListEditorWidget *>(mPropertyEditor);

        for (int i = 0, iMax = propertyListValue.count(); i < iMax; ++i)
            if (!propertyValue.compare(propertyListValue[i])) {
                propertyEditor->setCurrentIndex(i);

                break;
            }
    }

    // Next, we need to use the property's editor as our focus proxy and make
    // sure that it immediately gets the focus
    // Note: if we were not to immediately give the editor the focus, then the
    //       central widget would give the focus to the previously focused
    //       widget (see CentralWidget::updateGui()), so...

    setFocusProxy(pEditor);

    pEditor->setFocus();
}

//==============================================================================

void PropertyEditorWidget::editorClosed()
{
    // Make sure that we are editing a property

    if (!mPropertyEditor)
        return;

    // We have stopped editing a property, so make sure that if we were editing
    // a list item, then its value gets properly set
    // Note: we should always set (and force) the value of the property, even if
    //       we are not dealing with an 'empty' integer or double property since
    //       only the text of the property item has been updated (through
    //       QTreeView) at this stage, while Property::setValue() will do a few
    //       more things (e.g. update the tool tip)...

    if (mProperty->type() == Property::List) {
        mProperty->setValue(mProperty->listValue().isEmpty()?
                                mProperty->emptyListValue():
                                static_cast<ListEditorWidget *>(mPropertyEditor)->currentText(),
                            true);
    } else {
        // Not a list item, but still need to call setPropertyItem() so that the
        // item's tool tip gets updated

        QString value = mProperty->value();

        if (    value.isEmpty()
            && (   (mProperty->type() == Property::Integer)
                || (mProperty->type() == Property::Double)))
            // We are dealing with an 'empty' integer or double property, so set
            // its value to zero

            value = "0";

        mProperty->setValue(value, true);
    }

    // Reset our focus proxy and make sure that we get the focus (see
    // editorOpened() above for the reason)

    setFocusProxy(0);
    setFocus();

    // Reset some information about the property

    mProperty = 0;
    mPropertyEditor = 0;
}

//==============================================================================

void PropertyEditorWidget::selectProperty(Property *pProperty)
{
    // Select the property, if one is provided

    if (!pProperty)
        return;

    pProperty->select();
}

//==============================================================================

void PropertyEditorWidget::editProperty(Property *pProperty,
                                        const bool &pCommitData)
{
    // Check that we are dealing with a 'proper' property item and not a section

    if (pProperty && (pProperty->type() == Property::Section))
        return;

    // We want to edit a new property, so first stop the editing of the current
    // one, if any

    if (mPropertyEditor) {
        // A property is currently being edited, so commit its data and then
        // close its corresponding editor

        if (pCommitData)
            commitData(mPropertyEditor);

        closeEditor(mPropertyEditor,
                    pCommitData?
                        QAbstractItemDelegate::SubmitModelCache:
                        QAbstractItemDelegate::RevertModelCache);

        // Update our state
        // Note: this is very important otherwise our state will remain that of
        //       editing...

        setState(NoState);

        // Call editorClosed() to reset a few things

        editorClosed();
    }

    // Now that the editing of our old property has finished, we can start the
    // editing of our new property, if any

    if (pProperty) {
        // There is a new property to edit, so first make sure that it is
        // selected

        selectProperty(pProperty);

        // Now, we can 'properly' edit the property's value (assuming it's
        // editable)

        pProperty->edit();
    }
}

//==============================================================================

Properties PropertyEditorWidget::properties() const
{
    // Return our properties

    return mProperties;
}

//==============================================================================

void PropertyEditorWidget::finishPropertyEditing(const bool &pCommitData)
{
    // The user wants to finish the editing of the property

    editProperty(0, pCommitData);
}

//==============================================================================

void PropertyEditorWidget::removeAllProperties()
{
    // Remove all the properties we currently hold

    mModel->removeRows(0, mModel->rowCount());

    // By default, we don't want root decoration

    setRootIsDecorated(false);
}

//==============================================================================

void PropertyEditorWidget::goToNeighbouringProperty(const int &pShift)
{
    // Determine the index of the current index's neighbour

    QModelIndex neighbouringIndex = QModelIndex();

    if (pShift == 1)
        neighbouringIndex = indexBelow(currentIndex());
    else if (pShift == -1)
        neighbouringIndex = indexAbove(currentIndex());

    // Edit the neighbouring property, if any

    if (neighbouringIndex.isValid())
        editProperty(property(neighbouringIndex));
}

//==============================================================================

void PropertyEditorWidget::goToPreviousProperty()
{
    // Go to the previous property

    goToNeighbouringProperty(-1);
}

//==============================================================================

void PropertyEditorWidget::goToNextProperty()
{
    // Go to the next property

    goToNeighbouringProperty(1);
}

//==============================================================================

void PropertyEditorWidget::emitPropertyChanged()
{
    // Let people know that one of our properties has changed

    emit propertyChanged(qobject_cast<Property *>(sender()));
}

//==============================================================================

Property * PropertyEditorWidget::property(const QModelIndex &pIndex) const
{
    // Don't waste time if the given index isn't valid

    if (!pIndex.isValid())
        return 0;

    // Return our information about the property at the given index

    foreach (Property *property, mProperties)
        if (property->hasIndex(pIndex))
            return property;

    // Somehow, we couldn't find the property (how is that even possible?!),
    // so...

    return 0;
}

//==============================================================================

Property * PropertyEditorWidget::currentProperty() const
{
    // Return some information about the current property

    return property(currentIndex());
}

//==============================================================================

}   // namespace Core
}   // namespace OpenCOR

//==============================================================================
// End of file
//==============================================================================
