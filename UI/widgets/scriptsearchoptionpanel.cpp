#include "scriptsearchoptionpanel.h"
#include <QLineEdit>
#include <QLabel>
#include <QCheckBox>
#include <QRadioButton>
#include <QComboBox>
#include <QButtonGroup>
#include <QSet>
#include "UI/ela/ElaComboBox.h"
#include "UI/ela/ElaCheckBox.h"
#include "UI/ela/ElaLineEdit.h"
#include "UI/ela/ElaRadioButton.h"
#include "UI/widgets/component/flowlayout.h"

ScriptSearchOptionPanel::ScriptSearchOptionPanel(QWidget *parent) : QWidget(parent), optionChanged(false), allWidth(0), overlayOptions(nullptr)
{
    setLayout(new FlowLayout(this));
    layout()->setContentsMargins(0, 0, 0, 0);
}

bool ScriptSearchOptionPanel::hasOptions() const
{
    return script && !script->searchSettings().empty();
}

void ScriptSearchOptionPanel::setScript(QSharedPointer<ScriptBase> s, const QMap<QString, QString> *overlayOptions)
{
    this->script = s;
    optionChanged = false;
    optionItems.clear();
    allWidth = 0;
    this->overlayOptions = overlayOptions;
    if(!s) return;
    optionItems.resize(s->searchSettings().size());
    int i = 0;
    for(const ScriptBase::SearchSettingItem &item : s->searchSettings())
    {
        optionItems[i].searchSettingItem = &item;
        makeItem(optionItems[i]);
        allWidth += optionItems[i].widget->sizeHint().width();
        ++i;
    }
}

QMap<QString, QString> ScriptSearchOptionPanel::getOptionVals() const
{
    QMap<QString, QString> vals;
    for(const OptionItem &item : optionItems)
    {
        vals[item.searchSettingItem->key] = item.val;
    }
    return vals;
}

void ScriptSearchOptionPanel::makeItem(ScriptSearchOptionPanel::OptionItem &item)
{
    switch (item.searchSettingItem->dType)
    {
    case ScriptBase::SearchSettingItem::Text:
        makeTextItem(item);
        break;
    case ScriptBase::SearchSettingItem::Combo:
        makeComboItem(item);
        break;
    case ScriptBase::SearchSettingItem::Radio:
        makeRadioItem(item);
        break;
    case ScriptBase::SearchSettingItem::Check:
        makeCheckItem(item);
        break;
    case ScriptBase::SearchSettingItem::CheckList:
        makeCheckListItem(item);
        break;
    case ScriptBase::SearchSettingItem::Label:
        makeLabelItem(item);
        break;
    default:
        break;
    }
}

void ScriptSearchOptionPanel::makeTextItem(ScriptSearchOptionPanel::OptionItem &item)
{
    QWidget *container = new QWidget(this);
    layout()->addWidget(container);
    item.widget.reset(container);
    QHBoxLayout *hLayout = new QHBoxLayout(container);
    QLabel *tip = new QLabel(item.searchSettingItem->title, container);
    tip->setToolTip(item.searchSettingItem->description);
    tip->setOpenExternalLinks(true);
    QLineEdit *edit = new ElaLineEdit(item.searchSettingItem->value, container);
    if(overlayOptions && overlayOptions->contains(item.searchSettingItem->key))
    {
        edit->setText((*overlayOptions)[item.searchSettingItem->key]);
    }
    item.val = edit->text();
    QObject::connect(edit, &QLineEdit::textChanged, this, [&item, this](const QString &text){
        item.val = text;
        this->optionChanged = true;
    });
    hLayout->setContentsMargins(0, 0, 0, 0);
    hLayout->addWidget(tip);
    hLayout->addWidget(edit);
}

void ScriptSearchOptionPanel::makeComboItem(ScriptSearchOptionPanel::OptionItem &item)
{
    QWidget *container = new QWidget(this);
    layout()->addWidget(container);
    item.widget.reset(container);
    QHBoxLayout *hLayout = new QHBoxLayout(container);
    QLabel *tip = new QLabel(item.searchSettingItem->title, container);
    tip->setToolTip(item.searchSettingItem->description);
    tip->setOpenExternalLinks(true);
    QComboBox *combo = new ElaComboBox(container);
    combo->setEditable(false);
    QStringList choices = item.searchSettingItem->choices.split(',', Qt::SkipEmptyParts);
    combo->addItems(choices);
    combo->setCurrentIndex(choices.indexOf(item.searchSettingItem->value));
    if(overlayOptions && overlayOptions->contains(item.searchSettingItem->key))
    {
        combo->setCurrentIndex(choices.indexOf((*overlayOptions)[item.searchSettingItem->key]));
    }
    item.val = combo->currentText();
    QObject::connect(combo, &QComboBox::currentTextChanged, this, [&item, this](const QString &text){
        item.val = text;
        this->optionChanged = true;
    });
    hLayout->setContentsMargins(0, 0, 0, 0);
    hLayout->addWidget(tip);
    hLayout->addWidget(combo);
}

