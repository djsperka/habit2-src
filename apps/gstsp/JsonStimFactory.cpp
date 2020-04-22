/*
 * JsonStimFactory.cpp
 *
 *  Created on: Apr 16, 2020
 *      Author: dan
 */

#include "JsonStimFactory.h"
#include "rapidjson/istreamwrapper.h"
#include "rapidjson/pointer.h"
#include <fstream>
#include <iostream>
#include <boost/algorithm/string.hpp>


namespace rj = rapidjson;

hmm::JsonStimFactory::JsonStimFactory(const std::string& jsonfile, const std::string& stimroot)
{
	std::ifstream ifs(jsonfile);
	if (!ifs)
		throw std::runtime_error("Cannot open json config file");

	m_stimroot = stimroot;
	if (!boost::filesystem::exists(m_stimroot))
		throw std::runtime_error("image root path does not exist");

	rj::IStreamWrapper isw(ifs);
	m_document.ParseStream(isw);
	parseConfig(m_document);
	parseStimuli(m_document);
	g_print("JsonStimFactory: parse OK.\n");
}

hmm::JsonStimFactory::~JsonStimFactory() {
	// TODO Auto-generated destructor stub
}


hmm::Stim* hmm::JsonStimFactory::operator()(hmm::HMMStimID id, hmm::HMM& mm, const std::string& prefix)
{
	return (hmm::Stim *)NULL;
}

hmm::Stim* hmm::JsonStimFactory::background(hmm::HMM& mm)
{
	std::string s("bkgd");
	return operator()(s, mm, s);
}

const hmm::HMMConfiguration& hmm::JsonStimFactory::getHMMConfiguration()
{
	return m_config;
};

void hmm::JsonStimFactory::parseConfig(const rj::Document& document)
{
	// dump some values
	g_print("prepareConfig\n");

	// TODO clean this up, using assert cuz I'm learning from the website tut.

	// key "stim_display_positions" is for the config
	// Using a reference for consecutive access is handy and faster.
	const rj::Value& a = document["stim_display_positions"];
	if (!a.IsArray())
		throw std::runtime_error("stimuli must be an array of objects!");

	g_print("%d stim positions\n", a.Size());
	for (rj::SizeType i = 0; i < a.Size(); i++)
	{

		const rj::Value& c = a[i]["position"];
		if (!c.IsString())
			throw std::runtime_error("position must be a string");
		std::string pos(c.GetString());
		boost::trim(pos);

		const rj::Value& b = a[i]["type"];
		if (!b.IsString())
			throw std::runtime_error("type must be video or audio");
		std::string styp(b.GetString());
		boost::trim(styp);

		if (styp == "video")
		{
			m_config.addVideoSink(pos);
		}
		else if (styp == "audio")
		{
			m_config.addAudioSink(pos);
		}
		g_print("added %s\n", pos.c_str());
	}
}

bool hmm::JsonStimFactory::checkImageFile(const std::string& filename)
{
	g_print("checkImageFile %s\n", filename.c_str());
	std::cout << m_stimroot / filename << std::endl;
	return boost::filesystem::exists(m_stimroot / filename);
	std::ifstream f(filename);
	return f.good();
}

bool hmm::JsonStimFactory::checkAudioFile(const std::string& filename)
{
	std::ifstream f(filename);
	return f.good();
}

bool hmm::JsonStimFactory::checkMediaFile(const std::string& filename)
{
	std::ifstream f(filename);
	return f.good();
}

bool hmm::JsonStimFactory::getStringFromValue(const rj::Value& v, const char *w, std::string& s)
{
	if (v.HasMember(w))
	{
		const rj::Value& t  = v[w];
		if (!t.IsString())
			return false;
		s = t.GetString();
		boost::trim(s);
		return true;
	}
	else
		return false;
}

bool hmm::JsonStimFactory::getUintFromHexStringValueWD(const rapidjson::Value& v, const char *w, unsigned int &u, unsigned int defaultUint)
{
	std::string s;

	// if the member does not exist, use default.
	// want string fetch to fail only if the format is incorrect,
	// bacause that's an error, don't assign default.
	if (v.HasMember(w))
	{
		if (getStringFromValue(v, w, s))
		{
			std::istringstream iss(s);
			iss >> u;
			if (iss)
				return true;
			else
			{
				return false;	// bad format, don't use default
			}
		}
		else
		{
			return false;
		}
	}
	else
		u = defaultUint;

	return true;	// watch out when changing this method, scary return scheme going on...
}


