/*
 * q5player.cpp
 *
 *  Created on: Feb 23, 2017
 *      Author: dan
 */

#include <QApplication>
#include <QtDebug>
#include <QMessageBox>
#include <QDir>
#include <QDesktopWidget>
#include <QHBoxLayout>
#include <QGst/Init>
#include <QGst/ElementFactory>
#include <QGst/Pipeline>
#include <QGst/Element>
#include <QGst/Ui/VideoWidget>

#include "HTypes.h"
#include "HVideoWidget.h"


int main(int argc, char **argv)
{
	QApplication app(argc, argv);
	QGst::init(&argc, &argv);
	int iTestMode = 0;
	QString filename;
	bool bFile = false;

	if (argc > 1)
	{
		iTestMode = QString(argv[1]).toInt();
	}

	if (argc > 2)
	{
		bFile = true;
		filename = QString(argv[2]);
	}

	app.setApplicationName("vw");
	app.setOrganizationName("Infant Cognition Lab");
	app.setOrganizationDomain("infantcognitionlab.ucdavis.edu");

	QDialog dlg;

	// create pipeline
	QGst::PipelinePtr pipeline = QGst::Pipeline::create();


	if (iTestMode == 0)
	{
		qDebug() << "Using raw VideoWidget";
		QHBoxLayout *hbox = new QHBoxLayout;
		QGst::Ui::VideoWidget *vw = new QGst::Ui::VideoWidget;
		hbox->addWidget(vw);
		dlg.setLayout(hbox);

		// sink
		QGst::ElementPtr sink = QGst::ElementFactory::make("qwidget5videosink");
		Q_ASSERT(sink);


		if (!bFile)
		{
			// videotest src
			QGst::ElementPtr videotestsrc = QGst::ElementFactory::make("videotestsrc");
			Q_ASSERT(videotestsrc);

			// link
			pipeline->add(videotestsrc, sink);
			videotestsrc->link(sink);
		}
		else
		{
			QGst::ElementPtr filesrc = QGst::ElementFactory::make("filesrc");
			Q_ASSERT(filesrc);
			filesrc->setProperty("location", filename);

			QGst::ElementPtr jpegdec = QGst::ElementFactory::make("jpegdec");
			Q_ASSERT(jpegdec);

			QGst::ElementPtr convert = QGst::ElementFactory::make("autovideoconvert");
			Q_ASSERT(convert);

			QGst::ElementPtr imagefreeze = QGst::ElementFactory::make("imagefreeze");
			Q_ASSERT(imagefreeze);

			pipeline->add(filesrc, jpegdec, convert, imagefreeze, sink);
			filesrc->link(jpegdec);
			jpegdec->link(convert);
			convert->link(imagefreeze);
			imagefreeze->link(sink);
		}


		// assign sink to vw
		vw->setVideoSink(sink);

		// without this size is 0,0
		vw->setMinimumSize(320, 240);
		qDebug() << " vw size hint " << vw->sizeHint();

		// size hint is -1,-1 - TODO WHAT DOES THAT MEAN?
		vw->resize(vw->sizeHint());
	}
	else if (iTestMode == 1)
	{

		qDebug() << "Using HVideoWidget";
		QHBoxLayout *hbox = new QHBoxLayout;
		HVideoWidget *phvw = new HVideoWidget(HDisplayType::HDisplayTypeOriginalSize, true);
		hbox->addWidget(phvw);
		dlg.setLayout(hbox);

		// sink
		QGst::ElementPtr sink = QGst::ElementFactory::make("qwidget5videosink");
		Q_ASSERT(sink);

		if (!bFile)
		{
			// videotest src
			QGst::ElementPtr videotestsrc = QGst::ElementFactory::make("videotestsrc");
			Q_ASSERT(videotestsrc);

			// link
			pipeline->add(videotestsrc, sink);
			videotestsrc->link(sink);

		}
		else
		{
			QGst::ElementPtr filesrc = QGst::ElementFactory::make("filesrc");
			Q_ASSERT(filesrc);
			filesrc->setProperty("location", filename);

			QGst::ElementPtr jpegdec = QGst::ElementFactory::make("jpegdec");
			Q_ASSERT(jpegdec);

			QGst::ElementPtr convert = QGst::ElementFactory::make("autovideoconvert");
			Q_ASSERT(convert);

			QGst::ElementPtr imagefreeze = QGst::ElementFactory::make("imagefreeze");
			Q_ASSERT(imagefreeze);

			pipeline->add(filesrc, jpegdec, convert, imagefreeze, sink);
			filesrc->link(jpegdec);
			jpegdec->link(convert);
			convert->link(imagefreeze);
			imagefreeze->link(sink);
		}

		// assign sink to vw
		phvw->setVideoSink(sink);

		// without this size is 0,0
		phvw->setMinimumSize(320, 240);
		qDebug() << " vw size hint " << phvw->sizeHint();

		// size hint is -1,-1 - TODO WHAT DOES THAT MEAN?
		phvw->resize(phvw->sizeHint());

	}

	// play
	pipeline->setState(QGst::StatePlaying);

	dlg.show();
	dlg.exec();
	return 0;
}



