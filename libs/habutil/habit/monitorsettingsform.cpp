#include "monitorsettingsform.h"
#include "ui_monitorsettingsform.h"
#include "monitormanager.h"
#include "monitorsettings.h"


#include <QMessageBox>
#include <QDesktopWidget>

namespace GUILib {

MonitorSettingsForm::MonitorSettingsForm(const Habit::MonitorSettings& monitorSettings, QWidget *w)
    : QWidget(w)
    , monitorManager_(new System::MonitorManager)
    , monitorSettings_(monitorSettings)
{
    ui = new Ui::MonitorSettingsFormClass();
    ui->setupUi(this);
    identifyMonitors(false);
    updateMonitorSettings();
    QObject::connect(ui->identifyPushButton, SIGNAL(clicked()), this,  SLOT(identifyMonitors()));
	setFixedWidth(556);
}

void MonitorSettingsForm::setMonitorSettings(const Habit::MonitorSettings& monitorSettings)
{
    monitorSettings_ = monitorSettings;
    updateMonitorSettings();
}

Habit::MonitorSettings MonitorSettingsForm::getMonitorSettings() const
{
    return monitorSettings_;
}

void MonitorSettingsForm::updateMonitorSettings()
{
    ui->controlComboBox->setCurrentIndex(monitorSettings_.getControlMonitorIndex()+1);
    ui->leftComboBox->setCurrentIndex(monitorSettings_.getLeftMonitorIndex()+1);
    ui->centerComboBox->setCurrentIndex(monitorSettings_.getCenterMonitorIndex()+1);
    ui->rightComboBox->setCurrentIndex(monitorSettings_.getRightMonitorIndex()+1);
}

void MonitorSettingsForm::identifyMonitors(bool msgBox)
{
    int monitorsCount = monitorManager_->monitorsCount();
    ui->messageLabel->setText(QString("Habit has identified %1 monitors in this system").arg(monitorsCount));
    QStringList comboItems;
	comboItems.push_back("None");
	if (msgBox)
	{
		for (int i = 0; i < monitorsCount; i++)
		{
			QMessageBox box(QMessageBox::Information, "Information", QString("This monitor is identified as 'Monitor %1'").arg(i+1));
			QSize size = box.sizeHint();
			QRect screen = QApplication::desktop()->screenGeometry(i);
			QRect wind(screen.x() + (screen.width()-size.width())/2, screen.y() + (screen.height()-size.height())/2, size.width(), size.height());
			box.setGeometry(wind);
			box.setModal(true);
			box.exec();
		}
	}
	for(int i = 0; i < monitorsCount; ++i) {
        comboItems.push_back(QString("Monitor %1").arg(i + 1));
	}

	int controlIdx = ui->controlComboBox->currentIndex();
    ui->controlComboBox->clear();
    ui->controlComboBox->addItems(comboItems);
	if (msgBox && controlIdx < ui->controlComboBox->count())
	{
		ui->controlComboBox->setCurrentIndex(controlIdx);
	}
	else
		ui->controlComboBox->setCurrentIndex(0);

	int leftIdx = ui->leftComboBox->currentIndex();
    ui->leftComboBox->clear();
    ui->leftComboBox->addItems(comboItems);
	if (msgBox && leftIdx < ui->leftComboBox->count())
	{
		ui->leftComboBox->setCurrentIndex(leftIdx);
	}
	else
		ui->leftComboBox->setCurrentIndex(0);

	int centerIdx = ui->centerComboBox->currentIndex();
    ui->centerComboBox->clear();
    ui->centerComboBox->addItems(comboItems);
	if (msgBox && centerIdx < ui->centerComboBox->count())
	{
		ui->centerComboBox->setCurrentIndex(centerIdx);
	}
	else
		ui->centerComboBox->setCurrentIndex(0);

	int rightIdx = ui->rightComboBox->currentIndex();
    ui->rightComboBox->clear();
    ui->rightComboBox->addItems(comboItems);
	if (msgBox && rightIdx < ui->rightComboBox->count())
	{
		ui->rightComboBox->setCurrentIndex(rightIdx);
	}
	else
		ui->rightComboBox->setCurrentIndex(0);
}

MonitorSettingsForm::~MonitorSettingsForm()
{
    delete monitorManager_;
    delete ui;
}

void MonitorSettingsForm::initialize()
{
    ui->controlComboBox->setCurrentIndex(monitorSettings_.getControlMonitorIndex()+1);
    ui->leftComboBox->setCurrentIndex(monitorSettings_.getLeftMonitorIndex()+1);
    ui->centerComboBox->setCurrentIndex(monitorSettings_.getCenterMonitorIndex()+1);
    ui->rightComboBox->setCurrentIndex(monitorSettings_.getRightMonitorIndex()+1);
}

Habit::MonitorSettings MonitorSettingsForm::getConfigurationObject()
{
	Habit::MonitorSettings settings;
	settings.setId(monitorSettings_.getId());
	settings.setControlMonitorIndex(ui->controlComboBox->currentIndex()-1);
	settings.setCenterMonitorIndex(ui->centerComboBox->currentIndex()-1);
	settings.setRightMonitorIndex(ui->rightComboBox->currentIndex()-1);
	settings.setLeftMonitorIndex(ui->leftComboBox->currentIndex()-1);
	return settings;
}

void MonitorSettingsForm::setConfigurationObject( const Habit::MonitorSettings& settings)
{
	monitorSettings_ = settings;
	initialize();
}

}
