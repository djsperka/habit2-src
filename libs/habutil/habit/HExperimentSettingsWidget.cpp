#include "HExperimentSettingsWidget.h"
#include <QtGui>
/*
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QtDebug>
#include <QListViewItem>
*/
using namespace GUILib;

HExperimentSettingsWidget::HExperimentSettingsWidget(const Habit::ExperimentSettings& experimentSettings, QWidget* parent, bool bViewOnly)
: QWidget(parent)
, m_bViewOnly(bViewOnly)
, m_experimentSettings(experimentSettings)
{
	createComponents();
	makeConnections();
	doLayout();
	initialize();
}

// Returns the experiment settings as currently configured in the dialogs of this widget
Habit::ExperimentSettings HExperimentSettingsWidget::getExperimentSettings()
{
}

// Assign the experiment settings that this widget uses. This method implies a call to initialize()
void HExperimentSettingsWidget::setExperimentSettings(const Habit::ExperimentSettings& settings)
{
	m_experimentSettings = settings;
	initialize();
}

// synchronize the visible widgets/forms/etc to reflect what the underlying experiment settings hold.
void HExperimentSettingsWidget::initialize()
{

}

void HExperimentSettingsWidget::createComponents()
{
	m_pListView = new QListView();
	QListViewItem *generalItem = new QListViewItem(m_pListView, "General");
	QListViewItem *monitorSettingsItem = new QListViewItem(generalItem, "Monitors");
	QListViewItem *controlBarSettingsItem = new QListViewItem(generalItem, "Control Bar");
	QListViewItem *exptItem = new QListViewItem(m_pListView, "Experiment");
	QListViewItem *lookingItem = new QListViewItem(exptItem, "Looking");
	QListViewItem *attentionItem = new QListViewItem(exptItem, "Attention Getting");
	QListViewItem *preTestItem = new QListViewItem(exptItem, "PreTest Phase");
	QListViewItem *preTestStimuluiItem = new QListViewItem(exptItem, "PreTest Stimuli");
	QListViewItem *habItem = new QListViewItem(exptItem, "Habituation Phase");
	QListViewItem *habStimuluiItem = new QListViewItem(exptItem, "Habituation Stimuli");
	QListViewItem *testItem = new QListViewItem(exptItem, "Test Phase");
	QListViewItem *testStimuluiItem = new QListViewItem(exptItem, "Test Stimuli");

	m_pbtnSave = new QPushButton("Save");
	m_pbtnCancel = new QPushButton("Cancel");

	m_pStackedWidget = new QStackedWidget();
	// TODO: add forms and complete map
}

void HExperimentSettingsWidget::makeConnections()
{
	connect(m_pListView, SIGNAL(clicked(const QModelIndex& index)), this, itemClicked(const QModelIndex& index));
	connect(m_pbtnSave, SIGNAL(clicked()), this, SLOT(saveClicked()));
	connect(m_pbtnCancel, SIGNAL(clicked()), this, SLOT(cancelClicked()));

}

void HExperimentSettingsWidget::saveClicked()
{
	qDebug() << "saveClicked()";
}

void HExperimentSettingsWidget::cancelClicked()
{
	qDebug() << "cancelClicked()";
}

void HExperimentSettingsWidget::itemClicked(const QModelIndex& index)
{
	qDebug() << "HExperimentSettingsWidget::itemClicked " << index.row();
}

void HExperimentSettingsWidget::doLayout()
{
	QHBoxLayout *main = new QHBoxLayout();
	QVBoxLayout *right = new QVBoxLayout();
	QVBoxLayout *left = new QVBoxLayout();
	QHBoxLayout *buttons = new QHBoxLayout();

	buttons->addWidget(m_pbtnCancel);
	buttons->addWidget(m_pbtnSave);

	left->addWidget(m_pListView);

	right->addWidget(m_pStackedWidget);
	right->addLayout(buttons);

	main->addLayout(left);
	main->addLayout(right);

	setLayout(main);
}

