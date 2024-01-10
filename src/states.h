#ifndef STATES_H
#define STATES_H

typedef enum {
    OFF,
    ON,
    ERR
} PowerState;

typedef enum {
    NORMAL,
    FLATLINE,
    ARRHYTHMIA
} HeartRate;

typedef enum {
    SELF_TEST,
    CHECK_Person_RESPONSIVENESS,
    CALL_FOR_HELP,
    ATTACH_ELECTRODES,
    ANALYZE_RHYTHM,
    DELIVER_SHOCK,
    DELIVER_CPR,
    NO_CPR
} Stage;

#endif // STATES_H