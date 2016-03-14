/*
 * hypothesis_handler.cpp
 *
 *  Created on: Mar 23, 2015
 *      Author: frm-ag
 */

#ifndef PEOPLE_LEG_DETECTOR_INCLUDE_MHT_HYPOTHESIS_HANDLER_h_
#define PEOPLE_LEG_DETECTOR_INCLUDE_MHT_HYPOTHESIS_HANDLER_h_


//#include <base/handler/ObservationHandler.h>
//#include <base/handler/TrackHandler.h>
//#include <base/handler/DataAssociation.h>
//#include <base/messages/UIMessageReceiver.h>
//#include <communication/data/IPCData.h>
//#include <communication/handler/AbstractHandler.h>
//#include <communication/util/Logger.h>
//#include <mht/data/Hypothesis.h>
//#include <mht/data/HypothesisComparator.h>
//#include <mht/data/Assignment.h>
//#include <mht/handler/AssignmentHandler.h>
//#include <parameters/ParametersBase.h>
//#include <Eigen/Core>
//#include <vector>

#define DEBUG_HYPOTHESISHANDLER 1

/**
 * The HypothesisHandler contains all necessary methods of the MHT approach.
 * It is connected to the ObservationHandler to get input data,
 * to the TrackHandler to manage existing Tracks (from creation over continuation, occlusion to deletion),
 * and to the AssignmentHandler to generate the k best Assignments.
 * Furthermore, it contains all Hypotheses of the last n-scan-back cycles and provides
 * the scan-back pruning strategy.
 *
 * @author Matthias Luber (modified by Alexander Gutenkunst)
 */
class HypothesisHandler
{
public:

    /** Get the instance of the HypothesisHandler.
     * Create the HypothesisHandler if necessary. */
    //static HypothesisHandler* getInstance(unsigned int historyDepth = 10);
    /** Destroy the current instance. */
    /*static void destroyInstance()
    {
        delete m_instance;
        m_instance = NULL;
    }*/
    /** Destructor. */
    virtual ~HypothesisHandler();
    /** Get the name of the handler. */
    inline const char* getName() const
    {
        return "HypothesisHandler";
    }

    /** The HypothesisHandler listens to the ObservationHandler.
     * If the ObservationHandler has new Observations, this method is called
     * and starts the MHT data association method. */
    virtual void updatedObservationHandler();

    //--- This is in the repeat loop (repeat for every scan) ---//

    /** Init cycle.
     * @param timestamp Time of the current cycle. */
    AbstractHandler::HANDLER_STATE initCycle(ros::Time logtime);

    /** Create all Child Hypotheses. */
    //void createChildHypotheses(const std::vector<Assignment*>& assignments);

    /** Scan-back pruning strategy. */
    //void scanbackPruning();

#ifdef DEBUG
    /** Check Hypothesis for duplicates. */
    void checkDuplicatedHypothesis();
#endif
    /** Delete unconfirmed (deleted) Tracks from the Track vector. */
    // void deleteUnconfirmedTracks();

    /** Finalize cycle. */
    AbstractHandler::HANDLER_STATE finalizeCycle();

    //--- End of the repeat loop (repeat for every scan) ---//

    //--- Get the Hypotheses ---//

    /** Get the number of Hypotheses, SCANBACK steps in the past. */
    //inline unsigned int getScanbackHypothesesCount() const
    //{
    //    return m_allHypotheses[m_scanbackHistoryIndex].size();
    //}

    /** Get the number of Hypotheses in the last cycle (parent Hypotheses). */
    //inline unsigned int getLastHypothesesCount() const
    //{
    //    return m_allHypotheses[m_lastHistoryIndex].size();
    //}

/*    * Get the number of Hypotheses in the current cycle.
    inline unsigned int getCurrentHypothesesCount() const
    {
        return m_allHypotheses[m_currentHistoryIndex].size();
    }
    * Get the Hypotheses, SCANBACK steps in the past.
    inline const std::vector<Hypothesis*>& getScanbackHypotheses() const
    {
        return m_allHypotheses[m_scanbackHistoryIndex];
    }
    * Get the Hypotheses in the last cycle (parent Hypotheses).
    inline const std::vector<Hypothesis*>& getLastHypotheses() const
    {
        return m_allHypotheses[m_lastHistoryIndex];
    }
    * Get the Hypotheses in the cyrrent cycle.
    inline const std::vector<Hypothesis*>& getCurrentHypotheses() const
    {
        return m_allHypotheses[m_currentHistoryIndex];
    }
    * Get the Hypotheses, backward steps in the past.
    inline const std::vector<Hypothesis*>& getHypotheses(unsigned int backward) const
    {
        assert(backward <= m_historyDepth);
        unsigned int historyPosition = (m_currentHistoryIndex + m_historyDepth
                - backward) % (int) m_historyDepth;
        return m_allHypotheses[historyPosition];
    }*/
    /** Get sorted vector of last Hypotheses. */
    //std::vector<Hypothesis*> getLastHypothesesSorted();
    /** Returns a pointer to the best Hypothesis.
     * Either in scan-back depth (when scanback is set to true)
     * or in the current timestep. */
    //const Hypothesis* const getBestHypothesis(bool scanback = false) const;

