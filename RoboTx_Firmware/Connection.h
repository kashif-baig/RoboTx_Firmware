#ifndef CONNECTION_H
#define CONNECTION_H

#include "Task.h"

/**
 * Stores the connection state, which is determined by the ConnectionMessage sent
 * by the serial client. Input button and IR commands are only sent to
 * the client if the connection is in open state.
 */
class Connection : public Task
{
    bool _opened = false;
    bool _isAlive = false;

    const uint32_t _durationMs = 4000;

    /**
     * Sets the keep alive state of the connection.
     */
    void keepAlive(bool alive)
    {
        setTime();
        _isAlive = alive;
    }

public:
    // Sets connection state.
    void setOpen(bool opened)
    {
        _opened = opened;
        keepAlive(opened);
    }

    // Returns true if the connection state is set to open.
    bool isOpen() const
    {
        return _opened;
    }

    // Instructs the connection to be kept alive.
    void keepAlive()
    {
        keepAlive(true);
    }

    // Returns true if the connection is alive.
    bool isAlive() const
    {
        return _isAlive;
    }

    // If a keep alive request has not been received within expected duration, then set connection

    /**
     * If a keep alive request has not been received within expected duration
     * then set connection alive state to false.
     */
    void doEvents()
    {
        if (_isAlive && compareTime() > _durationMs)
        {
            _isAlive = false;
        }
    }
};

#endif
