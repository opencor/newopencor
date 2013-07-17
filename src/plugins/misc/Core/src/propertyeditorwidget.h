//==============================================================================
// Property editor widget
//==============================================================================

#ifndef PROPERTYEDITORWIDGET_H
#define PROPERTYEDITORWIDGET_H

//==============================================================================

#include "coreglobal.h"
#include "treeviewwidget.h"

//==============================================================================

#include <QComboBox>
#include <QLineEdit>
#include <QStandardItem>
#include <QStyledItemDelegate>

//==============================================================================

class QStandardItemModel;

//==============================================================================

namespace OpenCOR {
namespace Core {

//==============================================================================

class TextEditorWidget : public QLineEdit
{
    Q_OBJECT

public:
    explicit TextEditorWidget(QWidget *pParent = 0);

protected:
    virtual void keyPressEvent(QKeyEvent *pEvent);

Q_SIGNALS:
    void goToPreviousPropertyRequested();
    void goToNextPropertyRequested();
};

//==============================================================================

class IntegerEditorWidget : public TextEditorWidget
{
    Q_OBJECT

public:
    explicit IntegerEditorWidget(QWidget *pParent = 0);
};

//==============================================================================

class DoubleEditorWidget : public TextEditorWidget
{
    Q_OBJECT

public:
    explicit DoubleEditorWidget(QWidget *pParent = 0);
};

//==============================================================================

class ListEditorWidget : public QComboBox
{
    Q_OBJECT

public:
    explicit ListEditorWidget(QWidget *pParent = 0);

protected:
    virtual void keyPressEvent(QKeyEvent *pEvent);
    virtual void mouseDoubleClickEvent(QMouseEvent *pEvent);
    virtual void mousePressEvent(QMouseEvent *pEvent);

Q_SIGNALS:
    void goToPreviousPropertyRequested();
    void goToNextPropertyRequested();
};

//==============================================================================

class PropertyItemDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    virtual QWidget *createEditor(QWidget *pParent,
                                  const QStyleOptionViewItem &pOption,
                                  const QModelIndex &pIndex) const;

protected:
    virtual bool eventFilter(QObject *pObject, QEvent *pEvent);
    virtual void paint(QPainter *pPainter, const QStyleOptionViewItem &pOption,
                       const QModelIndex &pIndex) const;

Q_SIGNALS:
    void openEditor(QWidget *pEditor) const;

    void goToPreviousPropertyRequested();
    void goToNextPropertyRequested();

    void listPropertyChanged(const QString &pValue);
};

//==============================================================================

class Property;

class CORE_EXPORT PropertyItem : public QStandardItem
{
public:
    explicit PropertyItem(Property *pOwner);

    Property *owner() const;

private:
    Property *mOwner;
};

//==============================================================================

class PropertyEditorWidget;

class CORE_EXPORT Property : public QObject
{
    Q_OBJECT

public:
    enum Type {
        Section  = QStandardItem::UserType,
        String   = QStandardItem::UserType+1,
        Integer  = QStandardItem::UserType+2,
        Double   = QStandardItem::UserType+3,
        List     = QStandardItem::UserType+4
    };

    explicit Property(const Type &pType, PropertyEditorWidget *pParent);

    Type type() const;

    QStandardItem * parent() const;

    int row() const;

    Property * parentProperty() const;
    void setParentProperty(Property *pProperty);

    QList<Property *> properties() const;

    void add(Property *pProperty);
    void addTo(QStandardItem *pParent);

    QModelIndex index() const;
    bool hasIndex(const QModelIndex &pIndex) const;

    QString id() const;
    void setId(const QString &pId);

    bool isCheckable() const;
    void setCheckable(const bool &pCheckable);

    bool isChecked() const;
    void setChecked(const bool &pChecked);

    bool isEditable() const;
    void setEditable(const bool &pEditable);

    QString name() const;
    void setName(const QString &pName);

    QIcon icon() const;
    void setIcon(const QIcon &pIcon);

    int integerValue() const;
    void setIntegerValue(const int &pIntegerValue);

    double doubleValue() const;
    void setDoubleValue(const double &pDoubleValue,
                        const bool &pEmitSignal = true);

    QString value() const;
    void setValue(const QString &pValue, const bool &pForce = false,
                  const bool &pEmitSignal = true);

    QStringList listValue() const;
    void setListValue(const QStringList &pListValue);

    QString emptyListValue() const;
    void setEmptyListValue(const QString &pEmptyListValue);

    QString unit() const;
    void setUnit(const QString &pUnit);

    QString extraInfo() const;
    void setExtraInfo(const QString &pExtraInfo);

    bool isVisible() const;
    void setVisible(const bool &pVisible);

    void select() const;

    void edit() const;

private:
    PropertyEditorWidget *mOwner;

    Type mType;

    QString mId;

