/*
 * HStimulusInfoWidget.cpp
 *
 *  Created on: Oct 17, 2014
 *      Author: Oakeslab
 */

#include "HStimulusInfoWidget.h"
#include "HWorkspaceUtil.h"
#include "ui_HStimulusInfoForm.h"
#include <QtDebug>
#include <QSettings>
#include <QFileDialog>
#include <QColorDialog>


GUILib::HStimulusInfoWidget::HStimulusInfoWidget(const Habit::StimulusInfo& info, const QString& label, QWidget *parent, QDir dir)
: QWidget(parent)
, ui(new Ui::HStimulusInfoForm)
, m_info(info)
, m_label(label)
, m_rootDir(dir)
{
	//setFixedHeight(50);
	ui->setupUi(this);
	connections();
	initialize();
}

GUILib::HStimulusInfoWidget::~HStimulusInfoWidget()
{
	delete ui;
}

void GUILib::HStimulusInfoWidget::connections()
{
	connect(ui->pbSelectFile, SIGNAL(clicked()), this, SLOT(selectButtonClicked()));
	connect(ui->cbUseBackgroundColor, SIGNAL(clicked(bool)), this, SLOT(backgroundColorChecked(bool)));
}

void GUILib::HStimulusInfoWidget::setStimulusInfo(const Habit::StimulusInfo& info)
{
	m_info = info;
	initialize();
}

void GUILib::HStimulusInfoWidget::initialize()
{
	ui->labelPosition->setText(m_label);
	ui->lineeditFileBase->setText(m_info.getFileName());
	ui->checkboxLoop->setChecked(m_info.isLoopPlayBack());
	ui->sliderVolume->setValue(m_info.getVolume());
	ui->cbUseBackgroundColor->setChecked(m_info.isBackground());
	backgroundColorChecked(m_info.isBackground());
	ui->lineeditFileBase->setDisabled(m_info.isBackground());
}

Habit::StimulusInfo GUILib::HStimulusInfoWidget::getStimulusInfo()
{
	Habit::StimulusInfo info;
	info.setFileName(ui->lineeditFileBase->text());
	info.setVolume(ui->sliderVolume->value());
	info.setLoopPlayBack(ui->checkboxLoop->isChecked());
	info.setIsBackground(ui->cbUseBackgroundColor->isChecked());
	return info;
}

void GUILib::HStimulusInfoWidget::backgroundColorChecked(bool checked)
{
	ui->pbSelectFile->setDisabled(checked);
	ui->sliderVolume->setDisabled(checked);
	ui->checkboxLoop->setDisabled(checked);
	ui->lineeditFileBase->setDisabled(checked);
}

void GUILib::HStimulusInfoWidget::selectButtonClicked()
{
	QString path;
	Habit::StimulusInfo stimulusInfo = getStimulusInfo();	// get current filename, in case user has already changed it
	QFileInfo fileInfo(stimulusInfo.getFileName());
	QDir stimroot;
	habutilGetStimulusRootDir(stimroot);

	// Initial folder is the folder of the existing file (if any), otherwise the stim root dir.
	if (stimulusInfo.getFileName().isEmpty())
	{
		QDir lastdir;
		habutilGetLastDir(lastdir);
		path = lastdir.absolutePath();
	}
	else
	{
		path = fileInfo.path();
	}

	// Select file dialog....
	QString filter("All Video, Audio and Images (*.avi *.mp4 *.wmf *.asf *.wmv *.mov *.mp3 *.ogg *.wma *.wav *.aiff *.bmp *.gif *.png *.jpg *.jpeg *.pbm *.pgm *.ppm *.tif *.tiff *.pict);;All Video (*.avi *.mp4 *.wmf *.asf *.wmv *.mov);;All Audio (*.mp3 *.ogg *.wma *.wav *.aiff);;All Images (*.bmp *.gif *.png *.jpg *.jpeg *.pbm *.pgm *.ppm *.tif *.tiff *.pict);; Audio Video Interleave (*.avi);;MP4 file format (*.mp4);;Windows Media Video (*.wmf *.asf *.wmv);;QuickTime (*.mov);;All Files (*.*)");
	QString filename = QFileDialog::getOpenFileName(this, "", path, filter);
	qDebug() << "Selected file " << filename;

	// See if the file is below the stim root dir. If it is, then store the file as a relative path.
	// If the file lay outside the stim root dir, then store the file as an absolute path.
	if(!filename.isEmpty())
	{
		QFileInfo newFileInfo(filename);
		QString newFileCanonicalPath(newFileInfo.canonicalPath());
		habutilSetLastDir(newFileCanonicalPath);
		if (newFileCanonicalPath.startsWith(stimroot.canonicalPath()))
		{
			QDir relative(newFileCanonicalPath.right(newFileCanonicalPath.size() - stimroot.canonicalPath().size()));
			filename = relative.filePath(newFileInfo.fileName());
			//qDebug() << "selected file is in stimroot path " << stimroot.canonicalPath();
			//qDebug() << "relative selected file: " << filename;
		}
		stimulusInfo.setFileName(filename);
		setStimulusInfo(stimulusInfo);
	}
}

void GUILib::HStimulusInfoWidget::backgroundColorClicked()
{
	Habit::StimulusInfo stimulusInfo = getStimulusInfo();	// get current filename, in case user has already changed it
	stimulusInfo.setFileName(QString("color(background)"));
	setStimulusInfo(stimulusInfo);
}

void GUILib::HStimulusInfoWidget::customColorClicked()
{
	Habit::StimulusInfo stimulusInfo = getStimulusInfo();	// get current filename, in case user has already changed it

	// Open color picker...
	QColorDialog dlg;
	dlg.setOption(QColorDialog::DontUseNativeDialog);
	if (dlg.exec() == QDialog::Accepted)
	{
		QColor c = dlg.currentColor();
		stimulusInfo.setFileName(QString("rgb(%1,%2,%3)").arg(c.red()).arg(c.green()).arg(c.blue()));
		setStimulusInfo(stimulusInfo);
	}
}

