/************************************************************************************
 *  Copyright: National ICT Australia,  2006										*
 *  Developed at the Networks and Pervasive Computing program						*
 *  Author(s): Athanassios Boulis, Dimosthenis Pediaditakis							*
 *  This file is distributed under the terms in the attached LICENSE file.			*
 *  If you do not find this file, copies can be found by writing to:				*
 *																					*
 *      NICTA, Locked Bag 9013, Alexandria, NSW 1435, Australia						*
 *      Attention:  License Inquiry.												*
 *																					*
 ************************************************************************************/



#ifndef _RADIOMODULE_H_
#define _RADIOMODULE_H_

#include "WChannelGenericMessage_m.h"


#include <vector>
#include <omnetpp.h>
#include <cqueue.h>
#include "RadioControlMessage_m.h"
#include "MacGenericFrame_m.h"
#include "MacControlMessage_m.h"
#include "ResourceGenericManager.h"
using namespace std;


enum RadioState
{
	RADIO_STATE_TX = 101,
	RADIO_STATE_LISTEN = 102,
	RADIO_STATE_SLEEP = 103
};


enum nonValid_CS_Codes
{
	RADIO_IN_TX_MODE = 104,
	RADIO_SLEEPING = 105,
	RADIO_NON_READY = 106
};


class RadioModule : public cSimpleModule 
{
	private:
	// parameters and variables
		
		/*--- The .ned file's parameters ---*/
		double dataRate;
		double rxPower;					// in Watts, note: parameter in omnetpp.ini in mW
		double listenPower;				// in Watts, note: parameter in omnetpp.ini in mW
		double sleepPower;				// in Watts, note: parameter in omnetpp.ini in mW
		vector <double> txPowerConsumptionPerLevel;// in Watts, note: parameter in omnetpp.ini in mW
		int txPowerLevelUsed;
		int maxPhyFrameSize;
		int PhyFrameOverhead;
		int bufferSize;					//in kbytes
		int txModeUsed;
		double delaySleep2Listen;
		double delayListen2Sleep;
		double delayListen2Tx;
		double delaySleep2Tx;
		
		double delayTx2Sleep;
		double delayTx2Listen;
		
		double delayCSValid;
		
		
		/*--- Custom class parameters ---*/
		int self;						// the node's ID
		RadioState radioState;			// we are using the enum values RADIO_SLEEP RADIO_LISTEN from the msg definition as possible values to this variable
		
		//cQueue *radioBuffer;			// a buffer that can hold up to 9 values to get trasmitted
		MAC_GenericFrame **radioBuffer;
		int headTxBuffer;
		int tailTxBuffer;
		
		vector <double> txPowerLevels;	// the vector holding the different Tx PowerLevels, txPowerLevel[0] is the default
		int maxBufferSizeRecorded;
		simtime_t startListeningTime;
		simtime_t startSleepingTime;
		simtime_t startTxTime;
		ResourceGenericManager *resMgrModule;	//a pointer to the object of the Radio Module (used for direct method calls)
		int changingState;				//indicates that the Radio is in the middle of the process of changing from one state to another.
		int disabled;
		int isCSValid;
		
		int nextState;
		//int framesToNextState;			//the number of frames that have to be transmitted before the radio state changes.
		
		int nextTxMode;
		//int framesToNextTxMode;
		
		int nextPowerLevel;
		//int framesToNextPowerLevel;
		cOutVector valuesVector;
		
		int timesBlockTX;
		int timesBlockTX2;
		
	protected:
		virtual void initialize();
		virtual void handleMessage(cMessage *msg);
		virtual void finish();
		void readIniFileParameters(void);
		void senseCarrier(double interval);
		//int pushBuffer(MAC_GenericFrame *theFrame);
		//int popBuffer(MAC_GenericFrame *retFrame);
		double popAndSendToChannel();
		void encapsulateMacFrame(MAC_GenericFrame *macFrame, WChannel_GenericMessage *retWcFrame);
		void decapsulateReceivedFrame(WChannel_GenericMessage *wcFrame, MAC_GenericFrame *retMacFrame);
		int pushBuffer(MAC_GenericFrame *theMsg);
		MAC_GenericFrame* popBuffer();
		int getRadioBufferSize(void);
		
	public:
		int isCarrierSenseValid(void);
		int getTotalTxPowerLevels(void);
};

#endif //_RADIOMODULE_H_
