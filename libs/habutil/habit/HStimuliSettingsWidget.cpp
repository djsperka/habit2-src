/*
 * HStimuliSettingsWidget.cpp
 *
 *  Created on: Oct 21, 2014
 *      Author: Oakeslab
 */

#include "HStimuliSettingsWidget.h"
#include "HStimulusSettingsWidget.h"
#include "HStimulusSettingsOrderImportUtil.h"
#include "HWorkspaceUtil.h"
#include <QVBoxLayout>
#include <QGroupBox>
#include <QLabel>
#include <QFileDialog>
#include <QMessageBox>

#include <QtGlobal>
#if QT_VERSION < 0x050000
#include <QDesktopServices>
#else
#include <QStandardPaths>
#endif

using namespace Habit;
using namespace GUILib;

HStimuliSettingsWidget::HStimuliSettingsWidget(const QString& labelName, const StimuliSettings& stimuli, const StimulusDisplayInfo& info, QWidget* parent)
: QWidget(parent)
, m_stimuli(stimuli)
, m_stimulusDisplayInfo(info)
{
	create(labelName, info);
	connections();
	stimulusLayoutTypeChanged(info.getStimulusLayoutType().number());
}

void HStimuliSettingsWidget::create(const QString& labelName, const StimulusDisplayInfo& info)
{
	QDir root;
	habutilGetStimulusRootDir(root);

	m_pStimulusSettingsListWidget = new HStimulusSettingsListWidget(m_stimuli.stimuli(), m_stimulusDisplayInfo);
	m_pStimulusOrderListWidget = new HStimulusOrderListWidget(m_stimuli.orders(), m_stimuli.stimuli(), m_stimulusDisplayInfo.getStimulusLayoutType());
	m_pStimulusPreviewWidget = new HStimulusPreviewWidget(info, root, this);

	QGroupBox *g1 = new QGroupBox(QString("%1 Stimuli").arg(labelName));
	QVBoxLayout *v1 = new QVBoxLayout;
	v1->addWidget(m_pStimulusSettingsListWidget);
	g1->setLayout(v1);
	QGroupBox *g2 = new QGroupBox(QString("%1 Orders").arg(labelName));
	QVBoxLayout *v2 = new QVBoxLayout;
	v2->addWidget(m_pStimulusOrderListWidget);
	g2->setLayout(v2);

	QHBoxLayout *h12 = new QHBoxLayout;
	h12->addWidget(g1);
	h12->addWidget(g2);

	QGroupBox *g4 = new QGroupBox("Stimulus Preview");
	QVBoxLayout *v4 = new QVBoxLayout;
	v4->addWidget(m_pStimulusPreviewWidget);

	g4->setLayout(v4);

	QGroupBox *g3 = new QGroupBox("Import");
	QHBoxLayout *h3 = new QHBoxLayout;
	QLabel *l3 = new QLabel("Stimuli and/or orders for this phase can be imported from a text file.");
	m_pbImport = new QPushButton("Import");
	l3->setWordWrap(true);
	h3->addWidget(l3, 1);
	h3->addWidget(m_pbImport, 1);
	g3->setLayout(h3);

	QVBoxLayout *v = new QVBoxLayout;
	v->addLayout(h12);
	v->addWidget(g4, 1);
//	v->addStretch(1);
	v->addWidget(g3);
	setLayout(v);
}

void HStimuliSettingsWidget::connections()
{
	connect(m_pbImport, SIGNAL(clicked()), this, SLOT(importClicked()));
	connect(m_pStimulusSettingsListWidget, SIGNAL(previewStimulus(int)), this, SLOT(previewStimulus(int)));
	connect(m_pStimulusOrderListWidget, SIGNAL(previewOrder(int)), this, SLOT(previewOrder(int)));
	connect(m_pStimulusSettingsListWidget, SIGNAL(clearStimulus()), this, SLOT(clearStimulus()));
	connect(m_pStimulusOrderListWidget, SIGNAL(clearStimulus()), this, SLOT(clearStimulus()));
	//connect(m_pStimulusSettingsListWidget, SIGNAL(currentChanged(const QModelIndex&, const QModelIndex&)), this, SLOT(currentStimulusSelectionChanged(const QModelIndex&, const QModelIndex&)));
	//connect(m_pStimulusOrderListWidget, SIGNAL(currentChanged(const QModelIndex&, const QModelIndex&)), this, SLOT(currentOrderSelectionChanged(const QModelIndex&, const QModelIndex&)));
	connect(m_pStimulusSettingsListWidget, SIGNAL(stimulusSelectionChanged()), this, SLOT(stimulusSelectionChanged()));
	connect(m_pStimulusOrderListWidget, SIGNAL(orderSelectionChanged()), this, SLOT(orderSelectionChanged()));
}

void HStimuliSettingsWidget::clearStimulus()
{
	m_pStimulusPreviewWidget->clear();
}

void HStimuliSettingsWidget::previewStimulus(int row)
{
	//qDebug() << "HStimuliSettingsWidget::previewStimulus " << m_stimuli.stimuli().at(row).getName();
	m_pStimulusPreviewWidget->preview(m_stimuli.stimuli().at(row));
}

void HStimuliSettingsWidget::previewOrder(int row)
{
	QList< QPair<int, QString> > list;
	QString orderName;
	orderName = m_stimuli.orders().at(row).getName();

	if (!m_stimuli.getIndexedOrderList(orderName, list))
	{
		qDebug() << "Cannot get order list for order \"" << orderName << "\"";
		qDebug() << m_stimuli;
	}
	else
	{
#if 0
		QPair<int, QString> p;
		foreach(p, list)
		{
			qDebug() << "index " << p.first << " label " << p.second;
		}
#endif
		m_pStimulusPreviewWidget->preview(m_stimuli.stimuli(), list);
	}
}

