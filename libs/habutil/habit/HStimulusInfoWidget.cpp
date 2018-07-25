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
#include <QFileDialog>
#include <QColorDialog>
#include <QDragEnterEvent>
#include <QMimeData>



GUILib::HStimulusInfoWidget::HStimulusInfoWidget(const Habit::StimulusInfo& info, const QString& label, QWidget *parent, bool isVideoImage)
: QWidget(parent)
, ui(new Ui::HStimulusInfoForm)
, m_info(info)
, m_label(label)
, m_bIsVideoImage(isVideoImage)
{
	//setFixedHeight(50);
	ui->setupUi(this);
	initialize();
	connections();
	// djs - Qt4.8 on mac has a bug in the uri passed with the drop event. The filename
	// ends up looking like this: file:///.file/id=6571367.1706496
	// I think this is fixed for Qt>5.2. Wait until then....
	setAcceptDrops(true);
}

GUILib::HStimulusInfoWidget::~HStimulusInfoWidget()
{
	delete ui;
}

void GUILib::HStimulusInfoWidget::connections()
{
	connect(ui->pbSelectFile, SIGNAL(clicked()), this, SLOT(selectButtonClicked()));

	// can check/unckeck box from within the slot, so better make this queued connection.
	connect(ui->cbUseBackgroundColor, SIGNAL(clicked(bool)), this, SLOT(backgroundColorChecked(bool)), Qt::QueuedConnection);
}

//void GUILib::HStimulusInfoWidget::setStimulusInfo(const Habit::StimulusInfo& info)
//{
//	m_info = info;
//	initialize();
//	emit stimulusInfoChanged();
//}

void GUILib::HStimulusInfoWidget::initialize()
{
	ui->labelPosition->setText(m_label);
	ui->lineeditFileBase->setText(m_info.getFileName());
	ui->checkboxLoop->setChecked(m_info.isLoopPlayBack());
	ui->sliderVolume->setValue(m_info.getVolume());
	ui->cbUseBackgroundColor->setChecked(m_info.isBackground());

	ui->pbSelectFile->setDisabled(m_info.isBackground());
	ui->sliderVolume->setDisabled(m_info.isBackground());
	ui->checkboxLoop->setDisabled(m_info.isBackground());
	ui->lineeditFileBase->setDisabled(m_info.isBackground());
}

Habit::StimulusInfo GUILib::HStimulusInfoWidget::getStimulusInfo()
{
	Habit::StimulusInfo info;
	info.setId(m_info.getId());
	info.setFileName(ui->lineeditFileBase->text());
	info.setVolume(ui->sliderVolume->value());
	info.setLoopPlayBack(ui->checkboxLoop->isChecked());
	info.setIsBackground(ui->cbUseBackgroundColor->isChecked());
	// We do not do anything with color, but we should set it same as the original so it won't appear to have changed.
	info.setColor(m_info.getColor());
	return info;
}

void GUILib::HStimulusInfoWidget::backgroundColorChecked(bool checked)
{
	ui->pbSelectFile->setDisabled(checked);
	ui->sliderVolume->setDisabled(checked);
	ui->checkboxLoop->setDisabled(checked);
	ui->lineeditFileBase->setDisabled(checked);

	// if going from unchecked to checked, we're good.
	// If going the other direction, checked to unchecked - we will make sure a file gets selected so the
	// widget is not left in an inconsistent state (bkgd not checked, and no file selected).

	if (!checked)
	{
		if (ui->lineeditFileBase->text().isEmpty() && !doFileSelection())
		{
			// Must select a file, if they didn't we should set checkbox back to checked.
			ui->cbUseBackgroundColor->setChecked(true);
			// do not emit - there's really no change.
		}
		else
		{
			emit stimulusInfoChanged();
		}
	}
	else
	{
		emit stimulusInfoChanged();
	}
}

void GUILib::HStimulusInfoWidget::selectButtonClicked()
{
	if (doFileSelection())
	{
		emit stimulusInfoChanged();
	}
}


bool GUILib::HStimulusInfoWidget::doFileSelection()
{
	QString path;
	const QString filterVideoImage("All Video and Images (*.avi *.mp4 *.wmf *.asf *.wmv *.mov *.bmp *.gif *.png *.jpg *.jpeg *.pbm *.pgm *.ppm *.tif *.tiff *.pict);;All Video (*.avi *.mp4 *.wmf *.asf *.wmv *.mov);;All Audio (*.mp3 *.ogg *.wma *.wav *.aiff);;All Images (*.bmp *.gif *.png *.jpg *.jpeg *.pbm *.pgm *.ppm *.tif *.tiff *.pict);; Audio Video Interleave (*.avi);;MP4 file format (*.mp4);;Windows Media Video (*.wmf *.asf *.wmv);;QuickTime (*.mov);;All Files (*.*)");
	const QString filterAudio("All Audio and Video (*.mp3 *.ogg *.wma *.wav *.aiff *.avi *.mp4 *.wmf *.asf *.wmv *.mov);;All Files (*.*)");
	QString filter;
	QString filename;
	QDir stimroot = habutilGetStimulusRootDir();

	// Initial folder is the folder of the existing file (if any), otherwise the stim root dir.
	if (ui->lineeditFileBase->text().isEmpty())
	{
		QDir lastdir = habutilGetLastDir(m_bIsVideoImage);
		path = lastdir.absolutePath();
	}
	else
	{
		QFileInfo fileInfo(ui->lineeditFileBase->text());
		if (fileInfo.isRelative())
			path = habutilGetStimulusRootDir().absolutePath();
		else
			path = fileInfo.path();
	}

	// Select file dialog....
	if (m_bIsVideoImage) filter = filterVideoImage;
	else filter = filterAudio;
	filename = QFileDialog::getOpenFileName(this, "", path, filter);
	qDebug() << "Selected file " << filename;

	// See if the file is below the stim root dir. If it is, then store the file as a relative path.
	// If the file lay outside the stim root dir, then store the file as an absolute path.
	if(!filename.isEmpty())
	{
		QFileInfo newFileInfo(filename);
		QString newFileCanonicalPath(newFileInfo.canonicalPath());
		habutilSetLastDir(newFileCanonicalPath, m_bIsVideoImage);
		if (newFileCanonicalPath.startsWith(stimroot.canonicalPath()))
		{
			filename = stimroot.relativeFilePath(newFileInfo.canonicalFilePath());
			qDebug() << "selected file is in stimroot path " << stimroot.canonicalPath();
			qDebug() << "relative selected file: " << filename;
		}
		ui->lineeditFileBase->setText(filename);
		return true;
	}
	return false;
}

void GUILib::HStimulusInfoWidget::dragEnterEvent(QDragEnterEvent *event)
{
	if (event->mimeData()->hasFormat("text/uri-list"))
		event->acceptProposedAction();
}

void GUILib::HStimulusInfoWidget::dropEvent(QDropEvent * event)
{
	qDebug() << "got drop: " << event->mimeData()->data("text/uri-list");
}

