add_library(Utility STATIC
    Assert.cpp Assert.hpp Assert.inl
    DataBlock.hpp
    MemoryMappedFile.cpp MemoryMappedFile.hpp
    MemoryMappedFile_impl.cpp MemoryMappedFile_impl.hpp
    RAIIWrapper.hpp
    VirtualObject.cpp VirtualObject.hpp)
