#pragma once


class Handlers
{
public:
    static bool Processing(SimpleMessage *message);

private:
    static bool E(SimpleMessage *);
    static bool Request(SimpleMessage *);
    static bool FreqMeasure(SimpleMessage *);
    static bool Log(SimpleMessage *);
};
