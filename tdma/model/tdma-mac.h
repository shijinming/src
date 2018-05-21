#ifndef TMDA_MAC_H
#define TDMA_MAC_H

#include "ns3/packet.h"
#include "ns3/mac48-address.h"
#include "ns3/wifi-mac-header.h"
#include "ns3/event-id.h"
#include "ns3/random-variable-stream.h"
#include "ns3/vector.h"
#include "ns3/wifi-mac-queue.h"
#include "tdma-slot-manager.h"

#include "ns3/wifi-phy.h"
#include "ns3/ssid.h"
#include "ns3/qos-utils.h"

namespace tdma {

    class TdmaMac;

    class TdmaMacPhyListener : public ns3::WifiPhyListener
    {
    public:
        TdmaMacPhyListener(TdmaMac *mac);
        virtual~TdmaMacPhyListener();

        virtual void NotifyRxStart(ns3::Time duration);
        virtual void NotifyRxEndOk(void);
        virtual void NotifyRxEndError(void);
        virtual void NotifyTxStart(ns3::Time duration, double txPowerDbm);
        virtual void NotifyMaybeCcaBusyStart(ns3::Time duration);
        virtual void NotifySwitchingStart(ns3::Time duration);
        void NotifySleep(void) {};
        void NotifyWakeup(void) {};
    private:
        TdmaMac *m_TdmaMac;
    };

    class TdmaMac : public ns3::Object
    {
        friend class TdmaHelper;
    public:
        static ns3::TypeId GetTypeId (void);
        TdmaMac ();
        virtual ~TdmaMac ();

        ns3::Time GetGuardInterval (void) const;
        void SetGuardInterval (const ns3::Time gi);
        void SetSelectionIntervalRatio (double ratio);
        double GetSelectionIntervalRatio () const;
        void SetMinimumCandidateSetSize (uint32_t size);
        void SetAddress (ns3::Mac48Address address);
        ns3::Mac48Address GetAddress (void) const;
        void SetSsid (ns3::Ssid ssid);
        ns3::Ssid GetSsid (void) const;
        void SetBssid (ns3::Mac48Address bssid);
        ns3::Mac48Address GetBssid (void) const;
        void Enqueue (ns3::Ptr<const ns3::Packet> packet, ns3::Mac48Address to, ns3::Mac48Address from);
        void Enqueue (ns3::Ptr<const ns3::Packet> packet, ns3::Mac48Address to);
        void StartInitializationPhase ();
        bool SupportsSendFrom (void) const;
        void SetWifiPhy (ns3::Ptr<ns3::WifiPhy> phy);
        void SetForwardUpCallback (ns3::Callback<void, ns3::Ptr<ns3::Packet>, ns3::Mac48Address, ns3::Mac48Address> upCallback);
        void SetLinkUpCallback (ns3::Callback<void> linkUp);
        void SetLinkDownCallback (ns3::Callback<void> linkDown);
        void Receive (ns3::Ptr<ns3::Packet> packet, double rxSnr, ns3::WifiTxVector txVector);
        void HandleRxStart (ns3::Time duration);
        void HandleRxSuccess (void);
        void HandleRxFailure (void);
        void HandleMaybeCcaBusyStartNow (ns3::Time duration);
        void ConfigureStandard(enum ns3::WifiPhyStandard standard);

        void SetChannelWidth(uint32_t channelWidth);
        void SetFrequency(double frequency);


    protected:

        void
        ForwardUp (ns3::Ptr<ns3::Packet> packet, ns3::Mac48Address from, ns3::Mac48Address to);

        ns3::Ptr<ns3::WifiPhy> m_phy;
        ns3::Ptr<ns3::WifiMacQueue> m_queue;

        ns3::Callback<void, ns3::Ptr<ns3::Packet>, ns3::Mac48Address, ns3::Mac48Address> m_forwardUp;
        ns3::Callback<void> m_linkUp;
        ns3::Callback<void> m_linkDown;
        ns3::Mac48Address m_self;
        ns3::Mac48Address m_bssid;
        ns3::Ssid m_ssid;

    private:

        void Configure80211n_CCH (void);
        void Configure80211n_SCH (void);
        ns3::Time GetSlotDuration();
        void EndOfInitializationPhase ();
        void PerformNetworkEntry (uint32_t remainingSlots, double p);
        void DoTransmit(bool firstFrame);
        double m_selectionIntervalRatio;
        uint32_t m_minimumCandidateSetSize;
        ns3::Time m_guardInterval;
        ns3::WifiPreamble m_wifiPreamble;
        ns3::WifiMode m_wifiMode;
        uint8_t m_reportRate;
        ns3::Time m_frameDuration;
        uint32_t m_maxPacketSize;
        ns3::Ptr<ns3::UniformRandomVariable> m_timeoutRng;
        uint16_t m_slotsForRtdma;
        uint32_t m_channelWidth;
        double m_frequency;

        TdmaMacPhyListener *m_phyListener;
        bool m_rxOngoing;
        ns3::Time m_rxStart;
        bool m_startedUp;
        ns3::Ptr<TdmaSlotManager> m_manager;
        ns3::EventId m_endInitializationPhaseEvent;
        ns3::EventId m_nextTransmissionEvent;

        ns3::TracedCallback<ns3::Time, ns3::Time, ns3::Time> m_startupTrace;
        ns3::TracedCallback<ns3::Ptr<const ns3::Packet>, ns3::Time, bool> m_networkEntryTrace;
        ns3::TracedCallback<ns3::Ptr<const ns3::Packet>, uint32_t, uint8_t, uint32_t> m_txTrace;
        ns3::TracedCallback<ns3::Ptr<const ns3::Packet>, uint8_t, uint32_t> m_rxTrace;
        ns3::TracedCallback<ns3::Ptr<const ns3::Packet> > m_enqueueTrace;
        ns3::TracedCallback<ns3::Ptr<const ns3::Packet> > m_enqueueFailTrace;
    };

} // namespace tdma

#endif /* TDMA_MAC_H */
