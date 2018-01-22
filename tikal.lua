project "tikal"
	kind "StaticLib"

	includedirs {
        path.join(BOOST_DIR, "."),
        path.join(CTTI_DIR, "include"),
        path.join(HYPODERMIC_DIR, "."),
	}

	files {
		path.join(TIKAL_DIR, "tikal/**"),
	}
    
    removefiles {
        path.join(TIKAL_DIR, "tikal", "main.cpp"),
    }
    
    removeflags {
		"NoRTTI",
		"NoExceptions",
    }

	configuration { "linux-*" }
		buildoptions {
			"-fPIC",
		}

	configuration {}
