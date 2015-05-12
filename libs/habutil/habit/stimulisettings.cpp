#include "stimulisettings.h"
#include "maindao.h"

using namespace Habit;

// version string for input/output. See operator<<, operator>>
static const QString f_sVersion2("SSV2");

StimuliSettings::StimuliSettings(const HStimContext& context)
: ssList_()
, soList_()
, pcontext_(&context)
{}

StimuliSettings::StimuliSettings(const StimuliSettings& settings)
: ssList_(settings.stimuli())
, soList_(settings.orders())
, pcontext_(&settings.getStimContext())
{
}

StimuliSettings::~StimuliSettings()
{
}

StimuliSettings& StimuliSettings::operator=(const StimuliSettings& rhs)
{
	if (this != &rhs)
	{
		setStimuli(rhs.stimuli());
		setOrderList(rhs.orders());
		setStimContext(rhs.getStimContext());
	}
	return *this;
}

StimuliSettings StimuliSettings::clone()
{
	StimuliSettings settings(this->getStimContext());
	StimulusSettingsList list;
	QListIterator<StimulusSettings> it(stimuli());
	HStimulusOrderList olist;
	QListIterator<HStimulusOrder> ot(orders());
	while (it.hasNext())
	{
		list.append(it.next().clone());
	}
	settings.setStimuli(list);
	while (ot.hasNext())
	{
		olist.append(ot.next().clone());
	}
	settings.setOrderList(olist);
	return settings;
}


QDataStream & Habit::operator<< (QDataStream& stream, StimuliSettings settings)
{
	// write version to stream
	//stream.writeBytes(f_psVersion2, 4);
	stream << f_sVersion2 << settings.stimuli() << settings.orders() << settings.getStimContext().number();
	return stream;
}

QDataStream & Habit::operator>> (QDataStream& stream, StimuliSettings& settings)
{
	Habit::StimulusSettingsList sList;
	Habit::HStimulusOrderList oList;
	int icontext;
	QString sVersion;
	qint64 pos = stream.device()->pos();

	stream >> sVersion;
	if (sVersion == f_sVersion2)
	{
		// at version 2 the stimulus orders were added. Skip over the version string,
		// then proceed.
		stream >> sList >> oList >> icontext;
		settings.setStimuli(sList);
		settings.setOrderList(oList);
		settings.setStimContext(getStimContext(icontext));
	}
	else
	{
		stream.device()->seek(pos);
		stream >> sList >> icontext;
		settings.setStimuli(sList);
		settings.setStimContext(getStimContext(icontext));
	}
	return stream;
}

bool Habit::operator==(const Habit::StimuliSettings&lhs, const Habit::StimuliSettings& rhs)
{
	return (lhs.getStimContext() == rhs.getStimContext() &&
			lhs.stimuli() == rhs.stimuli() &&
			lhs.orders() == rhs.orders());
}

StimulusSettingsList& StimuliSettings::stimuli()
{
    return ssList_;
}

const StimulusSettingsList& StimuliSettings::stimuli() const
{
    return ssList_;
}

const HStimContext& StimuliSettings::getStimContext() const
{
	return *pcontext_;
}

void StimuliSettings::setStimContext(const HStimContext& context)
{
	pcontext_ = &context;
}

void StimuliSettings::setStimuli(const StimulusSettingsList& stimuli)
{
	ssList_.clear();
	for (StimulusSettingsListConstIterator it = stimuli.begin(); it < stimuli.end(); it++)
		addStimulus(*it);
}

void StimuliSettings::addStimulus(const StimulusSettings& settings)
{
	// make sure that new stimuli have correct context set
	StimulusSettings tmp(settings);
	tmp.setContext(getStimContext());
	ssList_.append(tmp);
}

void StimuliSettings::setOrderList(const HStimulusOrderList& list)
{
	soList_ = list;
}

void StimuliSettings::addOrder(const HStimulusOrder& order)
{
	soList_.append(order);
}

HStimulusOrderList& StimuliSettings::orders()
{
	return soList_;
}

const HStimulusOrderList& StimuliSettings::orders() const
{
	return soList_;
}

bool StimuliSettings::getIndexedOrderList(const QString& orderName, QList< QPair<int, QString> >& list) const
{
	bool b = false;
	HStimulusOrder order;
	b = soList_.find(orderName, order);
	if (b)
	{
		QStringList stimNames = ssList_.names();
		QStringListIterator it(order.getList());
		int i = -1;
		while (b && it.hasNext())
		{
			QString stimLabel = it.next();
			i = stimNames.indexOf(HStimulusOrder::getStim(stimLabel));
			if (i < 0) b = false;
			else
				list.append(QPair<int, QString>(i, HStimulusOrder::getLabel(stimLabel)));
		}
	}
	return b;
}




void StimuliSettings::loadFromDB(int experimentId) 
{
	MainDao dao;
	*this = dao.getStimuliSettings(experimentId, this->getStimContext());
}

bool Habit::StimuliSettings::saveToDB(size_t id_)
{
	Habit::MainDao dao;
	return dao.addOrUpdateStimuliSettings(id_, *this);
}


QStringList Habit::StimuliSettings::getStimulusNames() const
{
	QStringList result;
	QListIterator<Habit::StimulusSettings> it(ssList_);
	while (it.hasNext())
		result.append(it.next().getName());
	return result;
}

QStringList Habit::StimuliSettings::getOrderNames() const
{
	QStringList result;
	QListIterator<Habit::HStimulusOrder> it(soList_);
	while (it.hasNext())
		result.append(it.next().getName());
	return result;
}






QDebug Habit::operator<<(QDebug dbg, const StimuliSettings& ss)
{
	const StimulusSettingsList& c = ss.stimuli();
	HStimulusOrderList o = ss.orders();
	dbg.nospace() << "StimuliSettings: context " << ss.getStimContext().name() << endl;
	for (int i=0; i<c.size(); i++)
	{
		dbg.nospace() << "Stimulus " << i << endl;
		dbg.nospace() << c.at(i) << endl;
	}
	for (int i=0; i<o.size(); i++)
	{
		dbg.nospace() << "Order " << i << endl;
		dbg.nospace() << o.at(i) << endl;
	}
	return dbg.nospace();
}
