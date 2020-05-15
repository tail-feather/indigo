CXXFLAGS += -std=c++11
LDFLAGS += -lstdc++

ifeq ($(filter %libRicohCameraSDKCpp.so, $(SDK_LIB_SO)),"")
	# TODO: Mac .dylib
else
	FIX_SDK_LIB_SO = patchelf --set-rpath \$$ORIGIN $(BUILD_LIB)/libRicohCameraSDKCpp.so
endif
