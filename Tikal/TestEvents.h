#pragma once

#include "Event.h"
#include "PayloadEvent.h"

class VoidEvent : public tikal::Event {};

class CharEvent : public tikal::PayloadEvent<const char&> {};
