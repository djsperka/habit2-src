#include "treewizard.h"

#include <QtGui/QGridLayout>
#include <QtGui/QTreeWidgetItem>
#include <QtGui/QGroupBox>
#include <QtGui/QApplication>

using namespace GUILib;

const QString TreeWizard::NEW_EXPERIMENT = "-- New Experiment --";
const QString TreeWizard::CLONE_EXPERIMENT = "-- Clone Experiment --";

TreeWizard::TreeWizard(QWidget *parent)
: QWidget(parent),
  tree_(new QTreeWidget),
  currentPageIndex_(-1),
  pageArea_(new QStackedWidget),
  nodeNameFont_(QFont(tr("MS Shell Dlg 2"), 14)),
  pageNameFont_(QFont(tr("MS Shell Dlg 2"), 12))
{
    //resize(QSize(800, 500));

    tree_->setColumnCount(1);
    tree_->setHeaderHidden(true);
    tree_->setFixedWidth(250);

    nextBtn_ = new QPushButton("Next");
    nextBtn_->setFont(QFont(tr("MS Shell Dlg 2"), 12));
    QObject::connect(nextBtn_, SIGNAL(pressed()),
        this,  SLOT(onNext()));

    previousBtn_ = new QPushButton(tr("Previous"));
    previousBtn_->setFont(QFont(tr("MS Shell Dlg 2"), 12));
    QObject::connect(previousBtn_, SIGNAL(pressed()),
        this,  SLOT(onPrevious()));

    cancelBtn_ = new QPushButton(tr("Cancel"));
    cancelBtn_->setFont(QFont(tr("MS Shell Dlg 2"), 12));
    connect(cancelBtn_, SIGNAL(clicked()), this, SIGNAL(canceled()));

    doneBtn_ = new QPushButton(tr("Save"));
    doneBtn_->setFont(QFont(tr("MS Shell Dlg 2"), 12));
    connect(doneBtn_, SIGNAL(clicked()), this, SIGNAL(done()));

    QHBoxLayout* mainLayout = new QHBoxLayout;

	QVBoxLayout* leftSideLayout = new QVBoxLayout();
	
	experimentBox_ = new QComboBox;
    experimentBox_->setFont(QFont(tr("MS Shell Dlg 2"), 10));
	experimentBox_->addItem(NEW_EXPERIMENT);
	experimentBox_->addItem(CLONE_EXPERIMENT);
	experimentBox_->setCurrentIndex(-1);
	experimentBox_->setAutoCompletion(true);
	connect(experimentBox_, SIGNAL(activated(QString)), this, SIGNAL(experimentActivated(QString)));
    
    QGroupBox* experimentNameGrBx = new QGroupBox(tr("Experiment Name"));
    experimentNameGrBx->setAlignment(Qt::AlignBottom);
    experimentNameGrBx->setMaximumWidth(300);
    experimentNameGrBx->setMinimumWidth(250);
    QVBoxLayout* experimentNameLyt = new QVBoxLayout;
    experimentNameLyt->setMargin(5);
    experimentNameLyt->addWidget(experimentBox_);
    experimentNameGrBx->setLayout(experimentNameLyt);
    experimentNameGrBx->setFont(QFont(tr("MS Shell Dlg 2"), 10));

	leftSideLayout->addWidget(experimentNameGrBx, 0);
	leftSideLayout->addWidget(tree_,2);
    mainLayout->setMargin(0);
	mainLayout->addLayout(leftSideLayout);

    //pageArea_->setFixedSize(560, 460);
	//pageArea_->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	//pageArea_->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    pageArea_->setCurrentWidget(currentPage());

    QVBoxLayout* rightSideLayout = new QVBoxLayout;
    rightSideLayout->addWidget(pageArea_, 1);

    QHBoxLayout* controlLayout = new QHBoxLayout;
    controlLayout->addSpacerItem(new QSpacerItem(0,0, QSizePolicy::MinimumExpanding));
    controlLayout->addWidget(cancelBtn_, 0, Qt::AlignBottom | Qt::AlignLeft);
    controlLayout->addWidget(previousBtn_, 0, Qt::AlignBottom | Qt::AlignLeft);
    controlLayout->addWidget(nextBtn_, 0, Qt::AlignBottom | Qt::AlignLeft);
    controlLayout->addWidget(doneBtn_, 0, Qt::AlignBottom | Qt::AlignLeft);

    rightSideLayout->addLayout(controlLayout);
    mainLayout->addLayout(rightSideLayout);
    setLayout(mainLayout);

    QObject::connect(tree_, SIGNAL(currentItemChanged(QTreeWidgetItem* , QTreeWidgetItem*)),
        this,  SLOT(currentItemChanged(QTreeWidgetItem* , QTreeWidgetItem*)));

}

