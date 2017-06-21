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

#include "cAckDelay.h"

cAckDelay::cAckDelay(const char* name, simtime_t departure)
{
	this->setName(name);
	this->setDeparture(departure);
	return;
}

cAckDelay::~cAckDelay()
{
	// TODO Auto-generated destructor stub
}

simtime_t cAckDelay::getArrival() const
{
	return arrival;
}

void cAckDelay::setArrival(simtime_t arrival)
{
	this->arrival = arrival;
}

simtime_t cAckDelay::getDeparture() const
{
	return departure;
}

void cAckDelay::setDeparture(simtime_t departure)
{
	this->departure = departure;
}

simtime_t cAckDelay::getE2EDelay() const
{
	return arrival - departure;
}
