/*
 * HMMTypes.h
 *
 *  Created on: Oct 17, 2019
 *      Author: dan
 */

#ifndef HMMTYPES_H_
#define HMMTYPES_H_

namespace hmm {

enum class HMMSourceType
{
	NONE,
	VIDEO_ONLY,
	AUDIO_ONLY,
	AUDIO_VIDEO,
};

enum class HMMStreamType
{
	NONE,
	VIDEO,
	AUDIO
};

enum class HMMStimState
{
	NONE,
	PREROLLING,
	PREROLLED,
	PLAYING,
	PENDING_PAUSE,
	PAUSED
};

typedef int HMMStimPosition;
typedef unsigned long HMMStimID;
typedef unsigned long HMMInstanceID;

//class HMMInstanceID
//{
//	int m_instanceID;
//	HMMStimID m_stimID;
//
//	HMMInstanceID& operator=(const HMMInstanceID& inst);	// unimplemented - immutable
//public:
//	HMMInstanceID(int instanceID=0, HMMStimID stimid=0): m_instanceID(instanceID), m_stimID(stimid) {};
//	HMMInstanceID(const HMMInstanceID& inst): m_instanceID(inst.instanceid()), m_stimID(inst.stimid()) {};
//	virtual ~HMMInstanceID() {};
//
//	HMMStimID stimid() const { return m_stimID; }
//	int instanceid() const { return m_instanceID; }
//	bool operator<(const HMMInstanceID& inst) const
//	{
//		return (m_instanceID < inst.instanceid());
//	}
//};


};	// end namespace hmm

#endif /* HMMTYPES_H_ */
