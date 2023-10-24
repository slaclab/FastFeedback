/*
 * File:   Thread.cc
 * Author: lpiccoli
 * 
 * Created on May 24, 2010, 9:44 AM
 */

#include <iostream>
#include "Thread.h"
#include "Log.h"
#include "Override.h"

USING_FF_NAMESPACE

/**
 * Constructor that creates an actual EPICS thread.
 *
 * TODO: adjust the priority and stack sizes passed to the epicsThread
 * constructor.
 *
 * @param name name of the Thread
 * @author L.Piccoli
 */
FF::Thread::Thread(const std::string name, int priority) :
_done(false),
_eventCount(0),
_eventCountNotHandled(0),
_thread(*this, name.c_str(),epicsThreadGetStackSize(epicsThreadStackBig), priority),
_name(name),
_started(false),
_idleStats(50, "Thread Idle Stats"),
_processStats(50, "Thread Process Stats") {
    //    _join = new epicsMutex();
}

/**
 * Class destructor.
 */
FF::Thread::~Thread() {
    //    Event e(QUIT_EVENT);
    //    send(e);
    //    _join->lock();
    //    delete(_join);
}

/**
 * The run() method defines an event loop for all threads in the system.
 * Thread::run() handles general events such as RECONFIGURE_EVENT and
 * QUIT_EVENT.
 *
 * Thread subclasses should implement their own processEvent() method where
 * specific events can be handled. For example, the LoopThread should add
 * handlers for the MEASUREMENT_EVENT and PATTERN_EVENT events.
 *
 * NOTE: This method seems to be called during the instantiation of the 
 *       epicsThread using epicsThreadCallEntryPoint to call the virtual
 *       run() defined in epicsThreadRunable. Because of our inheritance 
 *       of epicsThreadRunable, this run() method is ran instead. 
 *       - Kyle Leleux (kleleux 10/16/2023)
 *
 * @author L.Piccoli
 */
void FF::Thread::run() {
    _channelAccess.attach();

    if (preRun() != 0) {
        _done = true;
    }
    _started = true;

    // Getting stuck here.
    while (!_done) {
        Event e;
        std::cout << "did I make it here" << std::endl;
//        if ((Override::getInstance().getOverrideState()) || (receive(e) == 0)){
        if ((receive(e) == 0)){
        std::cout << "did I make it here pt2" << std::endl;
      _idleStats.end();
      _processStats.start();
            _eventCount++;
            switch (e._type) {
                case QUIT_EVENT:
                    _done = true;
                    break;
                    // Pass the event onto the subclass and let it deal with it
    
                default:
                    if (processEvent(e) != 0) {
                        _eventCountNotHandled++;
                    }
            }
        _processStats.end();
        } else {
            // Problem getting event from queue
        }
    _idleStats.start();
    }
    postRun();
    _channelAccess.detach();
    //    _join->unlock();
    std::cout << "Do I get here" << std::endl;
    _thread.exit();

}
/**
 * Spawns the actual EPICS thread. This method must be called after
 * the Thread object is created in order to have the Thread running.
 *
 * @return 0 if Thread was started, -1 if it could not be started
 * (i.e. it is already running).
 * @author L.Piccoli
 */
int FF::Thread::start() {
  Log::getInstance() << Log::showtime << Log::flagConfig << Log::dpInfo
		     << "INFO: Thread::start() name=" << _name.c_str() << Log::dp;
  
    _done = false;
    //   _join->lock();
    _thread.start();
    return 0;
}

/**
 * Wait for the Thead to exit, i.e. receive the QUIT_EVENT
 *
 * @return 0 if Thread has quit the run() method, -1 if Thread
 * has not yet started.
 * @author L.Piccoli
 */
int FF::Thread::join() {
    if (!_started) {
        return -1;
    }
    while (!_done); // Can't get epicsMutex to behave as I expected
    //    _join->lock();
    return 0;
}

int FF::Thread::processEvent(Event& event) {
    std::cerr << "Thread::processEvent() called." << std::endl;
    return -1;
}

void FF::Thread::setPriority(int value) {
    _thread.setPriority(value);
}

void FF::Thread::showDebug() {
  _idleStats.show("  ");
  _processStats.show("  ");
  std::cout << "  events received:    " << _eventCount << std::endl;
  std::cout << "  events not handled: " << _eventCountNotHandled << std::endl;
  std::cout << "  events discarded:   " << getDiscardedEvents() << std::endl;
}
