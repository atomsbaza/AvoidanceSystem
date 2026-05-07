#ifndef NEWPING_STUB_H
#define NEWPING_STUB_H

class NewPing {
public:
  NewPing(int /*trigger*/, int /*echo*/, int /*maxDist*/) {}
  unsigned int ping_cm() { return 0; }
};

#endif