void HStimuliSettingsWidget::importClicked()
{
#if QT_VERSION >= 0x050000
	QString filename = QFileDialog::getOpenFileName(this, "", QStandardPaths::standardLocations(QStandardPaths::DesktopLocation)[0]);
#else
	QString filename = QFileDialog::getOpenFileName(this, "", QDesktopServices::storageLocation(QDesktopServices::DesktopLocation));
#endif


	bool b;
	Habit::StimulusSettingsList slist;
	Habit::HStimulusOrderList olist;
	bool bClobberAllStimuli = false;
	bool bClobberAllOrders = false;

	qDebug() << "Selected file " << filename;
	if (!filename.isEmpty())
	{
		b = importStimulusSettingsAndOrders(filename, slist, olist);
		qDebug() << slist.size() << "stim imported " << olist.size() << " orders imported";

		// We append the stimuli and orders to the widgets so the display isupdated correctly.
		// The widget and model use the same reference, so the local m_stimuli is also updated.
		// Check/verify that no duplicate names are added to the list

		QStringList stimNames = m_stimuli.getStimulusNames();
		QListIterator<Habit::StimulusSettings> imported_ss(slist);
		while (imported_ss.hasNext())
		{
			bool bAppend = false;
			bool bClobber = false;
			Habit::StimulusSettings ss = imported_ss.next();
			if (stimNames.contains(ss.getName()))
			{
				if (bClobberAllStimuli)
				{
					bAppend = false;
					bClobber = true;
				}
				else
				{
					int response = QMessageBox::warning(this, "Name collision!", QString("A stimulus named %1 already exists for this phase. Overwrite?").arg(ss.getName()),
							QMessageBox::Yes | QMessageBox::YesToAll | QMessageBox::No, QMessageBox::Yes);
					if (response == QMessageBox::Yes)
					{
						bAppend = false;
						bClobber = true;
					}
					else if (response == QMessageBox::YesToAll)
					{
						bAppend = false;
						bClobber = bClobberAllStimuli = true;
					}
				}
			}
			else
			{
				bAppend = true;
				bClobber = false;
			}
			if (bAppend) m_pStimulusSettingsListWidget->append(ss);
			if (bClobber) m_pStimulusSettingsListWidget->clobber(ss);
		}

		QStringList orderNames = m_stimuli.getOrderNames();
		QListIterator<Habit::HStimulusOrder> imported_orders(olist);
		while (imported_orders.hasNext())
		{
			bool bAppend = false;
			bool bClobber = false;
			Habit::HStimulusOrder so = imported_orders.next();
			if (orderNames.contains(so.getName()))
			{
				if (bClobberAllOrders)
				{
					bAppend = false;
					bClobber = true;
				}
				else
				{
					int response = QMessageBox::warning(this, "Name collision!", QString("An order named %1 already exists for this phase. Overwrite?").arg(so.getName()),
							QMessageBox::Yes | QMessageBox::YesToAll | QMessageBox::No, QMessageBox::Yes);
					if (response == QMessageBox::Yes)
					{
						bAppend = false;
						bClobber = true;
					}
					else if (response == QMessageBox::YesToAll)
					{
						bAppend = false;
						bClobber = bClobberAllOrders = true;
					}
				}
			}
			else
			{
				bAppend = true;
			}
			if (bAppend) m_pStimulusOrderListWidget->append(so);
			if (bClobber) m_pStimulusOrderListWidget->clobber(so);
		}
	}
}

void HStimuliSettingsWidget::stimulusLayoutTypeChanged(int i)
{
	m_pStimulusSettingsListWidget->setStimulusLayoutType(getStimulusLayoutType(i));
	m_pStimulusOrderListWidget->setStimulusLayoutType(getStimulusLayoutType(i));
	m_pStimulusPreviewWidget->setStimulusLayoutType(getStimulusLayoutType(i));
}

Habit::StimuliSettings HStimuliSettingsWidget::getStimuliSettings()
{
	Habit::StimuliSettings settings;
	settings.setStimuli(m_stimuli.stimuli());
	settings.setOrderList(m_stimuli.orders());
	return settings;
}

void HStimuliSettingsWidget::currentStimulusSelectionChanged(const QModelIndex& current, const QModelIndex& previous)
{
	//qDebug() << "HStimuliSettingsWidget::currentStimulusSelectionChanged( " << current.row() << ", " << previous.row() << ")";

	// look at current selection in order widget
	if (current.row() > -1)
	{
		m_pStimulusOrderListWidget->clearSelection();
	}
}

void HStimuliSettingsWidget::currentOrderSelectionChanged(const QModelIndex& current, const QModelIndex& previous)
{
	//qDebug() << "HStimuliSettingsWidget::currentOrderSelectionChanged( " << current.row() << ", " << previous.row() << ")";
	if (current.row() > -1)
	{
		m_pStimulusSettingsListWidget->clearSelection();
	}
}

void HStimuliSettingsWidget::stimulusSelectionChanged()
{
	//qDebug() << "HStimuliSettingsWidget::stimulusSelectionChanged()";
	m_pStimulusOrderListWidget->clearSelection();
}

void HStimuliSettingsWidget::orderSelectionChanged()
{
	//qDebug() << "HStimuliSettingsWidget::orderSelectionChanged()";
	m_pStimulusSettingsListWidget->clearSelection();
}
