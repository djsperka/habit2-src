/*
 * JsonStimFactory.h
 *
 *  Created on: Apr 16, 2020
 *      Author: dan
 */

#ifndef JSONSTIMFACTORY_H_
#define JSONSTIMFACTORY_H_

#include "StimFactory.h"
#include "HMM.h"
#include "rapidjson/document.h"
#include <map>
#include <boost/filesystem.hpp>

namespace hmm {

class JsonStimFactory: public StimFactory
{
public:
	// map srcpos "name" to stim position (int)
	typedef std::map<std::string, int> NameIndexMap;

private:
	rapidjson::Document m_document;
	HMMConfiguration m_config;
	NameIndexMap m_mapNameIndex;	// source pos names mapped to posid
	boost::filesystem::path m_stimroot;

	// these are called by constructor, each just parses part of the config.
	// each can throw
	void parseConfig(const rapidjson::Document& d);
	void parseStimuli(const rapidjson::Document& d);

	bool checkImageFile(const std::string& filename);
	bool checkMediaFile(const std::string& filename);
	bool checkAudioFile(const std::string& filename);
	bool checkSource(const rapidjson::Value& v);
	bool getStringFromValue(const rapidjson::Value& v, const char *w, std::string& s);
	bool getUintFromValueWD(const rapidjson::Value& v, const char *w, unsigned int &u, unsigned int defaultUint);
	bool getBoolFromValueWD(const rapidjson::Value& v, const char *w, bool &b, bool defaultBool);
	bool getUintFromHexStringValueWD(const rapidjson::Value& v, const char *w, unsigned int &u, unsigned int defaultUint);
	Stim::SourceP makeSourceFromValue(const rapidjson::Value& v, GstElement *pipeline, const std::string& prefix);
public:
	JsonStimFactory(const std::string& filename, const std::string& imageroot);
	virtual ~JsonStimFactory();

	hmm::Stim* operator()(hmm::HMMStimID id, hmm::HMM& mm, const std::string& prefix);
	hmm::Stim* background(hmm::HMM& mm);

	const HMMConfiguration& getHMMConfiguration();

};

} /* namespace hmm */

#endif /* JSONSTIMFACTORY_H_ */