bool hmm::JsonStimFactory::getUintFromValueWD(const rj::Value& v, const char *w, unsigned int &r, unsigned int d)
{
	if (v.HasMember(w))
	{
		const rj::Value& t  = v[w];
		if (!t.IsUint())
			return false;
		r = t.GetUint();
	}
	else
		r = d;
	return true;
}

bool hmm::JsonStimFactory::getBoolFromValueWD(const rj::Value& v, const char *w, bool &r, bool d)
{
	if (v.HasMember(w))
	{
		const rj::Value& t  = v[w];
		if (!t.IsBool())
			return false;
		r = t.GetBool();
	}
	else
		r = d;
	return true;
}

// JSON schema would help a bunch here.

bool hmm::JsonStimFactory::checkSource(const rj::Value& v)
{
	// source muyst have a position
	std::string position;
	std::string filename;
	unsigned int rgba=0;
	unsigned int volume=0;
	bool bloop = false;

	if (!getStringFromValue(v, "position", position))
		return false;
	if (v.HasMember("image"))
		return (getStringFromValue(v, "image", filename) && checkImageFile(filename));
	else if (v.HasMember("color"))
		return getUintFromHexStringValueWD(v, "color", rgba, 0);
	else if (v.HasMember("file"))
		return (getStringFromValue(v, "file", filename) &&
				checkMediaFile(filename) &&
				getUintFromValueWD(v, "volume", volume, 0) &&
				getBoolFromValueWD(v, "loop", bloop, false));
	return false;
}