void ScriptSearchOptionPanel::makeRadioItem(ScriptSearchOptionPanel::OptionItem &item)
{
    QWidget *container = new QWidget(this);
    layout()->addWidget(container);
    item.widget.reset(container);
    QHBoxLayout *hLayout = new QHBoxLayout(container);
    QLabel *tip = new QLabel(item.searchSettingItem->title, container);
    tip->setToolTip(item.searchSettingItem->description);
    tip->setOpenExternalLinks(true);
    hLayout->setContentsMargins(0, 0, 0, 0);
    hLayout->addWidget(tip);
    QStringList choices = item.searchSettingItem->choices.split(',', Qt::SkipEmptyParts);
    if(choices.isEmpty()) return;
    QButtonGroup *btnGroup = new QButtonGroup(container);
    QString selectItem = item.searchSettingItem->value;
    if(overlayOptions && overlayOptions->contains(item.searchSettingItem->key))
    {
        selectItem = (*overlayOptions)[item.searchSettingItem->key];
    }
    item.val = selectItem;
    for(const QString &s : choices)
    {
        QRadioButton *rbtn = new ElaRadioButton(s, container);
        rbtn->setFixedHeight(35);
        btnGroup->addButton(rbtn);
        hLayout->addWidget(rbtn);
        if(s == selectItem) rbtn->setChecked(true);
    }
    QObject::connect(btnGroup, &QButtonGroup::buttonToggled, [&item, this](QAbstractButton *btn, bool checked) {
        if(checked)
        {
            item.val = btn->text();
            this->optionChanged = true;
        }
    });
}

void ScriptSearchOptionPanel::makeCheckItem(ScriptSearchOptionPanel::OptionItem &item)
{
    QCheckBox *check = new ElaCheckBox(item.searchSettingItem->title, this);
    check->setFixedHeight(35);
    layout()->addWidget(check);
    item.widget.reset(check);
    check->setChecked(item.searchSettingItem->value != "0");
    if(overlayOptions && overlayOptions->contains(item.searchSettingItem->key))
    {
        check->setChecked((*overlayOptions)[item.searchSettingItem->key] != "0");
    }
    check->setToolTip(item.searchSettingItem->description);
    item.val = check->isChecked()? "1" : "0";
    QObject::connect(check, &QCheckBox::stateChanged, this, [&item, this](int state){
        item.val = state == Qt::CheckState::Checked? "1" : "0";
        this->optionChanged = true;
    });
}

void ScriptSearchOptionPanel::makeCheckListItem(ScriptSearchOptionPanel::OptionItem &item)
{
    QWidget *container = new QWidget(this);
    layout()->addWidget(container);
    item.widget.reset(container);
    QHBoxLayout *hLayout = new QHBoxLayout(container);
    QLabel *tip = new QLabel(item.searchSettingItem->title, container);
    tip->setToolTip(item.searchSettingItem->description);
    tip->setOpenExternalLinks(true);
    hLayout->setContentsMargins(0, 0, 0, 0);
    hLayout->addWidget(tip);
    QStringList choices = item.searchSettingItem->choices.split(',', Qt::SkipEmptyParts);
    if(choices.isEmpty()) return;
    QStringList checkedVals = item.searchSettingItem->value.split(',');
    if(overlayOptions && overlayOptions->contains(item.searchSettingItem->key))
    {
        checkedVals = (*overlayOptions)[item.searchSettingItem->key].split(',');
    }
    item.val = checkedVals.join(',');
    QSet<QString> checkedSet(checkedVals.begin(), checkedVals.end());
    QButtonGroup *btnGroup = new QButtonGroup(container);
    btnGroup->setExclusive(false);
    for(const QString &s : choices)
    {
        QCheckBox *check = new ElaCheckBox(s, container);
        check->setFixedHeight(35);
        btnGroup->addButton(check);
        hLayout->addWidget(check);
        if(checkedSet.contains(s)) check->setChecked(true);
    }
    QObject::connect(btnGroup, &QButtonGroup::buttonToggled, [btnGroup, &item, this](QAbstractButton *, bool ) {
        QStringList vals;
        for(QAbstractButton *btn : btnGroup->buttons())
        {
            if(btn->isChecked()) vals.append(btn->text());
        }
        item.val = vals.join(',');
        this->optionChanged = true;
    });
}

void ScriptSearchOptionPanel::makeLabelItem(ScriptSearchOptionPanel::OptionItem &item)
{    
    QLabel *label = new QLabel(item.searchSettingItem->title, this);
    label->setFixedHeight(35);
    layout()->addWidget(label);
    item.widget.reset(label);
    item.val = item.searchSettingItem->value;
    label->setToolTip(item.searchSettingItem->description);
    label->setOpenExternalLinks(true);
}
