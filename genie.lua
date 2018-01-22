solution "tikal"
	configurations {
		"Debug",
		"Release",
	}

	if _ACTION == "xcode4" then
		platforms {
			"Universal",
		}
	else
		platforms {
			"x32",
			"x64",
			"Native", -- for targets where bitness is not specified
		}
	end

	language "C++"
	startproject "tikalTestBed"

BOOST_DIR      = path.getabsolute("../boost")
CTTI_DIR       = path.getabsolute("../ctti")
HYPODERMIC_DIR = path.getabsolute("../Hypodermic")
TIKAL_DIR      = path.getabsolute(".")

local TIKAL_BUILD_DIR = path.join(TIKAL_DIR, ".build")
local BGFX_THIRD_PARTY_DIR = path.join(TIKAL_DIR, "3rdparty")

dofile (path.join(TIKAL_DIR, "toolchain.lua"))
if not toolchain(TIKAL_BUILD_DIR, THIRD_PARTY_DIR) then
	return -- no action specified
end
 
function tikalProject()
    project ("tikalTestBed")
        uuid (os.uuid("tikalTestBed"))
        kind "ConsoleApp"
        
    files {
        path.join(TIKAL_DIR, "src", "main.cpp"),
    }
    
	includedirs {
        path.join(BOOST_DIR, "."),
        path.join(CTTI_DIR, "include"),
        path.join(HYPODERMIC_DIR, "."),
		path.join(TIKAL_DIR, "."),
    }
    
	debugdir (path.join(TIKAL_DIR, "."))
    
    removeflags {
        "NoRTTI",
        "NoExceptions",
        "NoEditAndContinue",
    }
    
	links {
        "tikal"
    }
end

dofile("tikal.lua")

tikalProject()
