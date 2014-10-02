/**
 * The TibiaX Server - a free and open-source MMORPG server emulator
 * Copyright (C) 2014  TibiaX <http://tibiax.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include "otpch.h"

#include "depotlocker.h"
#include "tools.h"

DepotLocker::DepotLocker(uint16_t _type) :
	Container(_type)
{
	depotId = 0;
	maxSize = 3;
}

DepotLocker::~DepotLocker()
{
	//
}

Attr_ReadValue DepotLocker::readAttr(AttrTypes_t attr, PropStream& propStream)
{
	if (attr == ATTR_DEPOT_ID) {
		uint16_t _depotId;

		if (!propStream.GET_USHORT(_depotId)) {
			return ATTR_READ_ERROR;
		}

		setDepotId(_depotId);
		return ATTR_READ_CONTINUE;
	} else {
		return Item::readAttr(attr, propStream);
	}
}

ReturnValue DepotLocker::__queryAdd(int32_t index, const Thing* thing, uint32_t count,
                                    uint32_t flags, Creature* actor/* = nullptr*/) const
{
	return Container::__queryAdd(index, thing, count, flags, actor);
}

void DepotLocker::postAddNotification(Thing* thing, const Cylinder* oldParent, int32_t index, cylinderlink_t link /*= LINK_OWNER*/)
{
	if (getParent() != nullptr) {
		getParent()->postAddNotification(thing, oldParent, index, LINK_PARENT);
	}
}

void DepotLocker::postRemoveNotification(Thing* thing, const Cylinder* newParent, int32_t index, bool isCompleteRemoval, cylinderlink_t link /*= LINK_OWNER*/)
{
	if (getParent() != nullptr) {
		getParent()->postRemoveNotification(thing, newParent, index, isCompleteRemoval, LINK_PARENT);
	}
}

void DepotLocker::removeInbox(Inbox* inbox)
{
	auto cit = std::find(itemlist.begin(), itemlist.end(), inbox);
	if (cit == itemlist.end()) {
		return;
	}
	itemlist.erase(cit);
}
