#ifndef CDCC_ACM_H
#define CDCC_ACM_H

#include "cdcc.h"

class CDCC_ACM : public CDCConnector
{
public:
    // CDCC_ACM();
    int connect();
    int setBaudrate(uint32_t baud);
};

#endif // CDCC_CDCACMX_H