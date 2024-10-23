// Rate in bytes per tick
// Limitation : This does not deal with the tick counter wrapping
#define RATE  100

int sendAtRate(void * data, int length);
{
    static TickType_t lastIterationTickCount = 0;
    TickType_t currentTickCount = xTaskGetTickCount();
    TickType_t ticksSinceLastCall = currentTickCount - lastIterationTickCount;
    lastIterationTickCount = currentTickCount;
    
    long bytesAllowedSinceLastCall = RATE * ticksSinceLastCall;

    
    return bytesAllowedSinceLastCall < length ? bytesAllowedSinceLastCall : length;
}