    /** Get current WorldState.
     * Return the current best Hypothesis. */
/*    inline const WorldState* getWorldState() const
    {
        return getBestHypothesis(false);
    }*/

    //--- Listeners ---//

    /** Add an observer. */
    //inline void addListener(HypothesisHandlerListener* listener)
    //{
    //    m_listeners.push_back(listener);
    //}
    /** Remove an observer. */
    //void removeListener(const HypothesisHandlerListener* listener);

private:

    /** Private constructor, its a singleton! */
    HypothesisHandler(unsigned int historyDepth);
    /** HypothesisHandler instance. */
    static HypothesisHandler* m_instance;
#ifdef LOGGING
    /** File Logger. */
    Logger* m_logger;
    Logger* m_hypologger;
#endif
    /** ObservationHandler. */
    ObservationHandler* m_observationHandler;
    /** Indicator, whether an MHTDetector is used. */
    bool m_mhtDetectorUsed;
    /** TrackHandler. */
    TrackHandler* m_trackHandler;
    /** AssignmentHandler. */
    //AssignmentHandler* m_assignmentHandler;
    /** Index of the vector of hypotheses _scanback steps in past. */
    unsigned int m_scanbackHistoryIndex;
    /** HypothesisID counter. */
    unsigned int m_hypothesisIDCounter;
    /** Unique cycle IDs. */
    unsigned int m_uniqueCycleIDCounter;
    /** Clear indicator. Initialize a new Hypotheses Tree */
    bool m_clearRequested;

    /** All created Hypotheses in the last m_historyDepth-th timesteps. */
    //std::vector<Hypothesis*>* m_allHypotheses;
    /** Best leaf Hypotheses. */
    //Hypothesis** m_bestLeafHypotheses;
    /** Best root Hypotheses (in scanback depth). */
    //Hypothesis** m_bestRootHypotheses;
    /** Comparator to sort Hypothesis based on their likelihood. */
//    HypothesisLikelihoodComparator m_likelihoodComparator;
    /** Comparator to sort Hypothesis based on their root likelihood. */
//    HypothesisRootLikelihoodComparator m_rootLikelihoodComparator;
    /** Comparator to sort Hypothesis based on their index. */
//    HypothesisUniqueCycleIDComparator m_uniqueCycleIDComparator;
    /** Indices of approved Tracks. Will be removed. */
    std::vector<int> _approvedTrackIDs;

    //--- Listeners / Observers ---//

    /** Vector of observers attached to the handler. */
//    std::vector<HypothesisHandlerListener*> m_listeners;
    /** Update all attached observers. */
    inline void updateListeners()
    {
//        LOG("updateListeners\n");
//        DEBUG_FOUT(DEBUG_HYPOTHESISHANDLER,
//                "HypothesisHandler::%s( %i )\n", __func__, m_cycleCounter);
//        for (unsigned int lIndex = 0; lIndex < m_listeners.size(); ++lIndex) {
//            m_listeners[lIndex]->updatedHypothesisHandler();
//        }
    }

public:

    /** IPC module name of the MHT. */
    static const std::string MHT;
    /** Limit the cycletime. */
    static const std::string LIMIT_CYCLETIME_NAME;
    bool LIMIT_CYCLETIME;
    /** Cycletime in s. */
    static const std::string MAX_CYCLETIME_NAME;
    double MAX_CYCLETIME;
    /** Maximal number of Hypotheses at children level. */
    static const std::string MAX_HYPOTHESES_NAME;
    unsigned int MAX_HYPOTHESES;
    /** Maximal number of Hypotheses at root level (current - scanback). */
    static const std::string MAX_ROOT_HYPOTHESES_NAME;
    unsigned int MAX_ROOT_HYPOTHESES;
    /** Number of steps in the past to update root hypotheses. */
    static const std::string SCANBACK_NAME;
    unsigned int SCANBACK;
    /** Synchronize HypothesisHandler via ipc. */
    static const std::string PUBLISH_HYPOTHESISHANDLER_NAME;
    bool PUBLISH_HYPOTHESISHANDLER;

};


#endif /* PEOPLE_LEG_DETECTOR_INCLUDE_MHT_HYPOTHESIS_HANDLER_h_ */
