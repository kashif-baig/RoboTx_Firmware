#include "AccelGyroTask.h"

#if defined(MPU6050_SENSOR_AVAILABLE)

void AccelGyroTask::doEvents()
{
    if (!enabled())
    {
        return;
    }
    uint16_t timeDiff = compareTime();

    switch (_accelGyroTaskState)
    {
    case AccelGyroTaskState_Delay:
    {
        if (timeDiff >= ACCEL_GYRO_TASK_INTERVAL_MS)
        {
            setTime(timeDiff - ACCEL_GYRO_TASK_INTERVAL_MS);

            _accelGyroTaskState = AccelGyroTaskState_SendAccel;
        }
        break;
    }
    case AccelGyroTaskState_SendAccel:
    {
        if (timeDiff >= 3)
        {
            setTime(timeDiff - 3);

            uint16_t x, y, z;
            _mpu.getAccelRaw(&x, &y, &z);

            _accelGyroMessage->serializeAccel(x, y, z);
            //_accelGyroMessage->serialize(&MsgSerial);

            _accelGyroTaskState = AccelGyroTaskState_SendGyro;
        }
        break;
    }
    case AccelGyroTaskState_SendGyro:
    {
        if (timeDiff >= 3)
        {
            setTime(timeDiff - 3);

            uint16_t x, y, z;
            _mpu.getGyroRaw(&x, &y, &z);

            _accelGyroMessage->serializeGyro(x, y, z);
            //_accelGyroMessage->serialize(&MsgSerial);

            _accelGyroTaskState = AccelGyroTaskState_Delay;
        }
        break;
    }
    }
}

#endif