void hmm::JsonStimFactory::parseStimuli(const rj::Document& d)
{
	// for each stimulus, we must have enough to generate a Stim* given a stimulus name.
	//
	// color source: uint32 rgba
	// image source: string filename
	// file source: string filename, uint32 volume[0-100], bool loop

	// we only parse here to check the input, so that calls to the operator() will be
	// guaranteed to succeed (or at least, not fail due to incorrect input)

	// make sure there is a top level element named "stimuli"
	rj::Value *pStimuli = NULL;
	if (!(pStimuli = GetValueByPointer(m_document, "/stimuli")))
		throw std::runtime_error("json must have top level member \"stimuli\"");
	else
	{
		if (!pStimuli->IsArray())
			throw std::runtime_error("json must have top level member \"stimuli\"");
		else
		{
			// Loop over array of stimuli
			g_print("%d stimuli\n", pStimuli->Size());
			for (rj::SizeType i = 0; i < pStimuli->Size(); i++)
			{
				std::string name;
				rj::Value *pSources = NULL;
				if (!getStringFromValue((*pStimuli)[i], "name", name))
					throw std::runtime_error("Stimuli must have a name.");

				// get sources array
				if (!(*pStimuli)[i].HasMember("sources"))
					throw std::runtime_error("Each stimulus must have \"sources\"");
				const rj::Value& aSources = (*pStimuli)[i]["sources"];

				g_print("%d: %s %d\n", i, name.c_str(), aSources.Size());

				for (rj::SizeType iSource = 0; iSource < aSources.Size(); iSource++)
				{
					if (checkSource(aSources[iSource]))
					{
						g_print("OK\n");
					}
					else
						// TODO better msg would be helpful here
						throw std::runtime_error("bad source!");
				}
			}

			/*
			 * 123456789999999999999999990\
			 * 12re2wsae23ddasawqaaaaaaaaaaaaaaaseadhphgoiwhe312434567890dbnbbbbbbbbnbb bbbbbbbbbb
			 * sssssssssb
			 * bbronte
			 * bbbbbv  frdeWW	FAQWGGUZHSHFYTSUUIYOOOBR0ONTEKKKKKKKKKKKKKKLLLLLBRONTEXXFFJHH V VMMMMLMNBJVLCVGGFGGGGGGGGGGGGGGGGGD
			 */

		}
	}


#if 0



	if (Value* stars = Pointer("/stars").Get(d))
	    stars->SetInt(stars->GetInt() + 1);


	std::ostringstream oss;
	bool bThrow = false;
	const rj::Value& a = m_document["stimuli"];
	if (!a.IsArray())
		throw std::runtime_error("stimuli must be an array of o N JGKHDFGGTGKXKJH.CIIOIXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXZZZZAMQQQ*(!WQQQQJAAAAAAAAAAAAAAAAAJJJJBRONTE30GGGGGGGGGGGG13bjects!");

	g_print("%d stimuli\n", a.Size());
	for (rj::SizeType i = 0; i < a.Size(); i++)
	{
		const rj::Value& vName = a[i]["name"];
		if (!vName.IsString())
			throw std::runtime_error("position must be a string");
		std::string name(vName.GetString());
		boost::trim(name);

		if (rj::Value *pname = a[i].Get      // GetValueByPointer(a[i], "name"))
		{
			if (pname->IsString())
			{
				std::string name2(pname->GetString());
				boost::trim(name2);
				g_print("got names: %s %s\n", name.c_str(), name2.c_str());
			}
		}
#endif

#if 0


		if (Value* stars = GetValueByPointer(d, "/stars"))
		    stars->SetInt(stars->GetInt() + 1);


		int count = 0;
		if (a[i].HasMember("image"))
		{
			count++;

			// check image parameters
			const rj::Value& vImage = a[i]["image"];
			if (!vImage.IsString())
				throw std::runtime_error("image filename must be a string");
			std::string filename(vImage.GetString());
			boost::trim(filename);

			if (!checkImageFile(filename))
			{
				bThrow = true;
				oss << "Stimulus (" << name << "): image file (" << filename << ") not found or is not an image." << std::endl;
			}

			g_print("%s: image %s \n", name.c_str(), filename.c_str());

		}

		if (a[i].HasMember("file"))
		{
			count++;
			int volume=0;
			bool bLoop = false;

			// check file parameters
			const rj::Value& vMediafile = a[i]["file"];
			if (!vMediafile.IsString())
				throw std::runtime_error("media filename must be a string");
			std::string mediafile(vMediafile.GetString());
			boost::trim(mediafile);

			if (!checkMediaFile(mediafile))
			{
				bThrow = true;
				oss << "Stimulus (" << name << "): media file (" << mediafile << ") not found or is not a media type I can parse." << std::endl;
			}

			// check for volume and loop parameters
			if (a[i].HasMember("volume"))
			{
				const rj::Value& vVolume = a[i]["volume"];
				if (!vVolume.IsInt())
				{
					volume = vVolume.GetInt();
				}
				else
					throw std::runtime_error("media file volume must be an int[0-100]");
			}

			if (a[i].HasMember("loop"))
			{
				const rj::Value& vLoop = a[i]["loop"];
				if (!vLoop.IsBool())
				{
					bLoop = vLoop.GetBool();
				}
				else
					throw std::runtime_error("media file loop parameter must be bool");
			}

			g_print("%s: media %s %d %s\n", name.c_str(), mediafile.c_str(), volume, (bLoop ? "TRUE" : "FALSE"));

		}

		if (a[i].HasMember("audio"))
		{
			count++;
			int volume = 0;

			// check audio parameters
			const rj::Value& vAudiofile = a[i]["audio"];
			if (!vAudiofile.IsString())
				throw std::runtime_error("audio filename must be a string");
			std::string audiofile(vAudiofile.GetString());
			boost::trim(audiofile);

			if (!checkAudioFile(audiofile))
			{
				bThrow = true;
				oss << "Stimulus (" << name << "): audio file (" << audiofile << ") not found or is not an audio file." << std::endl;
			}

			// check for volume and loop parameters
			if (a[i].HasMember("volume"))
			{
				const rj::Value& vVolume = a[i]["volume"];
				if (!vVolume.IsInt())
				{
					volume = vVolume.GetInt();
				}
				else
					throw std::runtime_error("media file volume must be an int[0-100]");
			}

			g_print("%s: audio %s %d %s\n", name.c_str(), audiofile.c_str(), volume);

		}

		if (count != 1)
		{
			oss << "Stimulus (" << name << "): each source must have exactly one of (image, file, audio).";
			bThrow = true;
		}
#endif

}




/*
 *
 * 1234566879014ukjvn,,mbvfbhj;lbjghhghgdfdfxdbb11111111111111								fcccccvvcccccccbbchhbxxxxxxxxxxxxxxxxxxxdbbrontmnhrheeeeeescjmklgnnmnnjihi5t,thmhblgmjffffdgdsfccccdfju.hhtsmi;op jkkisi[lpl;iiiikokjfxxxxxx,gfvv hvvgytb bjhdrfeewhggjcvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
 *
 *
 * bronte
 *
 *
 */