void TreeWizard::currentItemChanged(QTreeWidgetItem* current, QTreeWidgetItem* previous)
{
	Q_UNUSED(previous);
	QString name = current->text(0);
	if(name != "General" && name != "Experiment" && name != "Stimuli") {
		setCurrentIndex(current->data(1, Qt::UserRole).toInt());
	}
}


void TreeWizard::setCurrentPage(int index)
{
    tree_->setCurrentItem(pagesItemsMapping_[index]);
	adjustSize();
}

void TreeWizard::setCurrentIndex(int index)
{
    currentPageIndex_ = index;
    //pageArea_->takeWidget();
    pageArea_->setCurrentIndex(currentPageIndex_);
    update();
}

QWidget* TreeWizard::currentPage()
{
    return currentPageIndex_== -1 ?  0 : wizardPages_.at(currentPageIndex_);
}

void TreeWizard::addPageItem(QWidget* page, const QString name, const QString parentNodeName)
{
	pageArea_->addWidget(page);
    if (currentPageIndex_ == -1) {
        currentPageIndex_ = 0;
        pageArea_->setCurrentWidget(page);
    }
    wizardPages_.push_back(page); 
    QList<QTreeWidgetItem*> treeItems = tree_->findItems(parentNodeName,Qt::MatchCaseSensitive);
    QTreeWidgetItem* pageItem = new QTreeWidgetItem(treeItems.at(0),QStringList(name), 0);
    pageItem->setData(1, Qt::UserRole, QVariant(wizardPages_.count()-1));
    pageItem->setFont(0, pageNameFont_);
    pagesItemsMapping_.insert(wizardPages_.count()-1, pageItem);
}

void TreeWizard::addNodeItem(const QString name, const QString parentNodeName)
{
	Q_UNUSED(parentNodeName);
    QTreeWidgetItem* nodeItem = new QTreeWidgetItem(QStringList(name));
    nodeItem->setFont(0, nodeNameFont_);
	Qt::ItemFlags flags = nodeItem->flags() & ~Qt::ItemIsSelectable;
	nodeItem->setFlags(flags);
    tree_->addTopLevelItem(nodeItem);
    nodeItem->setExpanded(true);
}

void TreeWizard::update()
{
    QWidget::update();

    nextBtn_->setDisabled(currentPageIndex_ == wizardPages_.count()-1);
    previousBtn_->setDisabled(currentPageIndex_ == 0);
}

void TreeWizard::onNext()
{
    if(currentPageIndex_ < wizardPages_.count()-1) {
        tree_->setCurrentItem(pagesItemsMapping_[currentPageIndex_+1]);
    }
}

void TreeWizard::setControlsEnabled(bool isEnabled)
{
	tree_->setEnabled(isEnabled);
	pageArea_->setEnabled(isEnabled);
	nextBtn_->setEnabled(isEnabled);
    previousBtn_->setEnabled(isEnabled);
    cancelBtn_->setEnabled(isEnabled);
    doneBtn_->setEnabled(isEnabled);
}

void TreeWizard::onPrevious()
{
    if(currentPageIndex_ > 0) {
        tree_->setCurrentItem(pagesItemsMapping_[currentPageIndex_-1]);
    }
}

TreeWizard::~TreeWizard()
{
    qDeleteAll(wizardPages_);
}

QComboBox* TreeWizard::getExperimentComboBox()
{
	return experimentBox_;
}