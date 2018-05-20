#ifndef TDMA_SLOT_MANAGER_H_
#define TDMA_SLOT_MANAGER_H_

#include "ns3/object.h"
#include "ns3/nstime.h"
#include "ns3/vector.h"
#include "ns3/traced-callback.h"
#include "ns3/mac48-address.h"

#include <map>
#include <set>

namespace tdma {

    class TdmaSlot : public ns3::Object {
    public:

        enum State {
            FREE,             // A slot is said to be free if it is neither used, neither busy
            ALLOCATED,        // A slot is reserved if it is allocated/reserved by another station/node
            BUSY              // A slot is busy if it is not allocated by anybody, but the energy
        };

        TdmaSlot (uint32_t index);
        virtual ~TdmaSlot();

        TdmaSlot::State GetState();
        bool IsInternallyAllocated();

        void MarkAsFree();
        bool IsFree();
        bool IsFree(ns3::Time until);
        void MarkAsAllocated(uint8_t timeout, ns3::Mac48Address owner, ns3::Vector position, ns3::Time notBefore);
        bool IsAllocated();
        void MarkAsInternallyAllocated(uint8_t timeout);
        void MarkAsBusy();
        bool IsBusy();
        void RebaseIndex(uint32_t index);

        uint32_t GetSlotIndex();
        void SetTimeout(uint8_t t);
        void SetInternalTimeout(uint8_t t);
        uint8_t GetTimeout();
        uint8_t GetInternalTimeout();
        ns3::Vector GetPosition();
        ns3::Mac48Address GetOwner();

    private:

        bool m_internal;
        State m_state;
        State m_previousState;
        uint32_t m_index;
        uint8_t m_timeout;
        uint8_t m_internalTimeout;
        ns3::Vector m_position;
        ns3::Mac48Address m_owner;
        ns3::Time m_notBefore;

    };

    class RandomAccessDetails : public ns3::Object {
    public:

        RandomAccessDetails ();
        virtual ~RandomAccessDetails();

        void SetWhen (ns3::Time when);
        void SetProbability (double p);
        void SetRemainingSlots (uint16_t left);

        ns3::Time GetWhen();
        double GetProbability();
        uint16_t GetRemainingSlots();

    private:

        ns3::Time m_when;
        double m_p;
        uint16_t m_slotsLeft;
    };

    class TdmaSlotManager : public ns3::Object {
    public:

        static ns3::TypeId GetTypeId (void);
        TdmaSlotManager();
        virtual ~TdmaSlotManager();

        void Setup(ns3::Time start, ns3::Time frameDuration, ns3::Time slotDuration, uint32_t candidateSlots);
        ns3::Time GetStart();
        ns3::Time GetFrameDuration();
        uint32_t GetSlotsPerFrame();
        void SetReportRate(uint32_t rate);
        void SetMinimumCandidateSlotSetSize (uint32_t size);
        void SetSelectionIntervalRatio(double ratio);
        void SelectNominalSlots();
        uint32_t GetCurrentReservationNo();
        void SelectTransmissionSlotForReservationWithNo (uint32_t n, uint8_t timeout);
        uint32_t ReSelectTransmissionSlotForReservationWithNo (uint32_t n, uint8_t timeout);
        ns3::Time GetTimeUntilTransmissionOfReservationWithNo (uint32_t n);
        uint32_t GetSlotIndexForTimestamp (ns3::Time t);
        uint32_t CalculateSlotOffsetBetweenTransmissions(uint32_t k, uint32_t l);
        bool NeedsReReservation (uint32_t n);
        uint8_t DecreaseTimeOutOfReservationWithNumber(uint32_t n);
        void MarkSlotAsAllocated(uint32_t index, uint8_t timeout, ns3::Mac48Address node, ns3::Vector position);
        void MarkSlotAsAllocated(uint32_t index, uint8_t timeout, ns3::Mac48Address node, ns3::Vector position, ns3::Time notbefore);
        void MarkSlotAsFreeAgain(uint32_t index);
        void MarkSlotAsBusy(uint32_t index);
        void RebaseFrameStart(ns3::Time now);
        ns3::Ptr<RandomAccessDetails> GetNetworkEntryTimestamp(uint32_t remainingSlots, double p);
        bool HasFreeSlotsLeft(uint32_t remainingSlots);
        bool IsCurrentSlotStillFree();
        ns3::Ptr<TdmaSlot> GetSlot(uint32_t index);
        uint64_t GetGlobalSlotIndexForTimestamp(ns3::Time t);

    private:

        void UpdateSlotObservations();
        ns3::Time GetTimeForSlotIndex(uint32_t index);

        std::map<uint32_t, ns3::Ptr<TdmaSlot> > m_slots;
        std::map<uint32_t, ns3::Ptr<TdmaSlot> > m_selections;
        std::map<ns3::Mac48Address, uint32_t> m_collisions;

        ns3::Time m_start;
        ns3::Time m_frameDuration;
        ns3::Time m_slotDuration;
        uint32_t m_rate;
        uint32_t m_ni;
        uint32_t m_siHalf;
        uint32_t m_current;
        uint32_t m_mininumCandidates;
        uint32_t m_numSlots;

        std::vector<uint32_t> m_nss;        // Nominal start slots
        ns3::Time m_lastFrameStart;

        ns3::TracedCallback<std::vector<uint32_t> > m_nominalSlotTrace;
        ns3::TracedCallback<uint32_t, uint32_t, bool> m_reservationTrace;
        ns3::TracedCallback<uint32_t, uint32_t, bool, bool> m_reReservationTrace;

        friend class TdmaSlotManagerTest;
    };

} // namespace tdma

#endif /* TDMA_SLOT_MANAGER_H_ */