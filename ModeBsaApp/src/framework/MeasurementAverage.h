/* 
 * File:   MeasurementAverage.h
 * Author: lpiccoli
 *
 * Created on November 30, 2010, 10:44 AM
 */

#ifndef _MEASUREMENTAVERAGE_H
#define	_MEASUREMENTAVERAGE_H

#include <string>

/**
 * This class keeps a moving average for collected measurements.
 * It is used by the Longitudinal algorithm
 *
 * @author L.Piccoli
 */
class MeasurementAverage {
public:
    MeasurementAverage(int samples = 50, std::string name = "Measurement Average");
    virtual ~MeasurementAverage();

    void add(double measurement);
    double getAverage();
    void clear();
    double getLatestValue();
    double getSum();
    int getHead() { return _head; };

private:
    /** Name for this measurement average (for display purposes only) */
    std::string _name;

    /** Buffer that holds a number of measurements */
    double *_buffer;

    /** Buffer size */
    int _size;

    /** Points to the next element to receive the measurement */
    int _head;

    /** Points to the last element that received a measurement */
    int _tail;

    /** Sum of the current measurements */
    double _sum;

    /** Current average measurement */
    double _average;

    /** Indicate whether the buffer is full of measurements */
    bool _full;
};

#endif	/* _MEASUREMENTAVERAGE_H */

