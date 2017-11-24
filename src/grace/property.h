/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#pragma once

#include <stdio.h>

#define property(type) struct : public Property <type>

template <typename T>
class Property
{
protected:
	T value;
public:
	virtual T get() {
		return value;
	}
	virtual void set(T new_value) {
		value = new_value;
	}
};

