from conan import ConanFile
from conan.tools.cmake import CMake, CMakeToolchain, CMakeDeps, cmake_layout


class QuorumKitConan(ConanFile):
    name = "quorumkit"
    version = "0.1.0"
    settings = "os", "compiler", "build_type", "arch"
    options = {"build_tests": [True, False]}
    default_options = {"build_tests": True}

    generators = "CMakeToolchain", "CMakeDeps"

    def requirements(self):
        self.requires("protobuf/3.21.12")
        self.requires("gflags/2.2.2")
        self.requires("leveldb/1.23")
        self.requires("openssl/3.4.1")
        self.requires("zlib/1.3.1")
        self.requires("brpc/1.11.0")

    def build_requirements(self):
        self.tool_requires("protobuf/3.21.12")
        if self.options.build_tests:
            self.test_requires("gtest/1.14.0")

    def layout(self):
        cmake_layout(self)

    def build(self):
        cmake = CMake(self)
        variables = {}
        if self.options.build_tests:
            variables["BUILD_UNIT_TESTS"] = "ON"
        cmake.configure(variables=variables)
        cmake.build()

    def package(self):
        cmake = CMake(self)
        cmake.install()
