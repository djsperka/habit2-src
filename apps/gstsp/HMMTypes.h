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


};	// end namespace hmm

#endif /* HMMTYPES_H_ */
