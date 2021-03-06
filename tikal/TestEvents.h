#pragma once

#include "Event.h"
#include "PayloadEvent.h"

class VoidEvent : public tikal::Event {};

class CharEvent : public tikal::PayloadEvent<const char&> {};
class CharEvent2 : public tikal::PayloadEvent<const char&> {};

class IntEvent : public tikal::PayloadEvent<const int&> {};
