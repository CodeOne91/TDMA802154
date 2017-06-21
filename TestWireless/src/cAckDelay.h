//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

#ifndef CACKDELAY_H_
#define CACKDELAY_H_
#include "inet/common/INETUtils.h"
#include "inet/common/INETMath.h"
#include "inet/common/ModuleAccess.h"
#include <omnetpp/cnamedobject.h>

class cAckDelay: public omnetpp::cNamedObject
{
public:
	cAckDelay(const char* name, simtime_t departure);
	virtual ~cAckDelay();
	simtime_t getArrival() const;
	void setArrival(simtime_t arrival);
	simtime_t getDeparture() const;
	void setDeparture(simtime_t departure);
	simtime_t getE2EDelay() const;

private:
	simtime_t arrival;
	simtime_t departure;
};

#endif /* CACKDELAY_H_ */
