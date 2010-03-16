/********************************************************************************
 *  Copyright: National ICT Australia,  2010									*
 *  Developed at the ATP lab, Networked Systems theme							*
 *  Author(s): Athanassios Boulis, Yuri Tselishchev								*
 *  This file is distributed under the terms in the attached LICENSE file.		*
 *  If you do not find this file, copies can be found by writing to:			*
 *																				*
 *      NICTA, Locked Bag 9013, Alexandria, NSW 1435, Australia					*
 *      Attention:  License Inquiry.											*
 ********************************************************************************/

#ifndef MEDWIN_MAC_MODULE_H
#define MEDWIN_MAC_MODULE_H

#include "VirtualMacModule.h"
#include "MedWinMacPacket_m.h"

#define TX_TIME(x) (phyLayerOverhead + x)*1/(1000*phyDataRate/8.0) //x are in BYTES
#define UNCONNECTED -1

// Guard time used to start RX earlier and allow less time to finish TX
#define GUARD_TIME (pastSyncIntervalNominal ? allocationSlotLength / 10.0 + extraGuardTime() : allocationSlotLength/10.0)
// Guard time used to delay the start of a transmission
#define GUARD_TX_TIME (pastSyncIntervalNominal ? extraGuardTime() : 0.0)
// We discovered that ending a TX we should really guard for 2*GT instead of 1*GT.
// We offer this option with the enhanceGuardTime parameter
#define GUARD_FACTOR (enhanceGuardTime ? 2.0 : 1.0)

#define SLEEP2TX (isRadioSleeping ? pTimeSleepToTX : 0.0)

#define MAC_SELF_ADDRESS self
#define MGMT_BUFFER_SIZE 16  // hold maximum of 16 management pakcets

using namespace std;

enum MacStates {
	MAC_SETUP = 1000,
	MAC_RAP = 1001,
	MAC_SCHEDULED_TX_ACCESS = 1002,
	MAC_SCHEDULED_RX_ACCESS = 1003,
	MAC_BEACON_WAIT = 1009,
	MAC_SLEEP = 1010
};

enum Timers {
	CARRIER_SENSING = 1,
	START_ATTEMPT_TX = 2,
	ACK_TIMEOUT = 3,
	START_SLEEPING = 4,
	START_SCHEDULED_TX_ACCESS = 5,
	START_SCHEDULED_RX_ACCESS = 6,
	WAKEUP_FOR_BEACON = 7,
	SYNC_INTERVAL_TIMEOUT = 8,
	SEND_BEACON = 9,
	HUB_SCHEDULED_ACCESS = 10,
	START_SETUP = 11
};

static int CWmin[8] = { 16, 16, 8, 8, 4, 4, 2, 1 };
static int CWmax[8] = { 64, 32, 32, 16, 16, 8, 8, 4};

class MedWinMacModule : public VirtualMacModule {
	private:
	bool isHub;
	int connectedHID;
	int connectedNID;
	int unconnectedNID;

	double allocationSlotLength;
	int RAP1Length;
	int beaconPeriodLength;
	int currentScheduleAssignmentStart;
	int currentFreeConnectedNID;

	int scheduledAccessStart;
	int scheduledAccessEnd;
	int scheduledAccessLength;
	int scheduledAccessPeriod;

	int scheduledRxAccessStart;
	int scheduledRxAccessEnd;

	double pTIFS;
	int phyLayerOverhead;
	double phyDataRate;
	int priority;
	double contentionSlotLength;

	int maxPacketRetries;
	int currentPacketTransmissions;
	int currentPacketCSFails;

	int CW;
	bool CWdouble;
	int backoffCounter;

	MacStates macState;

	bool pastSyncIntervalNominal;
	simtime_t syncIntervalAdditionalStart;

	MedWinMacPacket *packetToBeSent;
	simtime_t endTime; // the time when our right to TX ends. Covers RAP and scheduled access

	double SInominal;
	double mClockAccuracy;
	bool enhanceGuardTime;
	bool isRadioSleeping;
	double pTimeSleepToTX;

	bool waitingForACK;
	bool futureAttemptToTX;
	bool attemptingToTX;

	// a buffer to store Management packets that require ack and possible reTX
	// these packets are treated like data packets, but with higher priority
	queue <MedWinMacPacket *>MgmtBuffer;


	protected:
	void startup();
	void timerFiredCallback(int);
	void fromNetworkLayer(cPacket*, int);
	void fromRadioLayer(cPacket*,double,double);
	void finishSpecific();
	bool isPacketForMe(MedWinMacPacket * pkt);
	simtime_t extraGuardTime();
	void setHeaderFields(MedWinMacPacket * pkt, AcknowledgementPolicy_type ackPolicy, Frame_type frameType, Frame_subtype frameSubtype);
	void attemptTxInRAP();
	void attemptTX();
	bool canFitTx();
	void sendPacket();
	simtime_t timeToNextBeacon(simtime_t interval, int index, int phase);
};

#endif // MEDWIN_MAC_MODULE_H