    PropertyItem *mName;
    PropertyItem *mValue;
    PropertyItem *mUnit;

    QStringList mListValue;
    QString mEmptyListValue;

    QString mExtraInfo;

    Property * mParentProperty;
    QList<Property *> mProperties;

    QList<QStandardItem *> items() const;

    void updateToolTip();

Q_SIGNALS:
    void visibilityChanged(const bool &pVisible);
    void valueChanged(const QString &pOldValue, const QString &pNewValue);
};

//==============================================================================

typedef QList<Property *> Properties;

//==============================================================================

class PropertyEditorWidgetGuiStateProperty
{
public:
    explicit PropertyEditorWidgetGuiStateProperty(Property *pProperty,
                                                  const bool &pHidden,
                                                  const bool &pExpanded,
                                                  const QString &pValue);

    Property * property() const;

    bool isHidden() const;
    bool isExpanded() const;
    QString value() const;

private:
    Property *mProperty;

    bool mHidden;
    bool mExpanded;
    QString mValue;
};

//==============================================================================

typedef QList<PropertyEditorWidgetGuiStateProperty *> PropertyEditorWidgetGuiStateProperties;

//==============================================================================

class CORE_EXPORT PropertyEditorWidgetGuiState
{
public:
    explicit PropertyEditorWidgetGuiState(const QModelIndex &pCurrentProperty);
    ~PropertyEditorWidgetGuiState();

    PropertyEditorWidgetGuiStateProperties properties() const;
    void addProperty(PropertyEditorWidgetGuiStateProperty *pProperty);

    QModelIndex currentProperty() const;

private:
    PropertyEditorWidgetGuiStateProperties mProperties;

    QModelIndex mCurrentProperty;
};

//==============================================================================

class CORE_EXPORT PropertyEditorWidget : public TreeViewWidget
{
    Q_OBJECT

public:
    explicit PropertyEditorWidget(const bool &pAutoUpdateHeight,
                                  QWidget *pParent = 0);
    explicit PropertyEditorWidget(QWidget *pParent = 0);
    ~PropertyEditorWidget();

    virtual void retranslateUi();

    virtual void loadSettings(QSettings *pSettings);
    virtual void saveSettings(QSettings *pSettings) const;

    virtual QSize sizeHint() const;

    void selectFirstProperty();

    PropertyEditorWidgetGuiState * guiState();
    void setGuiState(PropertyEditorWidgetGuiState *pGuiState);

    Property * addSectionProperty(const QString &pName, Property *pParent = 0);
    Property * addSectionProperty(Property *pParent = 0);

    Property * addIntegerProperty(const int &pValue, Property *pParent = 0);
    Property * addIntegerProperty(Property *pParent = 0);

    Property * addDoubleProperty(const double &pValue, Property *pParent = 0);
    Property * addDoubleProperty(Property *pParent = 0);

    Property * addListProperty(const QStringList &pStringList,
                               Property *pParent = 0);
    Property * addListProperty(Property *pParent = 0);

    Property * addStringProperty(const QString &pString, Property *pParent = 0);
    Property * addStringProperty(Property *pParent = 0);

    Properties properties() const;

    Property * property(const QModelIndex &pIndex) const;
    Property * currentProperty() const;

    void finishPropertyEditing(const bool &pCommitData = true);

    void removeAllProperties();

protected:
    virtual void keyPressEvent(QKeyEvent *pEvent);
    virtual void mouseMoveEvent(QMouseEvent *pEvent);
    virtual void mousePressEvent(QMouseEvent *pEvent);
    virtual void mouseReleaseEvent(QMouseEvent *pEvent);
    virtual void resizeEvent(QResizeEvent *pEvent);

private:
    bool mAutoUpdateHeight;

    QStandardItemModel *mModel;

    Properties mProperties;

    Property *mProperty;
    QWidget *mPropertyEditor;

    bool mRightClicking;

    void constructor(const bool &pAutoUpdateHeight = false);

    void retranslateEmptyListProperties(QStandardItem *pItem);

    Property * addProperty(const Property::Type &pType, Property *pParent);

    void selectProperty(Property *pProperty);
    void editProperty(Property *pProperty, const bool &pCommitData = true);

    void goToNeighbouringProperty(const int &pShift);

    int childrenRowHeight(const QStandardItem *pItem) const;

Q_SIGNALS:
    void propertyChanged(Core::Property *pProperty);
    void listPropertyChanged(const QString &pValue);

private Q_SLOTS:
    void updateHeight();

    void editorOpened(QWidget *pEditor);
    void editorClosed();

    void goToPreviousProperty();
    void goToNextProperty();

    void emitPropertyChanged();
};

//==============================================================================

}   // namespace Core
}   // namespace OpenCOR

//==============================================================================

#endif

//==============================================================================
// End of file
//==============================================================================
